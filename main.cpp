/*
 *  V4L2 Video to NDI converter
 *
 *  This program can be used and distributed without restrictions.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

#include <getopt.h>             /* getopt_long() */

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <linux/videodev2.h>
#include <Processing.NDI.Lib.h>
#include <PixelFormatConverter.h>


#define CLEAR(x) memset(&(x), 0, sizeof(x))

//Full NDI
NDIlib_send_create_t NDI_send_create_desc;
NDIlib_send_instance_t pNDI_full_send;
NDIlib_video_frame_v2_t NDI_video_frame1;

zs::PixelFormatConverter converter;
zs::Frame yuy2Frame;
zs::Frame uyvyFrame;

enum io_method {
        IO_METHOD_READ,
        IO_METHOD_MMAP,
        IO_METHOD_USERPTR,
};

struct buffer {
        void   *start;
        size_t  length;
};

static char            *dev_name;
static enum io_method   io = IO_METHOD_MMAP;
static int              fd = -1;
static unsigned int     n_buffers;
static int              out_buf;
static int              force_yuyv = 0;
static int              force_uyvy = 0;
static int              force_nv12 = 0;
static int              fix_csi = 0;
static int              width = 0;
static int              height = 0;
static float            fps_N = 30000;
static float            fps_D = 1001;
static char             *ndi_name;
int                     m_width = 0;
int                     m_height = 0;
int                     m_format = 0;
int                     frame_buffer = 0;
int                     image_threaded = 0;
int                     last_read_frame = 0;
int                     num_v4l2_buffers = 4; 

struct timeval start, end;

unsigned int frames = 0;
double  frameRate = 30; //for 30fps
double  averageFrameTimeMilliseconds = 33.333;

// Queues for communcating between threads
// These (and the thread functions) should really be in C++ classes, but...
std::size_t m_max_depth = 1;    // How many items we will queue before dropping them - default
std::mutex m_lock;
std::condition_variable m_condvar;
std::queue<std::unique_ptr<v4l2_buffer>> m_queue;

// Thread for processing frames
std::thread image_thread;

//Buffers
struct buffer          *buffers;
uint8_t*               copy_buffer[16]; //16 is the max number of video buffers unless this number is changed

static void errno_exit(const char *s){
  fprintf(stderr, "%s error %d, %s\\n", s, errno, strerror(errno));
  exit(EXIT_FAILURE);
}

unsigned int fourcc(const char* format) {
	char fourcc[4];
	memset(&fourcc, 0, sizeof(fourcc));
	if (format != NULL)
	{
		strncpy(fourcc, format, 4);	
	}
	return v4l2_fourcc(fourcc[0], fourcc[1], fourcc[2], fourcc[3]);	
}

std::string fourcc(unsigned int format) {
	char formatArray[] = { (char)(format&0xff), (char)((format>>8)&0xff), (char)((format>>16)&0xff), (char)((format>>24)&0xff), 0 };
	return std::string(formatArray, strlen(formatArray));
}

static int xioctl(int fh, int request, void *arg){
  int r;
  do{
   r = ioctl(fh, request, arg);
  } while (-1 == r && EINTR == errno);
  return r;
}

static void open_device(const char *device_name, int &fd){ //open the device for reading - fd is a reference
  struct stat st;
  if (-1 == stat(device_name, &st)){
   fprintf(stderr, "Cannot identify '%s': %d, %s\n",device_name, errno, strerror(errno));
   exit(EXIT_FAILURE);
  }
  if (!S_ISCHR(st.st_mode)){
   fprintf(stderr, "%s is no device\n", device_name);
   exit(EXIT_FAILURE);
  }
  fd = open(device_name, O_RDWR /* required */ | O_NONBLOCK, 0);
  if (-1 == fd){
   fprintf(stderr, "Cannot open '%s': %d, %s\n",device_name, errno, strerror(errno));
   exit(EXIT_FAILURE);
  }
}

