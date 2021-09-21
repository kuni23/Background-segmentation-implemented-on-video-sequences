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

bgs::bgs(double threshold, double alpha, bool selective_bkg_update, int threshold2, bool rgb)
{
	_rgb=rgb;

	_threshold=threshold;

	_alpha=alpha;

	_selective_bkg_update=selective_bkg_update;

	_threshold2=threshold2;
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

		Frame.copyTo(_bkg);


	}
	//init the counter for the task 1.1.3
	_counter = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1);


}

//method to perform BackGroundSubtraction
void bgs::bkgSubtraction(cv::Mat Frame)
{

	if (!_rgb){
		cvtColor(Frame, Frame, COLOR_BGR2GRAY); // to work with gray even if input is color
		Frame.copyTo(_frame);

		//ADD YOUR CODE HERE
		//...

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

		//Merging the results into one Mat struct
		_bgsmask = splited_frame[0].clone();

		for(size_t i = 1; i < splited_frame.size(); i++) _bgsmask |= splited_frame[i];
	}

	//Making the masks to a binary form
	_bgsmask=_bgsmask/255;

	//Making the updating
	if(!_selective_bkg_update) {
		addWeighted( Frame, _alpha, _bkg, 1-_alpha, 0.0, _bkg);

	}
	else
	{


		//Making the selective update
		Mat frame_back;
		Mat bkg_back;
		Mat bkg_fore;

		//Here the background pixels have 1 value
		Frame.copyTo(frame_back, 1-_bgsmask);
		_bkg.copyTo(bkg_back, 1-_bgsmask);

		//Here the foreground pixels have 1 value
		_bkg.copyTo(bkg_fore, _bgsmask);


		addWeighted( frame_back, _alpha, bkg_back, 1-_alpha, 0.0, _bkg);
		_bkg=_bkg+bkg_fore;

	}


	//Making the counting part

	//Helper variables for this part


	Mat bkg_fore;
	Mat frame_fore;
	Mat counter_helper;
	Mat _countermask;

	//If foreground, then the pixel counting gets +1

	_counter=_counter+_bgsmask;

	//incorporating with background, when the pixel counter reaches the threshold
	//Inverz binary thresholding
	threshold(_counter, _countermask, _threshold2, 1, 1);
	_bkg.copyTo(bkg_fore, _countermask);
	Frame.copyTo(frame_fore, 1-_countermask);
	_bkg=bkg_fore+frame_fore;


	//COunters in the background pixels should be set to 0
	_counter.copyTo(counter_helper, _bgsmask);
	_counter=counter_helper;


	//Transforming back from the binary form
	_bgsmask=255*_bgsmask;

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




