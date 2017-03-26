#include"irisLocalization.h"
#include<opencv.hpp>
#include<iostream>
#include<armadillo>


template<typename T>
static void cv2arm(cv::Mat_<T>& cv_mat_in, arma::Mat<T>& arma_mat_out)
{
	arma_mat_out = arma::Mat<T>(cv_mat_in.ptr<T>(),
		static_cast<arma::uword>(cv_mat_in.cols),
		static_cast<arma::uword>(cv_mat_in.rows),
		true,
		true);

}
template<typename T>
static void arm2cv(arma::Mat<T>& arma_mat_in, cv::Mat_<T>& cv_mat_out)
{
	cv_mat_out = cv::Mat_<T>(static_cast<int>(arma_mat_in.n_rows),
		static_cast<int>(arma_mat_in.n_cols),
		const_cast<T*>(arma_mat_in.memptr()));
}


iris::irisLocater::irisLocater(cv::Mat image) :pupil_idx(0), light_idx(0), block_rows(0),block_cols(0)
{
	cv::Vec3f circles;
	binary_op(image);
	detect_block();
	pupil_dec(image,circles);
	cout << circles[0] << "  " << circles[1] << "  " << circles[2] << endl;;
	cv::circle(image, cv::Point(circles[0],circles[1]), 3, cv::Scalar(0, 255, 0), -1, 8, 0);
		////绘制圆轮廓  
	cv::circle(image, cv::Point(circles[0], circles[1]), circles[2], cv::Scalar(155, 50, 255), 1, 8, 0);
	cv::namedWindow("test");
	cv::imshow("test", image);
	cv::waitKey(100);
}
bool iris::irisLocater::light_evalue(const cv::Mat image)
{
	unsigned char *ptr = image.data;
	int r1 = 0, r2 = 0, r3 = 0;
	for (int m = 0; m < image.rows; m++)
	{
		for (int n = 0; n < image.cols; n++)
		{
			*(ptr + m*image.cols + n)<86 ? r1++ : (*(ptr + m*image.cols + n) > 170 ? r3++ : r2++);
		}
	}
	return (r1 > 0.65 * image.rows*image.cols || r3 > 0.85 * image.rows*image.cols) ? false : true;
}
/*
对图像进行二值化处理，将图像二值化
*/
void iris::irisLocater::binary_op(cv::Mat imagein)
{
	int m, n;//循环变量
	int min1 = 0, max1 = 0;    //灰度分布初始值和结束值
	double threshold1, threshold2, threshold3;
	double buf1=0, buf2=0;
	double p_tresh=100;
	arma::mat A;
	cv2arm(cv::Mat_<double>(imagein), A); 
	//cout << A.n_rows << "  " << A.n_cols;
	A = A.t();
	arma::vec image_vec= arma::vec(reshape(A,  A.n_elem, 1));
	//求解直方图
	arma::uvec histg = arma::histc(image_vec, arma::linspace<arma::vec>(0,255,256));
	for (m = 0; m < 254; m++)
	{
		if ((histg[m] > 0.0001*A.n_elem) && (histg[m + 1] > 0.0001*A.n_elem) && (histg[m + 2] > 0.0001*A.n_elem))
			break;
	}
	min1 = m+1;
	for (m = 255; m > 1; m--)     
	{
		if ((histg[m] > 0.0001*A.n_elem) && (histg[m - 1] > 0.0001*A.n_elem) && (histg[m - 2] > 0.0001*A.n_elem))
			break;
	}
	max1 = m+1;
	threshold1 = (min1 + 0.05*(max1 - min1));
	threshold2 = min1 + 10;
	pupil_idx = (int)threshold1;
	arma::mat mask;
	mask << threshold2 << threshold2 << threshold2 << arma::endr
		<< threshold2 << threshold1 << threshold2 << arma::endr
		<< threshold2 << threshold2 << threshold2 << arma::endr;
	for (m = 80; m < A.n_rows-80; m++)
		for (n = 80; n < A.n_cols-80; n++)
		{
			if (arma::accu(A(arma::span(m - 1, m + 1), arma::span(n - 1, n + 1))<mask)>8)
				pupilBin(m,n) = 1;
		}

	threshold3 = max1 - 0.05*(max1 - min1);
	light_idx = (int)threshold3;
	//cout << light_idx << "   " << pupil_idx << endl;
	for (m = 1; m < A.n_rows-1 ; m++)
		for (n = 1; n < A.n_cols-1; n++)
		{
			if (A(m, n) > threshold3)
			{
				
				buf1 = abs(arma::accu(A(arma::span(m + 1, m + 1), arma::span(n - 1, n + 1)) - A(arma::span(m - 1, m - 1), arma::span(n - 1, n + 1)))+A(m+1,n)-A(m-1,n));
				buf2 = abs(arma::accu(A(arma::span(m - 1, m + 1), arma::span(n + 1, n + 1)) - A(arma::span(m - 1, m + 1), arma::span(n - 1, n - 1))) + A(m , n+1) - A(m , n-1));

				if (buf1 > p_tresh || buf2 > p_tresh)
					lightBin(m, n) = 1;

				

			}
			
		}

}
float decideOverlap(const cv::Rect &r1, const cv::Rect &r2)
{
	int x1 = r1.x;
	int y1 = r1.y;
	int width1 = r1.width;
	int height1 = r1.height;

	int x2 = r2.x;
	int y2 = r2.y;
	int width2 = r2.width;
	int height2 = r2.height;

	int endx = min(x1 + width1, x2 + width2);
	int startx = max(x1, x2);
	int width = endx - startx;

	int endy = min(y1 + height1, y2 + height2);
	int starty = max(y1, y2);
	int height = endy - starty;

	float ratio = 0.0f;
	float Area, Area1, Area2;

	if (width <= 0 || height <= 0)
		return 0.0f;
	else
	{
		Area = width*height;
		Area1 = width1*height1;
		Area2 = width2*height2;
		ratio = Area / (Area1 + Area2 - Area);
	}

	return ratio;
}


