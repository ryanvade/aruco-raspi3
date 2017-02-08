
#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>
#include <iostream>
#include <string>

using namespace cv;

int main(int argc, char const *argv[]) {
  int cameraID = -1; // -1 is default camera
  int dictionaryId = 0;
  int markerSize = 200; // # Side Pixels
  size_t markerCount = 10;
  Mat marker;
  std::string fileName;

Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));
for (size_t i = 0; i < markerCount; i++) {
  fileName = "marker" + std::to_string(i) + ".png";
  std::cout << "Writing marker " << std::to_string(i) << " to " << fileName << std::endl;
  dictionary->drawMarker(i, markerSize, marker);
  imwrite(fileName, marker);
}
  return 0;
}
