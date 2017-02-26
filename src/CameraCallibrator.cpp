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

bool CameraCallibrator::callibrate() {
  std::vector<std::vector<cv::Vec2f>> imagePoints;
  cv::Mat cameraMatrix, distortionCoeffs;
  cv::Size resolution;
  MODE mode = CAPTURING;
  clock_t prevTimestamp = 0;
  if (!this->isCameraOpen())
    return false;
  while (mode == CAPTURING) // Get input images, when enough callibrate
  {
    cv::Mat view;
    view = this->getImage();
    if (mode == CAPTURING && imagePoints.size() >= this->numberFrames) {
      mode = (runCalibration(resolution, cameraMatrix, distortionCoeffs,
                             imagePoints))
                 ? CALIBRATING
                 : DETECTING;
    }
    if (view.empty()) {
      if (mode != CALIBRATED && !imagePoints.empty())
        runCalibration(resolution, cameraMatrix, distortionCoeffs, imagePoints);
      break;
    }

    resolution = view.size();
    // TODO: Check if needs to flip
    std::vector<cv::Vec2f> pointBuffer;
    // TODO: the following only works with chess boards
    bool found = cv::findChessboardCorners(view, resolution, pointBuffer,
                                           this->chessBoardFlags);

    // TODO: the following only works with chess boards
    if (found) {
      // Convert to grayscale
      cv::Mat viewGray;
      cv::cvtColor(view, viewGray, cv::COLOR_BGR2GRAY);
      cv::cornerSubPix(
          viewGray, pointBuffer, cv::Size(11, 11), cv::Size(-1, -1),
          cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30,
                           0.1));

      if (mode == CAPTURING &&
          (!this->isCameraOpen() ||
           clock() - prevTimestamp > this->delay * 1e-3 * CLOCKS_PER_SEC)) {
        imagePoints.push_back(pointBuffer);
        prevTimestamp = clock();
      }
    }
  }
  return (mode == CALIBRATED) ? true : false;
}

cv::Mat CameraCallibrator::getImage() {
  cv::Mat image;
  if (this->isCameraOpen())
    this->cameraInput >> image;
  return image;
}

bool CameraCallibrator::openCamera() {
  if (this->isCameraOpen())
    return true;
  return this->cameraInput.open(this->cameraId);
}

bool CameraCallibrator::isCameraOpen() { return this->cameraInput.isOpened(); }

bool CameraCallibrator::runCalibration(
    cv::Size resolution, cv::Mat cameraMatrix, cv::Mat distortionCoeffs,
    std::vector<std::vector<cv::Vec2f>> imagePoints) {
  std::vector<cv::Mat> rotationVectors, translationVectors;
  std::vector<float> reprojErrs;

  // Run the actual callilbration
  cameraMatrix = cv::Mat::eye(3, 3, CV_64F); // Fixes Aspect Ratio
  // TODO: need to give a specific aspect ratio ?
  distortionCoeffs = cv::Mat::zeros(8, 1, CV_64F); // Get zeroed matrix

  std::vector<std::vector<cv::Point3f>> objectPoints(1);

  // Get the Corner Positions of the input imagePs
  objectPoints[0].clear();
  // TODO: The following is only for the Chess Board
  for (size_t i = 0; i < this->chessBoardSize.height; i++) {
    for (size_t j = 0; j < this->chessBoardSize.width; j++) {
      objectPoints[0].push_back(
          cv::Point3f(j * this->squareSize, i * this->squareSize, 0));
    }
  }

  objectPoints.resize(imagePoints.size(), objectPoints[0]);
  // TODO: The following is only for Chess Boards
  double rms = cv::calibrateCamera(
      objectPoints, imagePoints, resolution, cameraMatrix, distortionCoeffs,
      rotationVectors, translationVectors, this->chessBoardFlags);

  bool calibrationSuccessful =
      cv::checkRange(cameraMatrix) && cv::checkRange(distortionCoeffs);
  double totalAverageError = getTotalAverageError(
      objectPoints, imagePoints, rotationVectors, translationVectors,
      cameraMatrix, distortionCoeffs, reprojErrs);

  if (calibrationSuccessful)
    this->saveResultsToFile(resolution, cameraMatrix, distortionCoeffs,
                            rotationVectors, translationVectors, reprojErrs,
                            imagePoints, totalAverageError);
}

