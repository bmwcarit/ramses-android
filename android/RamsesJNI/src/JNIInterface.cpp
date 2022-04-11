//  -------------------------------------------------------------------------
//  Copyright (C) 2021 BMW AG
//  -------------------------------------------------------------------------
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at https://mozilla.org/MPL/2.0/.
//  -------------------------------------------------------------------------

#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>

#include "RamsesBundle.h"
#include "ramses-logic/Property.h"
#include <unistd.h>

extern "C"
{
static jclass jIntegerClass;
static jclass jFloatClass;
static jclass jBooleanClass;

static jmethodID jIntegerConstructor;
static jmethodID jFloatConstructor;
static jmethodID jBooleanConstructor;

static_assert(sizeof(jint)== sizeof(int));
static_assert(sizeof(jfloat)== sizeof(float));
static_assert(sizeof(jboolean)== sizeof(bool));
static_assert(sizeof(jlong)== sizeof(void*));
static_assert(sizeof(jsize)== sizeof(int));
static_assert(sizeof(jlong)== sizeof(int64_t));

jint JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    JNIEnv* env = nullptr;
    // JNI_VERSION_1_6 is from android JNI docs(https://developer.android.com/training/articles/perf-jni#native-libraries).
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    jclass tempJIntegerClassRef = env->FindClass("java/lang/Integer");
    jclass tempJFloatClassRef = env->FindClass("java/lang/Float");
    jclass tempJBooleanClassRef = env->FindClass("java/lang/Boolean");

    if (!tempJIntegerClassRef || !tempJFloatClassRef || !tempJBooleanClassRef){
        return JNI_ERR;
    }

    jIntegerClass = static_cast<jclass>(env->NewGlobalRef(tempJIntegerClassRef));
    jFloatClass = static_cast<jclass>(env->NewGlobalRef(tempJFloatClassRef));
    jBooleanClass = static_cast<jclass>(env->NewGlobalRef(tempJBooleanClassRef));

    env->DeleteLocalRef(tempJIntegerClassRef);
    env->DeleteLocalRef(tempJFloatClassRef);
    env->DeleteLocalRef(tempJBooleanClassRef);

    jIntegerConstructor = env->GetMethodID(jIntegerClass, "<init>", "(I)V");
    jFloatConstructor = env->GetMethodID(jFloatClass, "<init>", "(F)V");
    jBooleanConstructor = env->GetMethodID(jBooleanClass, "<init>", "(Z)V");

    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM *vm, void* /*reserved*/) {
    JNIEnv* env = nullptr;
    vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);

    env->DeleteGlobalRef(jIntegerClass);
    env->DeleteGlobalRef(jFloatClass);
    env->DeleteGlobalRef(jBooleanClass);
}

jintArray loadJintArrayFromIntVec(JNIEnv* env, const int* const values, size_t arraySize)
{
    jintArray returnArray = env->NewIntArray(arraySize);
    if (!returnArray)
    {
        return nullptr;
    }
    env->SetIntArrayRegion(returnArray, 0, static_cast<jsize>(arraySize), values);
    return returnArray;
}

jfloatArray loadJfloatArrayFromFloatVec(JNIEnv* env, const float* const values, size_t arraySize)
{
    jfloatArray returnArray = env->NewFloatArray(arraySize);
    if (!returnArray)
    {
        return nullptr;
    }
    env->SetFloatArrayRegion(returnArray, 0, static_cast<jsize>(arraySize), values);
    return returnArray;
}

JNIEXPORT jlong JNICALL
Java_com_bmwgroup_ramses_RamsesBundle_createRamsesBundle(JNIEnv* /*env*/, jobject /*instance*/) {
    auto* ramsesBundle = new ramses_bundle::RamsesBundle();
    return reinterpret_cast<jlong>(ramsesBundle);
}

JNIEXPORT jboolean JNICALL
Java_com_bmwgroup_ramses_RamsesBundle_createDisplay(JNIEnv* env, jobject /*instance*/, jlong handle, jobject surface, jfloat red, jfloat green, jfloat blue, jfloat alpha) {
    ANativeWindow* nativeWindowHandle = ANativeWindow_fromSurface(env, surface);
    auto* ramsesBundle = reinterpret_cast<ramses_bundle::RamsesBundle*>(handle);
    return ramsesBundle->createDisplay(nativeWindowHandle, {red, green, blue, alpha});
}

