package OpenCvCanny;

import android.graphics.Bitmap;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;

/**
 * Created by root on 17-11-1.
 * canny的工作线程
 * 创建线程 发起canny过程 将jni回调类传入cpp
 */

public class ProcessHandlerThread extends HandlerThread {
    private static final String TAG = "ProcesseHandlerThread";
    private Handler processCanny;
    private LooperPreparedListenner looperPreparedListenner;

    private JNIProcessInteractCallback jniProcessInteractCallback;

    private Bitmap img1, img2;

    public ProcessHandlerThread(Handler callback) {
        super(TAG);
        jniProcessInteractCallback = new JNIProcessInteractCallback(callback);
    }

    //对内部线程的handler发异步消息

    public void startCannyProcess(Bitmap img1, Bitmap img2) {
        this.img1 = img1;
        this.img2 = img2;
        processCanny.obtainMessage().sendToTarget();
    }


    @Override
    public void onLooperPrepared() {
        processCanny = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                // 从这里传入callback interface, 开始进行调用canny的jni
                int[] imgpix1 = OpenCVCannyLib.bitmap2IntaArray(img1);
                int[] imgpix2 = OpenCVCannyLib.bitmap2IntaArray(img2);
                int w = img1.getWidth();
                int h = img1.getHeight();

                //在这里发起canny过程
                OpenCVCannyLib.cannyLauncher(imgpix1, imgpix2, w, h, jniProcessInteractCallback);

            }
        };

        looperPreparedListenner.onLooperPrepared();
    }


    public interface LooperPreparedListenner {
        void onLooperPrepared();
    }

    public void setLooperPreparedListenner(LooperPreparedListenner listenner) {
        looperPreparedListenner = listenner;
    }

    @Override
    public boolean quit() {
        getLooper().quit();
        return super.quit();
    }


}
