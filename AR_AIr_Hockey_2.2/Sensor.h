#pragma once
#include "GameData.h"

class Sensor {
public:
	Sensor(const int);
	void detectProjectionSize();
	void collectFrameFromCamera();
	void processFrame();
	void updatePaddles(const double);
private:
	double widthRatio_sensorToTable;
	double heightRatio_sensorToTable;
	double sensorFrame_width;
	double sensorFrame_height;

	std::vector<cv::KeyPoint> detectedPoints;

	cv::Mat bufferImage;
	cv::Ptr<cv::SimpleBlobDetector> sensorDetectionEngine;
	cv::VideoCapture sensor_ir;
};