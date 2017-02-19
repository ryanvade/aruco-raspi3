#include <cstdio>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include "CameraCallibrator.h"

CameraCallibrator::CameraCallibrator(int cameraId, bool flipVertical) {
  this->cameraId = (cameraId >= -1) ? cameraId : -1;
  this->flipVertical = flipVertical;
  this->chessBoardSize.height = 4;
  this->chessBoardSize.width = 4;
}

CameraCallibrator::CameraCallibrator() {
  this->cameraId = -1;
  this->chessBoardSize.height = 4;
  this->chessBoardSize.width = 4;
}

double CameraCallibrator::callibrate() {
  bool found;
  int flags = 0;
  cv::Size frameSize;
  std::vector<cv::Point2f> buffer;
  std::vector<cv::Mat> rotVects, transVects;
  std::vector<std::vector<cv::Point2f>> imagePoints;
  cv::Mat frame, grayFrame, cameraMatrix, distortionCoeffs;
  if (!this->openCamera())
    return -1;
  while (imagePoints.size() < this->numberFrames) {
    frame = this->cameraInput.grab();
    frameSize = frame.size();
    if (this->flipVertical)
      cv::flip(frame, frame, 0);

    found = cv::findChessboardCorners(frame, this->chessBoardSize, buffer,
                                      this->chessBoardFlags);
    if (found) {
      cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
      cv::cornerSubPix(
          grayFrame, buffer, cv::Size(11, 11), cv::Size(-1, -1),
          cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30,
                           0.1));
    }

    imagePoints.push_back(buffer);
    cv::drawChessboardCorners(frame, this->chessBoardSize, cv::Mat(buffer),
                              found);
  }
  cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
  distortionCoeffs = cv::Mat::zeros(8, 1, CV_64F);
  std::vector<std::vector<cv::Point3f>> objectPoints(1);
  for (size_t i = 0; i < (size_t)this->chessBoardSize.height; i++) {
    for (size_t j = 0; j < (size_t)this->chessBoardSize.width; j++) {
      objectPoints[0].push_back(cv::Point3f((2 * j + i % 2) * this->squareSize,
                                            i * this->squareSize, 0));
    }
  }

  objectPoints.resize(imagePoints.size(), objectPoints[0]);
  return cv::calibrateCamera(objectPoints, imagePoints, frameSize, rotVects,
                             transVects, cameraMatrix, distortionCoeffs, flags);
}

cv::Mat CameraCallibrator::getImage() {
  cv::Mat image;
  if (!this->cameraInput.isOpened()) {
    if (!this->cameraInput.open(this->cameraId))
      return image;
  }
  this->cameraInput >> image;
  return image;
}

bool CameraCallibrator::openCamera() {
  if (this->cameraInput.isOpened())
    return true;
  return this->cameraInput.open(this->cameraId);
}