JNIEXPORT jboolean JNICALL
Java_com_bmwgroup_ramses_RamsesBundle_destroyDisplay(JNIEnv* /*env*/, jobject /*instance*/, jlong handle) {
    auto* ramsesBundle = reinterpret_cast<ramses_bundle::RamsesBundle*>(handle);
    return ramsesBundle->destroyDisplay();
}

JNIEXPORT void JNICALL
Java_com_bmwgroup_ramses_RamsesBundle_resizeDisplay(JNIEnv* /*env*/, jobject /*instance*/, jlong handle, jint width, jint height) {
    auto* ramsesBundle = reinterpret_cast<ramses_bundle::RamsesBundle*>(handle);
    ramsesBundle->resizeDisplay(width, height);
}

JNIEXPORT jboolean JNICALL
Java_com_bmwgroup_ramses_RamsesBundle_setMaximumFramerate(JNIEnv* /*env*/, jobject /*instance*/, jlong handle, jfloat maximumFramerate) {
    auto* ramsesBundle = reinterpret_cast<ramses_bundle::RamsesBundle*>(handle);
    return ramsesBundle->setMaximumFramerate(maximumFramerate);
}

JNIEXPORT jboolean JNICALL
Java_com_bmwgroup_ramses_RamsesBundle_startRendering(JNIEnv* /*env*/, jobject /*instance*/, jlong handle) {
    auto* ramsesBundle = reinterpret_cast<ramses_bundle::RamsesBundle*>(handle);
    return ramsesBundle->startRendering();
}

JNIEXPORT jboolean JNICALL
Java_com_bmwgroup_ramses_RamsesBundle_stopRendering(JNIEnv* /*env*/, jobject /*instance*/, jlong handle) {
    auto* ramsesBundle = reinterpret_cast<ramses_bundle::RamsesBundle*>(handle);
    return ramsesBundle->stopRendering();
}

JNIEXPORT jboolean JNICALL
Java_com_bmwgroup_ramses_RamsesBundle_isRendering(JNIEnv* /*env*/, jobject /*instance*/, jlong handle) {
    auto* ramsesBundle = reinterpret_cast<ramses_bundle::RamsesBundle*>(handle);
    return ramsesBundle->isRendering();
}

JNIEXPORT jboolean JNICALL
Java_com_bmwgroup_ramses_RamsesBundle_isDisplayCreated(JNIEnv* /*env*/, jobject /*instance*/, jlong handle) {
    auto* ramsesBundle = reinterpret_cast<ramses_bundle::RamsesBundle*>(handle);
    return ramsesBundle->isDisplayCreated();
}

JNIEXPORT jboolean JNICALL
Java_com_bmwgroup_ramses_RamsesBundle_loadScene(JNIEnv* /*env*/, jobject /*instance*/, jlong handle, jint fdRamses, jlong fdRamsesOffset, jlong fdRamsesLength,
                                                jint fdRlogic, jlong fdRlogicOffset, jlong fdRlogicLength) {
    auto* ramsesBundle = reinterpret_cast<ramses_bundle::RamsesBundle*>(handle);

    std::unique_ptr<char[]> rlogicBuffer;
    if (fdRlogicLength != 0)
    {
        rlogicBuffer = std::make_unique<char[]>(fdRlogicLength);
        lseek(fdRlogic, fdRlogicOffset, SEEK_SET);
        read(fdRlogic, rlogicBuffer.get(), fdRlogicLength);
        close(fdRlogic);
    }
    return ramsesBundle->loadScene(fdRamses, fdRamsesOffset, fdRamsesLength, rlogicBuffer.get(), fdRlogicLength);
}

JNIEXPORT jlong JNICALL
Java_com_bmwgroup_ramses_RamsesBundle_getLogicNodeRootInput(JNIEnv* env, jobject /*instance*/, jlong handle, jstring logicNodeName) {
    auto* ramsesBundle = reinterpret_cast<ramses_bundle::RamsesBundle*>(handle);

    const char *nodeName = env->GetStringUTFChars(logicNodeName, 0);
    rlogic::Property* property = ramsesBundle->getLogicNodeRootInput(nodeName);
    env->ReleaseStringUTFChars(logicNodeName, nodeName);

    return reinterpret_cast<jlong>(property);
}

