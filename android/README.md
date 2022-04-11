# Whats's an AAR?
Basically an AAR is a packaged app module. For more info on aar look [here](https://developer.android.com/studio/projects/android-library). It uses a tool called [prefab](https://google.github.io/prefab/) to package C/C++ prebuilt libraries. This comes in handy to package the two prebuilt libraries [ramses and ramses logic](TODO replace with new link) to include native graphics into a native android app with only one package.

# RAMSES LOGIC AAR BUILD STEPS WITH BARE GRADLE

## WINDOWS
1. Install Android SDK 29<br />
    Option 1: Download Android Studio from https://developer.android.com/studio (includes SDK); let's you install sdk 29 with package manager (if not already installed with studio)<br />
    Option 2: Download sdk tools package from https://developer.android.com/studio and use to install sdk 29
    1. extract contents of zip to e.g. C:\android-cmdline-tools
    2. execute in windows cmd prompt
    ```shell
    C:\android-cmdline-tools\bin\sdkmanager.bat --sdk_root=/desired/sdk/path --install "platforms;android-29"
    ```

2. Configure path to Android SDK<br />
    Option 1: Place a file called "local.properties" into root directory (<ramses-logic>/android/ramses-aar) that contains one line:
    "sdk.dir=/path/to/your/android/sdk" for example: "sdk.dir=C\:\\Users\\UserX\\AppData\\Local\\Android\\Sdk"<br />
    Option 2: Set the environment variable "ANDROID_SDK_ROOT" in your environment variables to your local android sdk path

3. Install gradle
    1. From https://gradle.org/releases/ download gradle binary with at least version 6.5.1
    2. Extract the contents of the downloaded zip to a directory like C:\Gradle\gradle-6.5.1
    3. Add the path to binary folder to your path environment variable (C:\Gradle\gradle-6.5.1\bin)

4. Install ninja-build (prerequiste for android ndk build)
    1. Go to https://github.com/ninja-build/ninja/releases and download a suitable binary for Windows. Place ninja.exe in a suitable spot. For example, C:\Ninja. Now add C:\Ninja to your path environment variable.

5. Build the aar
    1. Open a windows cmd prompt and navigate to the root directory of the android aar project (<ramses-logic>/android/ramses-aar)
    2. Execute:
    ```shell
    gradle assembleDebug
    ```
    3. AAR can now be found in <ramses-logic>/android/ramses-aar/build/outputs/aar

## LINUX
1. Install Android SDK 29<br />
    Option 1: Download Android Studio from https://developer.android.com/studio (includes SDK); let's you install sdk 29 with package manager (if not already installed with studio)<br />
    Option 2: Download sdk tools package from https://developer.android.com/studio and use to install sdk 29
    1. extract contents of zip to e.g. /home/android-cmdline-tools
    2. execute in terminal
    ```shell
    /home/android-cmdline-tools/sdkmanager --sdk_root=/desired/sdk/path --install "platforms;android-29"
    ```

2. Configure path to Android SDK<br />
    Option 1: Place a file called "local.properties" into root directory (<ramses-logic>/android/ramses-aar) that contains one line:
    "sdk.dir=/path/to/your/android/sdk" for example: "sdk.dir=/home/users/UserX/Android/Sdk" <br />
    Option 2: Set the environment variable "ANDROID_HOME" <br />
    Add following two lines to the bottom of ~/.bashrc or ~/.profile: <br />
    1. export ANDROID_HOME=$HOME/Android/Sdk<br />
    2. export PATH=$PATH:$ANDROID_HOME/tools<br />

3. Install gradle
    1. Use guide how to install gradle for your linux distribution; for ubuntu 20.0.4 https://linuxize.com/post/how-to-install-gradle-on-ubuntu-20-04/ worked out of the box.

4. Install ninja-build (prerequiste for android ndk build)
    1. Execute in terminal:
    ```shell
    sudo apt update -y
    sudo apt install -y ninja-build
    ```

5. Build the aar
    1. open a terminal and navigate to the root directory of the android aar project (<ramses-logic>/android/ramses-aar)
    2. execute in terminal:
    ```shell
    gradle assembleDebug
    ```
    3. AAR can now be found in <ramses-logic>/android/ramses-aar/build/outputs/aar

# RAMSES LOGIC AAR BUILD STEPS WITH ANDROID STUDIO (WINDOWS AND LINUX)
1. Download Android Studio installer from https://developer.android.com/studio and execute to install Android Studio
2. Install ninja-build (prerequiste for android ndk build) -->see point 4 of building with bare gradle for Windows and Linux
3. Open Project and choose path <ramses-logic>/android/ramses-aar
4. Sync project from gradle files --> will install the right android platform and gradle version
5. Make project
6. AAR can now be found in <ramses-logic>/android/ramses-aar/build/outputs/aar
