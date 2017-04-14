#include <algorithm>
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

vector<int> ArucoDetector::arucoMarkersInView()
{
  vector<int> ids;
  vector<vector<Point2f>> corners, rejected;
  if(!this->isCameraOpen()) return ids;
  Mat img = this->getFrame();
  aruco::detectMarkers(img, this->dictionary, corners, ids, detectorParams, rejected);
  return ids;
}

Mat ArucoDetector::getFrame()
{
  Mat img;
  if(!this->isCameraOpen()) return img;
  this->camera >> img;
  if(this->flipVertical) flip(img, img, 1);
  return img;
}

bool ArucoDetector::arucoMarkerInView(int arucoId)
{
  vector<int> ids;
  vector<vector<Point2f>> corners, rejected;
  if(!this->isCameraOpen()) return false;
  Mat img = this->getFrame();
  aruco::detectMarkers(img, this->dictionary, corners, ids, detectorParams, rejected);
  if(find(ids.begin(), ids.end(), arucoId) != ids.end()) return true;
  return false;
}

bool ArucoDetector::vectorContains(vector<int> vec, int val)
{
  if(find(vec.begin(), vec.end(), val) != vec.end()) return true;
  return false;
}

vector<double> ArucoDetector::getPose(int arucoId)
{
  vector<double> rottransvec;
  rottransvec.assign(6, 0.0);
  if(!this->isCameraOpen()) return rottransvec;
  FileStorage fs(this->calibrationFile, FileStorage::READ);
  if(!fs.isOpened()) return rottransvec;

  Mat cameraMatrix, distortionCoefficients;

  fs["camera_matrix"] >> cameraMatrix;
  fs["distortion_coefficients"] >> distortionCoefficients;
  fs.release();

  Mat img = this->getFrame();
  vector<int> ids;
  vector<vector<Point2f>> corners, rejected;
  vector<Vec3d> rvecs, tvecs;

  // detect markers and estimate pose
  aruco::detectMarkers(img, this->dictionary, corners, ids, detectorParams,
                       rejected);
  if(ids.size() > 0 && vectorContains(ids, arucoId))
  {
    aruco::estimatePoseSingleMarkers(corners, this->arucoSquareSize, cameraMatrix,
                                     distortionCoefficients, rvecs, tvecs);

    size_t index = find(ids.begin(), ids.end(), arucoId) - ids.begin();
    Vec3d translation = tvecs[index];
    Vec3d rotation = rvecs[index];
    for(size_t i = 0; i < translation.rows; i++)
    {
        rottransvec.push_back(translation[i]);
    }

    for(size_t i = 0; i < rotation.rows; i++)
    {
        rottransvec.push_back(rotation[i]);
    }
  }
  return rottransvec;
}
