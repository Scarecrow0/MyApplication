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
    callbackInterface.setImageSize(w,h);
    //回调类的获得
    //这里在Cpp部分对JNI回调的接口进行了下封装 方便使用

    jint *img1_jintarr;
    jint *img2_jintarr;
    img1_jintarr = env->GetIntArrayElements(img1_param, NULL);
    img2_jintarr = env ->GetIntArrayElements(img2_param, NULL);


    Mat img1(h, w, CV_8UC4, (unsigned char *) img1_jintarr); // 初始化一个矩阵（图像）4通道的图像
    cvtColor(img1, img1, COLOR_BGR2GRAY); // 转为灰度图

    Mat img2(h, w, CV_8UC4, (unsigned char *) img1_jintarr);
    cvtColor(img2, img2, COLOR_BGR2GRAY);

    EdgeDetected(img1,img2,callbackInterface);

//   提取直线
    SegmentResult* segmentResult_img1 = SegmentExtract(img1);
    SegmentResult* segmentResult_img2 = SegmentExtract(img2);

//  画出结果
    callbackInterface.onSegmentExtracted
            (segmentResult_img1->LinesImage, segmentResult_img2->LinesImage);



//  垃圾回收
    delete segmentResult_img1;
    delete segmentResult_img2;
    env->ReleaseIntArrayElements(img1_param, img1_jintarr, 0);
    env->ReleaseIntArrayElements(img2_param, img2_jintarr, 0);
}

void EdgeDetected(Mat img1,Matb img2,CallbackInterface callbackInterface){
//todo 把各种canny 函数加进去

    GaussianBlur(img1, img1, Size(3, 3), 0, 0); // 高斯滤波
    Canny(img1, img1, 50, 150, 3); // 边缘检测
    GaussianBlur(img2, img2, Size(3, 3), 0, 0);
    Canny(img2, img2, 50, 150, 3); //

 /*
    blur(img1, img1, Size(3, 3));
    myCanny(img1, img1, 50, 200, 3);
    blur(img2, img2, Size(3, 3));
    myCanny(img2, img2, 50, 200, 3);
     */
    //通过传入的回调类 进行对app部分的回调
    callbackInterface.onEdgeDetected(img1,img2);
}

SegmentResult* SegmentExtract(Mat& img){
    // blur(image, image, Size(3, 3));  // 使用3x3内核来降噪
    // Canny(image, image, 50, 200, 3); // Apply canny edge

    // todo   需要用学长自己写的函数吗？
    // myCanny(image, image, 50, 200, 3);

    // imwrite(path + "(50,200)边缘检测结果图.jpg", image);
    // imshow(path+"边缘检测", image);

    // Create and LSD detector with standard or no refinement.
    // LSD_REFINE_NONE，没有改良的方式；
    // LSD_REFINE_STD，标准改良方式，将带弧度的线（拱线）拆成多个可以逼近原线段的直线度；
    // LSD_REFINE_ADV，进一步改良，计算出错误警告数量，减少尺寸进一步精确直线。
    Ptr<LineSegmentDetector> ls = createLineSegmentDetector(LSD_REFINE_STD, 0.99);
    vector<Vec4f> lines_std;

    // Detect the lines
    ls->detect(img, lines_std);


//   画出提取出直线后的图

  /*
    Mat drawnLines(img);
    ls->drawSegments(drawnLines, lines_std);

    */
// todo  这个直线提取有毛病的
    // Show found lines
    //Mat drawnLines(image);
    Mat only_lines(img.size(), img.type());
    //ls->drawSegments(drawnLines, lines_std);
    ls->drawSegments(only_lines, lines_std);
    // imshow(path, drawnLines);
    // imshow(path, only_lines);
    // imwrite(path + "直线提取.jpg", only_lines);
    SegmentResult* segmentResult = new SegmentResult(lines_std,only_lines);
    return segmentResult;
}


void SegmentMatch(SegmentResult img1, SegmentResult img2 , CallbackInterface callbackInterface){

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

void CallbackInterface::onEdgeDetected(Mat& img1, Mat& img2) {
    CallbackMethodSelect(img1, img2, ON_EDGE_DETECTED);
}

void CallbackInterface::onSegmentExtracted(Mat& img1, Mat& img2) {
    CallbackMethodSelect(img1, img2, ON_SEGMENT_EXTRACTED);
}

void CallbackInterface::onSegmentMatched(Mat& img1,Mat& img2,double rate){
    CallbackMethodSelect(img1, img2, ON_SEGMENT_MATCHED, rate);
}
void CallbackInterface::setImageSize(int w, int h) {
    this->w = w;
    this->h = h;
}

void CallbackInterface::CallbackMethodSelect(Mat &img1, Mat &img2, int action, double rate) {
    jintArray img1_arr = bitmapArrayFactory(env,img1,w,h);
    jintArray img2_arr = bitmapArrayFactory(env,img2,w,h);
    switch (action){
        case ON_EDGE_DETECTED:
            env->CallVoidMethod(this->callback_obj,this->methodID_methodonEdgeDetected,img1_arr,img2_arr,w,h);
            break;
        case ON_SEGMENT_EXTRACTED:
            env->CallVoidMethod(callback_obj,methodID_onSegmentExtracted,img1_arr,img2_arr,w,h);
            break;
        case ON_SEGMENT_MATCHED:
            env->CallVoidMethod(callback_obj,methodID_onSegmentMatched,img1_arr,img2_arr,w,h,rate);
            break;
        default:
            break;
    }
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
