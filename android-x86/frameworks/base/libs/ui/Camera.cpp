/*
**
** Copyright (C) 2008, The Android Open Source Project
** Copyright (C) 2008 HTC Inc.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

//#define LOG_NDEBUG 0
#define LOG_TAG "Camera"
#include <utils/Log.h>
#include <utils/IServiceManager.h>
#include <utils/threads.h>
#include <utils/IMemory.h>
#include <ui/Surface.h>
#include <ui/Camera.h>
#include <ui/ICameraService.h>

namespace android {

// client singleton for camera service binder interface
Mutex Camera::mLock;
sp<ICameraService> Camera::mCameraService;
sp<Camera::DeathNotifier> Camera::mDeathNotifier;

// establish binder interface to camera service
const sp<ICameraService>& Camera::getCameraService()
{
    Mutex::Autolock _l(mLock);
    if (mCameraService.get() == 0) {
        sp<IServiceManager> sm = defaultServiceManager();
        sp<IBinder> binder;
        do {
            binder = sm->getService(String16("media.camera"));
            if (binder != 0)
                break;
            LOGW("CameraService not published, waiting...");
            usleep(500000); // 0.5 s
        } while(true);
        if (mDeathNotifier == NULL) {
            mDeathNotifier = new DeathNotifier();
        }
        binder->linkToDeath(mDeathNotifier);
        mCameraService = interface_cast<ICameraService>(binder);
    }
    LOGE_IF(mCameraService==0, "no CameraService!?");
    return mCameraService;
}

// ---------------------------------------------------------------------------

Camera::Camera()
{
    init();
}

Camera::Camera(const sp<ICamera>& camera)
{
    init();
    // connect this client to existing camera remote
    if (camera->connect(this) == NO_ERROR) {
        mStatus = NO_ERROR;
        mCamera = camera;
        camera->asBinder()->linkToDeath(this);
    }
}

void Camera::init()
{
    mStatus = UNKNOWN_ERROR;
    mShutterCallback = 0;
    mShutterCallbackCookie = 0;
    mRawCallback = 0;
    mRawCallbackCookie = 0;
    mJpegCallback = 0;
    mJpegCallbackCookie = 0;
    mPreviewCallback = 0;
    mPreviewCallbackCookie = 0;
    mRecordingCallback = 0;
    mRecordingCallbackCookie = 0;
    mErrorCallback = 0;
    mErrorCallbackCookie = 0;
    mAutoFocusCallback = 0;
    mAutoFocusCallbackCookie = 0;
}

Camera::~Camera()
{
    disconnect();
}

sp<Camera> Camera::connect()
{
    LOGV("connect");
    sp<Camera> c = new Camera();
    const sp<ICameraService>& cs = getCameraService();
    if (cs != 0) {
        c->mCamera = cs->connect(c);
    }
    if (c->mCamera != 0) {
        c->mCamera->asBinder()->linkToDeath(c);
        c->mStatus = NO_ERROR;
    } else {
        c.clear();
    }
    return c;
}

void Camera::disconnect()
{
    LOGV("disconnect");
    if (mCamera != 0) {
        mErrorCallback = 0;
        mCamera->disconnect();
        mCamera = 0;
    }
}

status_t Camera::reconnect()
{
    LOGV("reconnect");
    sp <ICamera> c = mCamera;
    if (c == 0) return NO_INIT;
    return c->connect(this);
}

sp<ICamera> Camera::remote()
{
    return mCamera;
}

status_t Camera::lock()
{
    sp <ICamera> c = mCamera;
    if (c == 0) return NO_INIT;
    return c->lock();
}

status_t Camera::unlock()
{
    sp <ICamera> c = mCamera;
    if (c == 0) return NO_INIT;
    return c->unlock();
}

// pass the buffered ISurface to the camera service
status_t Camera::setPreviewDisplay(const sp<Surface>& surface)
{
    LOGV("setPreviewDisplay");
    if (surface == 0) {
        LOGE("app passed NULL surface");
        return NO_INIT;
    }
    sp <ICamera> c = mCamera;
    if (c == 0) return NO_INIT;
    return c->setPreviewDisplay(surface->getISurface());
}

status_t Camera::setPreviewDisplay(const sp<ISurface>& surface)
{
    LOGV("setPreviewDisplay");
    if (surface == 0) {
        LOGE("app passed NULL surface");
        return NO_INIT;
    }
    sp <ICamera> c = mCamera;
    if (c == 0) return NO_INIT;
    return c->setPreviewDisplay(surface);
}


// start preview mode, must call setPreviewDisplay first
status_t Camera::startPreview()
{
    LOGV("startPreview");
    sp <ICamera> c = mCamera;
    if (c == 0) return NO_INIT;
    return c->startPreview();
}

// start recording mode, must call setPreviewDisplay first
status_t Camera::startRecording()
{
    LOGV("startRecording");
    sp <ICamera> c = mCamera;
    if (c == 0) return NO_INIT;
    return c->startRecording();
}

// stop preview mode
void Camera::stopPreview()
{
    LOGV("stopPreview");
    sp <ICamera> c = mCamera;
    if (c == 0) return;
    c->stopPreview();
}

// stop recording mode
void Camera::stopRecording()
{
    LOGV("stopRecording");
    sp <ICamera> c = mCamera;
    if (c == 0) return;
    c->stopRecording();
}

// release a recording frame
void Camera::releaseRecordingFrame(const sp<IMemory>& mem)
{
    LOGV("releaseRecordingFrame");
    sp <ICamera> c = mCamera;
    if (c == 0) return;
    c->releaseRecordingFrame(mem);
}

// get preview state
bool Camera::previewEnabled()
{
    LOGV("previewEnabled");
    sp <ICamera> c = mCamera;
    if (c == 0) return false;
    return c->previewEnabled();
}

// get recording state
bool Camera::recordingEnabled()
{
    LOGV("recordingEnabled");
    sp <ICamera> c = mCamera;
    if (c == 0) return false;
    return c->recordingEnabled();
}

status_t Camera::autoFocus()
{
    LOGV("autoFocus");
    sp <ICamera> c = mCamera;
    if (c == 0) return NO_INIT;
    return c->autoFocus();
}

// take a picture
status_t Camera::takePicture()
{
    LOGV("takePicture");
    sp <ICamera> c = mCamera;
    if (c == 0) return NO_INIT;
    return c->takePicture();
}

// set preview/capture parameters - key/value pairs
status_t Camera::setParameters(const String8& params)
{
    LOGV("setParameters");
    sp <ICamera> c = mCamera;
    if (c == 0) return NO_INIT;
    return c->setParameters(params);
}

// get preview/capture parameters - key/value pairs
String8 Camera::getParameters() const
{
    LOGV("getParameters");
    String8 params;
    sp <ICamera> c = mCamera;
    if (c != 0) params = mCamera->getParameters();
    return params;
}

void Camera::setAutoFocusCallback(autofocus_callback cb, void *cookie)
{
    LOGV("setAutoFocusCallback");
    mAutoFocusCallback = cb;
    mAutoFocusCallbackCookie = cookie;
}

void Camera::setShutterCallback(shutter_callback cb, void *cookie)
{
    LOGV("setShutterCallback");
    mShutterCallback = cb;
    mShutterCallbackCookie = cookie;
}

void Camera::setRawCallback(frame_callback cb, void *cookie)
{
    LOGV("setRawCallback");
    mRawCallback = cb;
    mRawCallbackCookie = cookie;
}

void Camera::setJpegCallback(frame_callback cb, void *cookie)
{
    LOGV("setJpegCallback");
    mJpegCallback = cb;
    mJpegCallbackCookie = cookie;
}

void Camera::setPreviewCallback(frame_callback cb, void *cookie, int flag)
{
    LOGV("setPreviewCallback");
    mPreviewCallback = cb;
    mPreviewCallbackCookie = cookie;
    sp <ICamera> c = mCamera;
    if (c == 0) return;
    mCamera->setPreviewCallbackFlag(flag);
}

void Camera::setRecordingCallback(frame_callback cb, void *cookie)
{
    LOGV("setRecordingCallback");
    mRecordingCallback = cb;
    mRecordingCallbackCookie = cookie;
}

void Camera::setErrorCallback(error_callback cb, void *cookie)
{
    LOGV("setErrorCallback");
    mErrorCallback = cb;
    mErrorCallbackCookie = cookie;
}

void Camera::autoFocusCallback(bool focused)
{
    LOGV("autoFocusCallback");
    if (mAutoFocusCallback) {
        mAutoFocusCallback(focused, mAutoFocusCallbackCookie);
    }
}

void Camera::shutterCallback()
{
    LOGV("shutterCallback");
    if (mShutterCallback) {
        mShutterCallback(mShutterCallbackCookie);
    }
}

void Camera::rawCallback(const sp<IMemory>& picture)
{
    LOGV("rawCallback");
    if (mRawCallback) {
        mRawCallback(picture, mRawCallbackCookie);
    }
}

// callback from camera service when image is ready
void Camera::jpegCallback(const sp<IMemory>& picture)
{
    LOGV("jpegCallback");
    if (mJpegCallback) {
        mJpegCallback(picture, mJpegCallbackCookie);
    }
}

// callback from camera service when preview frame is ready
void Camera::previewCallback(const sp<IMemory>& frame)
{
    LOGV("frameCallback");
    if (mPreviewCallback) {
        mPreviewCallback(frame, mPreviewCallbackCookie);
    }
}

// callback from camera service when a recording frame is ready
void Camera::recordingCallback(const sp<IMemory>& frame)
{
    LOGV("recordingCallback");
    if (mRecordingCallback) {
        mRecordingCallback(frame, mRecordingCallbackCookie);
    }
}

// callback from camera service when an error occurs in preview or takePicture
void Camera::errorCallback(status_t error)
{
    LOGV("errorCallback");
    if (mErrorCallback) {
        mErrorCallback(error, mErrorCallbackCookie);
    }
}

void Camera::binderDied(const wp<IBinder>& who) {
    LOGW("ICamera died");
    if (mErrorCallback) {
        mErrorCallback(DEAD_OBJECT, mErrorCallbackCookie);
    }
}

void Camera::DeathNotifier::binderDied(const wp<IBinder>& who) {
    LOGV("binderDied");
    Mutex::Autolock _l(Camera::mLock);
    Camera::mCameraService.clear();
    LOGW("Camera server died!");
}

}; // namespace android

