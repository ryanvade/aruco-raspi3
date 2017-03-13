#ifndef ARUCO_DETECTOR_H
#define ARUCO_DETECTOR_H

#include <fstream>
#include <iostream>
#include <opencv2/aruco.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

class ArucoDetector {
public:
  explicit ArucoDetector(int cameraId, bool flipVertical);
  ArucoDetector();
  bool openCamera();
  bool isCameraOpen();
  vector<int> arucoMarkerInView();
  bool arucoMarkerInView(int arucoId);
  vector<int> getArucoMarkerPose(int arucoId);

private:
  int cameraId;
  Mat getImage();
  int dictionaryId;
  bool flipVertical;
  VideoCapture camera;
  string calibrationFile = "calibration.txt";
  Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(
      aruco::PREDEFINED_DICTIONARY_NAME(this->dictionaryId));
      Ptr<aruco::DetectorParameters> detectorParams =
          aruco::DetectorParameters::create();
};
#endif