JNIEXPORT jlong JNICALL
Java_com_bmwgroup_ramses_RamsesBundle_getLogicNodeRootOutput(JNIEnv* env, jobject /*instance*/, jlong handle, jstring logicNodeName) {
    auto* ramsesBundle = reinterpret_cast<ramses_bundle::RamsesBundle*>(handle);

    const char *nodeName = env->GetStringUTFChars(logicNodeName, 0);
    const rlogic::Property* property = ramsesBundle->getLogicNodeRootOutput(nodeName);
    env->ReleaseStringUTFChars(logicNodeName, nodeName);

    return reinterpret_cast<jlong>(property);
}

JNIEXPORT void JNICALL
Java_com_bmwgroup_ramses_RamsesBundle_dumpSceneToFile(JNIEnv* env, jobject /*instance*/, jlong handle, jstring filePath) {
    auto* ramsesBundle = reinterpret_cast<ramses_bundle::RamsesBundle*>(handle);

    const char *fP = env->GetStringUTFChars(filePath, 0);
    ramsesBundle->dumpSceneToFile(fP);
    env->ReleaseStringUTFChars(filePath, fP);
}

JNIEXPORT void JNICALL
Java_com_bmwgroup_ramses_RamsesBundle_dumpLogicToFile(JNIEnv* env, jobject /*instance*/, jlong handle, jstring filePath) {
    auto* ramsesBundle = reinterpret_cast<ramses_bundle::RamsesBundle*>(handle);

    const char *fP = env->GetStringUTFChars(filePath, 0);
    ramsesBundle->dumpLogicToFile(fP);
    env->ReleaseStringUTFChars(filePath, fP);
}

JNIEXPORT void JNICALL
Java_com_bmwgroup_ramses_RamsesBundle_executeRamshCommand(JNIEnv* env, jobject /*instance*/, jlong handle, jstring command) {
    auto* ramsesBundle = reinterpret_cast<ramses_bundle::RamsesBundle*>(handle);

    const char *cmd = env->GetStringUTFChars(command, 0);
    ramsesBundle->executeRamshCommand(cmd);
    env->ReleaseStringUTFChars(command, cmd);
}

JNIEXPORT jstring JNICALL
Java_com_bmwgroup_ramses_Property_getName(JNIEnv* env, jobject /*instance*/, jlong handle) {
    const auto* property = reinterpret_cast<rlogic::Property*>(handle);
    const auto propertyName = property->getName();
    return propertyName.empty() ? nullptr : env->NewStringUTF(propertyName.data());
}

JNIEXPORT jboolean JNICALL
Java_com_bmwgroup_ramses_Property_isLinked(JNIEnv* env, jobject /*instance*/, jlong handle) {
    const auto* property = reinterpret_cast<rlogic::Property*>(handle);
    return property->isLinked();
}

JNIEXPORT jlong JNICALL
Java_com_bmwgroup_ramses_Property_getChildByName(JNIEnv* env, jobject /*instance*/, jlong handle, jstring propertyChildName) {
    auto* property = reinterpret_cast<rlogic::Property*>(handle);

    const char *childName = env->GetStringUTFChars(propertyChildName, 0);
    rlogic::Property* childProperty = property->getChild(childName);
    env->ReleaseStringUTFChars(propertyChildName, childName);

    return reinterpret_cast<jlong>(childProperty);
}

JNIEXPORT jlong JNICALL
Java_com_bmwgroup_ramses_Property_getChildByIndex(JNIEnv* /*env*/, jobject /*instance*/, jlong handle, jint index) {
    auto* property = reinterpret_cast<rlogic::Property*>(handle);
    rlogic::Property* childProperty = property->getChild(static_cast<size_t>(index));
    return reinterpret_cast<jlong>(childProperty);
}

JNIEXPORT jint JNICALL
Java_com_bmwgroup_ramses_Property_getChildCount(JNIEnv* /*env*/, jobject /*instance*/, jlong handle) {
    const auto* property = reinterpret_cast<rlogic::Property*>(handle);
    const size_t childCount = property->getChildCount();
    return static_cast<int>(childCount);
}

