//
// Created by root on 17-11-1.
//

#ifndef JNIPROJECT_CANNY_LIB_H
#define JNIPROJECT_CANNY_LIB_H

#include "canny.h"
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <android/log.h>
#include <jni.h>

JNIEXPORT void JNICALL
Java_OpenCvCanny_OpenCVCannyLib_cannyLauncher(JNIEnv *env, jclass obj, jintArray img1_param, jintArray img2_param, int w, int h, jobject callback);

jintArray bitmapArrayFactory(JNIEnv *env, cv::Mat image, int w,int h);

#endif //JNIPROJECT_CANNY_LIB_H
