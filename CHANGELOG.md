# Ramses AAR CHANGELOG

## Version 1.0.2

**FIXED**

* Added support for x86 ABI

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
