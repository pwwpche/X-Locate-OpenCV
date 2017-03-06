#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
using namespace std;
using namespace cv;

int alpha_0; /**< 控制对比度 */
double alpha;
int beta;  /**< 控制亮度 */
int beta_0;
string path0_contrast;

void changeContrast(int value){
	alpha = 1.0 + (double)alpha_0 / 50.0 ;
	beta = beta_0 - 100;
	Mat image = imread(path0_contrast.c_str() );
    Mat new_image = Mat::zeros( image.size(), image.type() );
	//for( int y = 0; y < image.rows; y++ )
 //   {
 //       for( int x = 0; x < image.cols; x++ )
 //       {
 //           for( int c = 0; c < 3; c++ )
 //           {
 //               new_image.at<Vec3b>(y,x)[c] = saturate_cast<uchar>( alpha*( image.at<Vec3b>(y,x)[c] ) + beta );
 //           }
 //       }
 //   }
	image.convertTo(new_image, -1, alpha, beta);
	imshow("New Image", new_image);
}
int main_con( int argc, char** argv )
{
    /// 读入用户提供的图像
	//path0_contrast = "C:/yommy_scene.jpg" ;
	path0_contrast = "E:\\Metro_Photos_Compressed\\Metro3\\2x1st.jpg";
    /// 执行运算 new_image(i,j) = alpha*image(i,j) + beta


    /// 创建窗口
	namedWindow("New Image", 1);
	cvCreateTrackbar("contrast", "New Image", &alpha_0, 100, changeContrast);
	cvCreateTrackbar("brightness", "New Image", &beta_0, 100, changeContrast);
    /// 显示图像
	changeContrast(0);

    /// 等待用户按键
    waitKey();
    return 0;
}