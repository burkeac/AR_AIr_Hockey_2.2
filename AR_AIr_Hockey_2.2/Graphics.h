#pragma once
#include "GameData.h"

class Graphics {
public:
	Graphics();
	void printStatusToConsole(std::string message);
	void spawnWindow(bool);
	bool importResources(std::string);

	void pushToScreen();

	void drawGameplayImage();
	void drawStartupSplashImage();
	void drawGoalscoredImage(const bool);
	void drawGamewonImage(const bool);

private:
	double widthRatio_tableToGraphics;
	double heightRatio_tableToGraphics;

	int currentFrame_holdTime;

	cv::Mat screenBuffer;

	cv::Mat image_startupSplash;
	cv::Mat image_tableTop;
	cv::Mat image_goalPlayerOne;
	cv::Mat image_goalPlayerTwo;
	cv::Mat image_winPlayerOne;
	cv::Mat image_winPlayerTwo;
	cv::Mat image_error;
};