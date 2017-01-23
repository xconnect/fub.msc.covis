#include <stdio.h>
#include <string.h>
#include <iostream>
#include "hog.h"

const std::string path_images   = "./";
const std::string path_captures = "./captures/";

void gradients(cv::Mat &bgr, cv::Mat *grads) {
	cv::Mat sob_x, sob_y;
	grads[0] = cv::Mat(bgr.rows, bgr.cols, CV_8U);
	grads[1] = cv::Mat(bgr.rows, bgr.cols, CV_64F);
	cv::Sobel(bgr, sob_x, CV_64F, 1, 0, 3);
	cv::Sobel(bgr, sob_y, CV_64F, 0, 1, 3);

	for(int y=0;y<bgr.rows; ++y)
		for(int x=0; x<bgr.cols; ++x) {
			cv::Vec3d bgr_x = sob_x.at<cv::Vec3d>(y, x);
			cv::Vec3d bgr_y = sob_y.at<cv::Vec3d>(y, x);

			double atan   = 0.0;
			double weight = 0.0;

			for(int d=0; d<3; ++d) {
				double w = cv::sqrt(bgr_x[d] * bgr_x[d] + bgr_y[d] * bgr_y[d]);

				if(w > weight) {
					atan   = atan2(-bgr_y[d], bgr_x[d]);

					if(atan < 0)
						atan = M_PI + atan;

					weight = w;
				}
			}

			uint8_t atan_bin = 8;

			for(int i=0; i<8; ++i)
				if(atan < (i+1) * M_PI / 9.0) {
					atan_bin = i;
					break;
				}

			grads[0].at<uint8_t>(y, x) = atan_bin;
			grads[1].at<double>(y, x)  = weight;
	}
}

void drawGradients(cv::Mat *grads) {
	cv::Mat img, h, v;
	grads[0].convertTo(h, CV_32F);
	h /= 9;
	double min, max;
	cv::minMaxLoc(grads[1], &min, &max);
	v = grads[1] / max;
	v.convertTo(v, CV_32F);
	std::vector<cv::Mat> hsv;
	hsv.push_back(h);
	hsv.push_back(cv::Mat::ones(grads[0].rows, grads[0].cols, CV_32F));
	hsv.push_back(v);
	cv::merge(hsv, img);
	img *= 255;
	img.convertTo(img, CV_8U);
	cv::cvtColor(img, img, CV_HSV2BGR);
	cv::namedWindow("gradients");
	cv::imshow("gradients", img);
	cv::imwrite(path_captures + "gradients.png", img);
}

Hist::Hist(int _rows, int _cols, int _step, int _off) :
			rows(_rows),
			cols(_cols),
			s(_step),
			o(_off),
			h(0)
{}

HCells::HCells(cv::Mat *grads, int &size) :
			Hist(grads[0].rows / size, grads[0].cols / size, size, size / 2)
{
	h = new double**[rows];

	for(int i=0; i<rows; ++i) {
		h[i] = new double*[cols];

		for(int j=0; j<cols; ++j)
			h[i][j] = new double[9]();
	}

	for(int i=0; i<grads[0].rows - (grads[0].rows%size); ++i)
		for(int j=0; j<grads[0].cols - (grads[0].cols%size); ++j)
			h[i/size][j/size][grads[0].at<uint8_t>(i, j)] += grads[1].at<double>(i, j);
}

HBlocks::HBlocks(HCells &cells, int &size) :
			Hist(cells.rows - size + 1, cells.cols - size + 1, cells.s, cells.s * size / 2)
{
	h = new double**[rows];

	for(int i=0; i<rows; ++i) {
		h[i] = new double*[cols];

		for(int j=0; j<cols; ++j)
			h[i][j] = new double[9]();
	}

	for(int i=0; i<rows; ++i)
		for(int j=0; j<cols; ++j) {
			for(int y=0; y<size; ++y)
				for(int x=0; x<size; ++x)
					for(int k=0; k<9; ++k)
						h[i][j][k] += cells[i+y][j+x][k];

			double max = 0;

			for(int k=0; k<9; ++k)
				max = std::max(max, h[i][j][k]);

			if(max != 0.0)
				for(int k=0; k<9; ++k)
					h[i][j][k] /= max;
		}
}

