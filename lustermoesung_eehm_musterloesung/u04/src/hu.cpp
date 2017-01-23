#include <stdio.h>
#include <string.h>
#include <iostream>
#include "opencv2/opencv.hpp"
#include "hu.h"

const std::string path_images   = "../";
const std::string path_captures = "../captures/";

double mo_M(cv::Mat &img, int x, int y) {
	double m = 0.0;

	for(int i=0; i<img.rows; ++i) {
		uchar *p = img.ptr<uchar>(i);

		for(int j=0; j<img.cols; ++j) {
			m += (*p) * cv::pow(i+1, x) * cv::pow(j+1, y);
			++p;
		}
	}

	return m;
}

M_mo mo_mo(cv::Mat &img) {
	M_mo res;
	res.m00 = mo_M(img, 0, 0);
	res.m01 = mo_M(img, 0, 1);
	res.m10 = mo_M(img, 1, 0);
	res.m11 = mo_M(img, 1, 1);
	res.m02 = mo_M(img, 0, 2);
	res.m12 = mo_M(img, 1, 2);
	res.m20 = mo_M(img, 2, 0);
	res.m21 = mo_M(img, 2, 1);
	res.m03 = mo_M(img, 0, 3);
	res.m30 = mo_M(img, 3, 0);
	return res;
}

M_mo mo_mu(cv::Mat &img) {
	M_mo mo   = mo_mo(img);
	double xc = mo.m10 / mo.m00;
	double yc = mo.m01 / mo.m00;
	M_mo res;
	res.m00 = mo.m00;
	res.m01 = 0.0;
	res.m10 = 0.0;
	res.m11 = mo.m11 - yc * mo.m00;
	res.m02 = mo.m02 - yc * mo.m01;
	res.m12 = mo.m12 - 2.0 * yc * mo.m11 - xc * mo.m02 + 2.0 * cv::pow(yc, 2) * mo.m10;
	res.m20 = mo.m20 - xc * mo.m10;
	res.m21 = mo.m21 - 2.0 * xc * mo.m11 - yc * mo.m20 + 2.0 * cv::pow(xc, 2) * mo.m01;
	res.m03 = mo.m03 - 3.0 * yc * mo.m02 + 2.0 * cv::pow(yc, 2) * mo.m01;
	res.m30 = mo.m30 - 3.0 * xc * mo.m20 + 2.0 * cv::pow(xc, 2) * mo.m10;
	return res;
}

M_hu mo_hu(cv::Mat &img) {
	M_mo mo = mo_mu(img);
	M_hu res;
	res.h1 = mo.m20 + mo.m02;
	res.h2 = cv::pow(mo.m20 - mo.m02, 2) + cv::pow(2.0 * mo.m11, 2);	
	res.h3 = cv::pow(mo.m30 - 3.0 * mo.m12, 2) + cv::pow(3.0 * mo.m21 - mo.m03, 2);
	res.h4 = cv::pow(mo.m30 + mo.m12, 2) + cv::pow(mo.m21 + mo.m03, 2);
	res.h5 = (mo.m30 - 3.0 * mo.m12) * (mo.m30 + mo.m12) * (cv::pow(mo.m30 + mo.m12, 2) - 3.0 * cv::pow(mo.m21 + mo.m03, 2))
			+ (3.0 * mo.m21 - mo.m03) * (mo.m21 + mo.m03) * (3.0 * cv::pow(mo.m30 + mo.m12, 2) - cv::pow(mo.m21 + mo.m03, 2));
	res.h6 = (mo.m20 - mo.m02) * (cv::pow(mo.m30 + mo.m12, 2) - cv::pow(mo.m21 + mo.m03, 2)) + 4.0 * mo.m11 * (mo.m30 + mo.m12) * (mo.m21 + mo.m03);
	res.h7 = (3.0 * mo.m21 - mo.m03) * (mo.m30 + mo.m12) * (cv::pow(mo.m30 + mo.m12, 2) - 3.0 * cv::pow(mo.m21 + mo.m03, 2))
			- (mo.m30 - 3.0 * mo.m12) * (mo.m21 + mo.m03) * (3.0 * cv::pow(mo.m30 + mo.m12, 2) - cv::pow(mo.m21 + mo.m03, 2));
	return res;
}

cv::Mat bgr2bin(const cv::Mat &bgr) {
	cv::Mat gray, bin;
	cv::cvtColor(bgr, gray, CV_BGR2GRAY);
	cv::threshold(gray, bin, 1, 255, cv::THRESH_BINARY);
	return bin;
}

int main(int, char**) {
	cv::Mat stack       = bgr2bin(cv::imread(path_images + "heystack.png"));
	cv::Mat needle      = bgr2bin(cv::imread(path_images + "needle.png"));
	cv::Mat stack_cntr  = stack.clone();
	cv::Mat stack_draw  = stack.clone();
	cv::Mat result      = stack.clone();
	cv::vector<cv::vector<cv::Point> > contour;
	cv::vector<cv::Vec4i> hierarchy;
	cv::findContours(stack_cntr, contour, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	cv::vector<cv::Rect> rects;
	M_hu needle_hu  = mo_hu(needle);
	printf("\nNeedle Hu-moments:\nh1: %.1f\nh2: %.1f\nh3: %.1f\n"
		"h4: %.1f\nh5: %.1f\nh6: %.1f\nh7: %.1f\n\n",
		needle_hu.h1, needle_hu.h2, needle_hu.h3, needle_hu.h4,
		needle_hu.h5, needle_hu.h6, needle_hu.h7);
	int bestMatch   = 0;
	double error    = 1.7e+308;

	for(int i=0; i<contour.size(); ++i) {
		cv::Rect r = boundingRect(cv::Mat(contour[i]));
		cv::rectangle(stack_draw, cv::Point(r.x, r.y), cv::Point(r.x+r.width, r.y+r.height), cv::Scalar(255, 255, 255), 2);
		rects.push_back(r);
		cv::Mat sub = stack(r);
		M_hu hu     = mo_hu(sub);
		double e    = cv::pow(hu.h1 - needle_hu.h1, 2) + cv::pow(hu.h2 - needle_hu.h2, 2)
				+ cv::pow(hu.h3 - needle_hu.h3, 2) + cv::pow(hu.h4 - needle_hu.h4, 2)
				+ cv::pow(hu.h5 - needle_hu.h5, 2) + cv::pow(hu.h6 - needle_hu.h6, 2)
				+ cv::pow(hu.h7 - needle_hu.h7, 2);

		if(e < error) {
			bestMatch = i;
			error     = e;
		}
	}

	cv::rectangle(result, cv::Point(rects[bestMatch].x, rects[bestMatch].y), cv::Point(rects[bestMatch].x+rects[bestMatch].width, rects[bestMatch].y+rects[bestMatch].height), cv::Scalar(255, 255, 255), 2);
	cv::namedWindow("boxes");
	cv::namedWindow("result");
	cv::imshow("boxes", stack_draw);
	cv::imshow("result", result);
	cv::imwrite(path_captures + "capture_2-1.png", stack_draw);
	cv::imwrite(path_captures + "capture_2-2.png", result);
	cv::waitKey(0);
	return 0;
}
