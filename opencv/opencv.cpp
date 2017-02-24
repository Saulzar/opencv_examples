// opencv.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


int main()
{
	cv::VideoCapture cap(0); // open the default camera
	assert(cap.isOpened() && "failed to open camera!");  // check if we succeeded

	while (cap.grab()) {
		cv::Mat image;
		cap.retrieve(image);

		cv::imshow("image", image);
		cv::waitKey(1);
	}

}

