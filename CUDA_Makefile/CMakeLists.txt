cmake_minimum_required(VERSION 2.8)
project(RANSAC)

find_package(OpenCV REQUIRED)
find_package(CUDA REQUIRED)

	#link_directories(/usr/local/cuda-6.5/lib)
	
	#Set up and do:  nvcc -c -arch=sm_20 CUDA_RANSAC_Homography.cu 
	#set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}; -gencode arch=compute_20,code=sm_20)
	
	# Add main.cpp to a file to be compiled with CUDA. Also add the .cu file. 
	cuda_add_executable(main main.cpp CUDA_RANSAC_Homography.cu)
	
	# Link in libraries as normal. Make sure to link in the CUDA executable that
	# was built here. 
	target_link_libraries (main ${OpenCV_LIBS}  opencv_nonfree )
