//
// Created by root on 17-11-1.
//

#ifndef JNIPROJECT_CANNY_LIB_H
#define JNIPROJECT_CANNY_LIB_H

#include "canny.h"
#include "jni.h"
using namespace cv;

class CallbackInterface {
private:
    jclass callback_class;
    jobject callback_obj;
    jmethodID methodID_methodonEdgeDetected;
    jmethodID methodID_onSegmentExtracted;
    jmethodID methodID_onSegmentMatched;
    JNIEnv* env;
    //回调方法的获得

public:
    CallbackInterface(jobject& callback,JNIEnv* env);
    void onEdgeDetected(Mat& img1,Mat& img2,int w,int h);
    void onSegmentExtracted(Mat& img1,Mat& img2,int w,int h);
    void onSegmentMatched(Mat& img1,Mat& img2,int w,int h,double rate);

};

jintArray bitmapArrayFactory(JNIEnv *env, cv::Mat image, int w,int h);
void ResizeImage(cv::Mat& image1,cv::Mat& image2);

#endif //JNIPROJECT_CANNY_LIB_H
