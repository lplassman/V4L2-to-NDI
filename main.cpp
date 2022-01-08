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
NDIlib_video_frame_v2_t NDI_video_frame2;

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
static int              width = 0;
static int              height = 0;
static float            fps_N = 30000;
static float            fps_D = 1001;
static char             *ndi_name;
int                     m_width = 0;
int                     m_height = 0;
int                     m_format = 0;
int                     frame_buffer = 0;
uint8_t*                p_frame1;
uint8_t*                p_frame2;
int                     ndi_async = 0;
int                     image_threaded = 0;

//Buffers
struct buffer          *buffers;

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
  req.count = 4;
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

  bufs = (buffer*)calloc(req.count, sizeof(*bufs));

  if(!bufs){
   fprintf(stderr, "Out of memory\\n");
   exit(EXIT_FAILURE);
  }

  for(b = 0; b < req.count; ++b){
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

static void process_image_async(const void *p, int size){
 frame_buffer = 1 - frame_buffer; 
 //std::cout << "Frame size: " << size << std::endl; 
 if(frame_buffer == 0){
  p_frame1 = (uint8_t*)malloc(size);
  memcpy(p_frame1, (uint8_t*)p, size);
  if(force_yuyv == 1){ //if this is enabled - convert from YUY2 to UYVY for NDI
   yuy2Frame.data = p_frame1;
   if(!converter.Convert(yuy2Frame,uyvyFrame)){ //convert the YUY2 frame into a UYVY frame - NDI doesn't accept a YUY2 frame
    fprintf(stderr, "Convert failed\n");       
   }
   NDI_video_frame1.p_data = uyvyFrame.data; //link the UYVY frame data to the NDI frame
  }else{
   NDI_video_frame1.p_data = p_frame1; //link the UYVY frame data to the NDI frame 
  }
  NDIlib_send_send_video_async_v2(pNDI_full_send, &NDI_video_frame1); //send the data out to NDI
  free(p_frame2);
 }
 if(frame_buffer == 1){
  p_frame2 = (uint8_t*)malloc(size);
  memcpy(p_frame2, (uint8_t*)p, size);
  if(force_yuyv == 1){ //if this is enabled - convert from YUY2 to UYVY for NDI
   yuy2Frame.data = p_frame2;
   if(!converter.Convert(yuy2Frame,uyvyFrame)){ //convert the YUY2 frame into a UYVY frame - NDI doesn't accept a YUY2 frame
    fprintf(stderr, "Convert failed\n");       
   }
   NDI_video_frame2.p_data = uyvyFrame.data; //link the UYVY frame data to the NDI frame
  }else{
   NDI_video_frame2.p_data = p_frame2; //link the UYVY frame data to the NDI frame 
  }
  NDIlib_send_send_video_async_v2(pNDI_full_send, &NDI_video_frame2); //send the data out to NDI
  free(p_frame1);
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
}

static int read_frame(int &fd, enum v4l2_buf_type type, struct buffer *bufs, unsigned int n_buffs){ //this function reads the frame from the video capture device
  struct v4l2_buffer buf;
  //CLEAR(buf);
  buf.type = type;
  buf.memory = V4L2_MEMORY_MMAP;
  if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) { //dequeue the buffer - dumps data into the previously set mmap
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
  assert(buf.index < n_buffs);
  
  if(NDIlib_send_get_no_connections(pNDI_full_send, 10000)){ //wait for a NDI receiver to be present before continuing - no need to encode without a client connected
   if(ndi_async == 1){
    process_image_async(bufs[buf.index].start, buf.bytesused); //send the mmap frame buffer off to be processed
   }else{
    if(image_threaded == 1){
     std::thread image_process(process_image, bufs[buf.index].start, buf.bytesused); //send the mmap frame buffer off to be processed 
     image_process.join();
    }else{
     process_image(bufs[buf.index].start, buf.bytesused); //send the mmap frame buffer off to be processed 
    }
   }
  }

  
  if(-1 == xioctl(fd, VIDIOC_QBUF, &buf)){
   errno_exit("VIDIOC_QBUF");
  }
  return 1;
}

static int mainloop(void){
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
  NDI_video_frame1.FourCC = NDIlib_FourCC_type_UYVY; //set NDI to receive the type of frame that is going to be given to it - in this case UYVY
  
  if(ndi_async == 1){ //initialize frame2 for async
   NDI_video_frame2.xres = m_width; 
   NDI_video_frame2.yres = m_height;
   NDI_video_frame2.frame_rate_N = fps_N;
   NDI_video_frame2.frame_rate_D = fps_D;
   NDI_video_frame2.FourCC = NDIlib_FourCC_type_UYVY; //set NDI to receive the type of frame that is going to be given to it - in this case UYVY
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
                 "Version 1.0\n"
                 "Options:\n"
                 "-d | --device name   Video device name [%s]\n"
                 "-h | --help          Print this message\n"
                 "-f | --yuyv          Force pixel format to YUYV\n"
                 "-u | --uyvy          Force pixel format to UYVY\n"
                 "-x | --width         Width of Stream (in pixels)\n"
                 "-y | --height        Height of Stream (in pixels)\n"
                 "-n | --numerator     Set FPS (Frames-per-second) Numerator (default is 30000)\n"
                 "-e | --denominator   Set FPS (Frames-per-second) Denominator (default is 1001)\n"
                 "-i | --threaded      Set threading to be enabled for image processing\n"
                 "-a | --async         Set async to be enabled for NDI stream (default is disabled)\n"
                 "-v | --video name    Set name of NDI stream (default is Stream)\n"
                 "",
                 argv[0], dev_name);
}

static const char short_options[] = "d:hfux:y:n:e:iav:";

static const struct option
long_options[] = {
        { "device", required_argument, NULL, 'd' },
        { "help",   no_argument,       NULL, 'h' },
        { "yuyv", no_argument,       NULL, 'f' },
        { "uyvy", no_argument,       NULL, 'u' },
        { "width", required_argument,       NULL, 'x' },
        { "height", required_argument,       NULL, 'y' },
        { "numerator", required_argument,    NULL, 'n' },
        { "denominator", required_argument,  NULL, 'e' },
        { "threaded", no_argument,       NULL, 'i' },
        { "async", no_argument,       NULL, 'a' },
        { "video", required_argument,  NULL, 'v' },
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
    case 'a':
     ndi_async = 1;
     break; 
    case 'v':
     ndi_name = optarg;
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
  init_mmap(dev_name,fd,V4L2_BUF_TYPE_VIDEO_CAPTURE,&buffers,&n_buffers);
  start_capturing(dev_name, fd, V4L2_BUF_TYPE_VIDEO_CAPTURE, n_buffers);
  mainloop();
  stop_capturing(fd);
  uninit_device();
  close_device();
  fprintf(stderr, "\n");
  return 0;
}