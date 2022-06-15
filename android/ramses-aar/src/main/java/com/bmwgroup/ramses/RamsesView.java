package com.bmwgroup.ramses;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.NonNull;
import androidx.lifecycle.DefaultLifecycleObserver;
import androidx.lifecycle.Lifecycle;
import androidx.lifecycle.LifecycleOwner;

public class RamsesView extends SurfaceView implements SurfaceHolder.Callback{
    RamsesThread mRamsesThread;
    public MyObserver mMyObserver = new MyObserver();

    public RamsesView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        // TODO Auto-generated constructor stub
    }

    @Override


    public RamsesView(Context context, AttributeSet attrs) {
        super(context, attrs);
        // TODO Auto-generated constructor stub
    }

    void onUpdate(){

    }

    void onDisplayResize(int width, int height){

    }

    void onSceneLoaded(){

    }

    void onSceneLoadFailed(){

    }

    void onLogicUpdated(){

    }


    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Surface surface = holder.getSurface();
        try {
            mRamsesThread.createDisplayAndShowScene(surface, null);
            // Setting the framerate to 30 as every frame properties are updated in onUpdate and the default of 60 would do unnecessary work
            // Setting it directly after creating the display will make sure that it will be applied for the lifetime of the display
            mRamsesThread.setRenderingFramerate(30f);
            startRendering();
        } catch (InterruptedException e) {
            Log.e("MainActivity", "surfaceCreated failed: ", e);
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        Surface surface = holder.getSurface();
        try {
            mRamsesThread.createDisplayAndShowScene(surface, null);
            // Setting the framerate to 30 as every frame properties are updated in onUpdate and the default of 60 would do unnecessary work
            // Setting it directly after creating the display will make sure that it will be applied for the lifetime of the display
            mRamsesThread.setRenderingFramerate(30f);
            startRendering();
        } catch (InterruptedException e) {
            Log.e("MainActivity", "surfaceCreated failed: ", e);
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        try {
            mRamsesThread.destroyDisplay();
        } catch (InterruptedException e) {
            Log.e("MainActivity", "surfaceDestroyed failed: ", e);
        }
    }

    public void startRendering() {
        mRamsesThread.addRunnableToThreadQueue(
                () -> {
                    if (mRamsesThread.isDisplayCreated() && !mRamsesThread.isRendering()) {
                        mRamsesThread.startRendering();
                    }
                });
    }

    public void stopRunningRenderThread() {
        mRamsesThread.addRunnableToThreadQueue(
                () -> {
                    if (mRamsesThread.isRendering()) {
                        mRamsesThread.stopRendering();
                    }
                });
    }

    public class MyObserver implements DefaultLifecycleObserver {
        @Override
        public void onCreate(@NonNull LifecycleOwner owner) {
            //TODO create an interface to make these overridable by user
            mRamsesThread = new RamsesThread("", getContext()) {
                @Override
                protected void onUpdate() {
                    RamsesView.this.onUpdate();
                }

                @Override
                protected void onDisplayResize(int width, int height) {
                    RamsesView.this.onDisplayResize(width, height);
                }

                @Override
                protected void onSceneLoaded() {
                    RamsesView.this.onSceneLoaded();
                }

                @Override
                protected void onSceneLoadFailed() {
                    RamsesView.this.onSceneLoadFailed();
                }

                @Override
                protected void onLogicUpdated() {
                    RamsesView.this.onLogicUpdated();
                }
            };

            mRamsesThread.initRamsesThreadAndLoadScene(getContext().getAssets(), "G05.ramses", "G05.rlogic");
        }

        @Override
        public void onDestroy(@NonNull LifecycleOwner owner) {
            DefaultLifecycleObserver.super.onDestroy(owner);
            try {
                mRamsesThread.destroyRamsesBundleAndQuitThread();
            } catch (InterruptedException e) {
                Log.e("MainActivity", "onDestroy failed: ", e);
            }
        }

        @Override
        public void onResume(@NonNull LifecycleOwner owner) {
            DefaultLifecycleObserver.super.onResume(owner);
            startRendering();
        }

        @Override
        public void onPause(@NonNull LifecycleOwner owner) {
            DefaultLifecycleObserver.super.onPause(owner);
            stopRunningRenderThread();
        }

        public void registerLifecycle(Lifecycle lifecycle) {
            lifecycle.addObserver(this);
        }

        //TODO onWindowFocusChanged
    }
}