JNIEXPORT jboolean JNICALL
Java_com_bmwgroup_ramses_Property_hasChild(JNIEnv* env, jobject /*instance*/, jlong handle, jstring childName) {
    const auto* property = reinterpret_cast<rlogic::Property*>(handle);
    const char *name = env->GetStringUTFChars(childName, 0);
    jboolean hasChild = property->hasChild(name);
    env->ReleaseStringUTFChars(childName, name);
    return hasChild;
}

JNIEXPORT jboolean JNICALL
Java_com_bmwgroup_ramses_RamsesBundle_showScene(JNIEnv* /*env*/, jobject /*instance*/, jlong handle) {
    auto* ramsesBundle = reinterpret_cast<ramses_bundle::RamsesBundle*>(handle);

    jboolean result = ramsesBundle->showScene();
    return result;
}

JNIEXPORT jboolean JNICALL
Java_com_bmwgroup_ramses_RamsesBundle_updateLogic(JNIEnv* /*env*/, jobject /*instance*/, jlong handle) {
    auto* ramsesBundle = reinterpret_cast<ramses_bundle::RamsesBundle*>(handle);
    return ramsesBundle->updateLogic();
}

JNIEXPORT jboolean JNICALL
Java_com_bmwgroup_ramses_RamsesBundle_flushRamsesScene(JNIEnv* /*env*/, jobject /*instance*/, jlong handle) {
    auto* ramsesBundle = reinterpret_cast<ramses_bundle::RamsesBundle*>(handle);
    return ramsesBundle->flushRamsesScene();
}

JNIEXPORT void JNICALL
Java_com_bmwgroup_ramses_RamsesBundle_dispose(JNIEnv* /*env*/, jobject /*instance*/, jlong handle) {
    auto* ramsesBundle = reinterpret_cast<ramses_bundle::RamsesBundle*>(handle);
    delete ramsesBundle;
}

JNIEXPORT jintArray JNICALL
Java_com_bmwgroup_ramses_RamsesBundle_getDisplaySize(JNIEnv* env, jobject /*instance*/, jlong handle) {
    const auto* ramsesBundle = reinterpret_cast<ramses_bundle::RamsesBundle*>(handle);

    const std::optional<rlogic::vec2i> result = ramsesBundle->getDisplaySize();
    if (result)
    {
        return loadJintArrayFromIntVec(env, result.value().data(), result.value().size());
    }
    return nullptr;
}

JNIEXPORT jboolean JNICALL
Java_com_bmwgroup_ramses_RamsesBundle_linkProperties(JNIEnv* /*env*/, jobject /*instance*/, jlong handleRamsesBundle, jlong handleSource, jlong handleTarget) {
    auto* ramsesBundle = reinterpret_cast<ramses_bundle::RamsesBundle*>(handleRamsesBundle);
    const auto* sourceProperty = reinterpret_cast<rlogic::Property*>(handleSource);
    const auto* targetProperty = reinterpret_cast<rlogic::Property*>(handleTarget);

    return ramsesBundle->linkProperties(*sourceProperty, *targetProperty);
}

JNIEXPORT jboolean JNICALL
Java_com_bmwgroup_ramses_RamsesBundle_unlinkProperties(JNIEnv* /*env*/, jobject /*instance*/, jlong handleRamsesBundle, jlong handleSource, jlong handleTarget) {
    auto* ramsesBundle = reinterpret_cast<ramses_bundle::RamsesBundle*>(handleRamsesBundle);
    const auto* sourceProperty = reinterpret_cast<rlogic::Property*>(handleSource);
    const auto* targetProperty = reinterpret_cast<rlogic::Property*>(handleTarget);

    return ramsesBundle->unlinkProperties(*sourceProperty, *targetProperty);
}

JNIEXPORT jboolean JNICALL
Java_com_bmwgroup_ramses_Property_setInputPropertyValueFloat(JNIEnv* /*env*/, jobject /*instance*/, jlong handle, float value) {
    auto* property = reinterpret_cast<rlogic::Property*>(handle);
    return property->set<float>(value);
}

