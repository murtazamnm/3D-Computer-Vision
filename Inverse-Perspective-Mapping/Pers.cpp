#include "MatrixReaderWriter.h"
#include "opencv2/calib3d.hpp"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include <iostream>
#include <stdio.h>

using PointPairs = std::vector<std::pair<cv::Point2f, cv::Point2f>>;

struct NormalizedData
{
	cv::Mat T1;
	cv::Mat T2;
	PointPairs pointPairs;
};

NormalizedData normalizeData(const PointPairs& pointPairs)
{
	int ptsNum = pointPairs.size();

	// calculate means (they will be the center of coordinate systems)
	float mean1x = 0.0, mean1y = 0.0, mean2x = 0.0, mean2y = 0.0;
	for (int i = 0; i < ptsNum; i++)
	{
		auto& pp = pointPairs[i];
		mean1x += pp.first.x;
		mean1y += pp.first.y;
		mean2x += pp.second.x;
		mean2y += pp.second.y;
	}

	mean1x /= ptsNum;
	mean1y /= ptsNum;
	mean2x /= ptsNum;
	mean2y /= ptsNum;

	float spread1x = 0.0, spread1y = 0.0, spread2x = 0.0, spread2y = 0.0;
	for (int i = 0; i < ptsNum; i++)
	{
		auto& pp = pointPairs[i];
		spread1x += (pp.first.x - mean1x) * (pp.first.x - mean1x);
		spread1y += (pp.first.y - mean1y) * (pp.first.y - mean1y);
		spread2x += (pp.second.x - mean2x) * (pp.second.x - mean2x);
		spread2y += (pp.second.y - mean2y) * (pp.second.y - mean2y);
	}

	spread1x /= ptsNum;
	spread1y /= ptsNum;
	spread2x /= ptsNum;
	spread2y /= ptsNum;

	cv::Mat offs1 = cv::Mat::eye(3, 3, CV_32F);
	cv::Mat offs2 = cv::Mat::eye(3, 3, CV_32F);
	cv::Mat scale1 = cv::Mat::eye(3, 3, CV_32F);
	cv::Mat scale2 = cv::Mat::eye(3, 3, CV_32F);

	offs1.at<float>(0, 2) = -mean1x;
	offs1.at<float>(1, 2) = -mean1y;

	offs2.at<float>(0, 2) = -mean2x;
	offs2.at<float>(1, 2) = -mean2y;

	const float sqrt2 = sqrt(2);

	scale1.at<float>(0, 0) = sqrt2 / sqrt(spread1x);
	scale1.at<float>(1, 1) = sqrt2 / sqrt(spread1y);

	scale2.at<float>(0, 0) = sqrt2 / sqrt(spread2x);
	scale2.at<float>(1, 1) = sqrt2 / sqrt(spread2y);

	NormalizedData result;
	result.T1 = scale1 * offs1;
	result.T2 = scale2 * offs2;

	for (int i = 0; i < ptsNum; i++)
	{
		cv::Point2f p1;
		cv::Point2f p2;

		auto& pp = pointPairs[i];
		p1.x = sqrt2 * (pp.first.x - mean1x) / sqrt(spread1x);
		p1.y = sqrt2 * (pp.first.y - mean1y) / sqrt(spread1y);

		p2.x = sqrt2 * (pp.second.x - mean2x) / sqrt(spread2x);
		p2.y = sqrt2 * (pp.second.y - mean2y) / sqrt(spread2y);

		result.pointPairs.emplace_back(p1, p2);
	}

	return result;
}

cv::Mat calcHomography(PointPairs& pointPairs)
{
	// TODO
	// 1. normalize input
	// 2. fill A matrix
	// 3. calculate eigenvalues and eigenvectors
	// 4. fill H matrix
	// 5. denormalize output

	NormalizedData normalizedData = normalizeData(pointPairs);
	pointPairs = normalizedData.pointPairs;

	const size_t ptsNum = pointPairs.size();
	cv::Mat A(2 * ptsNum, 9, CV_32F);

	for (int i = 0; i < ptsNum; i++)
	{
		float u1 = pointPairs[i].first.x;
		float v1 = pointPairs[i].first.y;

		float u2 = pointPairs[i].second.x;
		float v2 = pointPairs[i].second.y;

		A.at<float>(2 * i, 0) = u1;
		A.at<float>(2 * i, 1) = v1;
		A.at<float>(2 * i, 2) = 1.0f;
		A.at<float>(2 * i, 3) = 0.0f;
		A.at<float>(2 * i, 4) = 0.0f;
		A.at<float>(2 * i, 5) = 0.0f;
		A.at<float>(2 * i, 6) = -u2 * u1;
		A.at<float>(2 * i, 7) = -u2 * v1;
		A.at<float>(2 * i, 8) = -u2;

		A.at<float>(2 * i + 1, 0) = 0.0f;
		A.at<float>(2 * i + 1, 1) = 0.0f;
		A.at<float>(2 * i + 1, 2) = 0.0f;
		A.at<float>(2 * i + 1, 3) = u1;
		A.at<float>(2 * i + 1, 4) = v1;
		A.at<float>(2 * i + 1, 5) = 1.0f;
		A.at<float>(2 * i + 1, 6) = -v2 * u1;
		A.at<float>(2 * i + 1, 7) = -v2 * v1;
		A.at<float>(2 * i + 1, 8) = -v2;
	}

	cv::Mat eVecs(9, 9, CV_32F), eVals(9, 9, CV_32F);
	std::cout << A << std::endl;
	cv::eigen(A.t() * A, eVals, eVecs);

	std::cout << eVals << std::endl;
	std::cout << eVecs << std::endl;

	cv::Mat H(3, 3, CV_32F);
	for (int i = 0; i < 9; i++)
		H.at<float>(i / 3, i % 3) = eVecs.at<float>(8, i);

	std::cout << H << std::endl;

	// normalize
	H = H * (1.0 / H.at<float>(2, 2));
	std::cout << H << std::endl;

	return normalizedData.T2.inv() * H * normalizedData.T1;
}


