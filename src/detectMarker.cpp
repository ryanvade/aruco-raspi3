
#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>
#include <iostream>

using namespace cv;

int main(int argc, char const *argv[]) {
  int cameraId = -1;
  int escapeKey = 27;
  int dictionaryId = 0;
  bool showRejected = false;
  bool estimatePose = false;
  float markerLength = 0.1;
  String video;
  Mat camMatrix, distCoeffs, image, imageCopy;
  VideoCapture inputVideo(cameraId);
  int waitTime = 10;
  double totalTime = 0;
  int totalIterations = 0;
  double tick, currentTime;
  std::vector<int> ids;
  std::vector<std::vector<Point2f>> corners, rejected;
  std::vector<Vec3d> rvecs, tvecs;

  if(!inputVideo.isOpened())
  {
    std::cout << "Unable to open camera." << std::endl;
    return -1;
  }

  Ptr<aruco::DetectorParameters> detectorParams = aruco::DetectorParameters::create();
  detectorParams->doCornerRefinement = true;

  Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));

  while(inputVideo.grab())
  {
    inputVideo.retrieve(image);
    tick = (double)getTickCount();
    aruco::detectMarkers(image, dictionary, corners, ids, detectorParams, rejected);

    currentTime = ((double)getTickCount() - tick) / getTickFrequency();
    totalTime += currentTime;
    totalIterations++;

    image.copyTo(imageCopy);
    if(ids.size() > 0)
      aruco::drawDetectedMarkers(imageCopy, corners, ids);

    imshow("out", imageCopy);

    char key = (char)waitKey(waitTime);
    if(key == escapeKey)break;
  }
  return 0;
}