static void init_device(const char *d_name, int fd, unsigned int d_type, unsigned int d_format, unsigned int d_width, unsigned int d_height){ //initialize device
  struct v4l2_capability cap;
  struct v4l2_format fmt;
  //Query capabilities
  if(-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)){
   if(EINVAL == errno){
    fprintf(stderr, "%s is no V4L2 device\n",d_name);
    exit(EXIT_FAILURE);
   }else{
    errno_exit("VIDIOC_QUERYCAP");
   }
  }
  fprintf(stderr, "Path: %s ",d_name);
  fprintf(stderr, "Driver: %s \n",cap.driver);
  if ((cap.capabilities & V4L2_CAP_VIDEO_OUTPUT)){fprintf(stderr, "%s support output\n",d_name);}

	if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)){fprintf(stderr, "%s support capture\n",d_name);}

	if ((cap.capabilities & V4L2_CAP_READWRITE)){fprintf(stderr, "%s support read/write\n",d_name);}
	if ((cap.capabilities & V4L2_CAP_STREAMING)){fprintf(stderr, "%s support streaming\n",d_name);}
  if ((cap.capabilities & V4L2_CAP_VIDEO_M2M_MPLANE)){fprintf(stderr, "%s support m2m mplane\n",d_name);}
  
	if ((cap.capabilities & V4L2_CAP_TIMEPERFRAME)){fprintf(stderr, "%s support timeperframe\n",d_name);} 
  
  CLEAR(fmt);
  fmt.type = d_type;
  if (xioctl(fd, VIDIOC_G_FMT, &fmt) == -1){
    fprintf(stderr, "Cannot get format\n");
	  errno_exit("VIDIOC_G_FMT");
	}
  std::cout << "Current pixel format: " << fourcc(fmt.fmt.pix.pixelformat) << std::endl;
  fprintf(stderr, "Current frame width: %u\n",fmt.fmt.pix.width); 
  fprintf(stderr, "Current frame height: %u\n",fmt.fmt.pix.height);  

  if (d_width != 0) {
		fmt.fmt.pix.width = d_width;
    fprintf(stderr, "Setting frame width to: %u\n",d_width);
	}
	if (d_height != 0) {
		fmt.fmt.pix.height = d_height;
    fprintf(stderr, "Setting frame height to: %u\n",d_height);
	}
	if (d_format != 0) {
		fmt.fmt.pix.pixelformat = d_format;
    std::cout << "Setting pixel format to: " << fourcc(d_format) << std::endl;
	}
  
  if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt)){
   fprintf(stderr, "Cannot set format\n"); 
   errno_exit("VIDIOC_S_FMT");
  }

  if (xioctl(fd, VIDIOC_G_FMT, &fmt) == -1){
    fprintf(stderr, "Cannot get format\n");
	  errno_exit("VIDIOC_G_FMT");
	}

  if((d_format != 0)&&(fmt.fmt.pix.pixelformat != d_format)){
   std::cout << "Cannot set pixel format to: " << fourcc(d_format) << "." << " Current pixel format: " << fourcc(fmt.fmt.pix.pixelformat) << std::endl;  
  }

  if((d_width != 0)&&(fmt.fmt.pix.width != d_width)){
   fprintf(stderr, "Cannot set frame width to: %u. Current width: %u\n",d_width, fmt.fmt.pix.width);  
  }

  if((d_height != 0)&&(fmt.fmt.pix.height != d_height)){
   fprintf(stderr, "Cannot set frame height to: %u. Current height: %u\n",d_height, fmt.fmt.pix.height);  
  }

  m_width = fmt.fmt.pix.width;
  m_height = fmt.fmt.pix.height;  
  m_format = fmt.fmt.pix.pixelformat;
}

