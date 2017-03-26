#ifndef IRIS_LOCALIZATION
#define IRIS_LOCALIZATION

#include<opencv.hpp>
#include<iostream>
#include<armadillo>

using namespace std;

namespace iris{
	
	class irisLocater{
		
	private:
		int pupil_idx;
		int light_idx;
		int block_rows;
		int block_cols;
		cv::Vec3f in;
		arma::mat pupilBin = arma::zeros<arma::mat>(480, 640);
		arma::mat lightBin = arma::zeros<arma::mat>(480, 640);
		//arma::mat pupilBin = arma::zeros(480, 640);
	public:

		bool status=false;
		irisLocater(cv::Mat image);
		bool light_evalue(const cv::Mat image);
		void binary_op(cv::Mat imagein);
		void detect_block();
		void pupil_dec(cv::Mat image, cv::Vec3f &circleIn);
	};
}

#endif