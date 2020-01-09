package skycrasher.launcher.android;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.graphics.PixelFormat;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;


class OrangeGrassView extends GLSurfaceView implements SensorEventListener
{
    private static String TAG = "OrangeGrassView";
    private static Renderer ogRenderer = null;
    private Context m_context = null;
    private float gravity[] = new float[3];
    private float accel[] = new float[3];

    public OrangeGrassView(Context context) 
    {
        super(context);
        m_context = context;
        setEGLConfigChooser(8, 8, 8, 0, 16, 0);
        setEGLContextClientVersion(3);
        
        ogRenderer = new Renderer();
        ApplicationInfo appInfo = null;
        PackageManager packMgmr = m_context.getPackageManager();
        try 
        {
            appInfo = packMgmr.getApplicationInfo("skycrasher.launcher.android", 0);
        }
        catch (NameNotFoundException e) 
        {
            e.printStackTrace();
            throw new RuntimeException("Unable to locate assets, aborting...");
        }
        Renderer.assetsPath = appInfo.sourceDir;
        setRenderer(ogRenderer);
    }

    public void onDestroy()
    {
        ogRenderer.onDestroy();
    }
    
    public boolean onKeyDown(final int keyCode, final KeyEvent event) 
    {
        queueEvent(new Runnable() 
        {
            // This method will be called on the rendering thread:
            public void run() 
            {
                ogRenderer.onKeyDown(keyCode, event);
            }
        });
        return true;
    }
    
    public boolean onTouchEvent(final MotionEvent event)
    {
        queueEvent(new Runnable() 
        {
            // This method will be called on the rendering thread:
            public void run()
            {
                ogRenderer.onTouchEvent(event);
            }
        });
        return true;
    }
    
    public void onSensorChanged(SensorEvent event)
    {
        if (event.sensor.getType() != Sensor.TYPE_ACCELEROMETER)
            return;
        
        final float alpha = 0.8f;
        gravity[0] = alpha*gravity[0] + (1 - alpha)*event.values[0];
        gravity[1] = alpha*gravity[1] + (1 - alpha)*event.values[1];
        gravity[2] = alpha*gravity[2] + (1 - alpha)*event.values[2];
        accel[0] = event.values[0] - gravity[0];
        accel[1] = event.values[1] - gravity[1];
        accel[2] = event.values[2] - gravity[2];
        OrangeGrassLib.onaccel(accel[0], accel[1]);
    }
    
    public void onAccuracyChanged(Sensor sensor, int accuracy) 
    {
        // TODO Auto-generated method stub
    }

    private static class Renderer implements GLSurfaceView.Renderer 
    {
        public static String assetsPath;

        public void onDrawFrame(GL10 gl) 
        {
            OrangeGrassLib.step();
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) 
        {
            OrangeGrassLib.init(width, height, Renderer.assetsPath);
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) 
        {
        }

        public void onTouchEvent(MotionEvent event)
        {
            int action = event.getAction();
            if (action == MotionEvent.ACTION_DOWN)
            {
                OrangeGrassLib.ontouchdown(event.getX(), event.getY());
            }
            else if (action == MotionEvent.ACTION_UP)
            {
                OrangeGrassLib.ontouchup(event.getX(), event.getY());
            }
        }

        public void onKeyDown(int keyCode, KeyEvent event) 
        {
            // TODO: implement
        }

        public void onDestroy() 
        {
            OrangeGrassLib.destroy();
        }
    }
}
