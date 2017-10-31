package OpenCvCanny;

import android.graphics.Bitmap;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;

/**
 * Created by root on 17-10-31.
 */

public class OpenCVCannyLib {

    static {
        System.loadLibrary("OpenCV"); // 加载编译好的.so动态库
    }

    /**
     * 声明native方法，调用OpenCV的边缘检测
     *
     * @param buf 图像
     * @param w   宽
     * @param h   高
     * @return 边缘图
     */

    //todo 接口函数修改
    public static native int[] canny(int[] buf, int w, int h);

    //      jni 相关函数定义
//      处理内部事务的线程
    private ProcesseHandlerThread processingThread;


    //  负责从外部接受调用的方法
    public void startMatching(Handler callback, Bitmap img1, Bitmap img2) {

    }


    private class ProcesseHandlerThread extends HandlerThread {
        private static final String TAG = "ProcesseHandlerThread";
        private Handler interactCallback,
                processCanny;

        public ProcesseHandlerThread(Handler callback) {
            super(TAG);
            this.interactCallback = callback;
        }

        //todo  对handler发异步消息

        // TODO 构建looper ready listener

        @Override
        public void onLooperPrepared() {

            processCanny = new Handler() {
                @Override
                public void handleMessage(Message msg) {
                    // todo: 从这里传入callback interface, 开始进行调用canny的jni
                    //

                }
            };
        }

    }

}
