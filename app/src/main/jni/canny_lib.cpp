//
// Created by root on 17-11-1.
// 这是写canny jni 部分代码的地方

#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <android/log.h>
#include <jni.h>

//来自调用函数的jni类
#include "OpenCvCanny_OpenCVCannyLib.h"

//cpp部分各种函数的头文件
#include "canny_lib.h"
#include "canny_lib_src/Line.h"
#include "canny_lib_src/MinHeap.h"
#include "canny_lib_src/main_canny_lib.h"

using namespace cv;
using namespace std;

// 记得各种java的int之类的类型都得转成jint什么的
JNIEXPORT void JNICALL Java_OpenCvCanny_OpenCVCannyLib_cannyLauncher
        (JNIEnv *env, jclass obj, jintArray img1_param, jintArray img2_param, jint w1, jint h1, jint w2, jint h2, jobject callback){

    CallbackInterface callbackInterface(callback,env);

    //回调类的获得
    //这里在Cpp部分对JNI回调的接口进行了下封装 方便使用

    jint *img1_jintarr;
    jint *img2_jintarr;
    img1_jintarr = env->GetIntArrayElements(img1_param, NULL);
    img2_jintarr = env ->GetIntArrayElements(img2_param, NULL);


    Mat img1(h1, w1, CV_8UC4, (unsigned char *) img1_jintarr); // 初始化一个矩阵（图像）4通道的图像
    cvtColor(img1, img1, COLOR_BGR2GRAY); // 转为灰度图

    Mat img2(h2, w2, CV_8UC4, (unsigned char *) img2_jintarr);
    cvtColor(img2, img2, COLOR_BGR2GRAY);

    double height = (double)img1.rows / img1.cols * img2.cols;
    resize(img1, img1, Size(img2.cols, height), 0, 0, CV_INTER_LINEAR);

    callbackInterface.setImageSize(img2.cols,height);
    EdgeDetected(img1,img2,callbackInterface);

//   提取直线
    SegmentResult* segmentResult_img1 = SegmentExtract(img1);
    SegmentResult* segmentResult_img2 = SegmentExtract(img2);

//  画出结果
    callbackInterface.onSegmentExtracted
            (segmentResult_img1->LinesImage, segmentResult_img2->LinesImage);

//  匹配直线以及画出结果传回rate
    SegmentMatch(segmentResult_img1,segmentResult_img2,callbackInterface);

//  垃圾回收
    delete segmentResult_img1;
    delete segmentResult_img2;
    env->ReleaseIntArrayElements(img1_param, img1_jintarr, 0);
    env->ReleaseIntArrayElements(img2_param, img2_jintarr, 0);
}

void EdgeDetected(Mat img1,Mat img2,CallbackInterface& callbackInterface){
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

    // Show found lines
    //Mat drawnLines(image);
    Mat only_lines(img.size(), img.type());
    //ls->drawSegments(drawnLines, lines_std);
    ls->drawSegments(only_lines, lines_std);
    cvtColor(only_lines,only_lines,COLOR_BGR2GRAY);
    // imshow(path, drawnLines);
    // imshow(path, only_lines);
    // imwrite(path + "直线提取.jpg", only_lines);
    SegmentResult* segmentResult = new SegmentResult(lines_std,only_lines);
    return segmentResult;
}


