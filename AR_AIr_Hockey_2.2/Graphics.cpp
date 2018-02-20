#include "Graphics.h"

using namespace cv;

Graphics::Graphics() {
	widthRatio_tableToGraphics = OUTPUT_IMAGE_WIDTH / table_width;
	heightRatio_tableToGraphics = OUTPUT_IMAGE_HEIGHT / table_height;
	currentFrame_holdTime = 1;
}

void Graphics::printStatusToConsole(std::string message) {
	std::cout << "STATUS: " << message << std::endl;
}

void Graphics::spawnWindow(bool makeFullscreen) {
	namedWindow(WINDOW_TITLE, CV_WINDOW_NORMAL);
	if(makeFullscreen)
		setWindowProperty(WINDOW_TITLE, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
}

bool Graphics::importResources(std::string path = "C:") {

#ifdef _WIN32
	image_startupSplash = imread(path + "\\startupSplash.png");
	image_tableTop = imread(path + "\\tableTop.png");
	image_goalPlayerOne = imread(path + "\\goalPlayerOne.png");
	image_goalPlayerTwo = imread(path + "\\goalPlayerTwo.png");
	image_winPlayerOne = imread(path + "\\winPlayerOne.png");
	image_winPlayerTwo = imread(path + "\\winPlayerTwo.png");
	image_error = imread(path + "\\error.png");
#else
	image_startupSplash = imread(path + "//startupSplash.png");
	image_tableTop = imread(path + "//tableTop.png");
	image_goalPlayerOne = imread(path + "//goalPlayerOne.png");
	image_goalPlayerTwo = imread(path + "//goalPlayerTwo.png");
	image_winPlayerOne = imread(path + "//winPlayerOne.png");
	image_winPlayerTwo = imread(path + "//winPlayerTwo.png");
	image_error = imread(path + "//error.png");
#endif

	if (image_startupSplash.empty() || image_tableTop.empty() || image_goalPlayerOne.empty() || image_goalPlayerTwo.empty() || image_winPlayerOne.empty() || image_winPlayerTwo.empty() || image_error.empty())
		return false;
	return true;
}

void Graphics::pushToScreen() {
	if (currentFrame_holdTime > 1) {
		auto frameTime = std::chrono::steady_clock::now();
		while (std::chrono::duration<long double, std::milli>(std::chrono::steady_clock::now() - frameTime).count() < currentFrame_holdTime) {
			double dur = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - frameTime).count();
			imshow(WINDOW_TITLE, screenBuffer);
			waitKey(1);
		}
	}
	else {
		imshow(WINDOW_TITLE, screenBuffer);
		waitKey(1);
	}
	currentFrame_holdTime = 1;
	screenBuffer = image_error;
}

void Graphics::drawGameplayImage() {
	image_tableTop.copyTo(screenBuffer);
	rectangle(screenBuffer, Rect(Point2d((widthRatio_tableToGraphics*table_width) - (widthRatio_tableToGraphics * WALL_PADDING_THICKNESS), heightRatio_tableToGraphics * (table_height - GOAL_WIDTH) / 2.0), Point2d((widthRatio_tableToGraphics*table_width), heightRatio_tableToGraphics * (table_height + GOAL_WIDTH) / 2.0)), Scalar(0, 0, 0), -1);
	rectangle(screenBuffer, Rect(Point2d(0, heightRatio_tableToGraphics * (table_height - GOAL_WIDTH) / 2.0), Point2d((widthRatio_tableToGraphics * WALL_PADDING_THICKNESS), heightRatio_tableToGraphics * (table_height + GOAL_WIDTH) / 2.0)), Scalar(0, 0, 0), -1);

	putText(screenBuffer, std::to_string(static_cast<int>(score_playerOne)), Point2d((widthRatio_tableToGraphics * table_width * 0.5) - 80, (heightRatio_tableToGraphics * table_height) - 60), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(50, 95, 105), 5);
	putText(screenBuffer, std::to_string(static_cast<int>(score_playerTwo)), Point2d((widthRatio_tableToGraphics * table_width * 0.5) + 40, (heightRatio_tableToGraphics * table_height) - 60), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(50, 95, 105), 5);

	circle(screenBuffer, Point2d((puck_position[0] * widthRatio_tableToGraphics), (puck_position[1] * heightRatio_tableToGraphics)), PUCK_RADIUS * widthRatio_tableToGraphics, Scalar(10, 80, 10), -1);
	circle(screenBuffer, Point2d((paddleOne_position[0] * widthRatio_tableToGraphics), (paddleOne_position[1] * heightRatio_tableToGraphics)), PADDLE_RADIUS * widthRatio_tableToGraphics, Scalar(255, 0, 0), 5);
	circle(screenBuffer, Point2d((paddleTwo_position[0] * widthRatio_tableToGraphics), (paddleTwo_position[1] * heightRatio_tableToGraphics)), PADDLE_RADIUS * widthRatio_tableToGraphics, Scalar(0, 0, 255), 5);
	currentFrame_holdTime = 1;
}

void Graphics::drawStartupSplashImage() {
	screenBuffer = image_startupSplash;
	currentFrame_holdTime = 3000;
}

void Graphics::drawGoalscoredImage(const bool wasPlayerOne) {
	if (wasPlayerOne)
		screenBuffer = image_goalPlayerOne;
	else
		screenBuffer = image_goalPlayerTwo;
	cv::imshow(WINDOW_TITLE, screenBuffer);
	waitKey(1500);
	currentFrame_holdTime = 3000;
}

void Graphics::drawGamewonImage(const bool wasPlayerOne) {
	if (wasPlayerOne)
		screenBuffer = image_winPlayerOne;
	else
		screenBuffer = image_winPlayerTwo;
	currentFrame_holdTime = 5000;
}