void iris::irisLocater::detect_block()
{
	//寻找子块的范围
	int index_range;
	arma::umat binMat = arma::zeros<arma::umat>(3, 4);
	arma::umat lightMat = arma::zeros<arma::umat>(3, 4);
	arma::umat smallBinMat = arma::zeros<arma::umat>(10, 5);
	int m, n;//循环变量

	for (m = 0; m < binMat.n_rows; m++)
		for (n = 0; n < binMat.n_cols; n++)
		{
			
			binMat(m, n) = arma::accu(pupilBin(arma::span(m * 160, m * 160 + 159), arma::span(n * 160, n * 160 + 159)));

			lightMat(m, n) = arma::accu(lightBin(arma::span(m * 160, m * 160 + 159), arma::span(n * 160, n * 160 + 159)));
		}
	binMat = binMat % (lightMat>0);//去除不存在光斑点的黑块
	arma::uword index_ = binMat.index_max();
	block_rows = (index_ % binMat.n_rows) * 160;
	block_cols = (index_ / binMat.n_rows) *160;

	//初始化各子块的起始行坐标
	index_range = 5;
	if (block_rows<320)
	{
		index_range = 10;
	}
	for (m = 0; m < index_range; m++)
		for (n = 0; n < 5; n++)
		{
			smallBinMat(m, n) = arma::accu(pupilBin(arma::span(block_rows + m * 32, block_rows + m * 32 + 31)
				, arma::span(block_cols + n * 32, block_cols + n * 32 + 31)));
		}
	index_ = smallBinMat.index_max();
	block_rows += (index_ % smallBinMat.n_rows) * 32+16;
	block_cols += (index_ / smallBinMat.n_rows) * 32+16;
	//cout << block_rows << "  " << block_cols;
}

void  iris::irisLocater::pupil_dec(cv::Mat image, cv::Vec3f & circleIn)
{
	vector<vector<cv::Point>> contours;
	vector<cv::Point> maxContour;
	double maxArea = 0;
	cv::Point2f cen;
	float rad;
	cv::Mat_<double>bin_Roi;
	float x = 0, y = 0, r = 0;
	arma::mat temp = pupilBin(arma::span(block_rows - 90, block_rows + 90), arma::span(block_cols - 90, block_cols + 90));
	arm2cv(temp, bin_Roi);
	cv::findContours(cv::Mat_<unsigned char>(bin_Roi), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	//寻找最大联通区域

	for (int i = 0; i < contours.size(); i++)
	{
		double area = cv::contourArea(contours[i]);
		if (area > maxArea)
		{
			maxArea = area;
			maxContour = contours[i];
		}
	}	

	cv::minEnclosingCircle(maxContour, cen, rad);
	cen.x += block_rows - 90;
	cen.y += block_cols - 90;

	circleIn[1] =cen.x;
	circleIn[0] =cen.y;
	circleIn[2] =rad+3;
}