HBlocks hBlocks(cv::Mat &img, int &s_cell, int &s_block) {
	cv::Mat grads[2];
	gradients(img, grads);
	HCells cells(grads, s_cell);
	HBlocks blocks(cells, s_block);
	return blocks;
}

void drawH(std::string title, Hist h, cv::Mat g, bool overlay=false) {
	cv::Mat img = cv::Mat::zeros(g.rows, g.cols, CV_64F);
	cv::Scalar col(255, 255, 255);

	if(overlay) {
		double min, max;
		g.copyTo(img);
		cv::minMaxLoc(g, &min, &max);
		img /= max;
	}

	for(int i=0; i<h.rows; ++i) {
		for(int j=0; j<h.cols; ++j) {
			uint8_t ang = 0;
			double mag  = 0.0;

			for(int k=0; k<9; ++k)
				if(h[i][j][k] > mag) {
					ang = k;
					mag = h[i][j][k];
				}

			if(mag > 0.0) {
				cv::Point s, t;
				int cy = h.s*i + h.o;
				int cx = h.s*j + h.o;
				int p[] = {-h.o, -2*h.o/3, -h.o/3, 0, h.o/3, 2*h.o/3, h.o};

				switch(ang) {
					case 0:
						s = cv::Point(cx+p[0], cy+p[3]);
						t = cv::Point(cx+p[6], cy+p[3]);
						break;
					case 1:
						s = cv::Point(cx+p[0], cy+p[5]);
						t = cv::Point(cx+p[6], cy+p[1]);
						break;
					case 2:
						s = cv::Point(cx+p[1], cy+p[6]);
						t = cv::Point(cx+p[5], cy+p[0]);
						break;
					case 3:
						s = cv::Point(cx+p[2], cy+p[6]);
						t = cv::Point(cx+p[4], cy+p[0]);
						break;
					case 4:
						s = cv::Point(cx+p[3], cy+p[6]);
						t = cv::Point(cx+p[3], cy+p[0]);
						break;
					case 5:
						s = cv::Point(cx+p[4], cy-p[6]);
						t = cv::Point(cx+p[2], cy+p[0]);
						break;
					case 6:
						s = cv::Point(cx+p[5], cy+p[6]);
						t = cv::Point(cx+p[1], cy+p[0]);
						break;
					case 7:
						s = cv::Point(cx+p[6], cy+p[1]);
						t = cv::Point(cx+p[0], cy+p[5]);
						break;
					default:
						s = cv::Point(cx+p[6], cy+p[3]);
						t = cv::Point(cx+p[0], cy+p[3]);
						break;
				}

				cv::line(img, s, t, col.mul(mag));
			}
		}
	}

	cv::namedWindow(title);
	cv::imshow(title, img);
	cv::imwrite(path_captures + title + ".png", img);
}

double *descriptor(HBlocks &h, cv::Rect roi) {
	double *d = new double[h.rows * h.cols * 9]();
	double *e = d;

	for(int i=roi.y; i<roi.y+roi.height; ++i)
		for(int j=roi.x; j<roi.x+roi.width; ++j)
			for(int k=0; k<9; ++k) {
				*e = h[i][j][k];
				++e;
			}

	return d;
}

cv::Rect find(cv::Mat &probe, cv::Mat &img, int &s_cell, int &s_block) {
	HBlocks b_prb = hBlocks(probe, s_cell, s_block);
	HBlocks b_img = hBlocks(img,   s_cell, s_block);
	cv::Rect r(0, 0, b_prb.cols, b_prb.rows);
	double *d_prb = descriptor(b_prb, r);
	double error = 2e+32;
	int x        = 0;
	int y        = 0;

	for(int i=0; i<b_img.rows-b_prb.rows; ++i)
		for(int j=0; j<b_img.cols-b_prb.cols; ++j) {
			r.x = j;
			r.y = i;
			double *d_img = descriptor(b_img, r);
			double  e     = 0;

			for(int k=0; k<b_prb.rows*b_prb.cols*9; ++k)
				e += sqrt((d_img[k]-d_prb[k]) * (d_img[k]-d_prb[k]));

			if(e < error) {
				error = e;
				y     = i;
				x     = j;
			}
		}

	return cv::Rect(x*s_cell, y*s_cell, probe.cols, probe.rows);
}

