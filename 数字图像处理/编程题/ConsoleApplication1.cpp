#include <iostream>
#include <opencv.hpp>
#include <string>
#include <vector>
#include <math.h>

class Experiment1
{
private:
	// 滤波器名称（Gaussian和Laplacian）
	std::vector<std::string> filter_name;
	std::vector<std::string> pic_color;

	// 原始图像（灰度图和彩色图）
	cv::Mat original_color_image;
	cv::Mat original_gray_image;

	// 处理后的图像
	std::vector<cv::Mat> color_image_process;
	std::vector<cv::Mat> gray_image_process;

	// 梯度幅值图像
	cv::Mat color_image_grad_laplacian;
	cv::Mat gray_image_grad_laplacian;

	// 模板尺寸:3*3,5*5,9*9
	std::vector<cv::Mat> gauss_template;

	// 以下模板尺寸固定
	cv::Mat laplacian_template;
	std::vector<int> gauss_template_sum;

	// 不同图片的 mask
	std::vector<cv::Mat> highPromote_gray_mask;
	std::vector<cv::Mat> highPromote_color_mask;

	// 滤波器的大小
	std::vector<int> filter_size;
	std::vector<std::string> filter_size_string;

public:
	// 初始化
	Experiment1(std::string path)
	{
		filter_name.push_back("Gaussian");
		filter_name.push_back("Laplacian");

		pic_color.push_back("灰度");
		pic_color.push_back("彩色");

		filter_size_string.push_back("3 x 3");
		filter_size_string.push_back("5 x 5");
		filter_size_string.push_back("7 x 7");

		original_color_image = cv::imread(path);
		original_gray_image.push_back(color2Gray(original_color_image));

		for (int i = 0; i < 3; i++)
		{
			filter_size.push_back(i * 2 + 3);
			makeGaussTemplate(i * 2 + 3);
		}

		makeLaplacianTemplate();
		computerGaussTemplateSum();
	}

	// 彩色图像转灰度图像
	cv::Mat color2Gray(cv::Mat src_image)
	{
		//创建与原图同类型和同大小的矩阵
		cv::Mat gray_image(src_image.rows, src_image.cols, CV_8UC1);
		if (src_image.channels() != 1)
		{
			for (int i = 0; i < src_image.rows; i++)
				for (int j = 0; j < src_image.cols; j++)
					gray_image.at<uchar>(i, j) = (src_image.at<cv::Vec3b>(i, j)[0] + src_image.at<cv::Vec3b>(i, j)[1] + src_image.at<cv::Vec3b>(i, j)[2]) / 3;
		}
		else
			gray_image = src_image.clone();
		return gray_image;
	}

	// 生成高斯模板
	void makeGaussTemplate(int size = 3, int sigma = 1)
	{
		cv::Mat gaussTemplate = cv::Mat::zeros(size, size, CV_32F);

		int center = size / 2;
		double min = GMD(center, center, sigma);
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				gaussTemplate.at<float>(i, j) = GMD(i - center, j - center) / min;

		gaussTemplate.convertTo(gaussTemplate, CV_8U);
		gauss_template.push_back(gaussTemplate);
	}

	// 计算正态分布
	double GMD(double x, double y, double sigma = 1)
	{
		return exp(-(x * x + y * y) / (2 * sigma * sigma));
	}

	// 计算高斯模板的和
	void computerGaussTemplateSum()
	{
		for (int k = 0; k < 3; k++)
		{
			int sum = 0;
			for (int i = 0; i < gauss_template[k].rows; i++)
				for (int j = 0; j < gauss_template[k].cols; j++)
					sum += gauss_template[k].at<uchar>(i, j);
			gauss_template_sum.push_back(sum);
		}
	}

	// 生成Laplacian
	void makeLaplacianTemplate()
	{
		laplacian_template = (cv::Mat_<float>(3, 3) << 0, 1, 0,
			1, -4, 1,
			0, 1, 0);
	}

	// 灰度空域滤波
	void graySpatialFiltering(int size_id = 0, int select = 0)
	{
		int size = filter_size[size_id];
		int m = size / 2;

		cv::Mat image_process = cv::Mat::zeros(original_gray_image.size(), original_gray_image.type());
		cv::Mat image_grad = cv::Mat::zeros(original_gray_image.size(), original_gray_image.type());
		for (int i = m; i < original_gray_image.rows - m; i++)
			for (int j = m; j < original_gray_image.cols - m; j++)
			{
				cv::Mat sub_matrix = original_gray_image(cv::Rect(j - m, i - m, size, size));
				int grad, value;
				if (select == 0)
					image_process.at<uchar>(i, j) = computerGaussResult(sub_matrix, size_id);
				else
					image_grad.at<uchar>(i, j) = computerLaplacianResult(sub_matrix);

				if (select)
				{
					grad = image_grad.at<uchar>(i, j);
					value = sub_matrix.at<uchar>(m, m);
					if (grad + value > 255)
						image_process.at<uchar>(i, j) = 255;
					else
						image_process.at<uchar>(i, j) = grad + value;
				}
			}
		gray_image_process.push_back(image_process);
		if (select)
			gray_image_grad_laplacian = image_grad;
	}

	// 计算高斯滤波
	int computerGaussResult(cv::Mat &image_block, int size_id)
	{
		int sum = gauss_template_sum[size_id];
		return image_block.dot(gauss_template[size_id]) / sum;
	}

	// 计算Laplacian滤波
	int computerLaplacianResult(cv::Mat &image_block)
	{
		float GMD = 0.0;
		for (int i = 0; i < image_block.rows; i++)
			for (int j = 0; j < image_block.cols; j++)
				GMD += float(image_block.at<uchar>(i, j)) * laplacian_template.at<float>(i, j);
		if (abs(GMD) > 255)
			return 255;
		else if (GMD < 0)
			return -GMD;
		else
			return GMD;
	}

