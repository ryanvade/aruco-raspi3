#include "CameraCallibrator.h"
#include <iostream>

int main(int argc, char const *argv[]) {
  CameraCallibrator *calibrator = new CameraCallibrator();
  calibrator->openCamera();
  bool res = calibrator->callibrate();
  if (res) {
    std::cout << "Calibration succesful" << std::endl;
  } else {
    std::cout << "Calibration failed" << std::endl;
  }
  return 0;
}