void SegmentMatch(SegmentResult* img1, SegmentResult* img2 , CallbackInterface& callbackInterface){
// Step1 创建直线
    vector<Line> lineSet1 = createLine(img1->LinesData);
    vector<Line> lineSet2 = createLine(img2->LinesData);

    img1 -> freeDataStoreSpace(); // 回收内存
    img2 -> freeDataStoreSpace();

    int threshold = 8; // 阈值【5-10】
    Mat dst1(img1->LinesImage.rows, img1->LinesImage.cols, CV_8UC3, Scalar(255,255,255));
    Mat dst2(img2->LinesImage.rows, img2->LinesImage.cols, CV_8UC3, Scalar(255,255,255));

    // Step2 删除较短的直线 (可选)
    lineSet1 = cleanShort(lineSet1);
    lineSet2 = cleanShort(lineSet2);

    // Step3 先进行直线的连接，然后聚合直线
    lineSet1 = connectLines(lineSet1, threshold, dst1); // 连接
    lineSet2 = connectLines(lineSet2, threshold, dst2);

    lineSet1 = clusterLines(lineSet1, threshold, dst1); // 聚合
    lineSet2 = clusterLines(lineSet2, threshold, dst2);

    size_t length1 = lineSet1.size();
    size_t length2 = lineSet2.size();

    // 画出聚合后的图像，便于分析
    // line(dst1, Point(0, 0), Point(10, 0), Scalar(255, 0, 0), 3, CV_AA); // 测试阈值
    //drawLine(lineSet1, dst1, Scalar(0,0,0), "连接、聚合后的图像1");
    //drawLine(lineSet2, dst2, Scalar(0,0,0), "连接、聚合后的图像2");


    // Step4. 从第一张图中选择一条直线，然后遍历第二张图，找到最佳的配对直线
    vector<vector<Line> > pairSet = makePair(lineSet1, lineSet2, threshold);
    size_t pairLen = pairSet.size(); // 有多少对直线

    // 画出配对后的图像，便于分析

    Mat img1_sgmt_match_res(img1->LinesImage.rows, img1->LinesImage.cols, CV_8UC3, Scalar(255,255,255));
    Mat img2_sgmt_match_res(img2->LinesImage.rows, img2->LinesImage.cols, CV_8UC3, Scalar(255,255,255));
    /*
    //Ptr<LineSegmentDetector> ls = createLineSegmentDetector(LSD_REFINE_STD, 0.99);
    Mat img1_sgmt_match_res(img1->LinesImage.size(), img1->LinesImage.type());
    Mat img2_sgmt_match_res(img2->LinesImage.size(), img2->LinesImage.type());
*/
    for (int i = 0; i < pairLen; i++) {
    	int b = rand() % 255; //产生三个随机数
    	int g = rand() % 255;
    	int r = rand() % 255;
    	vector<Line> v = pairSet[i];
    	line(img1_sgmt_match_res, v[0].start, v[0].end, Scalar(b, g, r), 2, CV_AA);
    	line(img2_sgmt_match_res, v[1].start, v[1].end, Scalar(b, g, r), 2, CV_AA);
    }

    //imshow("配对后的图像1", dst3);
    //imshow("配对后的图像2", dst4);


    // Step 5. 计算直线与其他直线的夹角，构造夹角矩阵
    vector<vector<double> > angleList1, angleList2;
    for (int i = 0; i < pairLen; i++) {
        vector<Line> v1 = pairSet[i];
        vector<double> angle1, angle2;

        for (int j = i + 1; j < pairLen; j++) {
            vector<Line> v2 = pairSet[j];
            angle1.push_back(getAngle(v1[0].k, v2[0].k));
            angle2.push_back(getAngle(v1[1].k, v2[1].k));
        }
        angleList1.push_back(angle1);
        angleList2.push_back(angle2);

        vector<double>().swap(angle1); // 回收内存
        vector<double>().swap(angle2);
    }

    // 然后计算夹角矩阵的相似度
    double rate = calculateCorr2(angleList1, angleList2);

    if (length1 != length2) {
        rate *= (double) min(length1, length2) / max(length1, length2);
    }

    callbackInterface.onSegmentMatched(img1_sgmt_match_res,img2_sgmt_match_res,rate);
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
    jintArray img1_arr,img2_arr ;
    switch (action){
        case ON_EDGE_DETECTED:
            img1_arr = bitmapArrayFactoryGrayScale(env,img1,w,h);
            img2_arr = bitmapArrayFactoryGrayScale(env,img2,w,h);
            env->CallVoidMethod(this->callback_obj,this->methodID_methodonEdgeDetected,img1_arr,img2_arr,w,h);
            break;
        case ON_SEGMENT_EXTRACTED:
            img1_arr = bitmapArrayFactoryGrayScale(env,img1,w,h);
            img2_arr = bitmapArrayFactoryGrayScale(env,img2,w,h);
            env->CallVoidMethod(callback_obj,methodID_onSegmentExtracted,img1_arr,img2_arr,w,h);
            break;
        case ON_SEGMENT_MATCHED:
            img1_arr = bitmapArrayFactoryColor(env,img1,w,h);
            img2_arr = bitmapArrayFactoryColor(env,img2,w,h);
            env->CallVoidMethod(callback_obj,methodID_onSegmentMatched,img1_arr,img2_arr,w,h,rate);
            break;
        default:
            break;
    }
}


/*
    安卓的 ARGB 8888：
        一个32bit的unsigned
        4个8 分别对应 透明度 红色 绿色 蓝色
        其中需要注意的是透明度是反过来的  即值越大越不透明 FFH就是完全不透明
        对一个unsigned int 使用移位 相加等位操作即可构建需要的像素了

    在OpenCV里 mat通过 uchar* data = image.ptr<uchar>(i)获得的像素
    如果是个彩色的 实际上是将一个像素的RGB挨着放在一起 称为通道
    取出进行操作时 要三个三个取

*/

jintArray bitmapArrayFactoryGrayScale(JNIEnv *env, Mat image, int w,int h){
    int* outImage = new int[w * h];
    int n = 0;
    for(int i = 0; i < h; i++) {
        uchar* data = image.ptr<uchar>(i);
        for(int j = 0; j < w; j++) {
            if(data[j] > 0) {
                outImage[n++] =  0xFF000000;
            }else {
                outImage[n++] =  0xFFFFFFFF;
            }
        }
    }

    int size = w * h;
    jintArray result = env->NewIntArray(size);
    env->SetIntArrayRegion(result, 0, size, outImage);
    return result;
}

jintArray bitmapArrayFactoryColor(JNIEnv *env, cv::Mat image, int w,int h){
    int* outImage = new int[w * h];
    int n = 0;
    for(int i = 0; i < h; i++) {
        uchar* data = image.ptr<uchar>(i);
        for(int j = 0; j < w; j++) {
            int pix_pointer = j * 3;
            unsigned int alpha = 0xFF000000;
            unsigned int red = data[pix_pointer];
            red = red << 16;
            unsigned int green = data[pix_pointer + 1];
            green = green << 8;
            unsigned int blue = data[pix_pointer + 2];
            outImage[n++] = alpha + red + green + blue;

        }
    }

    int size = w * h;
    jintArray result = env->NewIntArray(size);
    env->SetIntArrayRegion(result, 0, size, outImage);
    return result;
}