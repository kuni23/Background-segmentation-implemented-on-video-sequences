/* Applied Video Sequence Analysis (AVSA)
 *
 *	LAB1: Background Subtraction - Unix version
 *	Lab1.0AVSA2020.cpp
 *
 * 	Authors: José M. Martínez (josem.martinez@uam.es), Paula Moral (paula.moral@uam.es) & Juan Carlos San Miguel (juancarlos.sanmiguel@uam.es)
 *	VPULab-UAM 2020
 */


#include <iostream> //system libraries
#include <sstream> //for stringstream

#include <opencv2/opencv.hpp> //opencv libraries


//Header ShowManyImages
#include "ShowManyImages.hpp"

//Header fseg
#include "fgseg.hpp"

//namespaces
using namespace std; //to avoid using'std' to declare std functions and variables (e.g., std::out -> out)
using namespace cv;  //to avoid using 'cv' to declare OpenCV functions and variables (e.g., cv::Mat -> Mat)

// using namespace fgseg;

#define _CONSOLE_DEBUG 1 // flag for showing console debug messages (old style ;) )

int main(int argc, char ** argv)
{

	double t, acum_t; //variables for execution time
	int t_freq = getTickFrequency();

	//Paths for the dataset
	// In this example we assume that the dataset is available at
	 //"/home/avsa/datasets/dataset2012lite/dataset/baseline/...
	string dataset_path = "/home/avsa/AVSA2020datasets/dataset2012lite/dataset"; //SET THIS DIRECTORY according to your download
	string dataset_cat[1] = {"baseline"};
	//string baseline_seq[4] = {"highway","office","pedestrians","PETS2006"};
	string baseline_seq[3] = {"canoe", "fountain01","fountain02"};
	string image_path = "/input/in%06d.jpg"; //path to images - this format allows to read consecutive images with filename inXXXXXX.jpq (six digits) starting with 000001
//
	//string dataset_path = "/home/avsa/AVSA2020datasets/AVSASlidesVideos_dataset"; //SET THIS DIRECTORY according to your download
	//string dataset_cat[1] = {"foregroundSeg"};
	//string baseline_seq[5] = {"hall.avi", "empty_office.avi","stationary_objects.avi","eps_hotstart.avi","eps_shadows.avi"};
	//string image_path = ""; //path to images - this format allows to read consecutive images with filename inXXXXXX.jpq (six digits) starting with 000001

// if (argc==3)	{input=argv[3]);}

	//Paths for the results
	// In this example we assume that the results are stored in the project directory
	// "/home/avsa/eclipse-workspace/Lab1.1AVSA2020/
	string project_root_path = "/home/avsa/AVSA2020results/"; //SET THIS DIRECTORY according to your project
	string project_name = "Lab1.5.1AVSA2020"; // project exe name
	string results_path = project_root_path+"/"+project_name+"/results";

	// create directory to store results
	string makedir_cmd = "mkdir "+project_root_path+"/"+project_name;
	system(makedir_cmd.c_str()); //may raise error in console if path exists, but will work ...
	makedir_cmd = "mkdir "+results_path;
	system(makedir_cmd.c_str()); //may raise error in console if path exists, but will work ...


	int NumCat = sizeof(dataset_cat)/sizeof(dataset_cat[0]); //number of categories (have faith ... it works! ;) ... each string size is 32 -at leat for the current values-)

	//Loop for all categories
	for (int c=0; c<NumCat; c++ )
	{
		// create directory to store results for category
		string makedir_cmd = "mkdir "+results_path + "/" + dataset_cat[c];
		system(makedir_cmd.c_str()); //may raise error in console if path exists, but will work ...

		int NumSeq = sizeof(baseline_seq)/sizeof(baseline_seq[0]);  //number of sequences per category ((have faith ... it works! ;) ... each string size is 32 -at leat for the current values-)

		//Loop for all sequence of each category
		for (int s=0; s<NumSeq; s++ )
		{
			VideoCapture cap;//reader to grab videoframes

			//Compose full path of images
			string inputvideo = dataset_path + "/" + dataset_cat[c] + "/" + baseline_seq[s] +image_path;
			cout << "Accessing sequence at " << inputvideo << endl;

			//open the video file to check if it exists
			cap.open(inputvideo);
			if (!cap.isOpened()) {
				cout << "Could not open video file " << inputvideo << endl;
			return -1;
			}

			// create directory to store results for sequence
			string makedir_cmd = "mkdir "+results_path + "/" + dataset_cat[c] + "/" + baseline_seq[s];
			system(makedir_cmd.c_str()); //may raise error in console if path exists, but will work ...

//			//windows to visualize results individually
//			namedWindow("Frame"); //current frame
//			namedWindow("BackGround"); // current background
//			namedWindow("BackGround Subtraction Mask"); // fgmask w/o ghosts and shadow removal
//			namedWindow("ForeGround Mask"); // final fgmask

			// TO CHANGE (comment/uncomment/...) FOR DIFF PROJECTS
			//background subtraction parameters

			//initial deviation value
			double dev_weight = 45;

			//alpha updating parameter
			double alpha=0.05;

			//B upper Gaussian threshold
			double W_th= 0.9;

			//number of Gaussians, should be the same like in the settings (fgseg.hpp), if not error will be displayed
			int K=4;

			bool rgb = false;

			//fgseg::bgs avsa_bgs(tau, rgb); //construct object of the bgs class
			//fgseg::bgs avsa_bgs(tau, alpha, selective_bkg_update, rgb); //construct object of the bgs class
			fgseg::bgs avsa_bgs(K, dev_weight, W_th, alpha, rgb); //construct object of the bgs class
			// END - TO CHANGE FOR DIFF PROJECTS


			//main loop
			Mat img; // current Frame

			int it=1;
			acum_t=0;
			Mat bg0;
			for (;;) {

				//get frame
				cap >> img;

				//check if we achieved the end of the file (e.g. img.data is empty)
				if (!img.data)
					break;
				// it=1 => initialize bkg
				 if (it==1)
				 	 {


					 avsa_bgs.init_bkg(img);
					 Mat aux=avsa_bgs.getBG();
					 aux.copyTo(bg0); // to visualize the first background (for analyzing results visually)
				 	 }

		   		//Time measurement
           		t = (double)getTickCount();

           		//Apply your bgs algorithm
		        //...


           		avsa_bgs.bkgSubtraction(img);

           		//Not useful if it is in greyscale
		        //avsa_bgs.removeShadows(img);

				//...

		        //Time measurement
		        t = (double)getTickCount() - t;
//		        if (_CONSOLE_DEBUG) cout << "bgs_seg = " << 1000*t/t_freq << " milliseconds."<< endl;
		        acum_t=+t;


		        Mat bg=avsa_bgs.getBG();

		        Mat diff=avsa_bgs.getDiff();
		        Mat bgsmask=avsa_bgs.getBGSmask();
		        Mat shdmask=avsa_bgs.getShadowMask();
		        Mat fgmask=avsa_bgs.getFGmask();


		        stringstream it_ss;
		      	it_ss << setw(6) << setfill('0') << it;

		       	Mat bg_diff;
		    	absdiff(bg, bg0, bg_diff);
		    	bg_diff *=10; // to see better the diff

		    	//Be the same, no shadow suppression
		    	fgmask=bgsmask;

		    	ShowManyImages("GMM", 8, img, bg, bg0, bg_diff, diff, bgsmask, shdmask, fgmask);


		        string outFile=results_path + "/" + dataset_cat[c] + "/" + baseline_seq[s] + "/" +"out"+ it_ss.str() +".png";

//		       	if (_CONSOLE_DEBUG){cout << outFile << endl;}

		        bool write_result=false;

		        write_result=imwrite(outFile, fgmask);
		        if (!write_result) printf("ERROR: Can't save fg mask.\n");

				//exit if ESC key is pressed
				if(waitKey(30) == 27) break;
				it++;

			} //main loop
			cout << it-1 << "frames processed in " << 1000*acum_t/t_freq << " milliseconds."<< endl;

			//release all resources

			cap.release();
			destroyAllWindows();
			waitKey(0); // (should stop till any key is pressed .. doesn't!!!!!)
		}
	}
	return 0;
}





