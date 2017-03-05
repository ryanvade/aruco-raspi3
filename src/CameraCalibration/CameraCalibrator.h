#ifndef CAMERA_CALLIBRATOR_H
#define CAMERA_CALIBFRATOR_H

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <string>
#define DEBUG 0
typedef int MODE;

using namespace std;
using namespace cv;

enum MODES { CALIBRATING = 0, DETECTING = 1, CAPTURING = 2, CALIBRATED = 3 };
const Scalar RED(0, 0, 255), GREEN(0, 255, 0);

class CameraCalibrator {
public:
  explicit CameraCalibrator(int cameraId, bool flipVertical);
  CameraCalibrator();
  bool callibrate();
  bool openCamera();
  bool isCameraOpen();
  void calibrate();

private:
  int cameraId;
  const int FPS = 20;
  bool flipVertical = false;
  float chessBoardSquareSize = 0.0235f; // Meters
  float arucoSquareSize = 0.0;          // Meters
  int numImagesForCalibration = 15;
  string outFileName = "out.txt";
  VideoCapture inputVideo;
  Size chessBoardDimensions = Size(6, 9);
  vector<Point3f> chessBoardCorners;
  vector<Mat> images;
  vector<vector<Point2f>> foundChessboardCorners;
  Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
  Mat distanceCoefficients;
  int chessBoardFlags = cv::CALIB_CB_ADAPTIVE_THRESH |
                        cv::CALIB_CB_NORMALIZE_IMAGE | cv::CALIB_CB_FAST_CHECK;
  void calculateChessBoardPosition(vector<Point3f> &vec);
  void
  calculateChessBoardCornersFromImages(vector<vector<Point2f>> &foundCorners);
  void getImagesFromCamera();
  bool saveCalibration();
};
#endif
