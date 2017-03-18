#include <iostream>
#include "ArucoDetector.h"

using namespace std;

int main(int argc, char** argv)
{
	ArucoDetector* detector = new ArucoDetector(1, false);
	detector->openCamera();
	cout << "Running through marker in view" << endl;
	for(int i = 0; i < 10; i++)
	{
		if(detector->arucoMarkerInView(i))
		cout << "Marker " << i << " is in view" << endl;
		else
		cout << "Marker " << i << " is not in view" << endl;
	}
	cout << endl << "Running through markers in view" << endl;
	vector<int> markers = detector->arucoMarkersInView();
	for(auto itr = markers.begin(); itr != markers.end(); ++itr)
	{
		cout << "Marker " << *itr <<  " is in view." << endl;
		vector<double> rottransvec = detector->getPose(*itr);
		cout << endl << "Running get pose for id " << *itr << endl;
		for(auto itr = rottransvec.begin(); itr != rottransvec.end(); ++itr)
		{
			cout << *itr << endl;
		}
	}

	vector<double> rottransvec = detector->getPose(5);
	cout << endl << "Running get pose for id " << 5 << endl;
	for(auto itr = rottransvec.begin(); itr != rottransvec.end(); ++itr)
	{
		cout << *itr << endl;
	}
	return 0;
}
