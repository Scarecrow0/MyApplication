//
// Created by root on 17-11-1.
//

#ifndef JNIPROJECT_CANNY_LIB_H
#define JNIPROJECT_CANNY_LIB_H

#include "canny.h"


jintArray bitmapArrayFactory(JNIEnv *env, cv::Mat image, int w,int h);

#endif //JNIPROJECT_CANNY_LIB_H
