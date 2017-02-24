// opencv.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <Kinect.h>
 

cv::Mat1w getDepthFrame(IDepthFrame* frame) {

	IFrameDescription* desc = NULL;
	frame->get_FrameDescription(&desc);

	int width, height;
	desc->get_Width(&width);
	desc->get_Height(&height);

	unsigned int sz;
	unsigned short* buf;
	frame->AccessUnderlyingBuffer(&sz, &buf);

	// Construct opencv Mat (16 bit unsigned, 1 channel)
	return cv::Mat1w(height, width, buf);
}


cv::Mat3b getColorFrame(IColorFrame* frame) {

	IFrameDescription* desc = NULL;
	frame->get_FrameDescription(&desc);

	int width, height;
	desc->get_Width(&width);
	desc->get_Height(&height);

	cv::Mat4b image(height, width);

	frame->CopyConvertedFrameDataToArray(height * width * 4, image.data, ColorImageFormat::ColorImageFormat_Bgra);
	return image;
}





void displayDepth(cv::Mat1f const &depth) {

	// Get mean for non zero pixels (depth > 0)
	// For purposes of display, we normalize pixel values to range 0-1 (ish) for float image (0-255 for 8bit image)
	double mean = cv::mean(depth, depth > 0)[0];
	cv::imshow("depth", depth / mean);
}

int main()
{
	IKinectSensor* sensor = nullptr;         // Kinect sensor

	HRESULT hr = GetDefaultKinectSensor(&sensor);
	assert(sensor && "failed to find sensor");
	sensor->Open();

	// Initialize depth frame reader
	IDepthFrameSource* depthSource = NULL;
	sensor->get_DepthFrameSource(&depthSource);
	assert(depthSource);
	IDepthFrameReader* depthReader;     // Kinect depth data source
	depthSource->OpenReader(&depthReader);
	assert(depthReader);

	// Initialize color frame reader
	IColorFrameSource* colorSource = NULL;
	sensor->get_ColorFrameSource(&colorSource);
	assert(colorSource);
	IColorFrameReader* colorReader;     // Kinect color data source
	colorSource->OpenReader(&colorReader);
	assert(colorReader);

	while (1) {
		IDepthFrame* depth = NULL;
		IColorFrame* color = NULL;

		depthReader->AcquireLatestFrame(&depth);
		colorReader->AcquireLatestFrame(&color);
		if (depth) {

			//Get the depth frame and convert it to a floating point image, 1 channel (cv::Mat1f)
			cv::Mat1f depthImage = getDepthFrame(depth);
			displayDepth(depthImage);

			depth->Release();
		}

		if (color) {
			cv::Mat4b colorImage = getColorFrame(color);
			cv::imshow("color", colorImage);

			color->Release();
		}

		cv::waitKey(1);

	}



	colorSource->Release();
	depthSource->Release();
}

