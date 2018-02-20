#include "Sensor.h"

using namespace cv;

double distance(cv::Point2d a, cv::Point2d b) {
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

Sensor::Sensor(const int port) {
	sensor_ir = *(new VideoCapture(0));
	Mat setupImage;
	sensor_ir >> setupImage;
	sensorFrame_width = setupImage.cols;
	sensorFrame_height = setupImage.rows;

	SimpleBlobDetector::Params sensorDetectionEngineParameters;
	sensorDetectionEngineParameters.filterByArea = true;
	sensorDetectionEngineParameters.filterByCircularity = false;
	sensorDetectionEngineParameters.filterByColor = false;
	sensorDetectionEngineParameters.filterByConvexity = false;
	sensorDetectionEngineParameters.filterByInertia = false;
	sensorDetectionEngineParameters.minArea = 100 / pow(SENSOR_DOWNSAMPLE_RATIO, 2);
	sensorDetectionEngineParameters.maxArea = 10000 / pow(SENSOR_DOWNSAMPLE_RATIO, 2);
	sensorDetectionEngine = SimpleBlobDetector::create(sensorDetectionEngineParameters);
}

void Sensor::detectProjectionSize() {
	double projectorDistance = 1000;// use the uS sensor here
	table_width = projectorDistance * PROJECTOR_SPREAD_HORIZ;
	table_height = projectorDistance * PROJECTOR_SPREAD_VERT;
	table_center[0] = table_width / 2;
	table_center[1] = table_height / 2;
	table_centerLeft[0] = table_width / 4;
	table_centerLeft[1] = table_center[1];
	table_centerRight[0] = table_width * 3 / 4;
	table_centerRight[1] = table_center[1];
	widthRatio_sensorToTable = table_width / sensorFrame_width;
	heightRatio_sensorToTable = table_height / sensorFrame_height;
}

void Sensor::collectFrameFromCamera() {
	sensor_ir >> bufferImage;
}

void Sensor::processFrame() {
	Mat alphaImage;
	inRange(bufferImage, Scalar(0, 0, 0), Scalar(200, 200, 200), alphaImage);

	Mat dilatedAlphaImage;
	dilate(alphaImage, dilatedAlphaImage, 0, Point(-1, -1), 5);

	Mat imageForDetector;
	if (SENSOR_DOWNSAMPLE_RATIO > 1)
		resize(dilatedAlphaImage, imageForDetector, Size(sensorFrame_width / SENSOR_DOWNSAMPLE_RATIO, sensorFrame_height / SENSOR_DOWNSAMPLE_RATIO));
	else
		imageForDetector = dilatedAlphaImage;

	sensorDetectionEngine->detect(imageForDetector, detectedPoints);
}

void Sensor::updatePaddles(const double deltaTime_micros) {
	double paddleOne_lastPosition[2] = { paddleOne_position[0], paddleOne_position[1] };
	double paddleTwo_lastPosition[2] = { paddleTwo_position[0], paddleTwo_position[1] };

	if (SENSOR_DOWNSAMPLE_RATIO > 1)
		for (int i = 0; i < detectedPoints.size(); i++) {
			detectedPoints.at(i).pt.x *= SENSOR_DOWNSAMPLE_RATIO;
			detectedPoints.at(i).pt.y *= SENSOR_DOWNSAMPLE_RATIO;
		}


	int index = -1;
	double min_dist = 1e10;

	for (int i = 0; i < detectedPoints.size(); i++)
		if (distance(detectedPoints.at(i).pt, Point2d(paddleOne_lastPosition[0], paddleOne_lastPosition[1])) < min_dist && detectedPoints.at(i).pt.x < (sensorFrame_width / 2)) {
			index = i;
			min_dist = distance(detectedPoints.at(i).pt, Point2d(paddleOne_lastPosition[0], paddleOne_lastPosition[1]));
		}
	if (index != -1) {
		paddleOne_position[0] = detectedPoints.at(index).pt.x * widthRatio_sensorToTable;
		paddleOne_position[1] = detectedPoints.at(index).pt.y * heightRatio_sensorToTable;
	}

	index = -1;
	min_dist = 1e10;

	for (int i = 0; i < detectedPoints.size(); i++)
		if (distance(detectedPoints.at(i).pt, Point2d(paddleTwo_lastPosition[0], paddleTwo_lastPosition[1])) < min_dist && detectedPoints.at(i).pt.x > (sensorFrame_width / 2)) {
			index = i;
			min_dist = distance(detectedPoints.at(i).pt, Point2d(paddleTwo_lastPosition[0], paddleTwo_lastPosition[1]));
		}
	if (index != -1) {
		paddleTwo_position[0] = detectedPoints.at(index).pt.x * widthRatio_sensorToTable;
		paddleTwo_position[1] = detectedPoints.at(index).pt.y * heightRatio_sensorToTable;
	}

	paddleOne_velocity[0] = (paddleOne_position[0] - paddleOne_lastPosition[0]) / (deltaTime_micros / 1e6);
	paddleOne_velocity[1] = (paddleOne_position[1] - paddleOne_lastPosition[1]) / (deltaTime_micros / 1e6);
	paddleTwo_velocity[0] = (paddleTwo_position[0] - paddleTwo_lastPosition[0]) / (deltaTime_micros / 1e6);
	paddleTwo_velocity[1] = (paddleTwo_position[1] - paddleTwo_lastPosition[1]) / (deltaTime_micros / 1e6);
}
