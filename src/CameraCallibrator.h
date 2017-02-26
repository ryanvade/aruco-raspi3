#ifndef CAMERA_CALLIBRATOR_H
#define CAMERA_CALIBFRATOR_H

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <string>

#define DEBUG 1
typedef int MODE;
enum MODES { CALIBRATING = 0, DETECTING = 1, CAPTURING = 2, CALIBRATED = 3 };
const cv::Scalar RED(0, 0, 255), GREEN(0, 255, 0);

class CameraCallibrator {
public:
  explicit CameraCallibrator(int cameraId, bool flipVertical);
  CameraCallibrator();
  bool callibrate();
  bool openCamera();
  bool isCameraOpen();

private:
  int cameraId;
  int delay = 100;
  size_t numberFrames = 10;
  const char ESC_KEY = 27;
  std::string outputFile = "results";
  bool flipVertical = false;
  cv::VideoCapture cameraInput;
  cv::Size chessBoardSize;
  int squareSize = 50;
  int chessBoardFlags = cv::CALIB_CB_ADAPTIVE_THRESH |
                        cv::CALIB_CB_NORMALIZE_IMAGE | cv::CALIB_CB_FAST_CHECK;

  cv::Mat getImage();
  bool saveResults();
  bool runCalibration(cv::Size resolution, cv::Mat cameraMatrix,
                      cv::Mat distortionCoeffs,
                      std::vector<std::vector<cv::Vec2f>> imagePoints);
  double getTotalAverageError(
      const std::vector<std::vector<cv::Point3f>> &objectPoints,
      const std::vector<std::vector<cv::Vec2f>> &imagePoints,
      const std::vector<cv::Mat> &rotationVectors,
      const std::vector<cv::Mat> translationVectors,
      const cv::Mat &cameraMatrix, const cv::Mat &distortionCoeffs,
      std::vector<float> &perViewErrors);

  void saveResultsToFile(const cv::Size &resolution,
                         const cv::Mat &cameraMatrix,
                         const cv::Mat &distortionCoeffs,
                         const std::vector<cv::Mat> &rotationVectors,
                         const std::vector<cv::Mat> &translationVectors,
                         const std::vector<float> &reprojErrs,
                         const std::vector<std::vector<cv::Vec2f>> &imagePoints,
                         double totalAverageError);
};
#endif
