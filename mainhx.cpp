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
//#include <Processing.NDI.Lib.h>
#include <Processing.NDI.Advanced.h>
#include <PixelFormatConverter.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))
#define FMT_NUM_PLANES 1

//Full NDI
NDIlib_send_create_t NDI_full_send_create_desc;
NDIlib_send_instance_t pNDI_full_send;
NDIlib_video_frame_v2_t NDI_full_video_frame;

//NDI HX

NDIlib_send_create_t NDI_hx_send_create_desc;
NDIlib_send_instance_t pNDI_hx_send;
NDIlib_video_frame_v2_t NDI_hx_video_frame;


zs::PixelFormatConverter converter;
zs::Frame yuy2Frame;
zs::Frame uyvyFrame;

enum io_method {
        IO_METHOD_READ,
        IO_METHOD_MMAP,
        IO_METHOD_USERPTR,
};

struct buffer {
        void    *start;
        size_t  length;
};

struct buffer_mp {
    void   *start[FMT_NUM_PLANES];
    size_t  length[FMT_NUM_PLANES];
};

static char            *dev_name;
static char            *encoder_name;
static enum io_method   io = IO_METHOD_MMAP; 

static FILE            *out_fp;

static unsigned int     n_buffers;
static unsigned int     n_buffers_mp;
static unsigned int     n_buffers_mp_out;
static int              out_buf;
static int              force_yuyv = 0;
static int              force_uyvy = 0;
static int              width = 1920;
static int              height = 1080;
static float            fps_N = 30000;
static float            fps_D = 1001;
static char             *ndi_name;

static int              fd1 = -1; //reading device
static int              fd2 = -1; //encoder device
int                     m_width = 0;
int                     m_height = 0;
int                     m_format = 0;
//Buffers
struct buffer          *buffers;
struct buffer          output;
struct buffer          capture;

struct buffer_mp       *buffers_mp;
struct buffer_mp       *buffers_mp_out;

static void errno_exit(const char *s){
  fprintf(stderr, "%s error %d, %s\\n", s, errno, strerror(errno));
  exit(EXIT_FAILURE);
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
    fprintf(stderr, "%s is no V4L2 device\\n",d_name);
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
  if (d_width != 0) {
		fmt.fmt.pix.width = d_width;
	}
	if (d_height != 0) {
		fmt.fmt.pix.height = d_height;
	}
	if (d_format != 0) {
		fmt.fmt.pix.pixelformat = d_format;
	}

  m_width = fmt.fmt.pix.width;
  m_height = fmt.fmt.pix.height;  
  m_format = fmt.fmt.pix.pixelformat;
  
  if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt)){
   errno_exit("VIDIOC_S_FMT");
  }
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