JNIEXPORT jboolean JNICALL
Java_com_bmwgroup_ramses_Property_setInputPropertyValueInt(JNIEnv* /*env*/, jobject /*instance*/, jlong handle, int value) {
    auto* property = reinterpret_cast<rlogic::Property*>(handle);
    return property->set<int>(value);
}

JNIEXPORT jboolean JNICALL
Java_com_bmwgroup_ramses_Property_setInputPropertyValueVec2i(JNIEnv* /*env*/, jobject /*instance*/, jlong handle, jint value1, jint value2) {
    auto* property = reinterpret_cast<rlogic::Property*>(handle);
    return property->set<rlogic::vec2i>({value1, value2});
}

JNIEXPORT jboolean JNICALL
Java_com_bmwgroup_ramses_Property_setInputPropertyValueVec3i(JNIEnv* /*env*/, jobject /*instance*/, jlong handle, jint value1,
                                                             jint value2, jint value3) {
    auto* property = reinterpret_cast<rlogic::Property*>(handle);
    return property->set<rlogic::vec3i>({value1, value2, value3});
}

JNIEXPORT jboolean JNICALL
Java_com_bmwgroup_ramses_Property_setInputPropertyValueVec4i(JNIEnv* /*env*/, jobject /*instance*/, jlong handle, jint value1,
                                                             jint value2, jint value3, jint value4) {
    auto* property = reinterpret_cast<rlogic::Property*>(handle);
    return property->set<rlogic::vec4i>({value1, value2, value3, value4});
}

JNIEXPORT jboolean JNICALL
Java_com_bmwgroup_ramses_Property_setInputPropertyValueVec2f(JNIEnv* /*env*/, jobject /*instance*/, jlong handle, jfloat value1, jfloat value2) {
    auto* property = reinterpret_cast<rlogic::Property*>(handle);
    return property->set<rlogic::vec2f>({value1, value2});
}

JNIEXPORT jboolean JNICALL
Java_com_bmwgroup_ramses_Property_setInputPropertyValueVec3f(JNIEnv* /*env*/, jobject /*instance*/, jlong handle, jfloat value1,
                                                             jfloat value2, jfloat value3) {
    auto* property = reinterpret_cast<rlogic::Property*>(handle);
    return property->set<rlogic::vec3f>({value1, value2, value3});
}

JNIEXPORT jboolean JNICALL
Java_com_bmwgroup_ramses_Property_setInputPropertyValueVec4f(JNIEnv* /*env*/, jobject /*instance*/, jlong handle, jfloat value1,
                                                             jfloat value2, jfloat value3, jfloat value4) {
    auto* property = reinterpret_cast<rlogic::Property*>(handle);
    return property->set<rlogic::vec4f>({value1, value2, value3, value4});
}

JNIEXPORT jboolean JNICALL
Java_com_bmwgroup_ramses_Property_setInputPropertyValueBool(JNIEnv* /*env*/, jobject /*instance*/, jlong handle, jboolean value) {
    auto* property = reinterpret_cast<rlogic::Property*>(handle);
    return property->set<bool>(value);
}

JNIEXPORT jboolean JNICALL
Java_com_bmwgroup_ramses_Property_setInputPropertyValueString(JNIEnv* env, jobject /*instance*/, jlong handle, jstring value) {
    auto* property = reinterpret_cast<rlogic::Property*>(handle);
    const char *val = env->GetStringUTFChars(value, 0);
    bool result = property->set<std::string>(std::string(val));
    env->ReleaseStringUTFChars(value, val);
    return result;
}

JNIEXPORT jobject JNICALL
Java_com_bmwgroup_ramses_Property_getInputPropertyValueFloat(JNIEnv* env, jobject /*instance*/, jlong handle) {
    const auto* property = reinterpret_cast<rlogic::Property*>(handle);

    const std::optional<float> result = property->get<float>();
    if (result)
    {
        return env->NewObject(jFloatClass, jFloatConstructor, static_cast<float>(result.value()));
    }
    return nullptr;
}

