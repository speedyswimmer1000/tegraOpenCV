#include <cv.h>
#include <highgui.h>
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/gpu/gpu.hpp"

//using namespace cv::gpu;
//using namespace cv;
using namespace std;

int main(int argc, char** argv){
	int cuda = cv::gpu::getCudaEnabledDeviceCount();
	cout << cuda << endl;
	cv::gpu::setDevice(0);
	cv::gpu::GpuMat aa;
	
}