static void init_mmap(const char *device_name, int &fd, enum v4l2_buf_type type, struct buffer **bufs_out, unsigned int *n_bufs){  //initialize buffer for device
  struct v4l2_requestbuffers req;
  struct buffer *bufs;
  unsigned int b;
  CLEAR(req);
  req.count = num_v4l2_buffers; //changed from 8 to 4
  req.type = type;
  req.memory = V4L2_MEMORY_MMAP;

  if(-1 == xioctl(fd, VIDIOC_REQBUFS, &req)){
   if (EINVAL == errno) {
    fprintf(stderr, "%s does not support memory mappingn", device_name);
    exit(EXIT_FAILURE);
   }else{
    errno_exit("VIDIOC_REQBUFS");
   }
  }
  if (req.count < 2) {
   fprintf(stderr, "Insufficient buffer memory on %s\\n",device_name);
   exit(EXIT_FAILURE);
  }
  //std::cout << "Buffer Size: " << sizeof(*bufs) << std::endl;
  bufs = (buffer*)calloc(req.count, sizeof(*bufs));

  if(!bufs){
   fprintf(stderr, "Out of memory\\n");
   exit(EXIT_FAILURE);
  }

  for(b = 0; b < req.count; ++b){ //loop through all the buffers and assign values and functions
   struct v4l2_buffer buf;
   CLEAR(buf);
   buf.type        = type;
   buf.memory      = V4L2_MEMORY_MMAP;
   buf.index       = b;
   if(-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf)){
    errno_exit("VIDIOC_QUERYBUF");
   }
   fprintf(stderr, "Mapping %s buffer %u, len %u\n", device_name, b, buf.length);
   bufs[b].length = buf.length;
   bufs[b].start = mmap(NULL,buf.length,PROT_READ | PROT_WRITE,MAP_SHARED,fd, buf.m.offset);
   if (MAP_FAILED == bufs[b].start){
    errno_exit("mmap");
   }
  }
  *n_bufs = b;
  *bufs_out = bufs;
}

static void start_capturing(const char *device_name, int &fd, enum v4l2_buf_type type, unsigned int n_bufs){ //start capturing with main v4l2 device
  unsigned int i;
  for (i = 0; i < n_bufs; ++i) {
   struct v4l2_buffer buf;
   CLEAR(buf);
   buf.type = type;
   buf.memory = V4L2_MEMORY_MMAP;
   buf.index = i;
   if (-1 == xioctl(fd, VIDIOC_QBUF, &buf)){
    errno_exit("VIDIOC_QBUF");
   }else{
    fprintf(stderr, "Queueing %s buffer %u\n",device_name, i); 
   }
  }
  if (-1 == xioctl(fd, VIDIOC_STREAMON, &type)){
   errno_exit("VIDIOC_STREAMON");
  }else{
   fprintf(stderr, "Starting stream into mmap buffer map, %s\n",device_name);  
  }
}

int is_readable(int &fd, timeval* tv){
 fd_set fdset;
 FD_ZERO(&fdset);	
 FD_SET(fd, &fdset);
 return select(fd+1, &fdset, NULL, NULL, tv); 
}

int is_writable(int &fd, timeval* tv){
 fd_set fdset;
 FD_ZERO(&fdset);	
 FD_SET(fd, &fdset);
 return select(fd+1, NULL, &fdset, NULL, tv); 
}

void copy_frame(uint8_t* src, uint8_t* dst, uint32_t size){
 memcpy(dst, src, size);
}

static void queue_wait(void){
  // Lock the queue
  std::unique_lock<std::mutex> lock_queue(m_lock);

  // Until we are woken up with a frame
  while (m_queue.empty())
    m_condvar.wait(lock_queue);
}

static void queue_push(std::unique_ptr<v4l2_buffer> buf){
  // Lock the queue
  std::unique_lock<std::mutex> lock_queue(m_lock);

  // Queue the buffer
  m_queue.push(std::move(buf));
  // LOG(LOG_DBG, ">");	// Pushed an item on the queue

  // Drop items that are too old if the queue is not keeping up
  while ((m_max_depth) && (m_queue.size() > m_max_depth))
  {
    // Pull the item off the queue and requeue it so we don't loose buffers!
    std::unique_ptr<v4l2_buffer> item = std::move(m_queue.front());
    if(-1 == xioctl(fd, VIDIOC_QBUF, item.get())){
      errno_exit("VIDIOC_QBUF");
    }

    m_queue.pop();
    // LOG(LOG_ERR, "!");	// Dropped an item from the queue!
    printf("!"); fflush(stdout);
  }

  // Unlock the queue...
  lock_queue.unlock();

  // ...and wake up the listener
  m_condvar.notify_one();
}

