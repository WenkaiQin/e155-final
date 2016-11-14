#include <iostream>
#include <stdio.h>
#include <math.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

#define PI 3.1415926
#define FACE_TOWARDS 1
#define FACE_BACKWARDS 0

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
    Point center = Point( rec.x+rec.width/2, rec.y+rec.height/2 );
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

   // dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
   // erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
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

struct Angle {
    double theta;
    int orientation;
};

Angle findAngle(Point pt1, Point pt2, Point dest) {
    Angle ang;
    double rVec_x = (double) pt2.x - (double) pt1.x;
    double rVec_y = (double) pt2.y - (double) pt1.y;

    Point mid = Point((pt2.x + pt1.x)/2, (pt2.x + pt1.x)/2);

    double dVec_x = (double) mid.x - dest.x;
    double dVec_y = (double) mid.y - dest.y;

    double rVecAbs = sqrt(pow(rVec_x,2) + pow(rVec_y,2));
    double dVecAbs = sqrt(pow(dVec_x,2) + pow(dVec_y,2));
    if (rVecAbs*dVecAbs == 0) {
        cout << "Divided by zero!" << endl;
        ang.theta = 0;
        ang.orientation = FACE_TOWARDS;
        return ang;
    }
    double angle = acos(((rVec_x * dVec_x) + (rVec_y * dVec_y)) / 
        (rVecAbs * dVecAbs));
    ang.theta = angle;
    if (Point(rVec_x, rVec_y).dot(Point(dVec_x, dVec_y)) > 0) {
        ang.orientation = FACE_TOWARDS;
    }
    else {
        ang.orientation = FACE_BACKWARDS;
    }
    return ang;
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
    Mat frame, imgThresholdedBlue, imgThresholdedGreen;
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
        imgThresholdedBlue = thresholdImage(frame, 1);
        imgThresholdedGreen  = thresholdImage(frame, 2);   
        double blueX, blueY, greenX, greenY;

        Point blueCenter = findColorCenter(imgThresholdedBlue);
        Point greenCenter = findColorCenter(imgThresholdedGreen);
        blueX = blueCenter.x;
        blueY = blueCenter.y;
        greenX = greenCenter.x;
        greenY = greenCenter.y;
        
        Point test = Point(0, 0);

        Angle ang = findAngle(blueCenter, greenCenter, test);
        double angle = ang.theta * 180.0 / 3.14159;
        cout << "The angle is: ";
        cout << angle << endl;

        Point mid = Point((greenCenter.x + blueCenter.x)/2, (greenCenter.y + blueCenter.y)/2);

        line(frame, blueCenter, greenCenter, Scalar(165, 206, 94), 1, 8, 0);
        line(frame, mid, test, Scalar(255, 255, 0), 1, 8, 0);
 
        imshow("Video_Capture", frame);
        imshow("Thresholded_Blue", imgThresholdedBlue); // means blue
        imshow("Thresholded_Green", imgThresholdedGreen);
        cout << "Blue: " << blueX << ", " << blueY << endl;
        cout << "Green: " << greenX << ", " << greenY << endl;
        cout << "Orientation: " << ang.orientation << endl;
        // If keypress is esc for 30ms, exit the program
        if (waitKey(30) == 27) {
            cout << "Exit the program" << endl;
            break;
        }
    }
    return 0;
}

