package com.bmwgroup.ramses.ramsesview

interface RamsesCallback {
    fun onUpdate()
    fun onSceneLoaded()
    fun onDisplayResize(width: Int, height: Int)
    fun onSceneLoadedFailed()
    fun onLogicUpdated()
}