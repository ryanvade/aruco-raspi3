#include "DictionaryGenerator.h"
#include <opencv2/aruco.hpp>

using namespace std;
using namespace cv;

DictionaryGenerator::DictionaryGenerator() {
  this->numberOfMarkers = 20;
  this->dimensionOfMarkers = 30;
}

DictionaryGenerator::DictionaryGenerator(int numberOfMarkers,
                                         int dimensionOfMarkers) {
  this->numberOfMarkers = numberOfMarkers;
  this->dimensionOfMarkers = dimensionOfMarkers;
}

DictionaryGenerator::~DictionaryGenerator() {}

aruco::Dictionary DictionaryGenerator::generateNewDictionary() {
  return aruco::generateCustomDictionary(this->numberOfMarkers,
                                         this->dimensionOfMarkers);
}

bool DictionaryGenerator::saveDictionary(aruco::Dictionary *dictionary) {
  FileStorage fs(this->outFile, FileStorage::WRITE);
  if (!fs.isOpened())
    return false;
  fs << "MarkerSize" << dictionary->markerSize;
  fs << "MaxCorrectionBits" << dictionary->maxCorrectionBits;
  fs << "BytesList" << dictionary->bytesList;
  fs.release();
  return true;
}

aruco::Dictionary *DictionaryGenerator::getDictionaryFromFile(string filename) {
  int markerSize, maxCorrectionBits;
  cv::Mat bytesList;
  FileStorage fs(filename, FileStorage::READ);
  if (!fs.isOpened())
    return NULL;
  fs["MarkerSize"] >> markerSize;
  fs["MaxCorrectionBits"] >> maxCorrectionBits;
  fs["BytesList"] >> bytesList;
  return new aruco::Dictionary(bytesList, markerSize, maxCorrectionBits);
}
