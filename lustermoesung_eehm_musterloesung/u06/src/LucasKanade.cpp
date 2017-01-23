#include <fstream>
#include <opencv2/opencv.hpp>

int main(int, char**) {
	std::ifstream file("corners.cvs");
	std::vector<cv::Point2f> corners_prev;
    int a, b;
    char c;

    while (file >> a >> c >> b)
        corners_prev.push_back(cv::Point2f(a, b));

	cv::VideoCapture vc("beedance.avi");
	cv::VideoWriter vw("result.avi", CV_FOURCC('M','P','E','G'), 100, cv::Size(640, 480), true);
	cv::Mat frame_prev, frame_next;
	cv::Mat res = cv::Mat(480, 640, CV_8UC3, cv::Scalar(255, 255, 255));
	cv::Scalar col(0, 255, 0);
	cv::namedWindow("LK");
	vc.read(frame_prev);
	int i = 0;

	while(vc.read(frame_next)) {
		++i;
		std::vector<cv::Point2f> corners_next;
		std::vector<uchar> status;
		std::vector<float> err;
		calcOpticalFlowPyrLK(frame_prev, frame_next, corners_prev, corners_next, status, err);
		frame_next.copyTo(frame_prev);
		std::vector<cv::Point2f>::const_iterator it_cp = corners_prev.begin();
		std::vector<cv::Point2f>::const_iterator it_cn = corners_next.begin();

		for(std::vector<uchar>::const_iterator it_st=status.begin(); it_st!=status.end(); ++it_st) {
			if(*it_st == 1) {
				cv::circle(frame_next, *it_cn, 2, col, 2);

				if(i>10 && i<890) // HACK: uncoole daten an anfang und ende weglassen
					cv::line(res, *it_cp, *it_cn, col);
			}

			++it_cp;
			++it_cn;
		}

		corners_prev = corners_next;
		vw.write(frame_next);
		cv::imshow("LK", frame_next);
		cv::waitKey(1);
	}

	vw.release();
	cv::imshow("LK", res);
	cv::imwrite("capture.png", res);
	cv::waitKey(0);
	return 0;
}
