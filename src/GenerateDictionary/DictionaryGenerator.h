#ifndef DICTIONARY_GENERATOR_H
#define DICTIONARY_GENERATOR_H

#include <opencv2/aruco.hpp>

using namespace std;
using namespace cv;

class DictionaryGenerator {
private:
  int numberOfMarkers;
  int dimensionOfMarkers;
  string outFile = "kipr_dictionary.yml";

public:
  DictionaryGenerator();
  DictionaryGenerator(int numberOfMarkers, int dimensionOfMarkers);
  ~DictionaryGenerator();
  aruco::Dictionary generateNewDictionary();
  bool saveDictionary(aruco::Dictionary *dictionary);
  aruco::Dictionary *getDictionaryFromFile(string filename);
};
#endif
