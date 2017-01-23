#ifndef HOG_H_
#define HOG_H_

#include "opencv2/opencv.hpp"

class Hist {
public:
	Hist(int _rows, int _cols, int _step, int _off);
	int rows, cols, s, o;
	double ***h;
	double** &operator[](int i) {
		return h[i];
	};
};

class HCells : public Hist{
public:
	HCells(cv::Mat *grads, int &size);
};

class HBlocks : public Hist{
public:
	HBlocks(HCells &cells, int &size);
};

#endif /* HOG_H_ */
