#include "CameraCalibrator.h"

int main(int argc, char const *argv[]) {
  CameraCalibrator *calibrator = new CameraCalibrator(1, true);
  calibrator->calibrate();
  return 0;
}
