#include <stdio.h>
#include <string.h>
#include <iostream>
#include "opencv2/opencv.hpp"

const std::string path_racecar_src = "../../u03/";
const std::string path_captures    = "../captures/";

void saterr(cv::Mat *hsv, cv::Mat &mask) {
	cv::inRange(hsv[1], 60, 255, mask);
}

double boxsize(double &m00, double &m01, double &m10, double &m20, double &m02) {
	return 6.0 * cv::sqrt(m00);
}

void bgr2hue(cv::Mat &bgr, cv::Mat &hue, void (*errfun)(cv::Mat*, cv::Mat&) = saterr) {
	cv::Mat hsv, mask;
	cv::Mat hsv_ch[3];
	cv::cvtColor(bgr, hsv, CV_BGR2HSV);
	cv::split(hsv, hsv_ch);
	(*errfun)(hsv_ch, mask);
	hsv_ch[0].copyTo(hue, mask);
}

double *hueHist(cv::Mat &hue) {
	double *res = (double*)calloc(256, sizeof(double));
	uchar  *p_h;

	for(int i=0; i<hue.rows; ++i) {
		p_h = hue.ptr<uchar>(i);

		for(int j=0; j<hue.cols; ++j) {
			res[(*p_h)] += 1.0;
			++p_h;
		}
	}

	double *p_r = res;
	double  s   = 1.0 / (hue.rows * hue.cols - (*p_r));
	(*p_r)      = 0.0;

	for(int i=0; i<256; ++i) {
		(*p_r) *= s;
		++p_r;
	}

	return res;
}


void hue2prob(cv::Mat &hue, double *lut, cv::Mat &prob) {
	uchar  *p_h;
	double *p_p;

	for(int i=0; i<hue.rows; ++i) {
		p_h =  hue.ptr<uchar>(i);
		p_p = prob.ptr<double>(i);

		for(int j=0; j<hue.cols; ++j) {
			*p_p = (lut[*p_h]);
			++p_h;
			++p_p;
		}
	}
}

double zMom(cv::Mat &img, int x, int y) {
	// catch not implemented moments
	if((x!=0&&y!=0) || x>2 || y>2)
		throw 1;

	double m = 0.0;

	if(x == 0) {
		if(y == 0) {
			m = sum(img)[0];
		}
		else {
			cv::Mat red;
			cv::reduce(img, red, 1, CV_REDUCE_SUM);

			for(int i=0; i<red.rows; ++i)
				m += red.at<double>(i, 0) * cv::pow(i+1, y);
		}
	}
	else {
		cv::Mat red;
		cv::reduce(img, red, 0, CV_REDUCE_SUM);
		double *p = red.ptr<double>(0);

		for(int i=0; i<red.cols; ++i) {
			m += (*p) * cv::pow(i+1, x);
			++p;
		}
	}

	return m;
}

cv::Rect camshift(cv::Mat &bgr, double *lut, cv::Rect &window, void (*errfun)(cv::Mat*, cv::Mat&) = saterr,
		double (*sfun)(double&, double&, double&, double&, double&) = boxsize) {
	cv::Rect r(window.x, window.y, window.width, window.height);
	cv::Mat hue;
	bgr2hue(bgr, hue, errfun);
	cv::Mat prob(hue.rows, hue.cols, cv::DataType<double>::type);
	hue2prob(hue, lut, prob);
	cv::Mat sub(prob, r);
	double m00   = zMom(sub, 0, 0);
	double m00_2 = 0;
	double m10   = 0;
	double m01   = 0;

	while(m00 > m00_2) {
		m10   = zMom(sub, 1, 0);
		m01   = zMom(sub, 0, 1);
		r.x  += round(m10 / m00) - r.width  / 2;
		r.y  += round(m01 / m00) - r.height / 2;
		r.x   = cv::min(cv::max(r.x, 0), bgr.cols-r.width -1);
		r.y   = cv::min(cv::max(r.y, 0), bgr.rows-r.height-1);
		sub   = cv::Mat(prob, r);
		m00_2 = m00;
		m00   = zMom(sub, 0, 0);
	}

	double m20 = zMom(sub, 2, 0);
	double m02 = zMom(sub, 0, 2);
	double s   = sfun(m00, m01, m10, m20, m02);
	double ar  = (m20*cv::pow(m00/m10, 2)) / (m02*cv::pow(m00/m01, 2));
	int w2     = round(s * ar);
	int h2     = round(s / ar);
	w2         = cv::min(cv::min(w2, r.width +2*r.x), r.width +2*(bgr.cols-r.x-r.width -1));
	h2         = cv::min(cv::min(h2, r.height+2*r.y), r.height+2*(bgr.rows-r.y-r.height-1));
	r.x       -= (w2 - r.width ) / 2;
	r.y       -= (h2 - r.height) / 2;
	r.width    = w2;
	r.height   = h2;
	return r;
}

int main(int, char**) {
	cv::Rect win(465, 250, 190, 110);
	cv::Mat probe = cv::imread(path_racecar_src + "racecar.png", CV_LOAD_IMAGE_COLOR)(win);
	cv::Mat p_hue;
	bgr2hue(probe, p_hue);
	double *lut = hueHist(p_hue);
	cv::VideoCapture cap(path_racecar_src + "racecar.avi");

	if(!cap.isOpened())
		return 1;

	cv::VideoWriter out(path_captures + "result.avi", cap.get(CV_CAP_PROP_FOURCC), (int)cap.get(CV_CAP_PROP_FPS),
		cv::Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH), (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT)));
	cv::Mat frame;
	cv::namedWindow("racecar");
	int i=0;
	int j=0;

	while(cap.read(frame)) {
		win = camshift(frame, lut, win);
		cv::rectangle(frame, cv::Point(win.x, win.y), cv::Point(win.x+win.width, win.y+win.height), cv::Scalar(0, 0, 255), 3);
		cv::imshow("racecar", frame);
		cv::waitKey(30);
		out.write(frame);

		if(i==5 || i==50 || i==100 || i==200 || i==250 || i==303) {
			std::stringstream s;
			s << path_captures << "capture_1-" << (++j) << ".png";
			cv::imwrite(s.str(), frame);
		}

		++i;
	}

	out.release();
	return 0;
}
