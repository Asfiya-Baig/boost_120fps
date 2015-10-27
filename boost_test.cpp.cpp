#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <time.h>
#include <fstream>
#include<windows.h>
#include <cstdlib>
#include <math.h>
#include <ctime>
#include <fcntl.h>
#include <errno.h>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/thread.hpp"

using namespace std;
using namespace boost::posix_time;
using namespace cv;




int fps=120;
char key1, key2;
//Gloabal variables
Mat image, image1;
char fname[100], fname1[100];
char patdat[100], patdat1[100], patdat2[100], patdat3[100];
int main()
{
    //vars for 120 fps
	time_duration td, td1;
	ptime nextFrameTimestamp, currentFrameTimestamp, initialLoopTimestamp, finalLoopTimestamp;
	int delayFound = 0;



    //Setting up communication with arduino
HANDLE hSerial;
hSerial = CreateFile("COM4",GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
if(hSerial==INVALID_HANDLE_VALUE)
{
if(GetLastError()==ERROR_FILE_NOT_FOUND)
{
cout<<"File not found"<<endl;//serial port does not exist. Inform user.
}
cout<<"error :/"<<endl;//some other error occurred. Inform user.
}

DCB dcbSerialParams = {0};
dcbSerialParams.DCBlength=sizeof(dcbSerialParams);
if (!GetCommState(hSerial, &dcbSerialParams)) {
printf("error getting state");
}
dcbSerialParams.BaudRate=CBR_9600;
dcbSerialParams.ByteSize=8;
dcbSerialParams.StopBits=ONESTOPBIT;
dcbSerialParams.Parity=NOPARITY;
if(!SetCommState(hSerial, &dcbSerialParams)){
printf("error setting serial port state");
}
int n=1;
DWORD bytesWritten;



int framerate = 120;
double f=120;
char lpBuf[1] = {0};
ifstream infile("..//..//..//data.csv");		//Takes input from basic(GUI) for reading patient data and stores in the data.csv file
int a;
double fps1, fps2;
infile>>patdat;			//reading patients first name
infile>>patdat1;		//reading patients last name
infile>>patdat2;        //reading patients age
infile>>patdat3;        //reading patients gender
//cout<<"enter camera number"<<endl;
//cin>>a;
VideoCapture capture(0);	//Opens the camera of the device connected
VideoCapture capture1(1);

capture.set(CV_CAP_PROP_FRAME_WIDTH,640);
capture.set(CV_CAP_PROP_FRAME_HEIGHT,480);
capture.set(CV_CAP_PROP_EXPOSURE, -2);
double exp1 = capture.get(CV_CAP_PROP_EXPOSURE);

capture1.set(CV_CAP_PROP_FRAME_WIDTH,640);
capture1.set(CV_CAP_PROP_FRAME_HEIGHT,480);
capture1.set(CV_CAP_PROP_EXPOSURE, -2);
double exp2 = capture1.get(CV_CAP_PROP_EXPOSURE);
cout<<"exposure values"<<exp1<<"    "<<exp2<<endl;

capture>>image;			//Extract a frame and store in image matrix.
capture1>>image1;
int framewidth=image.cols;  //Obtain size of image matrix in the form of row7 s and columns
int frameheight=image.rows;


strcpy(fname, "..//..//Videos//");	//declaring path for storing video
strcat(fname, patdat);		//appending patients first name on the video
strcat(fname, "_");		//appending patients name on the video
strcat(fname, patdat1);		//appending patients last name on the video
strcat(fname, "_");
strcat(fname, patdat2);
strcat(fname, "_");
strcat(fname, patdat3);
strcat(fname, "_");
strcat(fname, "left");
strcat(fname,".avi");

strcpy(fname1, "..//..//Videos//");	//declaring path for storing video
strcat(fname1, patdat);		//appending patients first name on the video
strcat(fname1, "_");		//appending patients name on the video
strcat(fname1, patdat1);		//appending patients last name on the video
strcat(fname1, "_");
strcat(fname1, patdat2);
strcat(fname1, "_");
strcat(fname1, patdat3);
strcat(fname1, "_");
strcat(fname1, "right");
strcat(fname1,".avi");
fps1=20;
/*Define VideoiWriter object for storing the video*/
VideoWriter video(fname,CV_FOURCC('M','J','P','G'),fps1,cvSize(framewidth, frameheight));  //CV_FOURCC('M','J','P','G') is a motion-jpeg codec
VideoWriter video1(fname1,CV_FOURCC('M','J','P','G'),fps1,cvSize(framewidth, frameheight));

// initialize initial timestamps
	nextFrameTimestamp = microsec_clock::local_time();
	currentFrameTimestamp = nextFrameTimestamp;
	td = (currentFrameTimestamp - nextFrameTimestamp);

while(1)
{
    // wait for X microseconds until 1second/framerate time has passed after previous frame write
    while(td.total_microseconds() < 1000000/framerate)
        {
            //determine current elapsed time
            currentFrameTimestamp = microsec_clock::local_time();
			td = (currentFrameTimestamp - nextFrameTimestamp);
		}
    //	 determine time at start of write
    initialLoopTimestamp = microsec_clock::local_time();

    capture>>image;
    capture1>>image1;

    Size sz1 = image.size();
    Size sz2 = image1.size();
    Mat im3(sz1.height, sz1.width+sz2.width, CV_8UC3);
    Mat left(im3, Rect(0, 0, sz1.width, sz1.height));
    image.copyTo(left);
    Mat right(im3, Rect(sz1.width, 0, sz2.width, sz2.height));
    image1.copyTo(right);
    namedWindow("Video",WINDOW_NORMAL);
    imshow("Video",im3);

    key1 = waitKey(100); 	//Capture Keyboard stroke
    if (char(key1) == 32 )
	{
	        break; 		//If you hit ESC key loop will break and code will terminate

	}
}

WriteFile(hSerial, "s",n, &bytesWritten, NULL);

cout<<"No of bytes written are: "<<bytesWritten<<endl;
cout<<"The test has started"<<endl;
DWORD start = GetTickCount();
cout<<"Time = "<< start;

while(1)
{

    capture>>image;
    video<<image;
    capture1>>image1;
    video1<<image1;
    Size sz1 = image.size();
    Size sz2 = image1.size();
    Mat im3(sz1.height, sz1.width+sz2.width, CV_8UC3);
    Mat left(im3, Rect(0, 0, sz1.width, sz1.height));
    image.copyTo(left);
    Mat right(im3, Rect(sz1.width, 0, sz2.width, sz2.height));
    image1.copyTo(right);
    namedWindow("Video",WINDOW_NORMAL);
    imshow("Video",im3);

    //write previous and current frame timestamp to console
    cout << nextFrameTimestamp << " " << currentFrameTimestamp << " ";


	// add 1second/framerate time for next loop pause
	nextFrameTimestamp = nextFrameTimestamp + microsec(1000000/framerate);

	// reset time_duration so while loop engages
	td = (currentFrameTimestamp - nextFrameTimestamp);
    cout<< (td) <<" "<<endl;
	//determine and print out delay in ms, should be less than 1000/FPS
	//occasionally, if delay is larger than said value, correction will occur
	//if delay is consistently larger than said value, then CPU is not powerful
	// enough to capture/decompress/record/compress that fast.
	finalLoopTimestamp = microsec_clock::local_time();
	td1 = (finalLoopTimestamp - initialLoopTimestamp);
	delayFound = td1.total_milliseconds();
	cout << delayFound << endl;

	//output will be in following format
	//[TIMESTAMP OF PREVIOUS FRAME] [TIMESTAMP OF NEW FRAME] [TIME DELAY OF WRITING]
    // video1<<im3;

	//DWORD dwRead;
    //BOOL fWaitingOnRead = FALSE;
    //OVERLAPPED osReader = {0};


    // Create the overlapped event. Must be closed before exiting
    // to avoid a handle leak.
    //osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    //if (osReader.hEvent == NULL)
    // Error creating overlapped event; abort.

    //if (!fWaitingOnRead) {
    // Issue read operation.
    //if (!ReadFile(hSerial, lpBuf, 1, &dwRead, &osReader)) {
    //if (GetLastError() != ERROR_IO_PENDING) ;    // read not delayed?
         // Error in communications; report it.
    //else
      //   fWaitingOnRead = TRUE;
    //}
    //else {
      // read completed immediately
     // ReadFile(hSerial, lpBuf, 1, &dwRead, &osReader);
      //HandleASuccessfulRead(lpBuf, dwRead);
      //cout<<"Arduino-Read: "<<lpBuf[0]<<"  ->"<<"Excitation number"<<lpBuf[0]<<endl;
    //}
    //}
    DWORD elapsed = GetTickCount() - start;
    //cout<<"time elapsed ="<<elapsed/1000<<"secons"<<endl;
    cout<<"time left for test to end = "<<39-(elapsed/1000)<<endl;
    key2 = waitKey(100); 	//Capture Keyboard stroke
    if (char(key2) == 27 || elapsed==39000  )
	{
        cout<<"Test is complete"<<endl;
        break; 		//Ijf you hit ESC key loop will break and code will terminate  || lpBuf[0]=='9'

	}
}
CloseHandle(hSerial);   //Ending communication with arduino


capture.release();
capture1.release();

return 0;
}

