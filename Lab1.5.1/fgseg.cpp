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


//default constructor

bgs::bgs(int K, double dev_weight, double W_th, double alpha, bool rgb)
{
	_K=K;

	_dev_weight=dev_weight;

	_W_th=W_th;

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


		//Checking the size similarity
		if(_K!=(sizeof(weights)/sizeof(*weights)))
		{
			cout<<"Size problem!!"<<endl;
			exit(1);
		}


		//Init. the weights, means, devs to 0
		for(int k=0; k<_K; k++)
		{
			weights[k] = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1);
			means[k] = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1);
			devs[k] = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1);

		}


			//Init. the weights, means, devs: the first will be based on the Frame, the others will be random
			for(int i=0; i<Frame.rows; i++)
			{
				for(int j=0; j<Frame.cols; j++)
					{

						//First one based on the current Frame
						weights[0].at<double>(i, j)=1;
						means[0].at<double>(i, j)=(double) Frame.at<uchar>(i, j);
						devs[0].at<double>(i, j)=_dev_weight;



						for(int k=1; k<_K; k++)
						{
							//random means, lower weights, higher deviations
							weights[k].at<double>(i, j)=0.05;
							means[k].at<double>(i, j)=rand() % 254 + 1;
							devs[k].at<double>(i, j)=_dev_weight*2;


						}
						normalize(i, j);
					}
			}

	}
	else{
		cout << "Colour currently not supported" << endl;
		exit(1);
	}

}

//method to perform BackGroundSubtraction
void bgs::bkgSubtraction(cv::Mat Frame)
{

	if (!_rgb){
		cvtColor(Frame, Frame, COLOR_BGR2GRAY); // to work with gray even if input is color
		Frame.copyTo(_frame);

		for(int i=0; i<Frame.rows; i++)
		{
			for(int j=0; j<Frame.cols; j++)
			{
				//indicates that the current frame is already in one of the Gaussians
				bool found = false;
				//Current frame
				double f=Frame.at<uchar>(i, j);
				for(int k=0; k<_K; k++)
				{
					//Current values
					double w=weights[k].at<double>(i, j);
					double m=means[k].at<double>(i, j);
					double d=devs[k].at<double>(i, j);

					//dev/weight ratio is fixed!
					if((abs(f-m)<d) & (!found))
					{
						found=true;

						//If it is under threshold, then it is foreground
						if(IsFore(i, j, k))
						{
							_bgsmask.at<uchar>(i, j)=255;

						}
						else
						{
							_bgsmask.at<uchar>(i, j)=0;

						}

						//Updating
						w=(1-_alpha)*w+_alpha;
						m=_alpha*f+(1-_alpha)*m;
						d=sqrt(_alpha*((f-m)*(f-m))+(1-_alpha)*d*d);
					}
					else
					{
						//Updating
						w=(1-_alpha)*w;
					}

					//Updating with the refreshed values
					weights[k].at<double>(i, j)=w;
					means[k].at<double>(i, j)=m;
					devs[k].at<double>(i, j)=d;


				}

				if(!found)
				{
					//Creating new Gaussian and pixel becomes foreground


					//Creating new Gaussian
					weights[_K-1].at<double>(i, j)=0.05;
					means[_K-1].at<double>(i, j)=f;
					devs[_K-1].at<double>(i, j)=_dev_weight*2;
					_bgsmask.at<uchar>(i, j)=255;


				}

					//Normalizing the weights
					normalize(i, j);
					//Making the descending order based on weights
					ordering(i, j);

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

	//Weight normalization
	void bgs::normalize(int a, int b)
	{
		double sum = 0.0;

		for (int i = 0; i < _K; i++) {
		    sum += weights[i].at<double>(a, b);
		}

		for (int i = 0; i < _K; i++) {
			weights[i].at<double>(a, b)=weights[i].at<double>(a, b)*(1/sum);
		}

	}

	//Making the descending order in weights, devs and means at the same time
	//Reordering the weighs, means, devs arrays

	void bgs::ordering(int a, int b)
	{


		vector<double> w;
		vector<double> m;
		vector<double> d;
		for(int k=0; k<_K; k++)
		{
			w.push_back(weights[k].at<double>(a, b));
			m.push_back(means[k].at<double>(a, b));
			d.push_back(devs[k].at<double>(a, b));

		}

		vector<double> w2=w;
		vector<double> m2;
		vector<double> d2;

		sort(w.rbegin(), w.rend());

		for(size_t i=0; i<w.size(); i++)
		{
			for(size_t j=0; j<w2.size(); j++)
			{
				if(w[i]==w2[j]) {
					m2.push_back(m[j]);
					d2.push_back(d[j]);
				}
			}
		}
		m=m2;
		d=d2;

		for(int k=0; k<_K; k++)
		{
			weights[k].at<double>(a, b)=w[k];
			means[k].at<double>(a, b)=m[k];
			devs[k].at<double>(a, b)=d[k];
		}

	}

	//It is foreground pixel if the sum of the bigger/upper Gaussians is the at least W_th
	bool bgs::IsFore(int a, int b, int k)
	{
		double sum=0;


		for(int j=0; j<k; j++)
		{
			sum=sum+weights[j].at<double>(a, b);
		}

		if(sum>=_W_th)
		{
			return true;
		}
		else
		{
			return false;
		}

	}






