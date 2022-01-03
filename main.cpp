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

NDIlib_send_create_t NDI_send_create_desc;
NDIlib_send_instance_t pNDI_send;
NDIlib_video_frame_v2_t NDI_video_frame;

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
struct buffer          *buffers;
static unsigned int     n_buffers;
static int              out_buf;
static int              force_yuyv = 0;
static int              force_uyvy = 0;
static int              width = 0;
static int              height = 0;
static float            fps_N = 30000;
static float            fps_D = 1001;
static char             *ndi_name;

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

static void process_image(const void *p, int size){
 if(force_yuyv == 1){ //if this is enabled - convert from YUY2 to UYVY for NDI
  yuy2Frame.data = (uint8_t*)p;
  if(!converter.Convert(yuy2Frame,uyvyFrame)){ //convert the YUY2 frame into a UYVY frame - NDI doesn't accept a YUY2 frame
   fprintf(stderr, "Convert failed");       
  }
  NDI_video_frame.p_data = uyvyFrame.data; //link the UYVY frame data to the NDI frame
 }else{
  NDI_video_frame.p_data = (uint8_t*)p; //link the UYVY frame data to the NDI frame 
 }
 NDIlib_send_send_video_v2(pNDI_send, &NDI_video_frame); //send the data out to NDI
}

static int read_frame(void){ //this function reads the frame from the video capture device
  struct v4l2_buffer buf;
  CLEAR(buf);
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;
  if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
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
  assert(buf.index < n_buffers);
  if(NDIlib_send_get_no_connections(pNDI_send, 10000)){ //wait for a NDI receiver to be present before continuing - no need to encode without a client connected
   process_image(buffers[buf.index].start, buf.bytesused); //send the mmap frame buffer off to be processed
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
  NDI_send_create_desc.p_ndi_name = ndi_name;
  pNDI_send = NDIlib_send_create(&NDI_send_create_desc); 
  if (!pNDI_send){
   fprintf(stderr, "Failed to create NDI Send");
   exit(1);
  }
  yuy2Frame = zs::Frame(width,height, MAKE_FOURCC_CODE('Y','U','Y','2')); //initialize conversion frame storage - YUY2
  uyvyFrame.fourcc = MAKE_FOURCC_CODE('U','Y','V','Y'); //initialize conversion frame storage - UYVY
  NDI_video_frame.xres = width;
  NDI_video_frame.yres = height;
  NDI_video_frame.frame_rate_N = fps_N;
  NDI_video_frame.frame_rate_D = fps_D;
  NDI_video_frame.FourCC = NDIlib_FourCC_type_UYVY; //set NDI to receive the type of frame that is going to be given to it - in this case UYVY
  
  while(1){ //while loop for querying for new data from video capture device and reading new frames
   for(;;){
    fd_set fds;
    struct timeval tv;
    int r;
    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    /* Timeout. */
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    r = select(fd + 1, &fds, NULL, NULL, &tv);
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
    if(read_frame()){ //read new frame
     break;
    }
    /* EAGAIN - continue select loop. */
  }
 }
}

static void stop_capturing(void){
  enum v4l2_buf_type type;
  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type)){
   errno_exit("VIDIOC_STREAMOFF");
  }
}

