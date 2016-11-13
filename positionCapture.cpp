#include <iostream>
#include <stdio.h>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

int main() {

    VideoCapture cap(0);

    if (!cap.isOpened()) {
        cout << "Cannot open the video cam" << endl;
        return -1;
    }

    int width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    int height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    
    namedWindow("Video_Capture", CV_WINDOW_AUTOSIZE);
    namedWindow("Control", CV_WINDOW_AUTOSIZE);
    namedWindow("Thresholded_Capture", CV_WINDOW_AUTOSIZE);

    int lowH = 0;
    int highH = 179;
    int lowS = 0;
    int highS = 255;
    int lowV = 0;
    int highV = 255;

    cvCreateTrackbar("Low H", "Control", &lowH, 179);
    cvCreateTrackbar("High H", "Control", &highH, 179);
    cvCreateTrackbar("Low S", "Control", &lowS, 255);
    cvCreateTrackbar("High S", "Control", &highS, 255);
    cvCreateTrackbar("Low V", "Control", &lowV, 255);
    cvCreateTrackbar("High V", "Control", &highV, 255);


    while (true) {
        Mat frame;
        bool frameLoaded = cap.read(frame);
        if (!frameLoaded) {
            cout << "Frame is not loaded correctly" << endl;
            break;
        }
        
        Mat frameHSV;
        cvtColor(frame, frameHSV, COLOR_BGR2HSV);
        Mat imgThresholded;
        inRange(frameHSV, Scalar(lowH, lowS, lowV), Scalar(highH, highS, highV), imgThresholded);

        dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
        erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

        double x, y;

        Moments mom = moments(imgThresholded);
        if (mom.m00 > 10000) {
            x = (double) mom.m01 / (double) mom.m00;
            y = (double) mom.m10 / (double) mom.m00;
        }

        imshow("Video_Capture", frame);
        imshow("Thresholded_Capture", imgThresholded);

        cout << "X position: ";
        cout << x;
        cout << " Y position: ";
        cout << y << endl;

        if (waitKey(30) == 27) {
            cout << "Exit the program" << endl;
            break;
        }
    }
    return 0;
}