static void init_mmap_mp(const char *device_name, int &fd, enum v4l2_buf_type type, struct buffer_mp **bufs_out, unsigned int *n_bufs){  //initialize buffer for v4l2 mp
  struct v4l2_requestbuffers req;
  struct buffer_mp *bufs;
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

  bufs = (buffer_mp*)calloc(req.count, sizeof(*bufs));

  if(!bufs){
   fprintf(stderr, "Out of memory\\n");
   exit(EXIT_FAILURE);
  }

  for(b = 0; b < req.count; ++b){
   struct v4l2_buffer buf;
   struct v4l2_plane  planes[FMT_NUM_PLANES];
   unsigned int p;
   CLEAR(buf);
   buf.type        = type;
   buf.memory      = V4L2_MEMORY_MMAP;
   buf.index       = b;
   buf.length   = FMT_NUM_PLANES;
   buf.m.planes = planes;
   if(-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf)){
    errno_exit("VIDIOC_QUERYBUF");
   }
   fprintf(stderr, "Mapping %s buffer %u, len %u\n",device_name, b, buf.length);
   for (p = 0; p < FMT_NUM_PLANES; ++p) {
    fprintf(stderr, "Mapping plane %u, len %u\n", p, buf.m.planes[p].length);
    bufs[b].length[p] = buf.m.planes[p].length;
    bufs[b].start[p] = mmap(NULL, buf.m.planes[p].length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.planes[p].m.mem_offset);
    if (MAP_FAILED == bufs[b].start[p])
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

static void start_capturing_mp(const char *device_name, int &fd, enum v4l2_buf_type type, unsigned int n_bufs){ //start capturing
  unsigned int i;
  for (i = 0; i < n_bufs; ++i) {
   struct v4l2_buffer buf;
   struct v4l2_plane  planes[FMT_NUM_PLANES];
   CLEAR(buf);
   CLEAR(planes);
   buf.type = type;
   buf.memory = V4L2_MEMORY_MMAP;
   buf.index = i;
   buf.length   = FMT_NUM_PLANES;
   buf.m.planes = planes;
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

static int write_frame_mp(int &fd, enum v4l2_buf_type type, struct buffer_mp *bufs, unsigned int n_buffs, void *in_buf, size_t in_bytesused){ //this function write the frame to video output device
  struct v4l2_buffer buf;
  struct v4l2_plane  planes[FMT_NUM_PLANES];
  buf.type = type;
  buf.memory = V4L2_MEMORY_MMAP;
  buf.length   = FMT_NUM_PLANES;
  buf.m.planes = planes;
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
  unsigned int p;
  for (p = 0; p < FMT_NUM_PLANES; ++p){
   memcpy(bufs[buf.index].start[p], in_buf, in_bytesused); //possibly an expensive CPU operation
  }
  buf.bytesused = in_bytesused;

  if(-1 == xioctl(fd, VIDIOC_QBUF, &buf)){
   errno_exit("VIDIOC_QBUF");
  }
  return 1;
}

static void process_image(const void *p, int size){
 if(force_yuyv == 1){ //if this is enabled - convert from YUY2 to UYVY for NDI
  yuy2Frame.data = (uint8_t*)p;
  if(!converter.Convert(yuy2Frame,uyvyFrame)){ //convert the YUY2 frame into a UYVY frame - NDI doesn't accept a YUY2 frame
   fprintf(stderr, "Convert failed");       
  }
  NDI_full_video_frame.p_data = uyvyFrame.data; //link the UYVY frame data to the NDI frame
 }else{
  NDI_full_video_frame.p_data = (uint8_t*)p; //link the UYVY frame data to the NDI frame 
 }
 NDIlib_send_send_video_v2(pNDI_full_send, &NDI_full_video_frame); //send the data out to NDI

 int r1;
  struct timeval tv1;
  tv1.tv_sec = 1;
  tv1.tv_usec = 0;

  r1 = is_writable(fd2, &tv1); //see when encoder is writeable
  if (-1 == r1) {
     if (EINTR == errno){
     }
     errno_exit("select");
    }
    if(0 == r1){
     fprintf(stderr, "select timeout writing\n");
     exit(EXIT_FAILURE);
    }
  if(r1 == 1){
    //fprintf(stderr, "writing to buf\n");
    write_frame_mp(fd2, V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE, buffers_mp_out, n_buffers_mp_out, (char*)p, size);
  }
}

static void process_image_mp(const void *p, int size){
  //fprintf(stderr, "writing to file\n");
  //if (!out_fp){
   //out_fp = fopen("test.h264", "wb");
  //}
  //fwrite(p, size, 1, out_fp);
  
  // This is probably zero for non I-frames, but MUST be set of I-frames
  uint8_t* p_h264_extra_data = 0;
  uint32_t h264_extra_data_size = 0;
  // Compute the total size of the structure
  uint32_t packet_size = sizeof(NDIlib_compressed_packet_t) + size + h264_extra_data_size;
  std::cout << "P: " << p << std::endl;
  // Allocate the structure
  NDIlib_compressed_packet_t* p_packet = (NDIlib_compressed_packet_t*)malloc(packet_size);

  p_packet->version = sizeof(NDIlib_compressed_packet_t);
  p_packet->fourCC = NDIlib_FourCC_type_H264;
  p_packet->data_size = size;
  p_packet->extra_data_size = h264_extra_data_size;
  // Compute the pointer to the compressed h264 data, then copy the memory into place.
  //uint8_t* p_dst_h264_data = (uint8_t*)(p_packet + p_packet->version);
  uint8_t* p_frame_data = (uint8_t*)(p_packet + p_packet->version);
  memcpy(p_frame_data, (uint8_t*)p, size);
  std::cout << "Pointer: " << (p_packet + p_packet->version) << std::endl;
  NDI_hx_video_frame.p_data = (uint8_t*)p_packet;
  NDI_hx_video_frame.data_size_in_bytes = packet_size;
  NDIlib_send_send_video_v2(pNDI_hx_send, &NDI_hx_video_frame); //send the data out to NDI
}

static int read_frame(int &fd, enum v4l2_buf_type type, struct buffer *bufs, unsigned int n_buffs){ //this function reads the frame from the video capture device
  struct v4l2_buffer buf;
  CLEAR(buf);
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
   process_image(bufs[buf.index].start, buf.bytesused); //send the mmap frame buffer off to be processed
  }

  
  if(-1 == xioctl(fd, VIDIOC_QBUF, &buf)){
   errno_exit("VIDIOC_QBUF");
  }
  return 1;
}


static int read_frame_mp(int &fd, enum v4l2_buf_type type, struct buffer_mp *bufs, unsigned int n_buffs){ //this function reads the frame from the video capture device
  struct v4l2_buffer buf;
  struct v4l2_plane  planes[FMT_NUM_PLANES];
  CLEAR(buf);
  CLEAR(planes);
  buf.type = type;
  buf.memory = V4L2_MEMORY_MMAP;
  buf.length   = FMT_NUM_PLANES;
  buf.m.planes = planes;
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
  unsigned int p;
  for (p = 0; p < FMT_NUM_PLANES; ++p){
   process_image_mp(bufs[buf.index].start[p], bufs[buf.index].length[p]); //send the mmap frame buffer off to be processed
  }
  if(-1 == xioctl(fd, VIDIOC_QBUF, &buf)){
   errno_exit("VIDIOC_QBUF");
  }
  return 1;
}

static int mainloop(int fd){
  if (!NDIlib_initialize()){	// Cannot run NDI. Most likely because the CPU is not sufficient (see SDK documentation).
   fprintf(stderr, "CPU cannot run NDI");
   return 0;
  } 
  //Full NDI     
  NDI_full_send_create_desc.p_ndi_name = ndi_name;
  pNDI_full_send = NDIlib_send_create_v2(&NDI_full_send_create_desc); 
  if (!pNDI_full_send){
   fprintf(stderr, "Failed to create NDI Full Send");
   exit(1);
  }

  yuy2Frame = zs::Frame(width,height, MAKE_FOURCC_CODE('Y','U','Y','2')); //initialize conversion frame storage - YUY2
  uyvyFrame.fourcc = MAKE_FOURCC_CODE('U','Y','V','Y'); //initialize conversion frame storage - UYVY
  NDI_full_video_frame.xres = width;
  NDI_full_video_frame.yres = height;
  NDI_full_video_frame.frame_rate_N = fps_N;
  NDI_full_video_frame.frame_rate_D = fps_D;
  NDI_full_video_frame.FourCC = NDIlib_FourCC_type_UYVY; //set NDI to receive the type of frame that is going to be given to it - in this case UYVY
  
  //NDI HX
  NDI_hx_send_create_desc.p_ndi_name = "NDIHX";
  NDI_hx_send_create_desc.clock_video = true;
  pNDI_hx_send = NDIlib_send_create_v2(&NDI_hx_send_create_desc); 
  if (!pNDI_hx_send){
   fprintf(stderr, "Failed to create NDI HX Send");
   exit(1);
  }

  NDI_hx_video_frame.xres = width;
  NDI_hx_video_frame.yres = height;
  NDI_hx_video_frame.frame_rate_N = fps_N;
  NDI_hx_video_frame.frame_rate_D = fps_D;
  NDI_hx_video_frame.FourCC = (NDIlib_FourCC_video_type_e)NDIlib_FourCC_type_H264_highest_bandwidth;
  NDI_hx_video_frame.picture_aspect_ratio = 16.0f/9.0f;
  NDI_hx_video_frame.frame_format_type = NDIlib_frame_format_type_progressive;

  while(1){ //while loop for querying for new data from video capture device and reading new frames
   for(;;){

    struct timeval tv1;
    struct timeval tv2;
    int r1;
    int r2;
    tv1.tv_sec = 1;
    tv1.tv_usec = 0;
    tv2.tv_sec = 1;
    tv2.tv_usec = 0;

    r1 = is_readable(fd1, &tv1); //see when v4l2 video capture is ready
    r2 = is_readable(fd2, &tv2); //see when encoder is ready

    if (-1 == r1) {
     if (EINTR == errno){
      continue;
     }
     errno_exit("select");
    }
    if(0 == r1){
     fprintf(stderr, "select timeout\n");
     exit(EXIT_FAILURE);
    }

    if (-1 == r2) {
     if (EINTR == errno){
      continue;
     }
     errno_exit("select");
    }
    if(0 == r2){
     fprintf(stderr, "select timeout\n");
     //exit(EXIT_FAILURE);
    }

    if(r1 == 1){
     read_frame(fd, V4L2_BUF_TYPE_VIDEO_CAPTURE, buffers, n_buffers); //read new frame 
    }

    if(r2 == 1){
     //fprintf(stderr, "reading buf\n"); 
     read_frame_mp(fd2, V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE, buffers_mp, n_buffers_mp);
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
  
  //fd = -1;
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
                 "-v | --video name    Set name of NDI stream (default is Stream)\n"
                 "",
                 argv[0], dev_name);
}

static const char short_options[] = "d:hfux:y:n:e:v:";

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
        { "video", required_argument,  NULL, 'v' },
        { 0, 0, 0, 0 }
};

int main(int argc, char **argv){
  dev_name = (char*)"/dev/video0"; //default v4l2 device path
  encoder_name = (char*)"/dev/video11"; //default encoder device path
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
    case 'v':
     ndi_name = optarg;
     break;              
    default:
     usage(stderr, argc, argv);
     exit(EXIT_FAILURE);
   }
  }
  open_device(dev_name,fd1); //open v4l2 device 
  open_device(encoder_name,fd2); //open v4l2 device - encoder

  init_device(dev_name,fd1,V4L2_BUF_TYPE_VIDEO_CAPTURE,V4L2_PIX_FMT_UYVY,1920,1080); //init v4l2 device
  init_device(encoder_name,fd2,V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE,V4L2_PIX_FMT_UYVY,1920,1080); //init encoder input (output)
  init_device(encoder_name,fd2,V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE,V4L2_PIX_FMT_H264,1920,1080); //init encoder output (capture)

  init_mmap(dev_name,fd1,V4L2_BUF_TYPE_VIDEO_CAPTURE,&buffers,&n_buffers);
  init_mmap_mp(encoder_name,fd2,V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE,&buffers_mp_out,&n_buffers_mp_out);
  init_mmap_mp(encoder_name,fd2,V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE,&buffers_mp,&n_buffers_mp);
  
  start_capturing(dev_name, fd1, V4L2_BUF_TYPE_VIDEO_CAPTURE, n_buffers);
  start_capturing_mp(encoder_name, fd2, V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE, n_buffers_mp_out);
  start_capturing_mp(encoder_name, fd2, V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE, n_buffers_mp);

  mainloop(fd1);
  stop_capturing(fd1);
  uninit_device();
  close_device();
  fprintf(stderr, "\n");
  return 0;
}