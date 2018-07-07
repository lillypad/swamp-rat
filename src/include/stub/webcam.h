/*-----------------------------------------------------------------------\
| Copyright (C) 2018  Lilly Chalupowski                                  |
|                                                                        |
| This program is free software: you can redistribute it and/or modify   |
| it under the terms of the GNU General Public License as published by   |
| the Free Software Foundation, either version 3 of the License, or      |
| (at your option) any later version.                                    |
|                                                                        |
| This program is distributed in the hope that it will be useful,        |
| but WITHOUT ANY WARRANTY; without even the implied warranty of         |
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          |
| GNU General Public License for more details.                           |
|                                                                        |
| You should have received a copy of the GNU General Public License      |
| along with this program.  If not, see <https://www.gnu.org/licenses/>. |
\-----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

int webcam_device_handle(){
  int webcam_fd;
  webcam_fd = open("/dev/video0", O_RDWR);
  if (webcam_fd < 0){
    fprintf(stderr, "error: unable to read video device");
    return -1;
  }
  return webcam_fd;
}

struct v4l2_capability *webcam_get_info(int webcam_fd){
  struct v4l2_capability *p_webcam_cap = malloc(sizeof(struct v4l2_capability));
  ioctl(webcam_fd, VIDIOC_QUERYCAP, p_webcam_cap);
  if (!(p_webcam_cap->capabilities & V4L2_CAP_VIDEO_CAPTURE)){
    fprintf(stderr, "error: webcam does not support capture\n");
    return NULL;
  } else{
    return p_webcam_cap;
  }
}

struct v4l2_format *webcam_set_format(int webcam_fd, void *p_webcam_cap){
  struct v4l2_capability *p_webcam_cap_in = p_webcam_cap;
  struct v4l2_format *p_webcam_format = malloc(sizeof(struct v4l2_format));
  p_webcam_format->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  p_webcam_format->fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
  ioctl(webcam_fd, VIDIOC_S_FMT, p_webcam_format);
  return p_webcam_format;
}
