#include "CameraCallibrator.h"
#include <iostream>

int main(int argc, char const *argv[]) {
  CameraCallibrator *calibrator = new CameraCallibrator();
  double res = calibrator->callibrate();
  std::cout << "Calibration results " << res << std::endl;
  return 0;
}