static std::unique_ptr<v4l2_buffer> queue_pop_opt(void){
  // Lock the queue
  std::unique_lock<std::mutex> lock_queue(m_lock);

  if (m_queue.empty()) {
    return {};
  }

  // Get the item from the queue
  std::unique_ptr<v4l2_buffer> item = std::move(m_queue.front());
  m_queue.pop();

  // We can unlock the queue now
  lock_queue.unlock();

  // LOG(LOG_DBG, "<");	// Popped an item off the queue
  return item;
}

static void process_image_thread(void){
  // Used to signal exit
  bool exit_thread = false;

  v4l2_buffer* last_buf = nullptr;

  std::unique_ptr<NDIlib_video_frame_v2_t> frame, last_frame;

  // Cycle until we are told to exit
  while (true)
  {
    // Wait for the queue to have some data
    queue_wait();

    while (true)
    {
      // See if we have any data
      auto buf = queue_pop_opt();

      // If there was data available, buf will be valid, otherwise it will be false
      if (!buf) {
        // Nothing else on the queue to process, go to sleep
        break;
      }

      // A valid unique_ptr to nullptr is submitted as a signal to exit the thread
      if (buf.get() == nullptr) {
        exit_thread = true;
        break;
      }
      if(last_read_frame != buf->index){
       printf("#"); fflush(stdout); //getting behind in processing frames - adds latency if this is happening
      }
      //printf("P"); fflush(stdout); //processing frame debugging
      //printf("%x", buf->index & 0x0F);
      //printf("P\n"); fflush(stdout);

      // Convert to UYVY if necessary, copying in-place
      if(force_yuyv == 1){
        zs::Frame src, dst;
        src.fourcc = (uint32_t)zs::ValidFourccCodes::YUY2;
        src.width = m_width;
        src.height = m_height;
        src.size = buf->bytesused;
        src.data = (uint8_t*)buffers[buf->index].start;

        dst.fourcc = (uint32_t)zs::ValidFourccCodes::UYVY;
        dst.size = buf->bytesused;
        dst.data = (uint8_t*)buffers[buf->index].start;

        if(!converter.Convert(src,dst)){
          fprintf(stderr, "Convert failed\n");
        }

        // Zero the data elements or zs::~Frame will try to free the memory!
        src.data = dst.data = nullptr;
      }

      if(fix_csi == 1){ //fix for high cpu usage when using HDMI to CSI adapters
       copy_buffer[buf->index] = (uint8_t*)malloc(buf->bytesused);
       copy_frame((uint8_t*)buffers[buf->index].start, (uint8_t*)copy_buffer[buf->index], buf->bytesused);
      }

      // Create a new frame we can pass to the NDI stack
      frame = std::make_unique<NDIlib_video_frame_v2_t>();
      frame->xres = m_width;
      frame->yres = m_height;
      frame->frame_rate_N = fps_N;
      frame->frame_rate_D = fps_D;
      frame->line_stride_in_bytes = m_width * 2;
      frame->data_size_in_bytes = m_width * 2;
      if(force_nv12 == 1){
       frame->FourCC = NDIlib_FourCC_type_NV12; 
      }else{
       frame->FourCC = NDIlib_FourCC_type_UYVY;
      }

      if(fix_csi == 1){
       frame->p_data = (uint8_t*)copy_buffer[buf->index]; //copy the frame when CSI adapter is being used to prevent high CPU usage
      }else{
       frame->p_data = (uint8_t*)buffers[buf->index].start;
      }
      //std::cout<<"FrameData is:"<<(uint8_t*)buffers[buf->index].start<<std::endl;
      
      // We're now done with the previous v4l2 buffer, so requeue it
      if (last_buf){
        if(fix_csi == 1){
         free(copy_buffer[last_buf->index]); //free the buffer
        }
        //printf("D"); fflush(stdout); //dropping frame and requeue debugging
        //printf("%x", last_buf->index & 0x0F);
        //printf("D\n"); fflush(stdout);
        if(-1 == xioctl(fd, VIDIOC_QBUF, last_buf)){
          errno_exit("VIDIOC_QBUF");
        }
      }

      // Pass the frame to the NDI stack
      NDIlib_send_send_video_async_v2(pNDI_full_send, frame.get());

      // Keep references to what we passed to the NDI stack until we queue the
      // next frame, or the memory could disappear out from under us!

      last_buf = buf.release();
      last_frame = std::move(frame);
      
      //frames++; //keeps track of number of frames
    }
  }
}

