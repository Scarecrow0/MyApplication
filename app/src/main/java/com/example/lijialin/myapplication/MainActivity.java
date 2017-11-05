package com.example.lijialin.myapplication;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import OpenCvCanny.OpenCVCannyLibInteract;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // 获取图片，使用边缘检测提取图像的边缘
        Bitmap bitmap1 = BitmapFactory.decodeResource(getResources(), R.mipmap.image0);
        Bitmap bitmap2 = BitmapFactory.decodeResource(getResources(), R.mipmap.image1);
        ImageView imageView = (ImageView) findViewById(R.id.init_image1);
        imageView.setImageBitmap(bitmap1);
        imageView = (ImageView) findViewById(R.id.init_image2);
        imageView.setImageBitmap(bitmap2);
/*
        int width = bitmap.getWidth();
        int height = bitmap.getHeight();
        int[] pix = new int[width * height];
        bitmap.getPixels(pix, 0, width, 0, 0, width, height);
       int[] resultPixels = OpenCVCanny.canny(pix, pix, width, height,new JNIProcessInteractCallback(new Handler()));
        Bitmap resultBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_4444);
        resultBitmap.setPixels(resultPixels, 0, width, 0, 0, width, height);
        // 将边缘图显示出来
        ImageView view = (ImageView) findViewById(R.id.resultView);
        view.setImageBitmap(resultBitmap);
        view.setVisibility(View.VISIBLE);

*/
        OpenCVCannyLibInteract openCVCannyLibInteract = new OpenCVCannyLibInteract(new OpenCVCannyLibInteract.CallbackInterface() {
            @Override
            public void onEdgeDetectComplete(Bitmap img1, Bitmap img2) {
                Log.d("MainActivity", "onEdgeDetectComplete: ");
                ImageView view = (ImageView) findViewById(R.id.edge_detect_image1);
                view.setImageBitmap(img1);
                view.setVisibility(View.VISIBLE);
                view = (ImageView) findViewById(R.id.edge_detect_image2);
                view.setImageBitmap(img2);
                view.setVisibility(View.VISIBLE);
            }

            @Override
            public void onLineSegmentExtractionComplete(Bitmap img1, Bitmap img2) {
                Log.d("MainActivity", "onLineSegmentExtractionComplete: ");
                ImageView view = (ImageView) findViewById(R.id.segment_image1);
                view.setImageBitmap(img1);
                view.setVisibility(View.VISIBLE);
                view = (ImageView) findViewById(R.id.segment_image2);
                view.setImageBitmap(img2);
            }

            @Override
            public void onLineMatchimgComplete(Bitmap img1, Bitmap img2, double matchRate) {
                Log.d("MainActivity", "onLineMatchimgComplete: ");
                ImageView view = (ImageView) findViewById(R.id.segment_match_image1);
                view.setImageBitmap(img1);
                view.setVisibility(View.VISIBLE);
                view = (ImageView) findViewById(R.id.segment_match_image2);
                view.setImageBitmap(img2);
                view.setVisibility(View.VISIBLE);

                TextView textView = (TextView) findViewById(R.id.match_rate_show);
                textView.setText("匹配率 ： " + matchRate);
            }
        });

        openCVCannyLibInteract.startMatchimg(bitmap1, bitmap2);

    }
}
