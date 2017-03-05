#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

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
void CameraCalibrator::calculateChessBoardPosition(vector<Point3f> &corners) {
  for (size_t i = 0; i < (size_t)this->chessBoardDimensions.height; i++) {
    for (size_t j = 0; j < (size_t)this->chessBoardDimensions.width; j++) {
      // X = j * square size, Y = i * square size, Z = 0.0
      corners.push_back(Point3f(j * this->chessBoardSquareSize,
                                i * this->chessBoardSquareSize, 0.0f));
    }
  }
}

/*
 * Calculate Chess Board Corners
 *
 * Determines all the chess board corner positions
 *
 */
void CameraCalibrator::calculateChessBoardCornersFromImages(
    vector<vector<Point2f>> &foundCorners) {
  for (vector<Mat>::iterator iter = this->images.begin();
       iter != this->images.end(); iter++) {
    vector<Point2f> pointBuf;
    bool chessBoardFound = findChessboardCorners(
        *iter, this->chessBoardDimensions, pointBuf, this->chessBoardFlags);
    if (chessBoardFound) {
      foundCorners.push_back(pointBuf);
    }
    if (DEBUG) {
      drawChessboardCorners(*iter, this->chessBoardDimensions, pointBuf,
                            chessBoardFound);
      imshow("Found Corners", *iter);
      waitKey(27);
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
  Mat frame, drawToFrame, oefficients, cameraMatrix = Mat::eye(3, 3, CV_64F);
  vector<Mat> savedImages;
  vector<vector<Point2f>> markerCorners, rejectedCorners;

  if (!this->inputVideo.isOpened()) {
    return;
  }
  if (DEBUG) {
    namedWindow("Camera", CV_WINDOW_AUTOSIZE);
  }
  cout << "Found " << this->images.size() << " / "
       << this->numImagesForCalibration
       << " waiting 5 seconds for the first image" << endl;
  usleep(5 * 1000000);

  while (true) {
    if (!this->inputVideo.read(frame))
      break;

    vector<Vec2f> foundPoints;
    bool found = false;
    found = findChessboardCorners(frame, this->chessBoardDimensions,
                                  foundPoints, this->chessBoardFlags);
    frame.copyTo(drawToFrame);
    if (DEBUG) {
      // When in DEBUG, show the captured image
      drawChessboardCorners(drawToFrame, this->chessBoardDimensions,
                            foundPoints, found);
      if (found)
        imshow("Camera", drawToFrame);
      else
        imshow("Camera", frame);

      char c = waitKey(1000 / this->FPS);

      switch (c) {
      case ' ':
        // savedImages
        if (found) {
          Mat temp;
          frame.copyTo(temp);
          this->images.push_back(temp);
        }
        break;
      case 10:
        if (this->images.size() < this->numImagesForCalibration) {
          cout << "Not enough images" << endl;
          break;
        }
        cout << "returning to calibrate" << endl;
        return;
      case 27:
        // exit
        cout << "Exiting" << endl;
        exit(0);
        break;
      }
    } else {
      // When not in DEBUG, Automate the Image Capture Process
      if (this->images.size() >= this->numImagesForCalibration)
        return;
      if (found) {
        Mat temp;
        frame.copyTo(temp);
        this->images.push_back(temp);
        cout << "Found " << this->images.size() << " / "
             << this->numImagesForCalibration
             << " waiting 5 seconds for next image" << endl;
        usleep(5 * 1000000);
      }
    }
  }
}

/*
 * Get Images From Camera
 *
 * Grab images from a camera and store for calibration
 *
 */
void CameraCalibrator::calibrate() {
  this->getImagesFromCamera();
  cout << "Calibrating" << endl;
  vector<vector<Point2f>> imageSpacePoints;
  this->calculateChessBoardCornersFromImages(imageSpacePoints);
  vector<vector<Point3f>> worldCornerPoints(1);
  this->calculateChessBoardPosition(worldCornerPoints[0]);
  worldCornerPoints.resize(imageSpacePoints.size(), worldCornerPoints[0]);

  vector<Mat> rVecs, tVecs;
  this->distanceCoefficients = Mat::zeros(8, 1, CV_64F);
  // Performs the Calibration
  calibrateCamera(worldCornerPoints, imageSpacePoints,
                  this->chessBoardDimensions, this->cameraMatrix,
                  this->distanceCoefficients, rVecs, tVecs);
  if (this->saveCalibration())
    cout << "File saved" << endl;
  else
    cout << "Unable to save file" << endl;
}

bool CameraCalibrator::saveCalibration() {
  FileStorage fs(this->outFileName, FileStorage::WRITE);
  if (!fs.isOpened())
    return false;
  time_t tm;
  time(&tm);
  struct tm *t2 = localtime(&tm);
  char buf[1024];
  strftime(buf, sizeof(buf), "%c", t2);

  fs << "calibration_time" << buf;
  fs << "camera_matrix" << this->cameraMatrix;
  fs << "distortion_coefficients" << this->distanceCoefficients;
  fs.release();
  return true;
}
