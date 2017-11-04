//
// Created by root on 17-11-1.
// 这是写canny jni 部分代码的地方
// todo  构造自己的jni各个接口

#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <android/log.h>
#include "canny_lib_src/main_canny_lib.h"
#include <jni.h>
//来自调用函数的jni类
#include "OpenCvCanny_OpenCVCannyLib.h"
//cpp部分各种函数的头文件
#include "canny_lib.h"

using namespace cv;

// 记得各种java的int之类的类型都得转成jint什么的
JNIEXPORT void JNICALL Java_OpenCvCanny_OpenCVCannyLib_cannyLauncher
        (JNIEnv *env, jclass obj, jintArray img1_param, jintArray img2_param, jint w, jint h, jobject callback){

    CallbackInterface callbackInterface(callback,env);
    //回调类的获得

    jint *img1_jintarr;
    jint *img2_jintarr;
    img1_jintarr = env->GetIntArrayElements(img1_param, NULL);
    img2_jintarr = env ->GetIntArrayElements(img2_param, NULL);


    Mat image(h, w, CV_8UC4, (unsigned char *) img1_jintarr); // 初始化一个矩阵（图像）4通道的图像
    cvtColor(image, image, COLOR_BGR2GRAY); // 转为灰度图
    GaussianBlur(image, image, Size(5, 5), 0, 0); // 高斯滤波
    Canny(image, image, 50, 150, 3); // 边缘检测


    //通过传入的回调类 进行对app部分的回调
    callbackInterface.onEdgeDetected(image,image,w,h);


    env->ReleaseIntArrayElements(img1_param, img1_jintarr, 0);
    env->ReleaseIntArrayElements(img2_param, img2_jintarr, 0);
}

void EdgeDetected(Mat& img1,Mat& img2,CallbackInterface callbackInterface){
//todo 把各种canny 函数加进去
}





CallbackInterface::CallbackInterface(jobject& callback,JNIEnv* env) {
    this -> env = env;
    this->callback_obj = callback;
    this->callback_class = env->GetObjectClass(callback);
    //回调类的获得
    this->methodID_methodonEdgeDetected =
            env->GetMethodID(callback_class, "onEdgeDetectComplete", "([I[III)V");
    this->methodID_onSegmentExtracted =
            env->GetMethodID(callback_class, "onLineSegmentExtractionComplete", "([I[III)V");
    this ->methodID_onSegmentMatched =
            env->GetMethodID(callback_class, "onLineMatchimgComplete", "([I[IIID)V");
}

void CallbackInterface::onEdgeDetected(Mat& img1, Mat& img2, int w, int h) {
    jintArray img1_arr = bitmapArrayFactory(env,img1,w,h);
    jintArray img2_arr = bitmapArrayFactory(env,img2,w,h);
    env->CallVoidMethod(this->callback_obj,this->methodID_methodonEdgeDetected,img1_arr,img2_arr,w,h);
}

void CallbackInterface::onSegmentExtracted(Mat& img1, Mat& img2, int w, int h) {
    jintArray img1_arr = bitmapArrayFactory(env,img1,w,h);
    jintArray img2_arr = bitmapArrayFactory(env,img2,w,h);
    env->CallVoidMethod(callback_obj,methodID_onSegmentExtracted,img1_arr,img2_arr,w,h);
}

void CallbackInterface::onSegmentMatched(Mat& img1,Mat& img2,int w,int h,double rate){
    jintArray img1_arr = bitmapArrayFactory(env,img1,w,h);
    jintArray img2_arr = bitmapArrayFactory(env,img2,w,h);
    env->CallVoidMethod(callback_obj,methodID_onSegmentMatched,img1_arr,img2_arr,w,h,rate);
}


jintArray bitmapArrayFactory(JNIEnv *env, Mat image, int w,int h){
    int* outImage = new int[w * h];
    int n = 0;
    for(int i = 0; i < h; i++) {
        uchar* data = image.ptr<uchar>(i);
        for(int j = 0; j < w; j++) {
            if(data[j] == 255) {
                outImage[n++] = 0;
            }else {
                outImage[n++] = -1;
            }
        }
    }

    int size = w * h;
    jintArray result = env->NewIntArray(size);
    env->SetIntArrayRegion(result, 0, size, outImage);
    return result;
}
