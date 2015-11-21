#include <iostream>
#include "opencv2/opencv.hpp"
#include <sys/time.h>
#include <opencv2/nonfree/nonfree.hpp>

#define GPU 0

#if GPU
	#include "opencv2/gpu/gpu.hpp"
	//using namespace cv::gpu;
#endif

using namespace cv;
using namespace std;

inline double time(timeval *tim){
	gettimeofday(tim, NULL);
	return (tim->tv_sec + (tim->tv_usec/1000000.0) );
}

int main(int argc, char** argv){
	
	if (argc < 2){
		cout << "Insufficient args; usage ./cam <input file>\n";
		exit(0);
	}
	
	timeval tim;
	int thresh = 5;
	
#if GPU
	cout << "Initializing Cuda\n";
	cv::gpu::DeviceInfo info;
	cv::gpu::setDevice(0);
	gpu::FAST_GPU d2(thresh);
#endif

	FastFeatureDetector detector(thresh);
	SurfDescriptorExtractor extractor; 
	
	Mat frame;
	
#if GPU
	for (int i = 0; i < 1; i++){
		double total = 0.0;
		double max = 0.0;
		VideoCapture capture(argv[1]);
		if (!capture.isOpened() )
			throw "Error when opening video file.\n";
		cout << "Start loop GPU\n";
		for ( ; ; ){
			capture >> frame;
			if (frame.empty() )
				break;
			cvtColor(frame, frame, COLOR_BGR2GRAY);
			gpu::GpuMat kps, src;
			src.upload(frame);
			double start = time(&tim);
			d2(src, kps, kps);
			double end = time(&tim);
			double elapsed = end - start;
			if (elapsed > max)
				max = elapsed;
			total += elapsed;
			
		}
		cout << "Total: " << total << " Max: " << max << endl;
	}
#endif

	for (int i = 0; i < 1; i++){
		double total = 0.0;
		double max = 0.0;
		VideoCapture capture(argv[1]);
		if (!capture.isOpened() )
			throw "Error when opening video file.\n";
		cout << "Start loop CPU\n";
		for ( ; ; ){
			capture >> frame;
			if (frame.empty() )
				break;
			cvtColor(frame, frame, COLOR_BGR2GRAY);
			double start = time(&tim);
			vector<KeyPoint> keypoints;
			detector.detect(frame, keypoints);
			double end = time(&tim);
			double elapsed = end - start;
			Mat descriptors_keyframe;
			start =time(&tim);
			extractor.compute(frame, keypoints, descriptors_keyframe);  
			end = time(&tim);
			elapsed = end - start;
			if (elapsed > max)
				max = elapsed;
			total += elapsed;
			
		}
		cout << "Total: " << total << " Max: " << max << endl;
	}
}