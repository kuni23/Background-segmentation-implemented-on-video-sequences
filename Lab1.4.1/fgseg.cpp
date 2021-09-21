/* Applied Video Sequence Analysis (AVSA)
 *
 *	LAB1.0: Background Subtraction - Unix version
 *	fgesg.cpp
 *
 * 	Authors: José M. Martínez (josem.martinez@uam.es), Paula Moral (paula.moral@uam.es) & Juan Carlos San Miguel (juancarlos.sanmiguel@uam.es)
 *	VPULab-UAM 2020
 */

#include <opencv2/opencv.hpp>
#include "fgseg.hpp"
#include <vector>

using namespace fgseg;

//Peter KUn
using namespace cv;


//default constructor: deviation

bgs::bgs(int K, double dev_weight, bool selective, double alpha, bool rgb)
{
	_K=K;

	_dev_weight=dev_weight;

	_selective=selective;

	_rgb=rgb;

	_alpha=alpha;


}

//default destructor
bgs::~bgs(void)
{
}

//method to initialize bkg (first frame - hot start)
void bgs::init_bkg(cv::Mat Frame)
{

	if (!_rgb){
		cvtColor(Frame, Frame, COLOR_BGR2GRAY); // to work with gray even if input is color
		//ADD YOUR CODE HERE
		//...

		//Init.: these are not useful in this task..
		_bkg=Frame;
		_bgsmask=Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1);
		_diff=Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1);
		_shadowmask=Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1);
		//




			means = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1);
			devs = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1);






			//Init. the weights, means, the first will be based on the Frame, the others will be random
			for(int i=0; i<Frame.rows; i++)
			{
				for(int j=0; j<Frame.cols; j++)
					{

						//First one based on the current Frame
						means.at<double>(i, j)=(double) Frame.at<uchar>(i, j);
						devs.at<double>(i, j)=_dev_weight;



					}
			}

	}
	else{
		cout << "Colour currently not supported" << endl;
		exit(1);
	}

	_counter = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1);


}

//method to perform BackGroundSubtraction
void bgs::bkgSubtraction(cv::Mat Frame)
{

	if (!_rgb){
		cvtColor(Frame, Frame, COLOR_BGR2GRAY); // to work with gray even if input is color
		Frame.copyTo(_frame);
		//Checking whether the current frame values are in one of the Gaussian

		//Making the counting part

		//Helper variables for this part


		_bgsmask=_bgsmask/255;

		Mat bkg_fore;
		Mat frame_fore;
		Mat counter_helper;

		//If foreground, then the pixel counting gets +1

		_counter=_counter+_bgsmask;

		//incorporating with background, when the pixel counter reaches the threshold
		//Inverse binary thresholding
		threshold(_counter, _countermask, _K, 1, 1);
		_bkg.copyTo(bkg_fore, _countermask);
		Frame.copyTo(frame_fore, 1-_countermask);


		//COunters in the background pixels should be set 0
		_counter.copyTo(counter_helper, _bgsmask);
		_counter=counter_helper;

		_bkg=bkg_fore+frame_fore;

		//Transforming back from the binary form
		_bgsmask=255*_bgsmask;



		for(int i=0; i<Frame.rows; i++)
		{
			for(int j=0; j<Frame.cols; j++)
			{
				//indicates that the current frame is already in one of the Gaussians
				//Current frame
				double f=Frame.at<uchar>(i, j);

				double m=(double) _bkg.at<uchar>(i, j);
				double d=devs.at<double>(i, j);

					//dev/weight ratio is fixed!
					if((abs(f-m)>3*d))
					{
						_bgsmask.at<uchar>(i, j)=255;

					}
					else if(_selective)
					{
						_bgsmask.at<uchar>(i, j)=0;
						m=_alpha*f+(1-_alpha)*m;
						d=sqrt(_alpha*((f-m)*(f-m))+(1-_alpha)*d*d);

					}
					else
					{
						_bgsmask.at<uchar>(i, j)=0;
					}

					if(!_selective)
					{
						m=_alpha*f+(1-_alpha)*m;
						d=sqrt(_alpha*((f-m)*(f-m))+(1-_alpha)*d*d);
					}

						means.at<double>(i, j)=m;
						//cout<<d<<endl;
						devs.at<double>(i, j)=d;

						_bkg.at<uchar>(i, j)=(unsigned char) m;
						_diff.at<uchar>(i, j)=(unsigned char) (f-m);
			}
		}



	}



}

	//method to detect and remove shadows in the BGS mask to create FG mask
/*
	void bgs::removeShadows(cv::Mat Frame)
	{
		Mat hvs_Frame;
		Mat hvs_bkg;

		double alpha=0.5;
		double beta=0.9;
		double tau_s=120;
		double tau_h=100;

		_fgmask=Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1);
		_shadowmask=Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1);
		return;



		cvtColor(Frame, hvs_Frame, CV_BGR2HSV);
		cvtColor(_bkg, hvs_bkg, CV_BGR2HSV);

		vector<Mat> splited_Frame;
		vector<Mat> splited_bkg;
		_shadowmask = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1);

		////Creating the separated channels
		split(hvs_Frame, splited_Frame);
		split(hvs_bkg, splited_bkg);

		//FIRST VERSION- Faster!!!!!!!!!

		Mat diff_Sat;
		Mat diff_Hue;
		Mat_<double> div_Val;

		absdiff(splited_Frame[1], splited_bkg[1], diff_Sat);
		absdiff(2*splited_Frame[0], 2*splited_bkg[0], diff_Hue);
		divide(splited_Frame[2], splited_bkg[2], div_Val);

		_shadowmask=(alpha<=div_Val) & (div_Val<=beta) & (diff_Sat<=tau_s) & (min(360-div_Val, div_Val)<=tau_h);

		_shadowmask=_shadowmask & _bgsmask;

		absdiff(_bgsmask, _shadowmask, _fgmask);// eliminates shadows from bgsmask

	}

	*/
	//ADD ADDITIONAL FUNCTIONS HERE








