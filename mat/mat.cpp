#include <iostream>
#include <armadillo>
#include<opencv.hpp>
#include <io.h>
#include"irisLocalization.h"
using namespace std;
using namespace arma;
//
namespace my{
	template<typename T>
	static void cv2arm(cv::Mat_<T>& cv_mat_in, arma::Mat<T>& arma_mat_out)
	{
		arma_mat_out = arma::Mat<T>(cv_mat_in.ptr<T>(),
			static_cast<arma::uword>(cv_mat_in.cols),
			static_cast<arma::uword>(cv_mat_in.rows),
			true,
			true);
		//cv::Mat_<T> temp(cv_mat_in.t()); //todo any way to not create a temporary?
		////This compiles on VC10 but not g++ 4.9 (on Debian with OpenCV 2.4.9.1 and Arma 4.320.0)
		//arma_mat_out = arma::Mat<T>(temp.ptr<T>(), //<<"error: expected primary-expression before '(' token"
		//	static_cast<arma::uword>(temp.cols),
		//	static_cast<arma::uword>(temp.rows),
		//	true,
		//	true);
	}
	template<typename T>
	static void arm2cv(arma::Mat<T>& arma_mat_in, cv::Mat_<T>& cv_mat_out)
	{
		cv_mat_out = cv::Mat_<T>(static_cast<int>(arma_mat_in.n_cols),
			static_cast<int>(arma_mat_in.n_rows),
			const_cast<T*>(arma_mat_in.memptr()));
	}
}

void GetAllFormatFiles(std::string path, std::vector<std::string>& files, std::string format)
{
	//文件句柄    
	long   hFile = 0;
	//文件信息    
	struct _finddata_t fileinfo;
	std::string p;

	if ((hFile = _findfirst(p.assign(path).append("/*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					//files.push_back(p.assign(path).append("\\").append(fileinfo.name) );  
					GetAllFormatFiles(p.assign(path).append("/").append(fileinfo.name), files, format);
				}
			}

		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile);

	}
	if ((hFile = _findfirst(p.assign(path).append("/*." + format).c_str(), &fileinfo)) != -1)
	{
		do
		{

			files.push_back(p.assign(path).append("/").append(fileinfo.name));


		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile);
	}

}

void GetAllSubPath(std::string path, std::vector<std::string>& files)
{
	long   hFile = 0;
	//文件信息    
	struct _finddata_t fileinfo;
	std::string p;

	if ((hFile = _findfirst(p.assign(path).append("/*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					files.push_back(p.assign(path).append("/").append(fileinfo.name));
				}
			}

		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile);

	}
}
int num1=0, num2 = 0;
int main()
{
	//E:/reg/IrisCapture/bin/01/l/06l.bmp
	std::string path = "E:/reg/虹膜采集图像整理";
	std::vector<string> subpath1, subpath2, files;
	GetAllSubPath(path, subpath1);
	int sizereg = subpath1.size();
	std::cout << "sizereg: " << sizereg << std::endl;
	for (int i = 0; i < sizereg; i++)
	{
		GetAllSubPath(subpath1[i], subpath2);
		for (int j = 0; j < subpath2.size(); j++)
		{
			GetAllFormatFiles(subpath2[j], files, "bmp");
			for (int n = 0; n < files.size(); n++)
			{
				cv::Mat img = cv::imread(files[n]);
				cv::namedWindow("test");
				//cv::imshow("test", img);
				//cv::waitKey(1000);
				cv::cvtColor(img, img, CV_RGB2GRAY);
				//cout << img.rows;
				iris::irisLocater A(img);
				num1++;
				if (A.status)
					num2++;
				//mat B(3,3),C(3,3);
				//B << 1 << 1<< 11<< endr
				//	<< 1 << 1<< 1 << endr
				//	<< 1 << 1 << 10 << endr;
				//uword c = B.index_max();
				//cout << B((c) % B.n_rows, (c) / B.n_rows);
				/*C << 10 << 0 << 1 << endr
				<< 1 << 0 << 1 << endr
				<< 1 << 0 << 1 << endr;
				mat D = B%(C==1);
				D.print();*/
				/*my::cv2arm(cv::Mat_<double>(img), B);
				B = B.t();
				B(span(1, 1), span(0, 2)).print();*/
				//B.save("A.txt", raw_ascii);
				//umat C = (B >= 180);
				//arma::vec image_vec = arma::vec(reshape(B, B.n_elem, 1));
				//arma::uvec histg = arma::histc(image_vec, arma::linspace<vec>(0, 255, 256));
				//histg.print();
				//Mat<double> A; 
				//my::cv2arm(cv::Mat_<double>(img), A);
				//A = A.t();
				//A(span(25,27),span(3,6)).print();
				//mat c=conv_to<mat>::from(C);
				//c.print();
				/*cv::Mat_<double> im;
				mat D = A.pupilBin(span(A.block_rows - 80, A.block_rows + 80), span(A.block_cols - 80, A.block_cols + 80));
				my::arm2cv(D, im);
				im = im.t();
				for (int i = 0; i < im.rows; i++)
					for (int j = 0; j < im.cols; j++)
					{
						if (im[i][j] >0)
							im[i][j] = 255;
					}*/
				//cv::Mat ROi = img(cv::Range(A.block_rows - 60, A.block_rows + 60), cv::Range(A.block_cols - 60, A.block_cols + 60));
				//vector<cv::Vec3f> circles;
				//cv::Vec3f in;
				//float x=0, y=0, r=0;
				//HoughCircles(ROi, circles, CV_HOUGH_GRADIENT, 2, 2, 170, 65, 20, 70);
				//for (size_t i = 0; i < circles.size(); i++)
				//{
				//	x += circles[i][0];
				//	y += circles[i][1];
				//	r += circles[i][2];
				//	//cv::Point center(cvRound(circles[i][0]) + A.block_cols - 80, cvRound(circles[i][1]) + A.block_rows - 80);
				//	//int radius = cvRound(circles[i][2]);
				//	////绘制圆心  
				//	//circle(img, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
				//	////绘制圆轮廓  
				//	//circle(img, center, radius, cv::Scalar(155, 50, 255), 1, 8, 0);
				//}
				//in[0] = x / circles.size();
				//in[1] = y / circles.size();
				//in[2] = r / circles.size();
				//cv::Point center(cvRound(in[0]) + A.block_cols - 60, cvRound(in[1]) + A.block_rows - 60);
				//int radius = cvRound(in[2]);
				////绘制圆心  
				//circle(img, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
				////绘制圆轮廓  
				////circle(img, center, radius, cv::Scalar(155, 50, 255), 1, 8, 0);
				//cv::imshow("test", im);//cv::Mat_<unsigned char>(im));
				//cv::waitKey(1000);
				//double a = -1;
				//cout << abs(a);
			}
			//vec in=zeros(3);
			//cout << in[2] << endl;
			files.clear();
		}
		subpath2.clear();
	}
	cout << "total； " << num1 << "  success: " << num2 << endl;
	
	
}