static void process_image(const void *p, int size){
 if(force_yuyv == 1){ //if this is enabled - convert from YUY2 to UYVY for NDI
  yuy2Frame.data = (uint8_t*)p;
  if(!converter.Convert(yuy2Frame,uyvyFrame)){ //convert the YUY2 frame into a UYVY frame - NDI doesn't accept a YUY2 frame
   fprintf(stderr, "Convert failed\n");       
  }
  NDI_video_frame1.p_data = uyvyFrame.data; //link the UYVY frame data to the NDI frame
 }else{
  NDI_video_frame1.p_data = (uint8_t*)p; //link the UYVY frame data to the NDI frame 
 }
 NDIlib_send_send_video_v2(pNDI_full_send, &NDI_video_frame1); //send the data out to NDI
 //frames++;
}

static int read_frame(int &fd, enum v4l2_buf_type type, struct buffer *bufs, unsigned int n_buffs){ //this function reads the frame from the video capture device
  auto buf = std::make_unique<v4l2_buffer>();
  //CLEAR(buf);
  buf->type = type;
  buf->memory = V4L2_MEMORY_MMAP;
  if (-1 == xioctl(fd, VIDIOC_DQBUF, buf.get())) { //dequeue the buffer - dumps data into the previously set mmap
   switch (errno){
    case EAGAIN:
     return 0;
    case EIO:
     /* Could ignore EIO, see spec. */
     /* fall through */
    default:
     errno_exit("VIDIOC_DQBUF");
   }
  }
  assert(buf->index < n_buffs);
  last_read_frame = buf->index;
  //printf("R"); //reading frame debugging
  //printf("%x", buf->index & 0x0F);
  //printf("R\n"); fflush(stdout);

  if(NDIlib_send_get_no_connections(pNDI_full_send, 1000)){ //wait for a NDI receiver to be present before continuing - no need to encode without a client connected
   if(image_threaded == 1){
    queue_push(std::move(buf));
   }else{
    process_image(bufs[buf->index].start, buf->bytesused); //send the mmap frame buffer off to be processed
   }
  }else{
   if(image_threaded == 1){ 
    if(-1 == xioctl(fd, VIDIOC_QBUF, buf.get())){
     errno_exit("VIDIOC_QBUF");
    } 
   }
  }

  if(image_threaded == 0){
    if(-1 == xioctl(fd, VIDIOC_QBUF, buf.get())){
   errno_exit("VIDIOC_QBUF");
  }
  }
  return 1;
}