static void start_capturing(void){
  unsigned int i;
  enum v4l2_buf_type type;
  for (i = 0; i < n_buffers; ++i) {
   struct v4l2_buffer buf;
   CLEAR(buf);
   buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
   buf.memory = V4L2_MEMORY_MMAP;
   buf.index = i;
   if (-1 == xioctl(fd, VIDIOC_QBUF, &buf)){
    errno_exit("VIDIOC_QBUF");
   }
  }
  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (-1 == xioctl(fd, VIDIOC_STREAMON, &type)){
   errno_exit("VIDIOC_STREAMON");
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


static void init_mmap(void){  //initialize buffer
  struct v4l2_requestbuffers req;
  CLEAR(req);
  req.count = 4;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;

  if(-1 == xioctl(fd, VIDIOC_REQBUFS, &req)){
   if (EINVAL == errno) {
    fprintf(stderr, "%s does not support memory mappingn", dev_name);
    exit(EXIT_FAILURE);
   }else{
    errno_exit("VIDIOC_REQBUFS");
   }
  }
  if (req.count < 2) {
   fprintf(stderr, "Insufficient buffer memory on %s\\n",dev_name);
   exit(EXIT_FAILURE);
  }

  buffers = (buffer*)calloc(req.count, sizeof(*buffers));

  if(!buffers){
   fprintf(stderr, "Out of memory\\n");
   exit(EXIT_FAILURE);
  }

  for(n_buffers = 0; n_buffers < req.count; ++n_buffers){
   struct v4l2_buffer buf;
   CLEAR(buf);
   buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
   buf.memory      = V4L2_MEMORY_MMAP;
   buf.index       = n_buffers;
   if(-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf)){
    errno_exit("VIDIOC_QUERYBUF");
   }
   buffers[n_buffers].length = buf.length;
   buffers[n_buffers].start =
   mmap(NULL,buf.length,PROT_READ | PROT_WRITE,MAP_SHARED,fd, buf.m.offset);
   if (MAP_FAILED == buffers[n_buffers].start){
    errno_exit("mmap");
   }
  }
}


static void init_device(void){ //initialize device
  struct v4l2_capability cap;
  struct v4l2_format fmt;
  unsigned int min;
  if(-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)){
   if(EINVAL == errno){
    fprintf(stderr, "%s is no V4L2 device\\n",dev_name);
    exit(EXIT_FAILURE);
   }else{
    errno_exit("VIDIOC_QUERYCAP");
   }
  }
  if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)){
   fprintf(stderr, "%s is no video capture device\n",dev_name);
   exit(EXIT_FAILURE);
  }
  if (!(cap.capabilities & V4L2_CAP_STREAMING)){
   fprintf(stderr, "%s does not support streaming i/o\n",dev_name);
   exit(EXIT_FAILURE);
  }
  CLEAR(fmt);
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  if((width > 0)&&(height > 0)){ //resolution is set manually - set capture card to this resolution
   fmt.fmt.pix.width = width; //set current width and height of the current image
   fmt.fmt.pix.height = height;
  }

  if(force_yuyv == 1){
   fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
   if(-1 == xioctl(fd, VIDIOC_S_FMT, &fmt)){
    errno_exit("VIDIOC_S_FMT");
   }
   /* Note VIDIOC_S_FMT may change width and height. */
  }

  if(force_uyvy == 1){
   fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
   if(-1 == xioctl(fd, VIDIOC_S_FMT, &fmt)){
    errno_exit("VIDIOC_S_FMT");
   }
   /* Note VIDIOC_S_FMT may change width and height. */
  }

  

  if (-1 == xioctl(fd, VIDIOC_G_FMT, &fmt)){
   errno_exit("VIDIOC_G_FMT");
  }
  if((width == 0)&&(height == 0)){
   width = fmt.fmt.pix.width; //store current width and height of the current image
   height = fmt.fmt.pix.height;
  }

  /* Buggy driver paranoia. */
  min = fmt.fmt.pix.width * 2;
  if(fmt.fmt.pix.bytesperline < min){
   fmt.fmt.pix.bytesperline = min;
  }
  min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
  if(fmt.fmt.pix.sizeimage < min){
   fmt.fmt.pix.sizeimage = min;
  }
  init_mmap();
}

static void close_device(void){
  if(-1 == close(fd)){
   errno_exit("close");
  }
  fd = -1;
}

static void open_device(void){ //open the device for reading
  struct stat st;
  if (-1 == stat(dev_name, &st)){
   fprintf(stderr, "Cannot identify '%s': %d, %s\n",dev_name, errno, strerror(errno));
   exit(EXIT_FAILURE);
  }
  if (!S_ISCHR(st.st_mode)){
   fprintf(stderr, "%s is no devicen", dev_name);
   exit(EXIT_FAILURE);
  }
  fd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);
  if (-1 == fd){
   fprintf(stderr, "Cannot open '%s': %d, %s\n",dev_name, errno, strerror(errno));
   exit(EXIT_FAILURE);
  }
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
  open_device(); 
  init_device();
  start_capturing();
  mainloop();
  stop_capturing();
  uninit_device();
  close_device();
  fprintf(stderr, "\n");
  return 0;
}