#include "hw/buffer.h"
#include "hw/buffer_list.h"
#include "hw/device.h"
#include "hw/links.h"
#include "hw/v4l2.h"
#include "http/http.h"
#include "opts/opts.h"
#include "camera.h"

#include <signal.h>

http_method_t http_methods[] = {
  { "GET / ", http_index },
  { "GET /snapshot ", http_snapshot },
  { "GET /stream ", http_stream },
  { "GET /?action=snapshot ", http_snapshot },
  { "GET /?action=stream ", http_stream },
  { "GET /video ", http_video_html },
  { "GET /video.h264 ", http_video },
  { "GET /jmuxer.min.js ", http_jmuxer_js },
  { NULL, NULL }
};

camera_options_t camera_options = {
  .path = "/dev/video0",
  .width = 1920,
  .height = 1080,
  .format = 0,
  .nbufs = 4,
  .fps = 30,
  .allow_dma = true,
  .high_res_factor = 1.0,
  .low_res_factor = 0.0,
};

http_server_options_t http_options = {
  .port = 9092,
  .maxcons = 10
};

log_options_t log_options = {
  .debug = false,
  .verbose = false
};

option_value_t camera_formats[] = {
  { "DEFAULT", 0 },
  { "YUYV", V4L2_PIX_FMT_YUYV },
  { "MJPEG", V4L2_PIX_FMT_MJPEG },
  { "H264", V4L2_PIX_FMT_H264 },
  { "RG10", V4L2_PIX_FMT_SRGGB10P },
  {}
};

option_t all_options[] = {
  DEFINE_OPTION_PTR(camera, path, string),
  DEFINE_OPTION(camera, width, uint),
  DEFINE_OPTION(camera, height, uint),
  DEFINE_OPTION_VALUES(camera, format, camera_formats),
  DEFINE_OPTION(camera, nbufs, uint),
  DEFINE_OPTION(camera, fps, uint),
  DEFINE_OPTION_DEFAULT(camera, allow_dma, bool, "1"),
  DEFINE_OPTION(camera, high_res_factor, float),
  DEFINE_OPTION(camera, low_res_factor, float),
  DEFINE_OPTION(http, port, uint),
  DEFINE_OPTION(http, maxcons, uint),
  DEFINE_OPTION_DEFAULT(log, debug, bool, "1"),
  DEFINE_OPTION_DEFAULT(log, verbose, bool, "1"),
  {}
};

int main(int argc, char *argv[])
{
  int http_fd = -1;
  int ret = -1;
  const char *env;
  camera_t *camera;

  if (parse_opts(all_options, argc, argv) < 0) {
    return -1;
  }

  camera = camera_open(&camera_options);
  if (!camera) {
    goto error;
  }

  http_fd = http_server(&http_options, http_methods);
  if (http_fd < 0) {
    goto error;
  }

  ret = camera_run(camera);

error:
  close(http_fd);
  camera_close(camera);
  return ret;
}