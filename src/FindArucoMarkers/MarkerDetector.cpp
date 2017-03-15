
int main(int argc, char** argv)
{
	ArucoDetector detector = new ArucoDetector(2, false);
	detector->openCamera();
	std::cout << "Looking for markers" << std::endl;

	while(true) {
	}
}
