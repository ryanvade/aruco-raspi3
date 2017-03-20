#include "DictionaryGenerator.h"
#include <iostream>

using namespace std;

bool compareDictionaries(aruco::Dictionary *dict1, aruco::Dictionary *dict2) {
  return (dict1->markerSize == dict2->markerSize) &&
         (dict1->maxCorrectionBits == dict2->maxCorrectionBits);
}

int main(int argc, char const *argv[]) {
  // Instantiate a Dictionary Generator with
  // The number of markers in the dictionary is 16
  // The dimensions of the markers is 20
  DictionaryGenerator *dictionaryGenerator = new DictionaryGenerator(16, 20);
  // Generate the new Dictionary
  auto dictionary = dictionaryGenerator->generateNewDictionary();
  // Write the Dictionary to file
  dictionaryGenerator->saveDictionary(&dictionary);
  // Read the Dictionary from file
  auto retrievedDictionary =
      dictionaryGenerator->getDictionaryFromFile("kipr_dictionary.yml");
  if (compareDictionaries(&dictionary, retrievedDictionary))
    cout << "Dictionaries are the same" << endl;
  else
    cout << "Dictionaries are not the same" << endl;
  return 0;
}
