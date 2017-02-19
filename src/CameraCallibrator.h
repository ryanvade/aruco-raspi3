#ifndef CAMERA_CALLIBRATOR_H
#define CAMERA_CALIBFRATOR_H

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

class CameraCallibrator {
public:
  explicit CameraCallibrator(int cameraId, bool flipVertical);
  CameraCallibrator();
  double callibrate();
  bool openCamera();

private:
  int cameraId;
  size_t numberFrames = 10;
  const char ESC_KEY = 27;
  bool flipVertical = false;
  cv::VideoCapture cameraInput;
  cv::Size chessBoardSize;
  int squareSize;
  int chessBoardFlags = cv::CALIB_CB_ADAPTIVE_THRESH |
                        cv::CALIB_CB_NORMALIZE_IMAGE | cv::CALIB_CB_FAST_CHECK;

  cv::Mat getImage();
};
#endif
