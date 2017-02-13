#ifndef CAMERA_CALLIBRATOR_H
#define CAMERA_CALIBFRATOR_H

#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <cstdio>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

class CameraCallibrator {
public:
  CameraCallibrator(int cameraId);
  double callibrate();
private:
  int cameraId;
  const char ESC_KEY = 27;
  cv::VideoCapture cameraInput;

  cv::Mat getImage();
};
#endif