JNIEXPORT jobject JNICALL
Java_com_bmwgroup_ramses_Property_getInputPropertyValueInt(JNIEnv* env, jobject /*instance*/, jlong handle) {
    const auto* property = reinterpret_cast<rlogic::Property*>(handle);

    const std::optional<int> result = property->get<int>();
    if (result)
    {
        return env->NewObject(jIntegerClass, jIntegerConstructor, static_cast<int>(result.value()));
    }
    return nullptr;
}

JNIEXPORT jobject JNICALL
Java_com_bmwgroup_ramses_Property_getInputPropertyValueBool(JNIEnv* env, jobject /*instance*/, jlong handle) {
    const auto* property = reinterpret_cast<rlogic::Property*>(handle);

    const std::optional<bool> result = property->get<bool>();
    if (result)
    {
        return env->NewObject(jBooleanClass, jBooleanConstructor, static_cast<bool>(result.value()));
    }
    return nullptr;
}

JNIEXPORT jstring JNICALL
Java_com_bmwgroup_ramses_Property_getInputPropertyValueString(JNIEnv* env, jobject /*instance*/, jlong handle) {
    const auto* property = reinterpret_cast<rlogic::Property*>(handle);

    const std::optional<std::string> result = property->get<std::string>();
    return result ? env->NewStringUTF(result.value().c_str()) : nullptr;
}

JNIEXPORT jintArray JNICALL
Java_com_bmwgroup_ramses_Property_getInputPropertyValueVec2i(JNIEnv* env, jobject /*instance*/, jlong handle) {
    const auto* property = reinterpret_cast<rlogic::Property*>(handle);

    const std::optional<rlogic::vec2i> result = property->get<rlogic::vec2i>();
    if (result)
    {
        return loadJintArrayFromIntVec(env, result.value().data(), result.value().size());
    }
    return nullptr;
}

JNIEXPORT jintArray JNICALL
Java_com_bmwgroup_ramses_Property_getInputPropertyValueVec3i(JNIEnv* env, jobject /*instance*/, jlong handle) {
    const auto* property = reinterpret_cast<rlogic::Property*>(handle);

    const std::optional<rlogic::vec3i> result = property->get<rlogic::vec3i>();
    if (result)
    {
        return loadJintArrayFromIntVec(env, result.value().data(), result.value().size());
    }
    return nullptr;
}

JNIEXPORT jintArray JNICALL
Java_com_bmwgroup_ramses_Property_getInputPropertyValueVec4i(JNIEnv* env, jobject /*instance*/, jlong handle) {
    const auto* property = reinterpret_cast<rlogic::Property*>(handle);

    const std::optional<rlogic::vec4i> result = property->get<rlogic::vec4i>();
    if (result)
    {
        return loadJintArrayFromIntVec(env, result.value().data(), result.value().size());
    }
    return nullptr;
}

JNIEXPORT jfloatArray JNICALL
Java_com_bmwgroup_ramses_Property_getInputPropertyValueVec2f(JNIEnv* env, jobject /*instance*/, jlong handle) {
    const auto* property = reinterpret_cast<rlogic::Property*>(handle);

    const std::optional<rlogic::vec2f> result = property->get<rlogic::vec2f>();
    if (result)
    {
        return loadJfloatArrayFromFloatVec(env, result.value().data(), result.value().size());
    }
    return nullptr;
}

JNIEXPORT jfloatArray JNICALL
Java_com_bmwgroup_ramses_Property_getInputPropertyValueVec3f(JNIEnv* env, jobject /*instance*/, jlong handle) {
    const auto* property = reinterpret_cast<rlogic::Property*>(handle);

    const std::optional<rlogic::vec3f> result = property->get<rlogic::vec3f>();
    if (result)
    {
        return loadJfloatArrayFromFloatVec(env, result.value().data(), result.value().size());
    }
    return nullptr;
}

JNIEXPORT jfloatArray JNICALL
Java_com_bmwgroup_ramses_Property_getInputPropertyValueVec4f(JNIEnv* env, jobject /*instance*/, jlong handle) {
    const auto* property = reinterpret_cast<rlogic::Property*>(handle);

    const std::optional<rlogic::vec4f> result = property->get<rlogic::vec4f>();
    if (result)
    {
        return loadJfloatArrayFromFloatVec(env, result.value().data(), result.value().size());
    }
    return nullptr;
}
}
