#include "ArucoDetector.h"

ArucoDetector::ArucoDetector(int cameraId, bool flipVertical)
{
  this->cameraId = cameraId;
  this->flipVertical = flipVertical;
  detectorParams->doCornerRefinement = true;
}

ArucoDetector::ArucoDetector()
{
  this->cameraId = -1;
  this->flipVertical = flipVertical;
  detectorParams->doCornerRefinement = true;
}

bool ArucoDetector::openCamera()
{
  if(this->isCameraOpen()) return true;
  return this->camera.open(this->cameraId);
}

bool ArucoDetector::isCameraOpen()
{
  return this->camera.isOpened();
}

vector<int> ArucoDetector::arucoMarkerInView()
{
  vector<int> ids;
  vector<vector<Point2f>> corners, rejected;
  if(!this->isCameraOpen()) return ids;
  Mat img = this->getImage();
  aruco::detectMarkers(img, this->dictionary, corners, ids, detectorParams, rejected);
  return ids;
}

Mat ArucoDetector::getImage()
{
  Mat img;
  if(!this->isCameraOpen()) return img;
  this->camera >> img;
  if(this->flipVertical) flip(img, img, 1);
  return img;
}

bool ArucoDetector::arucoMarkerInView(int arucoId)
{
  return true;
}
