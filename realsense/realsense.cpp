// realsense.cpp : Defines the entry point for the console application.
//
#pragma warning( disable : C4996)

#include "stdafx.h"
#include <pxcsensemanager.h>

#include <opencv2/opencv.hpp>

void *getData(PXCImage *pxcImage, PXCImage::PixelFormat format, cv::Size &size) {
	size = cv::Size(pxcImage->QueryInfo().width, pxcImage->QueryInfo().height);
//	assert(pxcImage->QueryInfo().format == format);

	PXCImage::ImageData data;
	pxcImage->AcquireAccess(PXCImage::ACCESS_READ, format, &data);


	return data.planes[0];
}

cv::Mat3b toRgb(PXCImage *pxcImage) {
	cv::Size size;
	cv::Vec3b* data = (cv::Vec3b*)getData(pxcImage, PXCImage::PIXEL_FORMAT_RGB24, size);
	return cv::Mat3b(size.height, size.width, data);
}


cv::Mat1f toDepth(PXCImage *pxcImage) {
	cv::Size size;
	float* data = (float*)getData(pxcImage, PXCImage::PIXEL_FORMAT_DEPTH_F32, size);
	return cv::Mat1f(size.height, size.width, data);
}



void displayDepth(cv::Mat1f const &depth) {

	// Get mean for non zero pixels (depth > 0)
	// For purposes of display, we normalize pixel values to range 0-1 (ish) for float image (0-255 for 8bit image)
	double mean = cv::mean(depth, depth > 0)[0];
	cv::imshow("depth", depth / mean);
}




int main(int argc, char* argv[])
{

	PXCSession *session = PXCSession::CreateInstance();
	PXCSenseManager *sm = PXCSenseManager::CreateInstance();

	assert(session && sm && "unable to create instance");

	cv::Size frameSize = cv::Size(320, 240);
	float frameRate = 60;

	sm->EnableStream(PXCCapture::STREAM_TYPE_COLOR, frameSize.width, frameSize.height, frameRate);
	sm->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, frameSize.width, frameSize.height, frameRate);

	pxcStatus status = sm->Init();

	assert(status == PXC_STATUS_NO_ERROR && "could not open stream(s)");

	for (;;)
	{
		if (sm->AcquireFrame(true) < PXC_STATUS_NO_ERROR) break;

		PXCCapture::Sample *sample;
		sample = sm->QuerySample();

		if (sample->color) {
			cv::Mat3b frameColor = toRgb(sample->color);
			cv::imshow("Color", frameColor);
		}

		if (sample->depth) {
			cv::Mat1f depth = toDepth(sample->depth);
			displayDepth(depth);
		}

		int key = cv::waitKey(1);

		if (key == 27)
			break;

		sm->ReleaseFrame();
	}

	sm->Release();
	sm->Close();
	session->Release();
	return 0;
}
