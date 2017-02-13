#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <cstdio>

#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include "CameraCallibrator.h"


CameraCallibrator::CameraCallibrator(int cameraId)
{
  this->cameraId = (cameraId >= -1)? cameraId: -1;
}

double CameraCallibrator::callibrate()
{
  std::vector<cv::Point> stdDeviationsIntristics, stdDeviationsExtrinstics, perViewErrors;
  std::vector<std::vector<cv::Vec3f>> objectPoints, imagePoints;
  cv::Mat cameraMatrix, distCoefs, rvecs, tvecs;
  cv::Size imageSize;
  int flags = 0;
  return 0.0;
}

cv::Mat CameraCallibrator::getImage()
{
  cv::Mat image;
  if(!this->cameraInput.isOpened())
  {
    if(!this->cameraInput.open(this->cameraId)) return image;
  }
  this->cameraInput >> image;
  return image;
}
