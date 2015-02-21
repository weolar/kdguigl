package com.kdgui.lib;


import com.kdgui.lib.Cocos2dxGLSurfaceView;
import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;
import android.opengl.GLSurfaceView;
import android.content.res.AssetManager;

public class Cocos2dxRenderer implements GLSurfaceView.Renderer {
	static {
        System.loadLibrary("kdgui");
    }

   /**
    * @param width the current view width
    * @param height the current view height
    */
    public static native void init(int width, int height);
    public static native boolean step();
    native static void setAssetManager(AssetManager asset);
    
	Cocos2dxGLSurfaceView m_view = null;
	
	public Cocos2dxRenderer(Cocos2dxGLSurfaceView v) {
		m_view = v;
	}
	
	@Override
    public void onDrawFrame(GL10 gl) {
    	boolean b = step();
        //if (b)
        //	m_view.requestRender();
    }

	@Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        init(width, height);
    }

	@Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        // Do nothing.
    }
}