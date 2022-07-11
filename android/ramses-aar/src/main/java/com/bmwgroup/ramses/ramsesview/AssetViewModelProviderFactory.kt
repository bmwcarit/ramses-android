package com.bmwgroup.ramses.ramsesview

import android.app.Application
import android.util.Log
import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import java.lang.IllegalArgumentException

class AssetViewModelProviderFactory(private val assetViewModelSource: Application,
                                    private val ramsesFileName: String,
                                    private val rlogicFileName: String) : ViewModelProvider.Factory {

    companion object {
        const val LOGGING_TAG = "AssetViewModelProviderFactory"
    }

    override fun <T : ViewModel> create(modelClass: Class<T>): T {
        if (modelClass.isAssignableFrom(AssetViewModel::class.java)) {
            Log.i(LOGGING_TAG, "create is assignable")
            return AssetViewModel(assetViewModelSource, ramsesFileName, rlogicFileName) as T
        }
        throw IllegalArgumentException("Unknown ViewModel class")
    }
}