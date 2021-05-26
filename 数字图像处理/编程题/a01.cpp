#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>
#include <math.h>

using namespace std;

void showImage(vector<vector<float>> &image_2d)
{
    for (int i = 0; i < image_2d.size(); i++)
    {
        for (int j = 0; j < image_2d[0].size(); j++)
            // 访问并且输出(左对齐)二维矩阵的各个元素
            cout << setiosflags(ios::left) << setw(8) << +image_2d[i][j];
        cout << " ";
        cout << endl;
    }
}

void showImagev2(vector<vector<int>> &image_2d)
{
    for (int i = 0; i < image_2d.size(); i++)
    {
        for (int j = 0; j < image_2d[0].size(); j++)
        {
            cout << image_2d[i][j] << " ";
        }
        cout << endl;
    }
}

vector<vector<int>> Conv_2d(
    vector<vector<float>> &image, vector<vector<float>> &kernel,
    int m, int n, int u_pad, int v_pad)
{
    int image_row = image.size();      //获取图片矩阵的行数
    int image_col = image[0].size();   //获取图片矩阵的列数
    int kernel_row = kernel.size();    //获取卷积核矩阵的行数
    int kernel_col = kernel[0].size(); // 获取卷积核矩阵的列数
    // cout << "Image Size: " << image_row << "," << image_col << endl;
    // cout << "Kernel Size: " << kernel_row << "," << kernel_col << endl;
    vector<vector<int>> result; //定义二维vector用于接收卷积结果
    int result_row = m;         //计算结果矩阵的行数
    int result_col = n;         //计算结果矩阵的列数
    // cout << "Result Size: " << result_row << "," << result_col << endl;
    for (int i = 0; i < result_row; i++)
    {
        vector<int> row_result; //定义一维行矩阵以便于填入二维的result矩阵
        for (int j = 0; j < result_col; j++)
        {
            float res = 0; //用于记录每一次卷积过程，图片与卷积核矩阵中数字乘积的结果
            for (int k = 0; k < kernel_row; k++)
            {
                for (int m = 0; m < kernel_col; m++)
                {
                    long int row = i + k; //防止溢出
                    long int col = j + m; //防止溢出
                    if (row >= image_row || col >= image_col)
                    {
                        cout << "-[INFO] Preparing i=" << i << " j=" << j << endl;
                        cout << "-[CONFIG] k=" << k << " m=" << m << endl;
                        cout << "-[ERROR] ERROR!" << endl;
                        cout << endl;
                    }
                    // cout << "-[COMPUTE] " << image[row][col] << "*" << kernel[k][m] << endl;
                    float mul = image[row][col] * kernel[k][m]; //调用私有函数进行一次乘法运算
                    res = res + mul;                            //调用私有函数计算每次卷积与卷积核计算的总和
                    // cout << "-[RESULT] res=" << res << endl;
                }
            }
            row_result.push_back(round(res)); //将一次的卷积结果填入行矩阵中
        }
        result.push_back(row_result); //将某一行的卷积结果填入二维卷积结果矩阵中
    }
    return result;
}

int main()
{
    int m, n, u, v;
    float value;
    cin >> m >> n >> u >> v;
    // 填充大小
    int u_pad, v_pad;
    u_pad = (v - 1) / 2;
    v_pad = (u - 1) / 2;
    // 图像矩阵
    vector<vector<float>> ConvolutionImage;

    for (int p = 0; p < v_pad; p++)
    {
        vector<float> pad_vector;
        pad_vector.assign(n + u + 1, 0);
        ConvolutionImage.push_back(pad_vector);
    }
    for (int i = 0; i < m; i++)
    {
        vector<float> line_vector; //定义一维行矩阵以便于填入二维的result矩阵
        for (int p1 = 0; p1 < u_pad; p1++)
        {
            line_vector.push_back(0.0);
        }
        for (int j = 0; j < n; j++)
        {
            cin >> value;
            line_vector.push_back(value);
        }
        for (int p2 = 0; p2 < u_pad; p2++)
        {
            line_vector.push_back(0.0);
        }
        ConvolutionImage.push_back(line_vector);
    }
    for (int p = 0; p < v_pad; p++)
    {
        vector<float> pad_vector;
        pad_vector.assign(n + u - 1, 0);
        ConvolutionImage.push_back(pad_vector);
    }
    //卷积核矩阵
    vector<vector<float>> ConvolutionKernel;
    for (int i = 0; i < u; i++)
    {
        vector<float> line_vector; //定义一维行矩阵以便于填入二维的result矩阵
        for (int j = 0; j < v; j++)
        {
            cin >> value;
            line_vector.push_back(value);
        }
        ConvolutionKernel.push_back(line_vector);
    }

    // cout << "Image:" << endl;
    // showImage(ConvolutionImage); //输出结果
    // cout << "Conv:" << endl;
    // showImage(ConvolutionKernel); //输出结果

    //用于接收卷积结果
    vector<vector<int>> ConvolutionResult;
    ConvolutionResult = Conv_2d(
        ConvolutionImage, ConvolutionKernel,
        m, n, u_pad, v_pad);
    // cout << "Result:" << endl;
    showImagev2(ConvolutionResult); //输出结果

    // system("pause");
    return 0;
}

/*
5 5
3 3
9 0 6 2 2
9 7 6 2 10
3 5 9 9 7
7 5 8 0 5
2 9 6 5 5
0.0625    0.1250    0.0625
0.1250    0.2500    0.1250
0.0625    0.1250    0.0625
*/

/*
3 3
1 3
1 1 1
1 1 1
1 1 1
1 1 1
*/

/*
5 5
3 1
9 0 6 2 2
9 7 6 2 10
3 5 9 9 7
7 5 8 0 5
2 9 6 5 5
0.0625    0.1250    0.0625
*/

/*
5 5
3 5
1 1 1 1 1
1 1 1 1 1
1 1 1 1 1
1 1 1 1 1
1 1 1 1 1
1 1 1 1 1
1 1 1 1 1
1 1 1 1 1
*/
