/*
 * Copyright 2010, The Android Open Source Project
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "DeviceOrientationManager.h"

#include "WebViewCore.h"
#include <DeviceOrientationClientMock.h>
#include <JNIHelp.h>

using namespace WebCore;

namespace android {

DeviceOrientationManager::DeviceOrientationManager()
    : m_useMock(false)
{
}

void DeviceOrientationManager::useMock()
{
    m_useMock = true;
}

void DeviceOrientationManager::setMockOrientation(PassRefPtr<DeviceOrientation> orientation)
{
    if (!m_useMock)
        return;
    static_cast<DeviceOrientationClientMock*>(client())->setOrientation(orientation);
};

DeviceOrientationClient* DeviceOrientationManager::client()
{
    // FIXME: Implement real client.
    if (!m_client)
        m_client.set(m_useMock ? new DeviceOrientationClientMock() : 0);
    ASSERT(m_client);
    return m_client.get();
}

// JNI for android.webkit.MockDeviceOrientation
static const char* javaDeviceOrientationManagerClass = "android/webkit/DeviceOrientationManager";

static WebViewCore* getWebViewCore(JNIEnv* env, jobject webViewCoreObject)
{
    jclass webViewCoreClass = env->FindClass("android/webkit/WebViewCore");
    jfieldID nativeClassField = env->GetFieldID(webViewCoreClass, "mNativeClass", "I");
    return reinterpret_cast<WebViewCore*>(env->GetIntField(webViewCoreObject, nativeClassField));
}

static void useMock(JNIEnv* env, jobject, jobject webViewCoreObject)
{
    getWebViewCore(env, webViewCoreObject)->deviceOrientationManager()->useMock();
}

static void setMockOrientation(JNIEnv* env, jobject, jobject webViewCoreObject, bool canProvideAlpha, double alpha, bool canProvideBeta, double beta, bool canProvideGamma, double gamma)
{
    RefPtr<DeviceOrientation> orientation = DeviceOrientation::create(canProvideAlpha, alpha, canProvideBeta, beta, canProvideGamma, gamma);
    getWebViewCore(env, webViewCoreObject)->deviceOrientationManager()->setMockOrientation(orientation.release());
}

static JNINativeMethod gDeviceOrientationManagerMethods[] = {
    { "nativeUseMock", "(Landroid/webkit/WebViewCore;)V", (void*) useMock },
    { "nativeSetMockOrientation", "(Landroid/webkit/WebViewCore;ZDZDZD)V", (void*) setMockOrientation }
};

int register_device_orientation_manager(JNIEnv* env)
{
    jclass deviceOrientationManager = env->FindClass(javaDeviceOrientationManagerClass);
    LOG_ASSERT(deviceOrientationManager, "Unable to find class");
    return jniRegisterNativeMethods(env, javaDeviceOrientationManagerClass, gDeviceOrientationManagerMethods, NELEM(gDeviceOrientationManagerMethods));
}

} // namespace android
