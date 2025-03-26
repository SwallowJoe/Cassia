
#include <android_native_app_glue.h>
// OpenXR Headers
#include "xrcommon/DebugOutput.h"
#include "xrcommon/GraphicsAPI_Vulkan.h"
#include "xrcommon/OpenXRDebugUtils.h"

void android_main(struct android_app *app) {
    JNIEnv* env;
    app->activity->vm->AttachCurrentThread(&env, nullptr);

    XrInstance mXrInstance = XR_NULL_HANDLE;
    PFN_xrInitializeLoaderKHR xrInitializeLoaderKhr = nullptr;
    OPENXR_CHECK(xrGetInstanceProcAddr(XR_NULL_HANDLE,
                                       "xrInitializeLoaderKHR",
                                       (PFN_xrVoidFunction*) &xrInitializeLoaderKhr),
                 "Failed to get InstanceProcAddr for xrInitializeLoaderKHR");
    if (!xrInitializeLoaderKhr) {
        XR_LOG("xrInitializeLoaderKhr failed!")
        return;
    }

    XrLoaderInitInfoAndroidKHR loaderInitInfoAndroidKhr{XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR};
    loaderInitInfoAndroidKhr.applicationVM = app->activity->vm;
    loaderInitInfoAndroidKhr.applicationContext = app->activity->clazz;

    OPENXR_CHECK(xrInitializeLoaderKhr((XrLoaderInitInfoBaseHeaderKHR *) &loaderInitInfoAndroidKhr), "Failed to initialized loader for android!");

    // TODO: enter openxr.
}