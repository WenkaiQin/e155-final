#include "writeToUSB.hpp"
#include <iostream>
//#include <stdio.h>
#include <math.h>
//#include <unistd.h>
#include <string>
#include <zmq.hpp>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

#define PI 3.1415926
#define FACE_TOWARDS 1
#define FACE_BACKWARDS 0

using namespace cv;
using namespace std;
// Define color global
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
// This flag is used to specify if a new coordinate has been issued and not reached
bool newDest = false;

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
        // left click: it's blue
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
        // right click: it's green
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
    // pt2 is green center
    // pt1 is blue center
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

    // Use cross product to find robot orientation relative to the destination
    double crossProduct = rVec_x * dVec_y - rVec_y * dVec_x;
    if (crossProduct > 0) {
        ang.orientation = FACE_BACKWARDS;
    }
    else {
        ang.orientation = FACE_TOWARDS;
    }
    return ang;
}

double findDistance(int dst_x, int dst_y, Point robot) {
    double sumDistSq = pow((double)robot.x - (double)dst_x, 2) + pow((double)robot.y - (double)dst_y, 2);
    return sqrt(sumDistSq);
}

void controller( Angle ang, double dist, int fd) {
    if (!newDest) {
        // If no new Dest is given by the coordinate, just exit the controller.
        return;
    }

    double theta = ang.theta * 180 / PI;
    // rotation control
    char command = 60; // default to stop
    if (ang.orientation == FACE_TOWARDS) {
        // if robot angle is less then 70
        if (theta < 70) {
            // Turn left
            command = 24;
            writeByte(command, fd);
        }
        else if (theta > 110) {
            // Turn right
            command = 36;
            writeByte(command, fd);
        }
        else {
            if (dist < 10) {
                // Reached destination, keep still
                newDest = false;
                command = 60;
                writeByte(command, fd);
            }
            else {
                // Move Forward
                command = 20;
                writeByte(command, fd);
            }
        }
    }
    else {
        if (dist < 10) {
            // Reached destination, keep still
            newDest = false;
            command = 60;
            writeByte(command, fd);
        }
        else {
            if (theta < 70) {
                // Turn left
                command = 24;
                writeByte(command, fd);
            }
            else if (theta > 110) {
                // Turn right
                command = 36;
                writeByte(command, fd);
            }
            else {
                // Move Backward
                command = 40;
                writeByte(command, fd);
            }
        }
    }

    cout << "The command is: " << command << endl;
}

int main() {
    // Setup serial usb
    int fd = setupUSB();
    if (fd < 0) {
        cout << "Invalid USB-XBee connection" << endl;
    }
    // Setup camera
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cout << "Cannot open the video cam" << endl;
        return -1;
    }
    // Setup tcp socket to use message queue for IPC 
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind("tcp://*:5555");
    string destPosBuf;
    // Destionation coordinate
    int dst_x = 0;
    int dst_y = 0;
    
    // Setup mouse callback and window display
    MouseParam param;
    Mat frame, imgThresholdedBlue, imgThresholdedGreen;
    int width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    int height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    namedWindow("Video_Capture", CV_WINDOW_AUTOSIZE); 
    setMouseCallback("Video_Capture", onMouse, &param);

    while (true) {
        zmq::message_t request;
       
        // Try to receive any message sent from the client side
       try {
            socket.recv(&request, ZMQ_DONTWAIT);
            destPosBuf = string(static_cast<char*>(request.data()), request.size());
            zmq::message_t reply(3);
            memcpy(reply.data(), "Rec", 3);
            // Send reply to confirm with client
            socket.send(reply);
            sscanf(destPosBuf.c_str(), "x-pos=%d&y-pos=%d", &dst_x, &dst_y);
            // Set the new destination to be true, enter the controller
            newDest = true;
            // cout << destPosBuf << endl;
        }
        // If no message, deal with the error and continue
        catch (zmq::error_t e) {}

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

        // Find destination point and robot pos
        Point dst = Point(dst_x, dst_y);
        Point mid = Point((greenCenter.x + blueCenter.x)/2, (greenCenter.y + blueCenter.y)/2);
        // Find distance between robot and destination
        double dist = findDistance(dst_x, dst_y, mid);
        // Find angle between robot vector and destination vector
        Angle ang = findAngle(blueCenter, greenCenter, dst);
        double angle = ang.theta * 180.0 / PI;
        // Finally, time to send control signal...
        controller(ang, dist, fd);

        line(frame, blueCenter, greenCenter, Scalar(165, 206, 94), 1, 8, 0);
        line(frame, mid, dst, Scalar(255, 255, 0), 1, 8, 0); 
        imshow("Video_Capture", frame);
        //imshow("Thresholded_Blue", imgThresholdedBlue); // means blue
        //imshow("Thresholded_Green", imgThresholdedGreen);
        cout << "The angle is: ";
        cout << angle << endl;
        cout << "Blue: " << blueX << ", " << blueY << endl;
        cout << "Green: " << greenX << ", " << greenY << endl;
        cout << "Orientation: " << ang.orientation << endl;

        vector<int> compressionParams;
        compressionParams.push_back(CV_IMWRITE_JPEG_QUALITY);
        compressionParams.push_back(50);
        // Store the temp jpg file for video streaming
        imwrite("/tmp/video/img.jpg", frame, compressionParams);
        
        // If keypress is esc for >30ms, exit the program
        //
        if (waitKey(30) == 27) {
            cout << "Exit the program" << endl;
            break;
        }
    }
    // Clean up..
    tearDownUSB(fd);
    return 0;
}

