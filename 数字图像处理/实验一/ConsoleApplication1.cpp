#include <opencv.hpp>

int main() {
	cv::Mat image;
	image = cv::imread("E:/dog.png");
	cv::imshow("image", image);
	cv::waitKey(0);
	cv::destroyAllWindows();
	return 0;
}