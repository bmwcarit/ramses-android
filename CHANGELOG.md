# Ramses AAR CHANGELOG

## Version 1.1.0

**CHANGED**

* New ramses logic (v1.1.1)
* New ramses (27.0.122)

**ADDED**

* RamsesBundle.getFeatureLevel()
    * Use this method to get the feature level of the currently loaded logic engine
* RamsesBundle.getInterface() and RamsesThread.getInterface()
    * Use these methods as replacement for *.getLogicNodeRootInput(), they will
      return reliably a unique interface object, or null if such doesn't exist
* Added new method overload RamsesBundle::createDisplay that accepts samples for MSAA antialiasing
* Added new method overload RamsesThread::createDisplayAndShowScene that accepts samples for MSAA antialiasing
* Added new method RamsesBundle::dispatchRendererEvents (see the FIXED section for details)

**FIXED**

* RamsesThread consumes renderer events and doesn't report warnings any more
    * If you are using RamsesBundle, call RamsesBundle::dispatchRendererEvents() every frame to also suppress the warning
* Fixed RamsesBundle::linkProperties(....) - used to unlink instead of link
* RamsesBundle and RamsesThread don't spam error logs related to dispatchRendererEvents any more
    * Used to issue the logs when no renderer/display were created

## Version 1.0.2

**FIXED**

* Added support for X86 ABI
* RamsesThread consumes renderer events and doesn't report warnings any more
    * If you are using RamsesBundle, call RamsesBundle::dispatchRendererEvents() every frame to also suppress the warning

**ADDED**

* Added new method RamsesBundle::dispatchRendererEvents (see the FIXED section for details)

## Version 1.0.1

**ADDED**

* Added new method overload RamsesBundle::createDisplay that accepts samples for MSAA antialiasing
* Added new method overload RamsesThread::createDisplayAndShowScene that accepts samples for MSAA antialiasing
* Possibility to load a scene from binary files using parcel file descriptors instead of asset manager

**FIXED**

* Minor documentation issues
* Ramses periodic logs are enabled by default to allow easy profiling
* Added support for x86_64 ABI

## Version 1.0.0

**ADDED**

* Ramses to 27.0.119
* Ramses Logic v1.0.3
* Initial version of the RamsesJNI package (can be used from Java and Kotlin)
* A RamsesBundle object for easy interaction with Ramses
* A RamsesThread class for non-blocking integration with the UI Thread