std::vector<cv::Rect> find(cv::Mat &probe, cv::Mat &img, double th, int &s_cell, int &s_block) {
	std::vector<cv::Rect> res;
	HBlocks b_prb = hBlocks(probe, s_cell, s_block);
	HBlocks b_img = hBlocks(img,   s_cell, s_block);
	cv::Rect r(0, 0, b_prb.cols, b_prb.rows);
	double *d_prb = descriptor(b_prb, r);

	for(int i=0; i<b_img.rows-b_prb.rows; ++i)
		for(int j=0; j<b_img.cols-b_prb.cols; ++j) {
			r.x = j;
			r.y = i;
			double *d_img = descriptor(b_img, r);
			double  e     = 0;

			for(int k=0; k<b_prb.rows*b_prb.cols*9; ++k)
				e += sqrt((d_img[k]-d_prb[k]) * (d_img[k]-d_prb[k]));

			if(e < th)
				res.push_back(cv::Rect(j*s_cell, i*s_cell, probe.cols, probe.rows));
		}

	return res;
}

int main(int, char**) {
	int s_cell    = 8;
	int s_block   = 2;
	cv::Mat probe = cv::imread(path_images + "person.png");
	cv::Mat p1_1    = cv::imread(path_images + "people.png");
	cv::Mat p2_1    = cv::imread(path_images + "people2.png");
	cv::Mat p1_2    = p1_1.clone();
	cv::Mat p2_2    = p2_1.clone();
	cv::Mat grads[2];
	gradients(p1_1, grads);
	HCells cells(grads, s_cell);
	HBlocks blocks(cells, s_block);
	cv::Rect r1 = find(probe, p1_1, s_cell, s_block);
	cv::Rect r2 = find(probe, p2_1, s_cell, s_block);
	cv::rectangle(p1_1, cv::Point(r1.x, r1.y), cv::Point(r1.x+r1.width, r1.y+r1.height), cv::Scalar(255, 255, 255), 2);
	cv::rectangle(p2_1, cv::Point(r2.x, r2.y), cv::Point(r2.x+r2.width, r2.y+r2.height), cv::Scalar(255, 255, 255), 2);
	std::vector<cv::Rect> rs_1 = find(probe, p1_2, 2325, s_cell, s_block);
	std::vector<cv::Rect> rs_2 = find(probe, p2_2, 2340, s_cell, s_block);

	for(int i=0; i<rs_1.size(); ++i)
		cv::rectangle(p1_2, cv::Point(rs_1[i].x, rs_1[i].y), cv::Point(rs_1[i].x+rs_1[i].width, rs_1[i].y+rs_1[i].height), cv::Scalar(255, 255, 255), 2);

	for(int i=0; i<rs_2.size(); ++i)
		cv::rectangle(p2_2, cv::Point(rs_2[i].x, rs_2[i].y), cv::Point(rs_2[i].x+rs_2[i].width, rs_2[i].y+rs_2[i].height), cv::Scalar(255, 255, 255), 2);

	drawGradients(grads);
	drawH("cells", cells, grads[1]);
	drawH("blocks", blocks, grads[1]);
	cv::namedWindow("people 1");
	cv::imshow("people 1", p1_1);
	cv::imwrite(path_captures + "people1.png", p1_1);
	cv::namedWindow("people 2");
	cv::imshow("people 2", p2_1);
	cv::imwrite(path_captures + "people2.png", p2_1);
	cv::namedWindow("people 1 all");
	cv::imshow("people 1 all", p1_2);
	cv::imwrite(path_captures + "people1-2.png", p1_2);
	cv::namedWindow("people 2 all");
	cv::imshow("people 2 all", p2_2);
	cv::imwrite(path_captures + "people2-2.png", p2_2);
	cv::waitKey(0);
	return 0;
}
