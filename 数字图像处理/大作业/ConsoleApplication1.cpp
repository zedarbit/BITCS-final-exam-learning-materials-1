#include <opencv.hpp>
#include <math.h>
#include <vector>

#define PI acos(-1)

cv::Mat Color2Gray(cv::Mat src_image)
{
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

cv::Mat equalize_hist(cv::Mat input) {
	cv::Mat output = input.clone();
	int gray_sum = input.cols * input.rows;
	int gray[256] = { 0 };  //记录每个灰度级别下的像素个数
	double gray_prob[256] = { 0 };  //记录灰度分布密度
	double gray_distribution[256] = { 0 };  //记录累计密度
	int gray_equal[256] = { 0 };  //均衡化后的灰度值
	//统计每个灰度下的像素个数
	for (int i = 0; i < input.rows; i++)
	{
		uchar* p = input.ptr<uchar>(i);
		for (int j = 0; j < input.cols; j++)
		{
			int vaule = p[j];
			gray[vaule]++;
		}
	}
	//统计灰度频率
	for (int i = 0; i < 256; i++)
	{
		gray_prob[i] = ((double)gray[i] / gray_sum);
	}
	//计算累计密度
	gray_distribution[0] = gray_prob[0];
	for (int i = 1; i < 256; i++)
	{
		gray_distribution[i] = gray_distribution[i - 1] + gray_prob[i];
	}

	//重新计算均衡化后的灰度值
	for (int i = 0; i < 256; i++)
	{
		gray_equal[i] = (uchar)(255 * gray_distribution[i] + 0.5);
	}
	//直方图均衡化,更新原图每个点的像素值
	for (int i = 0; i < output.rows; i++)
	{
		uchar* p = output.ptr<uchar>(i);
		for (int j = 0; j < output.cols; j++)
		{
			p[j] = gray_equal[p[j]];
		}
	}
	return output;
}

cv::Mat Image2Binary(cv::Mat src_image, int threshold, float init_h, float end_h, float init_w, float end_w) {
	int value;
	float start_i = init_h*float(src_image.rows);
	float end_i = end_h * float(src_image.rows);
	float start_j = init_w * float(src_image.cols);
	float end_j = end_w * float(src_image.cols);
	cv::Mat binary_image(src_image.rows, src_image.cols, CV_8UC1);
	for (int i = 0; i < src_image.rows; i++) {
		for (int j = 0; j < src_image.cols; j++) {
			value = src_image.at<uchar>(i, j);
			if (value > threshold && i > start_i && i < end_i && j > start_j && j < end_j) {
				binary_image.at<uchar>(i, j) = 255;
			}
			else {
				binary_image.at<uchar>(i, j) = 0;
			}
		}
	}
	return binary_image;
}

cv::Mat BiMedianBlur(cv::Mat src_image, int size) {
	int count_w, count_b, value, ds=(size-1)/2;
	cv::Mat result(src_image.rows, src_image.cols, CV_8UC1);
	for (int i = 0; i < src_image.rows; i++) {
		for (int j = 0; j < src_image.cols; j++) {
			count_w = 0;
			count_b = 0;
			for (int di = -ds; di < ds; di++) {
				for (int dj = -ds; dj < ds; dj++) {
					if (i + di >= 0 && j + dj >= 0) {
						if (i + di < src_image.rows && j + dj < src_image.cols) {
							value = src_image.at<uchar>(i+di, j+dj);
							if (value == 0) {
								count_b += 1;
							}
							else {
								count_w += 1;
							}
						}
					}
				}
			}
			if (count_b > count_w) {
				result.at<uchar>(i, j) = 0;
			}
			else {
				result.at<uchar>(i, j) = 255;
			}
		}
	}
	return result;
}

cv::Mat ConvLap(cv::Mat src_image, int thresh) {
	int value;
	cv::Mat result = cv::Mat::zeros(src_image.size(), CV_8U);
	for (int i = 1; i < src_image.rows - 1; i++) {
		for (int j = 1; j < src_image.cols - 1; j++) {
			value = -4 * src_image.at<uchar>(i, j);
			value += src_image.at<uchar>(i - 1, j);
			value += src_image.at<uchar>(i + 1, j);
			value += src_image.at<uchar>(i, j - 1);
			value += src_image.at<uchar>(i, j + 1);
			if (value < 0) {
				value = -value;
			}
			if (value > thresh) {
				value = 255;
			}
			else
			{
				value = 0;
			}
			result.at<uchar>(i, j) = value;
		}
	}
	return result;
}

std::vector<float> hough_line_v(cv::Mat img, int threshold)
{
	int row, col;
	int i, k;
	//参数空间的参数极角angle(角度)，极径p;
	int angle, p;

	//累加器
	int **socboard;
	int *buf;
	int w, h;
	w = img.cols;
	h = img.rows;
	int Size;
	int offset;
	std::vector<float> lines;
	//申请累加器空间并初始化
	Size = w * w + h * h;
	Size = 2 * sqrt(Size) + 100;
	offset = Size / 2;
	socboard = (int **)malloc(Size * sizeof(int*));
	if (!socboard)
	{
		printf("mem err\n");
		return lines;
	}

	for (i = 0; i < Size; i++)
	{
		socboard[i] = (int *)malloc(181 * sizeof(int));
		if (socboard[i] == NULL)
		{
			printf("buf err\n");
			return lines;
		}
		memset(socboard[i], 0, 181 * sizeof(int));
	}

	//遍历图像并投票
	int src_data;
	p = 0;
	for (row = 0; row < img.rows; row++)
	{
		for (col = 0; col < img.cols; col++)
		{
			//获取像素点
			src_data = img.at<uchar>(row, col);

			if (src_data == 255)
			{
				for (angle = 0; angle < 181; angle++)
				{
					p = col * cos(angle * PI / 180.0) + row * sin(angle * PI / 180.0) + offset;

					//错误处理
					if (p < 0)
					{
						printf("at (%d,%d),angle:%d,p:%d\n", col, row, angle, p);
						printf("warrning!");
						printf("size:%d\n", Size / 2);
						continue;
					}
					//投票计分
					socboard[p][angle]++;

				}
			}
		}
	}

	//遍历计分板，选出符合阈值条件的直线
	int count = 0;
	int Max = 0;
	int kp, kt, r;
	kp = 0;
	kt = 0;
	for (i = 0; i < Size; i++)//p
	{
		for (k = 0; k < 181; k++)//angle
		{
			if (socboard[i][k] > Max)
			{
				Max = socboard[i][k];
				kp = i - offset;
				kt = k;
			}

			if (socboard[i][k] >= threshold)
			{
				r = i - offset;
				//lines_w.push_back(std::);
				lines.push_back(-1.0 * float(std::cos(k*PI / 180) / std::sin(k*PI / 180)));
				lines.push_back(float(r)/std::sin(k*PI / 180));
				count++;
			}
		}
	}
	//释放资源
	for (int e = 0; e < Size; e++)
	{
		free(socboard[e]);
	}
	free(socboard);
	return lines;
}

std::vector<float> merge_result(std::vector<float> lines) {
	float k=0, b=0, c=0;
	float k_mean, b_mean;
	float k1=0, b1=0, c1=0, k2=0, b2=0, c2=0;
	std::vector<float> new_lines;
	// y = k * x + b
	for (int i = 0; i < lines.size() - 1; i++) {
		k += lines[i];
		b += lines[i + 1];
		i++;
		c++;
	}
	k_mean = k / c;
	b_mean = b / c;
	for (int i = 0; i < lines.size() - 1; i++) {
		if (lines[i + 1] > b_mean) {
			c1 += 1;
			k1 += lines[i];
			b1 += lines[i + 1];
		}
		else {
			c2 += 1;
			k2 += lines[i];
			b2 += lines[i + 1];
		}
		i++;
	}
	std::cout << k1 << " " << b1 << " " << c1 << std::endl;
	std::cout << k2 << " " << b2 << " " << c2 << std::endl;
	new_lines.push_back(k1 / c1);
	new_lines.push_back(b1 / c1);
	new_lines.push_back(k2 / c2);
	new_lines.push_back(b2 / c2);
	return new_lines;
}

int main(int argc, char const *argv[]) {
	if (argc > 1) {
		int threshold = 122, size = 11;
		float init_h = 0.7, end_h = 0.95;
		float init_w = 0.2, end_w = 0.8;
		cv::Mat image, gray_image;
		cv::Mat binary_image, bi_median_image;
		cv::Mat grand_image, img_line;
		image = cv::imread(argv[1]);
		if(argc > 2)
			cv::imshow("raw image", image);
		float start_i = init_h * float(image.rows);
		float start_j = init_w * float(image.cols);
		gray_image = Color2Gray(image);
		if (argc > 2)
			cv::imshow("gray", gray_image);
		gray_image = equalize_hist(gray_image);
		if (argc > 2)
			cv::imshow("gray_equ", gray_image);
		binary_image = Image2Binary(gray_image, threshold, init_h, end_h, init_w, end_w);
		if (argc > 2)
			cv::imshow("binary", binary_image);
		bi_median_image = BiMedianBlur(binary_image, size);
		if (argc > 2)
			cv::imshow("binary_median", bi_median_image);
		grand_image = ConvLap(bi_median_image, 60);
		if (argc > 2)
			cv::imshow("grand", grand_image);
		std::vector<float> lines = hough_line_v(grand_image, 20);
		std::vector<float> new_lines = merge_result(lines);
		// std::vector<float> new_lines = lines;
		float k, b;
		// y = k * x + b
		for (int i = 0; i < new_lines.size() - 1; i++) {
			k = new_lines[i];
			b = new_lines[i + 1];
			if (k == 0) {
				continue;
			}
			i++;
			std::cout << "k=" << k;
			std::cout << " b=" << b << std::endl;
			cv::Point p1(int((start_i - b) / k), int(start_i));
			cv::Point p2(int((image.rows - b) / k), int(image.rows));
			line(image, p1, p2, cv::Scalar(0, 255, 0), 2);
		}
		std::string path = argv[1];
		std::string::size_type iPos = path.find_last_of('\\') + 1;
		std::string filename = path.substr(iPos, path.length() - iPos);
		std::string name = filename.substr(0, filename.rfind("."));
		std::cout << "data/" + name + ".txt" << std::endl;
		std::ofstream OutFile("data/"+ name + ".txt"); //利用构造函数创建txt文本，并且打开该文本
		OutFile << "k=" << k << " b=" << b << std::endl;  //把字符串内容"This is a Test!"，写入Test.txt文件
		OutFile.close();            //关闭Test.txt文件
		if (argc > 2)
			cv::imshow("result", image);
		cv::waitKey(0);
		cv::destroyAllWindows();
	}
	return 0;
}