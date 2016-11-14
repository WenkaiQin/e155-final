#include <iostream>
#include <stdio.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

using namespace cv;
using namespace std;

int lowH_b = 0;
int highH_b = 255;
int lowS_b = 0;
int highS_b = 255;
int lowV_b = 0;
int highV_b = 255;

int lowH_g = 0;
int highH_g = 255;
int lowS_g = 0;
int highS_g = 255;
int lowV_g = 0;
int highV_g = 255;

const int deltaHSV = 20;

struct MouseParam {
    Mat img;
    Point pt;
    int color;
};

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
    
    Mat frameHSV, imgThresholded;
    cvtColor(frame, frameHSV, COLOR_BGR2HSV);

    if (color == 1) { 
        inRange(frameHSV, Scalar(lowH_b, lowS_b, lowV_b), Scalar(highH_b, highS_b, highV_b), imgThresholded);
    }
    else {
        inRange(frameHSV, Scalar(lowH_g, lowS_g, lowV_g), Scalar(highH_g, highS_g, highV_g), imgThresholded);
    }

    dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
    erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
    return imgThresholded;
}

void onMouse(int event, int x, int y, int flags, void* param) {

    Mat frameHSV;
    Point pt = Point(x, y);
    MouseParam* info = (MouseParam*) param;
    int H, S, V;
    cvtColor(info->img, frameHSV, COLOR_BGR2HSV);
    Vec3b pixel = frameHSV.at<Vec3b>(y, x);
    H = (int) pixel[0];
    S = (int) pixel[1];
    V = (int) pixel[2];
    info->color = event;
    info->pt = pt;
    if (event == 1) {
        // if it's blue
        lowH_b = H - deltaHSV;
        highH_b = H + deltaHSV;
        lowS_b = S - deltaHSV;
        highS_b = S + deltaHSV;
        lowV_b = V - deltaHSV;
        highV_b = V + deltaHSV;
        cout << "Blue: the HSV values are: H: ";
        cout << H << " S: " << S << " V: " << V << endl;
    }
    else if (event == 2) {
        // if it;s green
        lowH_g = H - deltaHSV;
        highH_g = H + deltaHSV;
        lowS_g = S - deltaHSV;
        highS_g = S + deltaHSV;
        lowV_g = V - deltaHSV;
        highV_g = V + deltaHSV;
        cout << "Green: the HSV values are: H: ";
        cout << H << " S: " << S << " V: " << V << endl;
    }
}

int main() {

    VideoCapture cap(0);

    if (!cap.isOpened()) {
        cout << "Cannot open the video cam" << endl;
        return -1;
    }
    
    Point bluePos;
    Point greenPos;
    
    MouseParam param;
    Mat frame, imgThresholdedYellow, imgThresholdedGreen;
    int width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    int height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    namedWindow("Video_Capture", CV_WINDOW_AUTOSIZE); 
    setMouseCallback("Video_Capture", onMouse, &param);
  //  namedWindow("Thresholded_Capture", CV_WINDOW_AUTOSIZE);

    while (true) {
        bool frameLoaded = cap.read(frame);
        if (!frameLoaded) {
            cout << "Frame is not loaded correctly" << endl;
            break;
        }
        param.img = frame;
        imgThresholdedYellow = thresholdImage(frame, 1);
        imgThresholdedGreen  = thresholdImage(frame, 2);   
        double yellowX, yellowY, greenX, greenY;

        imshow("Video_Capture", frame);
        imshow("Thresholded_Blue", imgThresholdedYellow); // means blue
        imshow("Thresholded_Green", imgThresholdedGreen);

        Point yellowCenter = findColorCenter(imgThresholdedYellow);
        Point greenCenter = findColorCenter(imgThresholdedGreen);
        yellowX = yellowCenter.x;
        yellowY = yellowCenter.y;
        greenX = greenCenter.x;
        greenY = greenCenter.y;

        cout << "Blue: " << yellowX << ", " << yellowY << endl;
        cout << "Green: " << greenX << ", " << greenY << endl;
        // If keypress is esc for 30ms, exit the program
        if (waitKey(30) == 27) {
            cout << "Exit the program" << endl;
            break;
        }
    }
    return 0;
}

