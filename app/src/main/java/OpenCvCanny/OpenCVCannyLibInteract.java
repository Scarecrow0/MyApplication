package OpenCvCanny;

import android.graphics.Bitmap;
import android.os.Handler;
import android.os.Message;

/**
 * Created by root on 17-10-31.
 */

public class OpenCVCannyLibInteract {
    private CallbackInterface callbackInterface;
    private Handler interactHandler;
    private static final OpenCVCannyLib openCVCanny = new OpenCVCannyLib();

    public static final int ON_EDGE_DETECT_COMPLETE = 1,
            ON_LINE_SEGMENT_EXTRACTION_COMPLETE = 5,
            ON_LINE_MATCHING_COMPLETE = 85;


    public OpenCVCannyLibInteract(final CallbackInterface callbackInterface) {
        this.callbackInterface = callbackInterface;
        interactHandler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                CallbackResult callbackResult = (CallbackResult) msg.obj;
                switch (msg.what) {
                    case ON_EDGE_DETECT_COMPLETE:
                        callbackInterface
                                .onEdgeDetectComplete(callbackResult.img1,
                                        callbackResult.img2);
                        break;
                    case ON_LINE_SEGMENT_EXTRACTION_COMPLETE:
                        callbackInterface
                                .onLineSegmentExtractionComplete(callbackResult.img1,
                                        callbackResult.img2);
                        break;
                    case ON_LINE_MATCHING_COMPLETE:
                        callbackInterface
                                .onLineMatchimgComplete(callbackResult.img1,
                                        callbackResult.img2,
                                        callbackResult.matchRate);

                        break;
                    default:
                        break;
                }
            }
        };
    }

    public void startMatchimg(Bitmap img1, Bitmap img2) {
        //  TODO 这里传入图片进行调用
        openCVCanny.startMatching(interactHandler, img1, img2);
    }


    public interface CallbackInterface {
        void onEdgeDetectComplete(Bitmap img1, Bitmap img2);

        //当边缘提取完毕回调传回图像
        void onLineSegmentExtractionComplete(Bitmap img1, Bitmap img2);

        //当直线提取完毕传回图像
        void onLineMatchimgComplete(Bitmap img1, Bitmap img2, double matchRate);
        //直线匹配完成传回图像
    }


    public class CallbackResult {
        Bitmap img1, img2;
        double matchRate;

        public CallbackResult(Bitmap img1, Bitmap img2) {
            this.img1 = img1;
            this.img2 = img2;
        }

        public CallbackResult(Bitmap img1, Bitmap img2, double matchRate) {
            this.img1 = img1;
            this.img2 = img2;
            this.matchRate = matchRate;
        }
    }
}

