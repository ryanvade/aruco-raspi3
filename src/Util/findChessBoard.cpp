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

using namespace std;
using namespace cv;

int main(int argc, char const *argv[]) {
  int cameraId = -1;
  if (argc >= 2) {
    cout << argv[1] << std::endl;
    cameraId = atoi(argv[1]);
  }
  VideoCapture inputVideo(cameraId);
  Mat frame, drawToFrame, distanceCoefficients,
      cameraMatrix = Mat::eye(3, 3, CV_64F);
  vector<Mat> savedImages;
  vector<vector<Point2f>> markerCorners, rejectedCorners;
  Size chessBoardDimensions = Size(6, 9);
  int chessBoardFlags = cv::CALIB_CB_ADAPTIVE_THRESH |
                        cv::CALIB_CB_NORMALIZE_IMAGE | cv::CALIB_CB_FAST_CHECK;

  if (!inputVideo.isOpened()) {
    return -1;
  }
  namedWindow("Camera", CV_WINDOW_AUTOSIZE);

  while (true) {
    if (!inputVideo.read(frame))
      break;

    vector<Vec2f> foundPoints;
    bool found = false;
    found = findChessboardCorners(frame, chessBoardDimensions, foundPoints,
                                  chessBoardFlags);
    frame.copyTo(drawToFrame);
    drawChessboardCorners(drawToFrame, chessBoardDimensions, foundPoints,
                          found);
    if (found)
      imshow("Camera", drawToFrame);
    else
      imshow("Camera", frame);

    char c = waitKey(1000 / 20);
  }
  return 0;
}
