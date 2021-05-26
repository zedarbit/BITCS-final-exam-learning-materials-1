#include <opencv.hpp>
#include <string>
#include "slic.h"

int main(int argc, char *argv[]){
    std::string path="E:/dog.png";
	cv::Mat image = cv::imread(path);
	Superpixels SLIC(image);
	cv::Mat Sp_image = SLIC.viewSuperpixels();
	cv::Mat Cl_image = SLIC.colorSuperpixels();
	cv::imshow("image", image);
	cv::imshow("boundaries", Sp_image);
	cv::imshow("cluster", Cl_image);
	cv::waitKey(0);
	return 0;
}