static int mainloop(void){
  //gettimeofday(&start, NULL); //store starting time
  if (!NDIlib_initialize()){	// Cannot run NDI. Most likely because the CPU is not sufficient (see SDK documentation).
   fprintf(stderr, "CPU cannot run NDI");
   return 0;
  } 
  //Full NDI     
  NDI_send_create_desc.p_ndi_name = ndi_name;
  pNDI_full_send = NDIlib_send_create(&NDI_send_create_desc); 
  if (!pNDI_full_send){
   fprintf(stderr, "Failed to create NDI Full Send");
   exit(1);
  }
  yuy2Frame = zs::Frame(m_width,m_height, MAKE_FOURCC_CODE('Y','U','Y','2')); //initialize conversion frame storage - YUY2
  uyvyFrame.fourcc = MAKE_FOURCC_CODE('U','Y','V','Y'); //initialize conversion frame storage - UYVY
  NDI_video_frame1.xres = m_width;
  NDI_video_frame1.yres = m_height;
  NDI_video_frame1.frame_rate_N = fps_N;
  NDI_video_frame1.frame_rate_D = fps_D;
  if(force_nv12 == 1){
   NDI_video_frame1.FourCC = NDIlib_FourCC_type_NV12; 
  }else{
   NDI_video_frame1.FourCC = NDIlib_FourCC_type_UYVY; //set NDI to receive the type of frame that is going to be given to it - in this case UYVY
  }


  while(1){ //while loop for querying for new data from video capture device and reading new frames
   for(;;){
    struct timeval tv;
    int r;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    r = is_readable(fd, &tv); //see when v4l2 video capture is ready
    if (-1 == r) {
     if (EINTR == errno){
      continue;
     }
     errno_exit("select");
    }
    if(0 == r){
     fprintf(stderr, "select timeout\n");
     exit(EXIT_FAILURE);
    }

    if(r == 1){
     read_frame(fd, V4L2_BUF_TYPE_VIDEO_CAPTURE, buffers, n_buffers); //read new frame 
    }
    /* EAGAIN - continue select loop. */
    //gettimeofday(&end, NULL);
  
    // Calculating total time taken by the program.
    //double time_taken;
  
    //time_taken = (end.tv_sec - start.tv_sec) * 1e6;
    //time_taken = (time_taken + (end.tv_usec - start.tv_usec)) * 1e-6;

      //if(time_taken>1.0){ //every second
        //frameRate = (double)frames*0.5 +  frameRate*0.5; //more stable
        //frames = 0;
        //gettimeofday(&start, NULL);
        //averageFrameTimeMilliseconds  = 1000.0/(frameRate==0?0.001:frameRate);
        //std::cout<<"FrameRate was:"<<frameRate<<std::endl;
       // std::cout<<"FrameTime was:"<<averageFrameTimeMilliseconds<<std::endl;
      //}
  }
 }
}

static void stop_capturing(int fd){
  enum v4l2_buf_type type;
  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type)){
   errno_exit("VIDIOC_STREAMOFF");
  }
}


static void uninit_device(void){
  unsigned int i;
  for (i = 0; i < n_buffers; ++i){
   if (-1 == munmap(buffers[i].start, buffers[i].length)){
    errno_exit("munmap");
   }
  }
  free(buffers);
}


static void close_device(void){
  if(-1 == close(fd)){
   errno_exit("close");
  }
  fd = -1;
}


static void usage(FILE *fp, int argc, char **argv){
        fprintf(fp,
                 "Usage: %s [options]\n\n"

                 "'!' = Frame has been dropped\n"
                 "'#' = Increased Latency Occuring\n\n"

                 "Version 1.0\n"
                 "Options:\n"
                 "-d | --device name   Video device name [%s]\n"
                 "-h | --help          Print this message\n"
                 "-f | --yuyv          Force pixel format to YUYV\n"
                 "-u | --uyvy          Force pixel format to UYVY\n"
                 "-m | --nv12          Force pixel format to NV12\n"
                 "-x | --width         Width of Stream (in pixels)\n"
                 "-y | --height        Height of Stream (in pixels)\n"
                 "-n | --numerator     Set FPS (Frames-per-second) Numerator (default is 30000)\n"
                 "-e | --denominator   Set FPS (Frames-per-second) Denominator (default is 1001)\n"
                 "-i | --threaded      Set threading to be enabled for image processing\n"
                 "-v | --video name    Set name of NDI stream (default is Stream)\n"
                 "-q | --queue level   Set the number of frames will be allowed to stay in queue before dropping (default is 1)\n"
                 "-c | --fix csi       Fixes high CPU usage when using HDMI to CSI adapters (threaded mode only)\n"
                 "",
                 argv[0], dev_name);
}

