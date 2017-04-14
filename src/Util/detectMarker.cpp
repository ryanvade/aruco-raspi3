
#include <fstream>
#include <iostream>
#include <opencv2/aruco.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

int main(int argc, char const *argv[]) {
  int dictionaryId = 0;
  // float markerLength = 0.07f;
  float markerLength = 0.025f;
  Mat frame, frameCopy, cameraMatrix = Mat::zeros(3, 3, CV_64F),
                        distanceCoefficients = Mat::zeros(1, 5, CV_64F);
  vector<int> markerIds;
  vector<vector<Point2f>> markerCorners, rejectedMarkers;
  aruco::DetectorParameters parameters;
  vector<Vec3d> rotationVectors, translationVectors;
  FileStorage fs("calibration.txt",
                 FileStorage::READ);

  Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(
      aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));
  Ptr<aruco::DetectorParameters> detectorParams =
      aruco::DetectorParameters::create();
  detectorParams->doCornerRefinement = true;

  if (!fs.isOpened()) {
    cout << "could not open file" << endl;
    return -1;
  }
  fs["camera_matrix"] >> cameraMatrix;
  fs["distortion_coefficients"] >> distanceCoefficients;
  fs.release();
  VideoCapture inputVideo;
  int waitTime;
  inputVideo.open(1);
  waitTime = 10;

  double totalTime = 0;
  int totalIterations = 0;

  while (inputVideo.grab()) {
    Mat image, imageCopy;
    inputVideo.retrieve(image);

    double tick = (double)getTickCount();

    vector<int> ids;
    vector<vector<Point2f>> corners, rejected;
    vector<Vec3d> rvecs, tvecs;

    // detect markers and estimate pose
    aruco::detectMarkers(image, dictionary, corners, ids, detectorParams,
                         rejected);
    aruco::estimatePoseSingleMarkers(corners, markerLength, cameraMatrix,
                                     distanceCoefficients, rvecs, tvecs);
    double currentTime = ((double)getTickCount() - tick) / getTickFrequency();
    totalTime += currentTime;
    totalIterations++;
    if (totalIterations % 30 == 0) {
      cout << "Detection Time = " << currentTime * 1000 << " ms "
           << "(Mean = " << 1000 * totalTime / double(totalIterations) << " ms)"
           << endl;
    }

    image.copyTo(imageCopy);
    if (ids.size() > 0) {
      aruco::drawDetectedMarkers(imageCopy, corners, ids);

      for (unsigned int i = 0; i < ids.size(); i++)
        aruco::drawAxis(imageCopy, cameraMatrix, distanceCoefficients, rvecs[i],
                        tvecs[i], markerLength * 0.5f);
    }

    imshow("out", imageCopy);
    char key = (char)waitKey(waitTime);
    if (key == 27)
      break;
  }

  return 0;
}
