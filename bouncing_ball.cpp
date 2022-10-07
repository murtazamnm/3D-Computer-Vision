//A ball that just bounces

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#define width 600
#define height 500
cv::Mat image;
int xCoord, yCoord;
int x, y;
int dx, dy;

void redraw() {

	cv::rectangle(image, cv::Point(0, 0), cv::Point(width, height), cv::Scalar(0, 0, 0), cv::FILLED);
	cv::rectangle(image, cv::Point(xCoord, yCoord), cv::Point(xCoord + 100, yCoord + 10), cv::Scalar(255, 100, 0), cv::FILLED);
	cv::circle(image, cv::Point(x, y), 20, cv::Scalar(255,255,255), 2);
	cv::imshow("Display Window", image);

}

void MouseCallBackFunc(int event, int x,int y, int flags, void* userdata) {
	if (event == cv::EVENT_LBUTTONDOWN) {
		xCoord = x;
		redraw();
	}
}

int main() {
	image = cv::Mat::zeros(height, width, CV_8UC3);
	xCoord = 300;
	yCoord = 450;
	x = 80;
	y = 80;
	dx = 7;
	dy = 10;
	redraw();

	cv::namedWindow("Display Window", cv::WINDOW_AUTOSIZE);
	cv::imshow("Display Window", image);
	cv::setMouseCallback("Display Window", MouseCallBackFunc, NULL);

	int key;
	int jump = 13;
	while (1) {
		key = cv::waitKey(100);
		if (key == 27) break;
		switch (key) {
		case 'a':
			if(xCoord <= 0){
				//Do nothing
			}
			else{
				xCoord -= jump;
				break;
			}
		case 'd':
			if(xCoord >= 500){
				//Do nothing
			}
			else{
				xCoord += jump;
				break;
			}
		}
		if (y >= 430) {
			if (x >= xCoord && x <= xCoord + 100) {
				dy *= -1;
			}
			else {
				break;
			}
		}
		else if (y-20 <= 0) {
			dy *= -1;
		}
		if ((x+20) >= 600) {
			dx *= -1;
		}
		else if ((x-20) <= 0) {
			dx *= -1;
		}
		x += dx;
		y += dy;
		redraw();
	}
	return 0;
}


