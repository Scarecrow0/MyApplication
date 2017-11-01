//
// Created by root on 17-11-1.
// 这是写canny jni 部分代码的地方
// todo  构造自己的jni各个接口

#include "canny_lib.h"
#include <jni.h>

using namespace cv;


JNIEXPORT void JNICALL
Java_OpenCvCanny_OpenCVCannyLib_cannyLauncher(JNIEnv *env, jclass obj, jintArray img1_param, jintArray img2_param, int w, int h, jobject callback) {

    jclass callback_class = env->GetObjectClass(obj);
    //回调类的获得
    jmethodID onEdgeDetected =
            env->GetMethodID(callback_class, "onEdgeDetectComplete", "([I[III)V");
    jmethodID onSegmentExtracted =
            env->GetMethodID(callback_class, "onLineSegmentExtractionComplete", "([I[III)V");

    jmethodID onSegmentMatched =
            env->GetMethodID(callback_class, "onLineMatchimgComplete", "([I[IIID)V");
    //回调方法的获得


    jint *img1_jintarr;
    jint *img2_jintarr;
    img1_jintarr = env->GetIntArrayElements(img1_param, NULL);

    img2_jintarr = env ->GetIntArrayElements(img2_param, NULL);
  /*  if(img2_jintarr == NULL){
        return;
    }*/

    Mat image(h, w, CV_8UC4, (unsigned char *) img1_jintarr); // 初始化一个矩阵（图像）4通道的图像
    cvtColor(image, image, COLOR_BGR2GRAY); // 转为灰度图
    GaussianBlur(image, image, Size(5, 5), 0, 0); // 高斯滤波
    Canny(image, image, 50, 150, 3); // 边缘检测

    jintArray result_edgedetect = bitmapArrayFactory(env,image,w,h);

    __android_log_print(1,"from jni hhhh","dsadasda");
    env->CallVoidMethod(callback,onEdgeDetected,result_edgedetect,result_edgedetect,w,h);


    env->ReleaseIntArrayElements(img1_param, img1_jintarr, 0);
    env->ReleaseIntArrayElements(img2_param, img2_jintarr, 0);

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