// tranformation of images
void transformImage(const cv::Mat& origImg, cv::Mat& newImage, const cv::Mat& tr, bool isPerspective)
{
	cv::Mat invTr = tr.inv();
	const int WIDTH = origImg.cols;
	const int HEIGHT = origImg.rows;

	const int newWIDTH = newImage.cols;
	const int newHEIGHT = newImage.rows;

	for (int x = 0; x < newWIDTH; x++)
	{
		for (int y = 0; y < newHEIGHT; y++)
		{
			cv::Mat pt(3, 1, CV_32F);
			pt.at<float>(0, 0) = x;
			pt.at<float>(1, 0) = y;
			pt.at<float>(2, 0) = 1.0;

			cv::Mat ptTransformed = invTr * pt;
			if (isPerspective)
				ptTransformed = (1.0 / ptTransformed.at<float>(2, 0)) * ptTransformed;

			int newX = round(ptTransformed.at<float>(0, 0));
			int newY = round(ptTransformed.at<float>(1, 0));

			if ((newX >= 0) && (newX < WIDTH) && (newY >= 0) && (newY < HEIGHT))
				newImage.at<cv::Vec3b>(y, x) = origImg.at<cv::Vec3b>(newY, newX);
		}
	}
}

void convertCoordinates(std::vector<std::pair<cv::Point2f, cv::Point2f>>& pointPairs, MatrixReaderWriter mtxrw) {

	int r = mtxrw.rowNum;
	int c = mtxrw.columnNum;
	for (int i = 0; i < mtxrw.columnNum; i++)
	{
		std::pair<cv::Point2f, cv::Point2f> currPts;
		currPts.first = cv::Point2f((float)mtxrw.data[i], (float)mtxrw.data[c + i]);
		currPts.second = cv::Point2f((float)mtxrw.data[2 * c + i], (float)mtxrw.data[3 * c + i]);
		pointPairs.emplace_back(currPts);
		std::cout << currPts.first << " * " << currPts.second << "\n";
	}

}

int main(int argc, char** argv)
{
	// loading the coordinates
	MatrixReaderWriter mtxrw1("features.txt");
	MatrixReaderWriter mtxrw2("rect.txt");

	// move coordinates from file to a vector
	std::vector<std::pair<cv::Point2f, cv::Point2f>> pointPairs1;
	std::vector<std::pair<cv::Point2f, cv::Point2f>> pointPairs2;
	convertCoordinates(pointPairs1, mtxrw1);
	convertCoordinates(pointPairs2, mtxrw2);

	// calculating homography matrix to stitch the images
	cv::Mat H1 = calcHomography(pointPairs1);
	cv::Mat H2 = calcHomography(pointPairs1);
	// set paths
	std::string left_path = "input/dev1_";
	std::string right_path = "input/dev2_";
	std::string res_path = "data/";

	int numberof_images = 13;

	for (int i = 1;i < numberof_images + 1;i++) {

		cv::Mat image1 = cv::imread(left_path + std::to_string(i) + ".jpg");
		cv::Mat image2 = cv::imread(right_path + std::to_string(i) + ".jpg");
		cv::Mat transformedImage = cv::Mat::zeros(1.5 * image1.size().height, 3.0 * image1.size().width, image1.type());
		cv::Mat final_image = cv::Mat::zeros(transformedImage.size().height - 700, transformedImage.size().width - 3500, transformedImage.type());
		transformImage(image1, transformedImage, H1, true);
		transformImage(image2, transformedImage, cv::Mat::eye(3, 3, CV_32F), true);
		transformImage(transformedImage, final_image, H2, true);
		cv::imwrite(res_path + std::to_string(i) + ".png", final_image);

		std::cout << i << std::endl;
	}
	for (int i = 1;i < numberof_images + 1;i++) {
		cv::Mat frame = cv::imread(res_path + std::to_string(i) + ".png");
		cv::VideoWriter video("output.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 10, cv::Size(frame.cols, frame.rows));
		video.write(frame);
	}
	return 0;
}
