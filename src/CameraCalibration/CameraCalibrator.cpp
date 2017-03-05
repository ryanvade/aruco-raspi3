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

#include "CameraCalibrator.h"

CameraCalibrator::CameraCalibrator(int cameraId, bool flipVertical) {
  this->cameraId = (cameraId >= -1) ? cameraId : -1;
  this->flipVertical = flipVertical;
  this->inputVideo.open(this->cameraId);
}

CameraCalibrator::CameraCalibrator() {
  this->cameraId = -1;
  this->flipVertical = false;
  this->inputVideo.open(this->cameraId);
}

/*
 * Calculate Chess Board Position
 *
 * Determines the chess board Position
 *
 */
void CameraCalibrator::calculateChessBoardPosition() {
  for (size_t i = 0; i < (size_t)this->chessBoardDimensions.height; i++) {
    for (size_t j = 0; j < (size_t)this->chessBoardDimensions.width; j++) {
      // X = j * square size, Y = i * square size, Z = 0.0
      this->chessBoardCorners.push_back(Point3f(j * this->chessBoardSquareSize,
                                                i * this->chessBoardSquareSize,
                                                0.0f));
    }
  }
}

/*
 * Calculate Chess Board Corners
 *
 * Determines all the chess board corner positions
 *
 */
void CameraCalibrator::calculateChessBoardCornersFromImages() {
  for (vector<Mat>::iterator iter = this->images.begin(); iter != images.end();
       iter++) {
    vector<Point2f> pointBuf;
    bool chessBoardFound = findChessboardCorners(
        *iter, this->chessBoardDimensions, pointBuf, this->chessBoardFlags);
    if (chessBoardFound) {
      this->foundChessboardCorners.push_back(pointBuf);
    }
    if (DEBUG) {
      drawChessboardCorners(*iter, this->chessBoardDimensions, pointBuf,
                            chessBoardFound);
      imshow("Found Corners", *iter);
      waitKey(0);
    }
  }
}

/*
 * Get Images From Camera
 *
 * Grab images from a camera and store for calibration
 *
 */

void CameraCalibrator::getImagesFromCamera() {
  Mat frame, drawToFrame, distanceCoefficients,
      cameraMatrix = Mat::eye(3, 3, CV_64F);
  vector<Mat> savedImages;
  vector<vector<Point2f>> markerCorners, rejectedCorners;

  if (!this->inputVideo.isOpened()) {
    return;
  }
  if (DEBUG) {
    namedWindow("Camera", CV_WINDOW_AUTOSIZE);
  }

  while (true) {
    if (!this->inputVideo.read(frame))
      break;

    vector<Vec2f> foundPoints;
    bool found = false;
    found = findChessboardCorners(frame, this->chessBoardDimensions,
                                  foundPoints, this->chessBoardFlags);
    frame.copyTo(drawToFrame);
    if (DEBUG) {
      drawChessboardCorners(drawToFrame, this->chessBoardDimensions,
                            foundPoints, found);
      if (found)
        imshow("Camera", drawToFrame);
      else
        imshow("Camera", frame);

      char c = waitKey(1000 / this->FPS);
    }
  }
}
