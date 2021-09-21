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

using namespace fgseg;

//Peter KUn
using namespace cv;


//default constructor

bgs::bgs(double threshold, double alpha, bool selective_bkg_update, bool rgb)
{
	_rgb=rgb;

	_threshold=threshold;

	_alpha=alpha;

	_selective_bkg_update=selective_bkg_update;
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

		_bkg = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1); // void function for Lab1.0 - returns zero matrix
		//ADD YOUR CODE HERE
		//...

		//The first frame: the first image
		_bkg=Frame;

		//...
	}
	else{
		//cout << "Colour currently not supported" << endl;
		//exit(1);




		Frame.copyTo(_bkg);


	}

}

//method to perform BackGroundSubtraction
void bgs::bkgSubtraction(cv::Mat Frame)
{

	if (!_rgb){
		cvtColor(Frame, Frame, COLOR_BGR2GRAY); // to work with gray even if input is color
		Frame.copyTo(_frame);


		absdiff(Frame, _bkg, _diff);

		threshold(_diff, _bgsmask, _threshold, 255, 0);


		//...
	}
	else{

		absdiff(Frame, _bkg, _diff);


		//Splitting RGB into R, G, B channels
		vector<Mat> splited_frame;



		////Creating the background mask
		split(_diff, splited_frame);
		for (size_t i = 0; i < splited_frame.size(); i++)
			threshold(splited_frame[i], splited_frame[i], _threshold, 255, 0);

		//Merging the results into one
		_bgsmask = splited_frame[0].clone();

		for(size_t i = 1; i < splited_frame.size(); i++) _bgsmask |= splited_frame[i];

	}

	if(!_selective_bkg_update) {
		addWeighted( Frame, _alpha, _bkg, 1-_alpha, 0.0, _bkg);

	}
	else
	{

		Mat frame_back;
		Mat bkg_back;

		Mat bkg_fore;
		//Here the background pixels have 1 value
		Frame.copyTo(frame_back, _fgmask);
		_bkg.copyTo(bkg_back, _fgmask);

		//Here the foreground pixels have 1 value
		_bkg.copyTo(bkg_fore, _bgsmask);


		addWeighted( frame_back, _alpha, bkg_back, 1-_alpha, 0.0, _bkg);
		_bkg=_bkg+bkg_fore;



	}

}

	//method to detect and remove shadows in the BGS mask to create FG mask
	void bgs::removeShadows()
	{
		// init Shadow Mask (currently Shadow Detection not implemented)
		_bgsmask.copyTo(_shadowmask);// creates the mask (currently with bgs)

		//ADD YOUR CODE HERE
		//...
		absdiff(_bgsmask, _bgsmask, _shadowmask);// currently void function mask=0 (should create shadow mask)
		//...

		absdiff(_bgsmask, _shadowmask, _fgmask);// eliminates shadows from bgsmask
	}

	//ADD ADDITIONAL FUNCTIONS HERE




