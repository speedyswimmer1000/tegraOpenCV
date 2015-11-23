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
		double min = 100.0;
		bool started = false;
		int numFrames = 0;
		VideoCapture capture(argv[1]);
		if (!capture.isOpened() )
			throw "Error when opening video file.\n";
		cout << "Start loop CPU\n";
		//	double start = time(&tim);
		//	double end = time(&tim);
		//	double elapsed = end - start;
		Mat descriptors, descriptors_keyframe; // Have to be in scope
		vector<KeyPoint> keypoints, first_keypoints;
		for ( ; ; ){
			double start = time(&tim);
			capture >> frame;
			if (frame.empty() )
				break;
			cvtColor(frame, frame, COLOR_BGR2GRAY);
			detector.detect(frame, keypoints);
			extractor.compute(frame, keypoints, descriptors);  
			if (!started){
				started = true;
				first_keypoints = keypoints;
				descriptors.copyTo(descriptors_keyframe);
				vector< vector< DMatch > > doubleMatches;
				BFMatcher matcher;
				matcher.knnMatch( descriptors, descriptors, doubleMatches, 2 );
			}else{ // BF Matching
				vector< vector< DMatch > > doubleMatches;
				vector< DMatch > matches;
				vector< DMatch > good_matches;
				BFMatcher matcher;
				matcher.knnMatch( descriptors, descriptors_keyframe, doubleMatches, 2 );
				double ratio = 0.8;
				for (int i = 0; i < doubleMatches.size(); i++) {
					if (doubleMatches[i][0].distance < ratio * 
						doubleMatches[i][1].distance){
					good_matches.push_back(doubleMatches[i][0]);
					}
				}
				vector<Point2d> matched_kps_moved, matched_kps_keyframe;
				for( int i = 0; i < good_matches.size(); i++ ){
				  matched_kps_moved.push_back( keypoints[ good_matches[i].queryIdx ].pt );  // Left frame
				  matched_kps_keyframe.push_back( first_keypoints[ good_matches[i].trainIdx ].pt );
				}
				if (! (matched_kps_moved.size() < 4 || matched_kps_keyframe.size() < 4) ){
					std::vector<uchar> status; 
	
					double fMatP1 = 1.0;
					double fMatP2 = 0.995;
		
				// Use RANSAC and the fundamental matrix to take out points that don't fit geometrically
					findFundamentalMat(matched_kps_moved, matched_kps_keyframe,
						CV_FM_RANSAC, fMatP1, fMatP2, status);
				}
				double end = time(&tim);	
				double elapsed = end - start;
				cout << "Elapsed = " << elapsed << endl;
				if (elapsed > max){
					max = elapsed;
				}
				if (elapsed < min){
					min = elapsed;
				}
				total += elapsed;
				numFrames++;
			}
		}
		cout << "Total: " << total << " Max: " << max << " Min: " << min << " Average: " << total / numFrames << endl;
	}
}
