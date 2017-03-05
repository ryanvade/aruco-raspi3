#include "CameraCalibrator.h"
#include <iostream>

int main(int argc, char const *argv[]) {
  CameraCalibrator *calibrator = new CameraCalibrator(1, true);
  calibrator->getImagesFromCamera();

  return 0;
}
