#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

int height = 600;
int width = 800;


Mat createPerspectiveTraf(float angle,float skew,float dx,float dy,float scale1, float scale2,float persp1, float persp2){
    Mat transformation(3,3,CV_32F);

    //Offset
	
    Mat offsetTr=Mat::eye(3,3,CV_32F);
    offsetTr.at<float>(0,2)=dx;
    offsetTr.at<float>(1,2)=dy;


    //Skew
    Mat skewTr=Mat::eye(3,3,CV_32F);
    skewTr.at<float>(0,1)=skew;

    //Scale
    Mat scaleTr=Mat::eye(3,3,CV_32F);
    scaleTr.at<float>(0,0)=scale1;
    scaleTr.at<float>(1,1)=scale2;


    //Rotation
    Mat rotTr=Mat::eye(3,3,CV_32F);
    rotTr.at<float>(0,0)=rotTr.at<float>(1,1)=cos(angle);
    rotTr.at<float>(0,1)=-sin(angle);
    rotTr.at<float>(1,0)=sin(angle);
    transformation=offsetTr*rotTr*skewTr*scaleTr;
    cout << "Affine Traf: " <<transformation <<endl ;


    //Perspectivity
    transformation.at<float>(2,0)=persp1;
    transformation.at<float>(2,1)=persp2;
    cout << "Perspective Traf: " <<transformation <<endl;


	return transformation;
}

void transformImage(Mat origImg,Mat& newImage,Mat tr,bool isPerspective){
    Mat invTr=tr.inv();
    const int WIDTH=origImg.cols;
    const int HEIGHT=origImg.rows;


    for (int x=0;x<WIDTH;x++) for (int y=0;y<HEIGHT;y++){
        Mat pt(3,1,CV_32F);
        pt.at<float>(0,0)=x;
        pt.at<float>(1,0)=y;
        pt.at<float>(2,0)=1.0;

        Mat ptTransformed=invTr*pt;
        if (isPerspective) ptTransformed=(1.0/ptTransformed.at<float>(2,0))*ptTransformed;

        int newX=round(ptTransformed.at<float>(0,0));
        int newY=round(ptTransformed.at<float>(1,0));

        if ((newX>=0)&&(newX<WIDTH)&&(newY>=0)&&(newY<HEIGHT)) newImage.at<Vec3b>(y,x)=origImg.at<Vec3b>(newY,newX);
    }
}



int main( int argc, char** argv )
{
    if( argc != 2){
     cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
     return -1;
    }

    Mat image;
    image = imread(argv[1]);

    if(!image.data ){
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }
    
    Mat transformedImage = Mat::zeros(image.size(),image.type());

    Mat trf=createPerspectiveTraf(0.0,0.0,300.0,300.0,1,1,0.0,0.0);
    transformImage(image,transformedImage,trf,false);

    namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Display window", transformedImage);     
    
    int key;
    float angle = 0.0;
    float skew = 0.0;
    float dx = 100.0;
    float dy = 100.0;
    float scale1 = 0.5;
    float scale2 = 0.5;
    float persp1 = 0.0;
    float persp2 = 0.0;
    bool isPers;
    while (true){
        key = cv::waitKey(100);// Wait for a keystroke in the window
        if(key == 27) break;
        isPers = false;
        switch (key)
        {
        case 'a':
            angle+=0.1f;
            break;
        case 'q':
            angle-=0.1f;
            break;
        case 's':
            scale2+=0.1;
            break;
        case 'w':
            scale2-=0.1;
            break;
        case 'd':
            scale1+=0.1;
            break;
        case 'e':
            scale1-=0.1;
            break;
        case 'f':
            skew+=0.1;
            break;
        case 'r':
            skew-=0.1;
            break;
        case 'g':
            dy+=10;
            break;
        case 't':
            dy-=10;
            break;
        case 'h':
            dx+=10;
            break;
        case 'z':
            dx-=10;
            break;
        case 'j':
            persp1+=0.0001;
            isPers = true;
            break;
        case 'u':
            persp1-=0.0001;
            isPers = true;
            break;
        case 'k':
            persp2+=0.0001;
            isPers = true;
            break;
        case 'i':
            persp2-=0.0001;
            isPers = true;
            break;
        default:
            break;
        }
        if(key != -1){
            transformedImage = Mat::zeros(height,width,CV_8UC3);
            trf=createPerspectiveTraf(angle,skew,dx,dy,scale1,scale2,persp1,persp2);
            transformImage(image,transformedImage,trf,isPers);
            imshow("Display window", transformedImage); }
    }                                         
    return 0;
}