	// 彩色图像滤波
	void colorSpatialFiltering(int size_id = 0, int select = 0)
	{
		int size = filter_size[size_id];
		int m = size / 2;

		cv::Mat image_process = cv::Mat::zeros(original_color_image.size(), original_color_image.type());
		cv::Mat image_grad = cv::Mat::zeros(original_color_image.size(), original_color_image.type());
		std::vector<cv::Mat> channels;
		cv::split(original_color_image, channels);

		for (int i = m; i < original_color_image.rows - m; i++)
			for (int j = m; j < original_color_image.cols - m; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					cv::Mat sub_matrix = channels[k](cv::Rect(j - m, i - m, size, size));
					int grad, value;
					if (select == 0)
						image_process.at<cv::Vec3b>(i, j)[k] = computerGaussResult(sub_matrix, size_id);
					else
						image_grad.at<cv::Vec3b>(i, j)[k] = computerLaplacianResult(sub_matrix);

					if (select)
					{
						grad = image_grad.at<cv::Vec3b>(i, j)[k];
						value = sub_matrix.at<uchar>(m, m);
						if (grad + value > 255)
							image_process.at<cv::Vec3b>(i, j)[k] = 255;
						else
							image_process.at<cv::Vec3b>(i, j)[k] = grad + value;
					}
				}
			}
		color_image_process.push_back(image_process);
		if (select)
			color_image_grad_laplacian = image_grad;
	}

	// 测试灰度高斯滤波器
	void test_GaussGrayFilter()
	{
		int filter_id = 0;
		for (int j = 0; j < filter_size.size(); j++)
		{
			graySpatialFiltering(j, filter_id);
			std::cout << pic_color[0] << "-----" << filter_name[filter_id] << "-----尺寸大小: " << filter_size[j] << ": 运行完毕!!!!  \n";
		}

		cv::imshow(filter_name[filter_id] + "----准备就绪,可以开始!", original_color_image);
		cv::waitKey(0);

		// 显示灰度滤波结果
		cv::imshow("原始图像", original_gray_image);
		cv::waitKey(0);
		for (int j = 0; j < filter_size.size(); j++)
		{
			cv::imshow(pic_color[0] + "-----" + filter_name[filter_id] + "-----" + filter_size_string[j], gray_image_process[j]);
			cv::waitKey(0);
		}
		std::cout << "\n \n";
		cv::destroyAllWindows();
	}

	// 测试灰度Laplacian滤波器
	void test_LaplacianGrayFilter()
	{
		int filter_id = 1;
		graySpatialFiltering(0, filter_id);
		std::cout << pic_color[0] << "-----" << filter_name[filter_id] << ": 运行完毕!!!!  \n";

		cv::imshow(filter_name[filter_id] + "----准备就绪,可以开始!", original_color_image);
		cv::waitKey(0);

		// 显示灰度滤波结果
		cv::imshow("原始图像", original_gray_image);
		cv::waitKey(0);

		cv::imshow(pic_color[0] + "-----" + filter_name[filter_id], gray_image_process[filter_size.size()]);
		cv::waitKey(0);
		cv::imshow(pic_color[0] + "-----" + filter_name[filter_id] + "-----" + "灰度幅值: ", gray_image_grad_laplacian);
		cv::waitKey(0);
		std::cout << "\n \n";
		cv::destroyAllWindows();
	}

	// 测试彩色高斯滤波器
	void test_GaussColorFilter()
	{
		int filter_id = 0;
		for (int j = 0; j < filter_size.size(); j++)
		{
			colorSpatialFiltering(j, filter_id);
			std::cout << pic_color[1] << "-----" << filter_name[filter_id] << "-----尺寸大小: " << filter_size[j] << ": 运行完毕!!!!  \n";
		}

		cv::imshow(filter_name[filter_id] + "----准备就绪,可以开始!", original_color_image);
		cv::waitKey(0);

		cv::imshow("原始图像", original_color_image);
		cv::waitKey(0);
		for (int j = 0; j < filter_size.size(); j++)
		{
			cv::imshow(pic_color[1] + "-----" + filter_name[filter_id] + "-----" + filter_size_string[j], color_image_process[j]);
			cv::waitKey(0);
		}
		std::cout << "\n \n";
		cv::destroyAllWindows();
	}

	// 测试彩色Laplacian滤波器
	void test_LaplacianColorFilter()
	{
		int filter_id = 1;
		colorSpatialFiltering(0, filter_id);
		std::cout << pic_color[1] << "-----" << filter_name[filter_id] << ": 运行完毕!!!!  \n";

		cv::imshow(filter_name[filter_id] + "----准备就绪,可以开始!", original_color_image);
		cv::waitKey(0);

		cv::imshow("原始图像", original_color_image);
		cv::waitKey(0);

		cv::imshow(pic_color[1] + "-----" + filter_name[filter_id], color_image_process[filter_size.size()]);
		cv::waitKey(0);
		if (filter_id) {
			cv::imshow(pic_color[1] + "-----" + filter_name[filter_id] + "-----" + "灰度幅值: ", color_image_grad_laplacian);
			cv::waitKey(0);
		}
		std::cout << "\n \n";
		cv::destroyAllWindows();
	}
};

int main()
{
	std::string path;
	path = "E:/lenna.jpg";
	Experiment1 img(path);

	img.test_GaussGrayFilter();
	img.test_LaplacianGrayFilter();

	img.test_GaussColorFilter();
	img.test_LaplacianColorFilter();

	return 1;
}


