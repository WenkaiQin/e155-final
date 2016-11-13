#include <iostream>
#include <stdio.h>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

Point findColorCenter(Mat imgThresholded) {
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;

    findContours( imgThresholded, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE,Point(0, 0) );
    
    if (contours.size() == 0) {
        return Point(-1, -1);
    }

    double biggestContourArea = 0;
    vector<Point> biggestContour = *(contours.begin());

    for (vector< vector<Point> >::iterator it = contours.begin(); it != contours.end(); ++it) {
        double area = contourArea(*it);
        if (biggestContourArea < area) {
            biggestContourArea = area;
            biggestContour = *it;
        }
    } 
    Rect rec = boundingRect(biggestContour);
    Point center = Point( (rec.x+rec.width)/2, (rec.y+rec.height)/2 );
    return center;
}

Mat thresholdImage(Mat frame, int color) {
    int lowH = 0;
    int highH = 179;
    int lowS = 0;
    int highS = 255;
    int lowV = 0;
    int highV = 255;
    if (color == 1) {
        // color is yellow    
        lowH = 0;
        highH = 179;
        lowS = 0;
        highS = 255;
        lowV = 0;
        highV = 255;
    }
    else if (color == 2) {
        // color is green
        lowH = 0;
        highH = 179;
        lowS = 0;
        highS = 255;
        lowV = 0;
        highV = 255;
    }
    else {
        lowH = 0;
        highH = 179;
        lowS = 0;
        highS = 255;
        lowV = 0;
        highV = 255;
    }
    
    Mat frameHSV, imgThresholded;
    cvtColor(frame, frameHSV, COLOR_BGR2HSV);
    inRange(frameHSV, Scalar(lowH, lowS, lowV), Scalar(highH, highS, highV), imgThresholded);

//    dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
//    erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
    return imgThresholded;
}


int main() {

    VideoCapture cap(0);

    if (!cap.isOpened()) {
        cout << "Cannot open the video cam" << endl;
        return -1;
    }

    int width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    int height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    
    namedWindow("Video_Capture", CV_WINDOW_AUTOSIZE);
  //  namedWindow("Thresholded_Capture", CV_WINDOW_AUTOSIZE);

    while (true) {
        Mat frame, imgThresholdedYellow, imgThresholdedGreen;
        bool frameLoaded = cap.read(frame);
        if (!frameLoaded) {
            cout << "Frame is not loaded correctly" << endl;
            break;
        }
        imgThresholdedYellow = thresholdImage(frame, 1);
        imgThresholdedGreen  = thresholdImage(frame, 2);   
        double yellowX, yellowY, greenX, greenY;

        imshow("Video_Capture", frame);
//        imshow("Thresholded_Capture", imgThresholdedYellow);

        Point yellowCenter = findColorCenter(imgThresholdedYellow);
        Point greenCenter = findColorCenter(imgThresholdedGreen);
        yellowX = yellowCenter.x;
        yellowY = yellowCenter.y;
        greenX = greenCenter.x;
        greenY = greenCenter.y;

        cout << "Yellow: " << yellowX << ", " << yellowY << endl;
        cout << "Green: " << greenX << ", " << greenY << endl;
        // If keypress is esc for 30ms, exit the program
        if (waitKey(30) == 27) {
            cout << "Exit the program" << endl;
            break;
        }
    }
    return 0;
}

