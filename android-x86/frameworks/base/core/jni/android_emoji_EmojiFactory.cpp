#include "SkTypes.h"
#include "SkImageDecoder.h"

#define LOG_TAG "DoCoMoEmojiFactory_jni"
#include <utils/Log.h>
#include <utils/String8.h>

#include "EmojiFactory.h"
#include <nativehelper/JNIHelp.h>

#include <dlfcn.h>
// #include <pthread.h>

namespace android {

// Note: This class is originally developed so that libandroid_runtime does
// not have to depend on libemoji which is optional library. However, we
// cannot use this class, since current (2009-02-16) bionic libc does not allow
// dlopen()-ing inside dlopen(), while not only this class but also libemoji
// uses dlopen().
class EmojiFactoryCaller {
 public:
  EmojiFactoryCaller();
  virtual ~EmojiFactoryCaller();
  EmojiFactory *TryCallGetImplementation(const char* name);
  EmojiFactory *TryCallGetAvailableImplementation();
 private:
  void *m_handle;
  EmojiFactory *(*m_get_implementation)(const char*);
  EmojiFactory *(*m_get_available_implementation)();
};

EmojiFactoryCaller::EmojiFactoryCaller() {
  m_handle = dlopen("libemoji.so", RTLD_LAZY | RTLD_LOCAL);
  const char* error_str = dlerror();
  if (error_str) {
    LOGI("Failed to load libemoji.so: %s", error_str);
    return;
  }

  m_get_implementation =
      reinterpret_cast<EmojiFactory *(*)(const char*)>(
          dlsym(m_handle, "GetImplementation"));
  error_str = dlerror();
  if (error_str) {
    LOGE("Failed to get symbol of GetImplementation: %s", error_str);
    dlclose(m_handle);
    m_handle = NULL;
    return;
  }

  m_get_available_implementation =
      reinterpret_cast<EmojiFactory *(*)()>(
          dlsym(m_handle,"GetAvailableImplementation"));
  error_str = dlerror();
  if (error_str) {
    LOGE("Failed to get symbol of GetAvailableImplementation: %s", error_str);
    dlclose(m_handle);
    m_handle = NULL;
    return;
  }
}

EmojiFactoryCaller::~EmojiFactoryCaller() {
  if (m_handle) {
    dlclose(m_handle);
  }
}

EmojiFactory *EmojiFactoryCaller::TryCallGetImplementation(
    const char* name) {
  if (NULL == m_handle) {
    return NULL;
  }
  return m_get_implementation(name);
}

EmojiFactory *EmojiFactoryCaller::TryCallGetAvailableImplementation() {
  if (NULL == m_handle) {
    return NULL;
  }
  return m_get_available_implementation();
}

// Note: bionic libc's dlopen() does not allow recursive dlopen(). So currently
// we cannot use EmojiFactoryCaller here.
// static EmojiFactoryCaller* gCaller;
// static pthread_once_t g_once = PTHREAD_ONCE_INIT;

static jclass    gString_class;

static jclass    gBitmap_class;
static jmethodID gBitmap_constructorMethodID;

static jclass    gEmojiFactory_class;
static jmethodID gEmojiFactory_constructorMethodID;

// static void InitializeCaller() {
//   gCaller = new EmojiFactoryCaller();
// }

static jobject create_java_EmojiFactory(
    JNIEnv* env, EmojiFactory* factory, jstring name) {
  jobject obj = env->AllocObject(gEmojiFactory_class);
  if (obj) {
    env->CallVoidMethod(obj, gEmojiFactory_constructorMethodID,
                        (jint)factory, name);
    if (env->ExceptionCheck() != 0) {
      LOGE("*** Uncaught exception returned from Java call!\n");
      env->ExceptionDescribe();
      obj = NULL;
    }
  }
  return obj;
}

static jobject android_emoji_EmojiFactory_newInstance(
    JNIEnv* env, jobject clazz, jstring name) {
  // pthread_once(&g_once, InitializeCaller);

  if (NULL == name) {
    return NULL;
  }

  const jchar* jchars = env->GetStringChars(name, NULL);
  jsize len = env->GetStringLength(name);
  String8 str(String16(jchars, len));

  // EmojiFactory *factory = gCaller->TryCallGetImplementation(str.string());
  EmojiFactory *factory = EmojiFactory::GetImplementation(str.string());

  env->ReleaseStringChars(name, jchars);

  return create_java_EmojiFactory(env, factory, name);
}

static jobject android_emoji_EmojiFactory_newAvailableInstance(
    JNIEnv* env, jobject clazz) {
  // pthread_once(&g_once, InitializeCaller);

  // EmojiFactory *factory = gCaller->TryCallGetAvailableImplementation();
  EmojiFactory *factory = EmojiFactory::GetAvailableImplementation();
  if (NULL == factory) {
    return NULL;
  }
  String16 name_16(String8(factory->Name()));
  jstring jname = env->NewString(name_16.string(), name_16.size());
  if (NULL == jname) {
    return NULL;
  }

  return create_java_EmojiFactory(env, factory, jname);
}

static jobject android_emoji_EmojiFactory_getBitmapFromAndroidPua(
    JNIEnv* env, jobject clazz, jint nativeEmojiFactory, jint pua) {
  EmojiFactory *factory = reinterpret_cast<EmojiFactory *>(nativeEmojiFactory);

  int size;
  const char *bytes = factory->GetImageBinaryFromAndroidPua(pua, &size);
  if (bytes == NULL) {
    return NULL;
  }

  SkBitmap *bitmap = new SkBitmap;
  if (!SkImageDecoder::DecodeMemory(bytes, size, bitmap)) {
    LOGE("SkImageDecoder::DecodeMemory() failed.");
    return NULL;
  }

  jobject obj = env->AllocObject(gBitmap_class);
  if (obj) {
    env->CallVoidMethod(obj, gBitmap_constructorMethodID,
                        reinterpret_cast<jint>(bitmap), false, NULL);
    if (env->ExceptionCheck() != 0) {
      LOGE("*** Uncaught exception returned from Java call!\n");
      env->ExceptionDescribe();
      return NULL;
    }
  }

  return obj;
}

static void android_emoji_EmojiFactory_destructor(
    JNIEnv* env, jobject obj, jint nativeEmojiFactory) {
  EmojiFactory *factory = reinterpret_cast<EmojiFactory *>(nativeEmojiFactory);
  delete factory;
}

static jint android_emoji_EmojiFactory_getAndroidPuaFromVendorSpecificSjis(
    JNIEnv* env, jobject obj, jint nativeEmojiFactory, jchar sjis) {
  EmojiFactory *factory = reinterpret_cast<EmojiFactory *>(nativeEmojiFactory);
  return factory->GetAndroidPuaFromVendorSpecificSjis(sjis);
}

static jint android_emoji_EmojiFactory_getVendorSpecificSjisFromAndroidPua(
    JNIEnv* env, jobject obj, jint nativeEmojiFactory, jint pua) {
  EmojiFactory *factory = reinterpret_cast<EmojiFactory *>(nativeEmojiFactory);
  return factory->GetVendorSpecificSjisFromAndroidPua(pua);
}

static jint android_emoji_EmojiFactory_getAndroidPuaFromVendorSpecificPua(
    JNIEnv* env, jobject obj, jint nativeEmojiFactory, jint vsu) {
  EmojiFactory *factory = reinterpret_cast<EmojiFactory *>(nativeEmojiFactory);
  return factory->GetAndroidPuaFromVendorSpecificPua(vsu);
}

static jint android_emoji_EmojiFactory_getVendorSpecificPuaFromAndroidPua(
    JNIEnv* env, jobject obj, jint nativeEmojiFactory, jint pua) {
  EmojiFactory *factory = reinterpret_cast<EmojiFactory *>(nativeEmojiFactory);
  return factory->GetVendorSpecificPuaFromAndroidPua(pua);
}

static jint android_emoji_EmojiFactory_getMaximumVendorSpecificPua(
    JNIEnv* env, jobject obj, jint nativeEmojiFactory) {
  EmojiFactory *factory = reinterpret_cast<EmojiFactory *>(nativeEmojiFactory);
  return factory->GetMaximumVendorSpecificPua();
}

static jint android_emoji_EmojiFactory_getMinimumVendorSpecificPua(
    JNIEnv* env, jobject obj, jint nativeEmojiFactory) {
  EmojiFactory *factory = reinterpret_cast<EmojiFactory *>(nativeEmojiFactory);
  return factory->GetMinimumVendorSpecificPua();
}

static jint android_emoji_EmojiFactory_getMaximumAndroidPua(
    JNIEnv* env, jobject obj, jint nativeEmojiFactory) {
  EmojiFactory *factory = reinterpret_cast<EmojiFactory *>(nativeEmojiFactory);
  return factory->GetMaximumAndroidPua();
}

static jint android_emoji_EmojiFactory_getMinimumAndroidPua(
    JNIEnv* env, jobject obj, jint nativeEmojiFactory) {
  EmojiFactory *factory = reinterpret_cast<EmojiFactory *>(nativeEmojiFactory);
  return factory->GetMinimumAndroidPua();
}

static JNINativeMethod gMethods[] = {
  { "newInstance", "(Ljava/lang/String;)Landroid/emoji/EmojiFactory;",
    (void*)android_emoji_EmojiFactory_newInstance},
  { "newAvailableInstance", "()Landroid/emoji/EmojiFactory;",
    (void*)android_emoji_EmojiFactory_newAvailableInstance},
  { "nativeDestructor", "(I)V",
    (void*)android_emoji_EmojiFactory_destructor},
  { "nativeGetBitmapFromAndroidPua", "(II)Landroid/graphics/Bitmap;",
    (void*)android_emoji_EmojiFactory_getBitmapFromAndroidPua},
  { "nativeGetAndroidPuaFromVendorSpecificSjis", "(IC)I",
    (void*)android_emoji_EmojiFactory_getAndroidPuaFromVendorSpecificSjis},
  { "nativeGetVendorSpecificSjisFromAndroidPua", "(II)I",
    (void*)android_emoji_EmojiFactory_getVendorSpecificSjisFromAndroidPua},
  { "nativeGetAndroidPuaFromVendorSpecificPua", "(II)I",
    (void*)android_emoji_EmojiFactory_getAndroidPuaFromVendorSpecificPua},
  { "nativeGetVendorSpecificPuaFromAndroidPua", "(II)I",
    (void*)android_emoji_EmojiFactory_getVendorSpecificPuaFromAndroidPua},
  { "nativeGetMaximumVendorSpecificPua", "(I)I",
    (void*)android_emoji_EmojiFactory_getMaximumVendorSpecificPua},
  { "nativeGetMinimumVendorSpecificPua", "(I)I",
    (void*)android_emoji_EmojiFactory_getMinimumVendorSpecificPua},
  { "nativeGetMaximumAndroidPua", "(I)I",
    (void*)android_emoji_EmojiFactory_getMaximumAndroidPua},
  { "nativeGetMinimumAndroidPua", "(I)I",
    (void*)android_emoji_EmojiFactory_getMinimumAndroidPua}
};

static jclass make_globalref(JNIEnv* env, const char classname[])
{
    jclass c = env->FindClass(classname);
    SkASSERT(c);
    return (jclass)env->NewGlobalRef(c);
}

static jfieldID getFieldIDCheck(JNIEnv* env, jclass clazz,
                                const char fieldname[], const char type[])
{
    jfieldID id = env->GetFieldID(clazz, fieldname, type);
    SkASSERT(id);
    return id;
}

int register_android_emoji_EmojiFactory(JNIEnv* env) {
  gBitmap_class = make_globalref(env, "android/graphics/Bitmap");
  gBitmap_constructorMethodID = env->GetMethodID(gBitmap_class, "<init>",
                                                 "(IZ[B)V");
  gEmojiFactory_class = make_globalref(env, "android/emoji/EmojiFactory");
  gEmojiFactory_constructorMethodID = env->GetMethodID(
      gEmojiFactory_class, "<init>", "(ILjava/lang/String;)V");
  return jniRegisterNativeMethods(env, "android/emoji/EmojiFactory",
                                  gMethods, NELEM(gMethods));
}

}  // namespace android