double CameraCallibrator::getTotalAverageError(
    const std::vector<std::vector<cv::Point3f>> &objectPoints,
    const std::vector<std::vector<cv::Vec2f>> &imagePoints,
    const std::vector<cv::Mat> &rotationVectors,
    const std::vector<cv::Mat> translationVectors, const cv::Mat &cameraMatrix,
    const cv::Mat &distortionCoeffs, std::vector<float> &perViewErrors) {
  std::vector<cv::Vec2f> newImagePoints;
  size_t totalPoints = 0;
  double totalErr = 0.0, curr;
  perViewErrors.resize(objectPoints.size());

  for (size_t i = 0; i < objectPoints.size(); i++) {
    cv::projectPoints(objectPoints[i], rotationVectors[i],
                      translationVectors[i], cameraMatrix, distortionCoeffs,
                      newImagePoints);
    curr = cv::norm(imagePoints[i], newImagePoints, cv::NORM_L2);
    size_t n = objectPoints[i].size();
    newImagePoints[i] = (float)std::sqrt(curr * curr / n);
    totalErr += curr * curr;
    totalPoints += n;
  }

  return std::sqrt(totalErr / totalPoints);
}

void CameraCallibrator::saveResultsToFile(
    const cv::Size &resolution, const cv::Mat &cameraMatrix,
    const cv::Mat &distortionCoeffs,
    const std::vector<cv::Mat> &rotationVectors,
    const std::vector<cv::Mat> &translationVectors,
    const std::vector<float> &reprojErrs,
    const std::vector<std::vector<cv::Vec2f>> &imagePoints,
    double totalAverageError) {
  cv::FileStorage fs(this->outputFile, cv::FileStorage::WRITE);
  time_t tm;
  time(&tm);
  struct tm *t2 = localtime(&tm);
  char buf[1024];
  strftime(buf, sizeof(buf), "%c", t2);

  fs << "calibration_time" << buf;

  if (!rotationVectors.empty() || !reprojErrs.empty())
    fs << "nr_of_frames"
       << (int)std::max(rotationVectors.size(), reprojErrs.size());
  fs << "image_width" << resolution.width;
  fs << "image_height" << resolution.height;
  fs << "board_width" << this->chessBoardSize.width;
  fs << "board_height" << this->chessBoardSize.height;
  fs << "square_size" << this->squareSize;
  fs << "camera_matrix" << cameraMatrix;
  fs << "distortion_coefficients" << distortionCoeffs;
  fs << "avg_reprojection_error" << totalAverageError;

  // cv::CV_Assert(rotationVectors[0].type() == translationVectors[0].type());
  cv::Mat bigmat((int)rotationVectors.size(), 6,
                 CV_MAKETYPE(rotationVectors[0].type(), 1));
  bool needReshapeR = rotationVectors[0].depth() != 1 ? true : false;
  bool needReshapeT = translationVectors[0].depth() != 1 ? true : false;

  for (size_t i = 0; i < rotationVectors.size(); i++) {
    cv::Mat r = bigmat(cv::Range(int(i), int(i + 1)), cv::Range(0, 3));
    cv::Mat t = bigmat(cv::Range(int(i), int(i + 1)), cv::Range(3, 6));
    if (needReshapeR)
      rotationVectors[i].reshape(1, 1).copyTo(r);
    else
      r = rotationVectors[i].t();
    if (needReshapeT)
      translationVectors[i].reshape(1, 1).copyTo(t);
    else
      r = translationVectors[i].t();
  }
  fs.writeComment(
      "a set of 6-tuples (rotation vector + translation vector) for each view");
  fs << "extrinsic_parameters" << bigmat;

  if (!imagePoints.empty()) {
    cv::Mat imagePtMat((int)imagePoints.size(), (int)imagePoints[0].size(),
                       CV_32FC2);
    for (size_t i = 0; i < imagePoints.size(); i++) {
      cv::Mat r = imagePtMat.row(int(i)).reshape(2, imagePtMat.cols);
      cv::Mat imgpti(imagePoints[i]);
      imgpti.copyTo(r);
    }
    fs << "image_points" << imagePtMat;
  }
}