static const char short_options[] = "d:hfumx:y:n:e:iv:q:c";

static const struct option
long_options[] = {
        { "device", required_argument, NULL, 'd' },
        { "help",   no_argument,       NULL, 'h' },
        { "yuyv", no_argument,       NULL, 'f' },
        { "uyvy", no_argument,       NULL, 'u' },
        { "nv12", no_argument,       NULL, 'm' },
        { "width", required_argument,       NULL, 'x' },
        { "height", required_argument,       NULL, 'y' },
        { "numerator", required_argument,    NULL, 'n' },
        { "denominator", required_argument,  NULL, 'e' },
        { "threaded", no_argument,       NULL, 'i' },
        { "video", required_argument,  NULL, 'v' },
        { "queue", required_argument,  NULL, 'q' },
        { "fix", no_argument,       NULL, 'c' },
        { 0, 0, 0, 0 }
};

int main(int argc, char **argv){
  dev_name = (char*)"/dev/video0"; //default v4l2 device path
  ndi_name = (char*)"Stream"; //default NDI stream name
  for (;;) {
   int idx;
   int c;
   c = getopt_long(argc, argv,short_options, long_options, &idx);
   if (-1 == c){
    break;
   }
   switch(c){
    case 'd':
     dev_name = optarg;
     break;
    case 'h':
     usage(stdout, argc, argv);
     exit(EXIT_SUCCESS);
    case 'f':
     force_yuyv = 1;
     break;
    case 'u':
     force_uyvy = 1;
     break; 
    case 'm':
     force_nv12 = 1;
     break; 
    case 'x':
     width = atoi(optarg);
     break; 
    case 'y':
     height = atoi(optarg);
     break; 
    case 'n':
     fps_N = atof(optarg);
     break;
    case 'e':
     fps_D = atof(optarg);
     break;   
    case 'i':
     image_threaded = 1;
     break; 
    case 'v':
     ndi_name = optarg;
     break; 
    case 'q': //set queue limit - default to 1
     m_max_depth = atoi(optarg);
     break;   
    case 'c': //set HDMI to CSI fix - applies a memcpy to the frame before going to the NDI stack. Not sure why this fixes the issue
     fix_csi = 1;
     break;            
    default:
     usage(stderr, argc, argv);
     exit(EXIT_FAILURE);
   }
  }
  open_device(dev_name, fd); //open v4l2 device
  if(force_uyvy == 1){
   init_device(dev_name, fd, V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_PIX_FMT_UYVY, width, height); //init v4l2 device
  }
  if(force_yuyv == 1){
   init_device(dev_name, fd, V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_PIX_FMT_YUYV, width, height); //init v4l2 device 
  }
  if(force_nv12 == 1){
   init_device(dev_name, fd, V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_PIX_FMT_NV12, width, height); //init v4l2 device 
  }
  init_mmap(dev_name,fd,V4L2_BUF_TYPE_VIDEO_CAPTURE,&buffers,&n_buffers);

  if (image_threaded == 1){
    fprintf(stderr, "Image Processing Threading Enabled\n");
    image_thread = std::thread(&process_image_thread); //start image processing thread
  }
  fprintf(stderr, "Current Queue Level %u\n",m_max_depth); 
  start_capturing(dev_name, fd, V4L2_BUF_TYPE_VIDEO_CAPTURE, n_buffers);
  mainloop();
  stop_capturing(fd);

  if (image_threaded == 1){
    // Signal the image thread to exit by sending an empty message
    auto nullmsg = std::make_unique<v4l2_buffer>();
    queue_push(std::move(nullmsg));
    image_thread.join();
  }

  uninit_device();
  close_device();
  fprintf(stderr, "\n");
  return 0;
}