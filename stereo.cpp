// Luke Mitchell 2013
// https://github.com/lukem512/stereodepth

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

String logo_cascade_name = "haarcascade_frontalface_default.xml"; 
CascadeClassifier logo_cascade;

// detect faces
std::vector<Rect> faceDetect(Mat &frame);

// compute the depth of a common point on 2 images
float depth(int xl, int xr, float T, float f);

int main( int argc, const char** argv )
{
	VideoCapture cap0(0), cap1(1);
	Mat frame0, frame1, frame0_gray, frame1_gray;
	
	if(!cap0.isOpened())
	{
		printf("Error: could not load camera 0.\n");
		return -1;
	}
	
	if(!cap1.isOpened())
	{
		printf("Error: could not load camera 1.\n");
		return -1;
	}
	
	// load the Haar face classifier
	if( !logo_cascade.load(logo_cascade_name) )
	{
	  printf("Error loading Haar cascade\n");
	  return -1;
	}
	
	namedWindow("Camera 0", 2);
  namedWindow("Camera 1", 2);
	
	while (1) {
	  waitKey(20);
	  cap0 >> frame0;
	  cap1 >> frame1;
	  
	  if(!frame0.data)
		{
			printf("Error: no frame data from camera 0\n");
			break;
		}
		
	  if(!frame1.data)
		{
			printf("Error: no frame data from camera 1\n");
			break;
		}
		
		flip(frame0, frame0, 1);
		flip(frame1, frame1, 1);
		
		cvtColor(frame0, frame0_gray, CV_BGR2GRAY);
    equalizeHist(frame0_gray, frame0_gray);
    cvtColor(frame1, frame1_gray, CV_BGR2GRAY);
    equalizeHist(frame1_gray, frame1_gray);
    
    // detect faces
    std::vector<Rect> faces0 = faceDetect(frame0_gray);
    std::vector<Rect> faces1 = faceDetect(frame1_gray);
    
    // get largest face from each
    int index0 = -1, index1 = -1, curMax = 0;
    
    for (int i = 0; i < faces0.size(); i++) {
      if ((faces0[i].width * faces0[i].height) > curMax) {
        index0 = i;
      }
    }
    
    curMax = 0;
    for (int i = 0; i < faces1.size(); i++) {
      if ((faces1[i].width * faces1[i].height) > curMax) {
        index1 = i;
      }
    }
    
    // are they the sameish size?
    if (index0 != -1 && index1 != -1) {
      int size0 = faces0[index0].width * faces0[index0].height;
      int size1 = faces1[index1].width * faces1[index1].height;

      // magic value, for now
      if (abs(size0 - size1) < 4000) {
        float d = depth(faces0[index0].x, faces1[index1].x, 5.0, 40.0);
        cout << "depth is " << d << endl;
      }
    }
		
	  imshow("Camera 0", frame0_gray);
	  imshow("Camera 1", frame1_gray);
	}
	
	return EXIT_SUCCESS;
}

// xl is an x-coord on the left image
// xr is an x-coord on the right image
// T is the distance between the cameras
// f is the focal length of the cameras (assumed the same)
float depth(int xl, int xr, float T, float f)
{
  return (f * T) / (float)(xl - xr);
}

// performs Haar face-detection
std::vector<Rect> faceDetect(Mat &frame)
{ 
  std::vector<Rect> faces;
  logo_cascade.detectMultiScale( frame, faces, 1.1, 1, 0|CV_HAAR_SCALE_IMAGE, Size(50, 50), Size(500,500) );

  for( int i = 0; i < faces.size(); i++ )
  {
    rectangle(frame, Point(faces[i].x, faces[i].y), Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height), Scalar( 0, 255, 0 ), 2);
  }
  
  return faces;
}


