package com.bmwgroup.ramses.ramsesview

import android.app.Application
import android.content.Context
import android.util.AttributeSet
import android.view.SurfaceHolder
import android.view.SurfaceView
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.ViewTreeViewModelStoreOwner
import androidx.lifecycle.findViewTreeViewModelStoreOwner
import androidx.lifecycle.get
import com.bmwgroup.ramseslogicbundle.R


class RamsesView(context: Context, attrs: AttributeSet?) : SurfaceView(context, attrs), SurfaceHolder.Callback {
    val mRamsesFileName: String
    val mRlogicFileName: String


    companion object {
        const val TAG = "RamsesView"
    }
    init {
        context.theme.obtainStyledAttributes(
            attrs,
            R.styleable.RamsesView,
            0, 0).apply {
                try {
                    mRamsesFileName = getString(R.styleable.RamsesView_ramsesFileName) ?: ""
                    mRlogicFileName = getString(R.styleable.RamsesView_rlogicFileName) ?: ""
                } finally {
                    recycle()
                }
        }
        holder.addCallback(this@RamsesView)
    }

    fun setRamsesCallback(callback: RamsesCallback?) {
        viewModel.mRamsesCallback = callback
    }

    private fun initViewTreeOwners() {
        val activity = (context as AppCompatActivity)
        ViewTreeViewModelStoreOwner.set(activity.window.decorView, activity)
    }

    val viewModel by lazy {
        ViewModelProvider(findViewTreeViewModelStoreOwner()!!).get<AssetViewModel>()
        //initViewTreeOwners()
        //val viewModelFactory = AssetViewModelProviderFactory(context.applicationContext as Application, mRamsesFileName, mRlogicFileName)
        //ViewModelProvider(findViewTreeViewModelStoreOwner()!!, viewModelFactory).get<AssetViewModel>()
    }

    override fun onAttachedToWindow() {
        super.onAttachedToWindow()
        viewModel.initRamses()
    }

    override fun surfaceCreated(surfaceHolder: SurfaceHolder) {
        viewModel.onSurfaceCreatedDelegate(surfaceHolder.surface)
    }

    override fun surfaceChanged(surfaceHolder: SurfaceHolder, format: Int, width: Int, height: Int) {
        viewModel.onSurfaceChangedDelegate(width, height)
    }
    override fun surfaceDestroyed(surfaceHolder: SurfaceHolder) {
        surfaceHolder.surface.release()
        viewModel.onSurfaceDestroyedDelegate()
    }

    override fun onDetachedFromWindow() {
        super.onDetachedFromWindow()
        viewModel.tearDownRamses()
    }
}