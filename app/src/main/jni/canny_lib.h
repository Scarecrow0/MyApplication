//
// Created by root on 17-11-1.
//

#ifndef JNIPROJECT_CANNY_LIB_H
#define JNIPROJECT_CANNY_LIB_H

#include "canny.h"
#include "jni.h"
#include "canny_lib_src/main_canny_lib.h"
using namespace cv;


//对JNI回调函数的一个封装 便于使用
class CallbackInterface {
private:
    static const int ON_EDGE_DETECTED = 2,
                     ON_SEGMENT_EXTRACTED = 65,
                     ON_SEGMENT_MATCHED = 95;
    int w,h;

    JNIEnv* env;
    jclass callback_class;
    jobject callback_obj;
    jmethodID methodID_methodonEdgeDetected;
    jmethodID methodID_onSegmentExtracted;
    jmethodID methodID_onSegmentMatched;

    void CallbackMethodSelect(Mat& img1, Mat& img2, int action,double rate=0);
    //回调方法的获得

public:
    CallbackInterface(jobject& callback,JNIEnv* env);
    void setImageSize(int w,int h);
    void onEdgeDetected(Mat& img1,Mat& img2);
    void onSegmentExtracted(Mat& img1,Mat& img2);
    void onSegmentMatched(Mat& img1,Mat& img2,double rate);

};

//将直线提取后的结果数据和绘图后的结果进行打包 便于值传递
struct SegmentResult{
    vector<Vec4f> LinesData;
    Mat LinesImage;
    SegmentResult(vector<Vec4f> lines, Mat image){
        LinesData = lines;
        LinesImage  = image;
    }

    void freeDataStoreSpace(){
        vector<Vec4f>().swap(LinesData);
    }
};

jintArray bitmapArrayFactoryGrayScale(JNIEnv *env, cv::Mat image, int w,int h);
jintArray bitmapArrayFactoryColor(JNIEnv *env, cv::Mat image, int w,int h);
//工厂方法 将Mat 转为jintarray  便于回调
void ResizeImage(cv::Mat& image1,cv::Mat& image2);

void EdgeDetected(Mat img1,Mat img2,CallbackInterface& callbackInterface);
//提取边缘过程 提取完成后进行回调

SegmentResult* SegmentExtract(Mat& img);
//直线提取 对单个图片操作 返回打包后的结果

void SegmentMatch(SegmentResult* img1, SegmentResult* img2 , CallbackInterface& callbackInterface);
//直线匹配 更新匹配结果
//  todo  这里匹配后的直线数据需要保存作进一步处理吗

#endif //JNIPROJECT_CANNY_LIB_H
