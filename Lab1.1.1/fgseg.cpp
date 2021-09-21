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
bgs::bgs(double threshold, bool rgb)
{
	_rgb=rgb;

	_threshold=threshold;
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

		Frame.copyTo(_bkg);

	}
	else{

		Frame.copyTo(_bkg);

	}

}

//method to perform BackGroundSubtraction
void bgs::bkgSubtraction(cv::Mat Frame)
{

	if (!_rgb){
		cvtColor(Frame, Frame, COLOR_BGR2GRAY); // to work with gray even if input is color
		Frame.copyTo(_frame);


		//Simple binary thresholding performed
		absdiff(Frame, _bkg, _diff);

		threshold(_diff, _bgsmask, _threshold, 255, 0);

	}
	else{


		absdiff(Frame, _bkg, _diff);


		//Splitting RGB into R, G, B channels
		vector<Mat> splited_frame;

		split(_diff, splited_frame);



		for (size_t i = 0; i < splited_frame.size(); i++)
			threshold(splited_frame[i], splited_frame[i], _threshold, 255, 0);

		//Merging the results into one
		_bgsmask = splited_frame[0].clone();

		for(size_t i = 1; i < splited_frame.size(); i++) _bgsmask |= splited_frame[i];

	    }

}

//method to detect and remove shadows in the BGS mask to create FG mask
void bgs::removeShadows()
{
	// init Shadow Mask (currently Shadow Detection not implemented)
	_bgsmask.copyTo(_shadowmask); // creates the mask (currently with bgs)

	//ADD YOUR CODE HERE
	//...
	absdiff(_bgsmask, _bgsmask, _shadowmask);// currently void function mask=0 (should create shadow mask)
	//...

	absdiff(_bgsmask, _shadowmask, _fgmask); // eliminates shadows from bgsmask
}

//ADD ADDITIONAL FUNCTIONS HERE




