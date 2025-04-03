//
// Created by jh on 2025/3/26.
//

#define LOG_TAG "XrApp"

#include "XrApp.h"
#include "Log.h"
#include "xrcommon/OpenXRDebugUtils.h"
#include "Color.h"
#include <algorithm>
#include <random>
#include <memory>

// TODO: 放入合适位置
namespace ephemeral {
    PFN_xrCreateHandTrackerEXT xrCreateHandTrackerExt = nullptr;
    PFN_xrDestroyHandTrackerEXT xrDestroyHandTrackerExt = nullptr;
    PFN_xrLocateHandJointsEXT xrLocateHandJointsExt = nullptr;
}

XrApp::XrApp(GraphicsAPI_Type type, android_app *app)
        : mGraphicsApiType(type), mApp(app) {
    app->onAppCmd = XrApp::handleAndroidAppCmd;
    app->userData = &mAndroidAppState;

    // TODO: 改成传入参数配置
    mRequestInstanceExtensions.emplace_back(XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME);
    mRequestInstanceExtensions.emplace_back(XR_EXT_HAND_TRACKING_EXTENSION_NAME);
    mRequestInstanceExtensions.emplace_back(XR_EXT_HAND_INTERACTION_EXTENSION_NAME);
    mRequestInstanceExtensions.emplace_back(XR_KHR_COMPOSITION_LAYER_DEPTH_EXTENSION_NAME);
    mRequestInstanceExtensions.emplace_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);
    mRequestInstanceExtensions.emplace_back(GetGraphicsAPIInstanceExtensionString(mGraphicsApiType));
}

XrApp::~XrApp() {
    LOGI("~XrApp")
}

void XrApp::run() {
    // 1. 初始化XR相关环境
    initializeXrEnvironment();
    // 2. 处理主体逻辑
    while (mApplicationRunning) {
        // 2.1 处理Android系统事件
        pollAndroidSystemEvents();
        // 2.2 处理事件
        pollXrEvents();
        // 2.3 XR渲染
        if (mSessionRunning) {
            renderFrame();
        }
    }
    // 3. 回收资源
    releaseResources();
}

void XrApp::initializeXrEnvironment() {
    LOGI("initializeXrEnvironment");
    // 1. 创建XrInstance
    createXrInstance();
    // *2. 创建Debug工具
    createDebugMessenger();
    // 3. 初始化SystemId
    initializeSystemId();
    // 4. 初始化设备
    initializeDevice();
    // 5. 创建Session
    createSession();
    // 6. 创建ReferenceSpace
    createReferenceSpace();
    // 7. 创建XrSwapChain
    createXrSwapChains();
    // 8. 构建渲染管线
    createPipelineManager();
}

void XrApp::releaseResources() {
    destroyPipeline();
    destroyXrSwapChains();
    destroyReferenceSpace();
    destroySession();
    destroyDebugMessenger();
    destroyXrInstance();
}

void XrApp::createXrInstance() {
    LOGI("createXrInstance");
    // 1. 首先构建XrApplicationInfo
    XrApplicationInfo applicationInfo;
    strncpy(applicationInfo.applicationName, "XrGraphics", XR_MAX_APPLICATION_NAME_SIZE);
    applicationInfo.applicationVersion = 1;
    strncpy(applicationInfo.engineName, "OpenXR Engine", XR_MAX_ENGINE_NAME_SIZE);
    applicationInfo.engineVersion = 1;
    applicationInfo.apiVersion = XR_CURRENT_API_VERSION;

    // 2. 构建XrInstanceCreateInfo
    XrInstanceCreateInfo instanceCreateInfo {XR_TYPE_INSTANCE_CREATE_INFO};
    instanceCreateInfo.createFlags = 0;
    instanceCreateInfo.applicationInfo = applicationInfo;
    // 2.1 处理激活的ApiLayer
    activeApiLayers();
    instanceCreateInfo.enabledApiLayerCount = mActiveApiLayers.size();
    instanceCreateInfo.enabledApiLayerNames = mActiveApiLayers.data();
    // 2.2 处理激活的instance扩展
    activeInstanceExtensions();
    instanceCreateInfo.enabledExtensionCount = mActiveInstanceExtensions.size();
    instanceCreateInfo.enabledExtensionNames = mActiveInstanceExtensions.data();
    // 2.3 构建XrInstanceCreateInfoAndroidKHR, 绑定vm和activity
    XrInstanceCreateInfoAndroidKHR xrInstanceCreateInfoAndroidKhr {XR_TYPE_INSTANCE_CREATE_INFO_ANDROID_KHR};
    xrInstanceCreateInfoAndroidKhr.applicationVM = mApp->activity->vm;
    xrInstanceCreateInfoAndroidKhr.applicationActivity = mApp->activity->clazz;
    instanceCreateInfo.next = (XrBaseInStructure*) &xrInstanceCreateInfoAndroidKhr;

    // 3. 创建XrInstance
    OPENXR_CHECK(xrCreateInstance(&instanceCreateInfo, &mXrInstance),
                 "Failed to create XrInstance!");

    // 4. Hand插件
    OPENXR_CHECK(xrGetInstanceProcAddr(mXrInstance,
                                       "xrCreateHandTrackerEXT",
                                       (PFN_xrVoidFunction* )&ephemeral::xrCreateHandTrackerExt),
                 "Failed to get xrCreateHandTrackerEXT.");
    OPENXR_CHECK(xrGetInstanceProcAddr(mXrInstance,
                                       "xrDestroyHandTrackerEXT",
                                       (PFN_xrVoidFunction* )&ephemeral::xrDestroyHandTrackerExt),
                 "Failed to get xrDestroyHandTrackerEXT.");
    OPENXR_CHECK(xrGetInstanceProcAddr(mXrInstance,
                                       "xrLocateHandJointsEXT",
                                       (PFN_xrVoidFunction* )&ephemeral::xrLocateHandJointsExt),
                 "Failed to get xrLocateHandJointsEXT.");

}


void XrApp::destroyXrInstance() {
    OPENXR_CHECK(xrDestroyInstance(mXrInstance), "Failed to destroy instance!");
}

void XrApp::initializeSystemId() {
    LOGI("initializeSystemId");
    XrSystemGetInfo systemGetInfo{XR_TYPE_SYSTEM_GET_INFO};
    systemGetInfo.formFactor = mFormFactor;

    OPENXR_CHECK(xrGetSystem(mXrInstance, &systemGetInfo, &mSystemId),
                 "Failed to get SystemId.");

    mXrSystemProperties.next = &mXrSystemHandTrackingPropertiesExt;
    OPENXR_CHECK(xrGetSystemProperties(mXrInstance, mSystemId, &mXrSystemProperties),
                 "Failed to get system properties.")
}

void XrApp::initializeDevice() {
    LOGI("initializeDevice");
    initViewConfigurationViews();
    initEnvironmentBlendModes();
}

void XrApp::initViewConfigurationViews() {
    // 1. 根据请求的ViewConfigurationType找到本机支持的所有ViewConfigurationType
    uint32_t viewConfigurationCount = 0;
    OPENXR_CHECK(xrEnumerateViewConfigurations(
            mXrInstance, mSystemId,
            0, &viewConfigurationCount,
            nullptr), "Failed to enumerate view configurations!");
    mViewConfigurationTypes.resize(viewConfigurationCount);
    OPENXR_CHECK(xrEnumerateViewConfigurations(
                    mXrInstance, mSystemId,
                    viewConfigurationCount, &viewConfigurationCount,
                    mViewConfigurationTypes.data()), "Failed to enumerate view configurations!");
    // 2. 选择合适的ViewConfigurationType
    selectViewConfiguration();

    // 3. 初始化对应的ViewConfigurationView
    uint32_t viewConfigurationViewCount = 0;
    OPENXR_CHECK(xrEnumerateViewConfigurationViews(
            mXrInstance, mSystemId, mViewConfigurationType,
            0, &viewConfigurationViewCount, nullptr
            ), "Failed to enumerate view configuration views.");
    // 注意这里需要初始化带类型，否则会报错。
    mViewConfigurationViews.resize(viewConfigurationViewCount, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
    OPENXR_CHECK(xrEnumerateViewConfigurationViews(
            mXrInstance, mSystemId, mViewConfigurationType,
            viewConfigurationViewCount, &viewConfigurationViewCount,
            mViewConfigurationViews.data()
    ), "Failed to enumerate view configuration views!");
}

void XrApp::selectViewConfiguration() {
    // 找第一个支持的ViewConfigurationType
    for (const XrViewConfigurationType &viewConfigurationType: mAppViewConfigurationTypes) {
        if (std::find(mViewConfigurationTypes.begin(), mViewConfigurationTypes.end(), viewConfigurationType) != mViewConfigurationTypes.end()) {
            mViewConfigurationType = viewConfigurationType;
            break;
        }
    }
    if (mViewConfigurationType == XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM) {
        LOGW("Failed to find a view configuration type. Defaulting to XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO.")
        mViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    }

    // LOGI("selectViewConfiguration " << mViewConfigurationType);
}

void XrApp::initEnvironmentBlendModes() {
    // 1. 找到本机支持所有EnvironmentBlendMode
    uint32_t environmentBlendModeCount = 0;
    OPENXR_CHECK(xrEnumerateEnvironmentBlendModes(
            mXrInstance, mSystemId, mViewConfigurationType,
            0, &environmentBlendModeCount,
            nullptr), "Failed to enumerate environment blend mode");
    mEnvironmentBlendModes.resize(environmentBlendModeCount);
    OPENXR_CHECK(xrEnumerateEnvironmentBlendModes(
            mXrInstance, mSystemId, mViewConfigurationType,
            environmentBlendModeCount, &environmentBlendModeCount,
            mEnvironmentBlendModes.data()), "Failed to enumerate environment blend mode");

    // 2. 选择合适的EnvironmentBlendMode
    for (const XrEnvironmentBlendMode &mode: mAppEnvironmentBlendModes) {
        if (std::find(mEnvironmentBlendModes.begin(), mEnvironmentBlendModes.end(), mode) !=
            mEnvironmentBlendModes.end()) {
            mEnvironmentBlendMode = mode;
            break;
        }
    }
    if (mEnvironmentBlendMode == XR_ENVIRONMENT_BLEND_MODE_MAX_ENUM) {
        LOGW("Failed to find a environment blend mode. Defaulting to XR_ENVIRONMENT_BLEND_MODE_OPAQUE.")
        mEnvironmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
    }
}

void XrApp::createSession() {
    LOGI("createSession");
    XrSessionCreateInfo info {XR_TYPE_SESSION_CREATE_INFO};
    mGraphicsApi = std::make_shared<GraphicsAPI_Vulkan>(mXrInstance, mSystemId);
    info.next = mGraphicsApi->GetGraphicsBinding();
    info.createFlags = 0;
    info.systemId = mSystemId;

    OPENXR_CHECK(xrCreateSession(mXrInstance, &info, &mSession),
                 "Failed to create session!");
}

void XrApp::destroySession() {
    OPENXR_CHECK(xrDestroySession(mSession), "Failed to destroy session!");
}

void XrApp::createReferenceSpace() {
    LOGI("createReferenceSpace");
    XrReferenceSpaceCreateInfo info {XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
    info.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
    info.poseInReferenceSpace = {
            {0.0f, 0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 0.0f},
    };

    OPENXR_CHECK(xrCreateReferenceSpace(mSession, &info, &mLocalSpace),
                 "Failed to create reference space!")
}

void XrApp::destroyReferenceSpace() {
    OPENXR_CHECK(xrDestroySpace(mLocalSpace), "Failed to destroy reference space!")
}

void XrApp::createXrSwapChains() {
    LOGI("createXrSwapChains");
    // 1. 获取本机支持的所有交换链格式
    queryAllSupportXrSwapchainFormats();
    // 2. 获取Vulkan选择的交换链格式
    int64_t colorSwapchainFormat = mGraphicsApi->SelectColorSwapchainFormat(mXrSwapChainFormats);
    int64_t depthSwapchianFormat = mGraphicsApi->SelectDepthSwapchainFormat(mXrSwapChainFormats);
    // 3. 为每一个ViewConfigurationView创建对应的color和depth交换链
    mColorSwapchainInfos.resize(mViewConfigurationViews.size());
    mDepthSwapchainInfos.resize(mViewConfigurationViews.size());
    for (size_t i = 0; i < mViewConfigurationViews.size(); i++) {
        auto &colorSwapchainInfo = mColorSwapchainInfos[i];
        createXrSwapchain(colorSwapchainInfo, GraphicsAPI::SwapchainType::COLOR,
                           (XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT),
                           colorSwapchainFormat,
                           mViewConfigurationViews[i].recommendedSwapchainSampleCount,
                           mViewConfigurationViews[i].recommendedImageRectWidth,
                           mViewConfigurationViews[i].recommendedImageRectHeight);

        auto &depthSwapchianInfo = mDepthSwapchainInfos[i];
        createXrSwapchain(depthSwapchianInfo, GraphicsAPI::SwapchainType::DEPTH,
                          (XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT),
                          depthSwapchianFormat,
                          mViewConfigurationViews[i].recommendedSwapchainSampleCount,
                          mViewConfigurationViews[i].recommendedImageRectWidth,
                          mViewConfigurationViews[i].recommendedImageRectHeight);
    }
}

void XrApp::queryAllSupportXrSwapchainFormats() {
    uint32_t formatCount = 0;
    OPENXR_CHECK(xrEnumerateSwapchainFormats(mSession, 0, &formatCount, nullptr),
                 "Failed to enumerate swapchain formats!");
    mXrSwapChainFormats.resize(formatCount);
    OPENXR_CHECK(xrEnumerateSwapchainFormats(mSession, formatCount,
                                             &formatCount, mXrSwapChainFormats.data()),
                 "Failed to enumerate swapchain formats!");
}

void XrApp::createXrSwapchain(SwapchainInfo& info, GraphicsAPI::SwapchainType swapchainType,
                              XrSwapchainUsageFlags usageFlags, int64_t format,
                              uint32_t sampleCount, uint32_t width, uint32_t height) {

    XrSwapchainCreateInfo swapchainCreateInfo {XR_TYPE_SWAPCHAIN_CREATE_INFO};
    swapchainCreateInfo.createFlags = 0;
    swapchainCreateInfo.usageFlags = usageFlags;
    swapchainCreateInfo.format = format;
    swapchainCreateInfo.sampleCount = sampleCount;
    swapchainCreateInfo.width = width;
    swapchainCreateInfo.height = height;
    swapchainCreateInfo.faceCount = 1;
    swapchainCreateInfo.arraySize = 1;
    swapchainCreateInfo.mipCount = 1;
    OPENXR_CHECK(xrCreateSwapchain(mSession, &swapchainCreateInfo, &info.xrSwapchain),
                 "Failed to create swap chain!");

    uint32_t swapchainImageCount = 0;
    OPENXR_CHECK(xrEnumerateSwapchainImages(info.xrSwapchain, 0, &swapchainImageCount, nullptr),
                 "Failed to enumerate swap chain images!");
    XrSwapchainImageBaseHeader *swapchainImageBaseHeader =
            mGraphicsApi->AllocateSwapchainImageData(info.xrSwapchain,
                                                     swapchainType,
                                                     swapchainImageCount);
    OPENXR_CHECK(xrEnumerateSwapchainImages(info.xrSwapchain,
                                            swapchainImageCount,
                                            &swapchainImageCount,
                                            swapchainImageBaseHeader),
                 "Failed to enumerate swap chain images!");

    auto imageViewType = GraphicsAPI::ImageViewCreateInfo::Type::RTV;
    auto imageViewView = GraphicsAPI::ImageViewCreateInfo::View::TYPE_2D;
    auto imageAspect = GraphicsAPI::ImageViewCreateInfo::Aspect::COLOR_BIT;

    if (swapchainType == GraphicsAPI::SwapchainType::DEPTH) {
        imageViewType = GraphicsAPI::ImageViewCreateInfo::Type::DSV;
        imageAspect = GraphicsAPI::ImageViewCreateInfo::Aspect::DEPTH_BIT;
    }

    for (auto index = 0; index < swapchainImageCount; index++) {
        GraphicsAPI::ImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.image = mGraphicsApi->GetSwapchainImage(info.xrSwapchain, index);
        imageViewCreateInfo.type = imageViewType;
        imageViewCreateInfo.view = imageViewView;
        imageViewCreateInfo.format = info.xrSwapchainFormat;
        imageViewCreateInfo.aspect = imageAspect;
        imageViewCreateInfo.baseMipLevel = 0;
        imageViewCreateInfo.levelCount = 1;
        imageViewCreateInfo.baseArrayLayer = 0;
        imageViewCreateInfo.layerCount = 1;
        info.imageViews.push_back(mGraphicsApi->CreateImageView(imageViewCreateInfo));
    }
}

void XrApp::destroyXrSwapChains() {
    for (auto index = 0; index < mViewConfigurationViews.size(); index++) {
        for (auto &imageView: mColorSwapchainInfos[index].imageViews) {
            mGraphicsApi->DestroyImageView(imageView);
        }
        for (auto &imageView : mDepthSwapchainInfos[index].imageViews) {
            mGraphicsApi->DestroyImageView(imageView);
        }

        mGraphicsApi->FreeSwapchainImageData(mColorSwapchainInfos[index].xrSwapchain);
        mGraphicsApi->FreeSwapchainImageData(mDepthSwapchainInfos[index].xrSwapchain);

        OPENXR_CHECK(xrDestroySwapchain(mColorSwapchainInfos[index].xrSwapchain),
                     "Failed to destroy swap chain!");
        OPENXR_CHECK(xrDestroySwapchain(mDepthSwapchainInfos[index].xrSwapchain),
                     "Failed to destroy swap chain!");
    }
}

bool XrApp::renderFrame() {
    // LOGI("renderFrame");
    // 1. 获取xr渲染帧信息
    XrFrameState frameState{XR_TYPE_FRAME_STATE};
    XrFrameWaitInfo frameWaitInfo{XR_TYPE_FRAME_WAIT_INFO};
    OPENXR_CHECK(xrWaitFrame(mSession, &frameWaitInfo, &frameState), "Failed to wait frame!");

    // 2. 告知Xr合成器应用开始绘制帧
    XrFrameBeginInfo frameBeginInfo{XR_TYPE_FRAME_BEGIN_INFO};
    OPENXR_CHECK(xrBeginFrame(mSession, &frameBeginInfo), "Failed to begin frame!");

    // 3. 绘制layer
    RenderLayerInfo renderLayerInfo;
    renderLayerInfo.predictedDisplayTime = frameState.predictedDisplayTime;
    bool validateSessionState = (mSessionState == XR_SESSION_STATE_SYNCHRONIZED
            || mSessionState == XR_SESSION_STATE_VISIBLE
            || mSessionState == XR_SESSION_STATE_FOCUSED);
    if (validateSessionState && frameState.shouldRender) {
        renderLayers(renderLayerInfo);
    }

    // 4. 告知Xr合成器此帧绘制完毕, 可以合成上屏了
    XrFrameEndInfo frameEndInfo{XR_TYPE_FRAME_END_INFO};
    frameEndInfo.displayTime = frameState.predictedDisplayTime;
    frameEndInfo.environmentBlendMode = mEnvironmentBlendMode;
    frameEndInfo.layerCount = static_cast<uint32_t>(renderLayerInfo.compositionLayers.size());
    frameEndInfo.layers = renderLayerInfo.compositionLayers.data();
    OPENXR_CHECK(xrEndFrame(mSession, &frameEndInfo), "Failed to end frame!");
    return true;
}

bool XrApp::renderLayers(RenderLayerInfo& renderLayerInfo) {
    // LOGI("renderLayers");
    std::vector<XrView> xrViews(mViewConfigurationViews.size(), {XR_TYPE_VIEW});

    XrViewState xrViewState{XR_TYPE_VIEW_STATE};
    XrViewLocateInfo xrViewLocateInfo{XR_TYPE_VIEW_LOCATE_INFO};
    xrViewLocateInfo.displayTime = renderLayerInfo.predictedDisplayTime;
    xrViewLocateInfo.viewConfigurationType = mViewConfigurationType;
    xrViewLocateInfo.space = mLocalSpace;

    uint32_t viewCount = 0;
    XrResult xrLocateViewsResult = xrLocateViews(mSession, &xrViewLocateInfo, &xrViewState,
                                    static_cast<uint32_t>(xrViews.size()), &viewCount,
                                    xrViews.data());
    if (xrLocateViewsResult != XR_SUCCESS) {
        LOGE("renderLayers failed to locate views!")
        return false;
    }

    renderLayerInfo.compositionLayerProjectionViews.resize(viewCount,
                                                           {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW});
    renderLayerInfo.compositionLayerDepthInfos.resize(viewCount,
                                                      {XR_TYPE_COMPOSITION_LAYER_DEPTH_INFO_KHR});

    for (uint32_t viewIndex = 0; viewIndex < viewCount; viewIndex++) {
        XrSwapchainImageAcquireInfo xrSwapchainImageAcquireInfo{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};
        // 处理颜色
        uint32_t colorImageIndex = 0;
        OPENXR_CHECK(xrAcquireSwapchainImage(
                mColorSwapchainInfos[viewIndex].xrSwapchain,
                &xrSwapchainImageAcquireInfo,
                &colorImageIndex), "Failed to acquire swap chain image for color!");
        // 处理深度
        uint32_t depthImageIndex = 0;
        OPENXR_CHECK(xrAcquireSwapchainImage(
                mDepthSwapchainInfos[viewIndex].xrSwapchain,
                &xrSwapchainImageAcquireInfo,
                &depthImageIndex), "Failed to acquire swap chain image for depth!");

        XrSwapchainImageWaitInfo xrSwapchainImageWaitInfo{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};
        xrSwapchainImageWaitInfo.timeout = XR_INFINITE_DURATION;
        OPENXR_CHECK(xrWaitSwapchainImage(mColorSwapchainInfos[viewIndex].xrSwapchain,
                                          &xrSwapchainImageWaitInfo),
                     "Failed to wait swap chain image for color!");
        OPENXR_CHECK(xrWaitSwapchainImage(mDepthSwapchainInfos[viewIndex].xrSwapchain,
                                          &xrSwapchainImageWaitInfo),
                     "Failed to wait swap chain image for depth!");

        const uint32_t width = mViewConfigurationViews[viewIndex].recommendedImageRectWidth;
        const uint32_t height = mViewConfigurationViews[viewIndex].recommendedImageRectHeight;

        // TODO: move to pipeline.
        GraphicsAPI::Viewport viewport = {
                0.0f, 0.0f,
                (float)width, (float)height,
                0.0f, 1.0f
        };
        GraphicsAPI::Rect2D scissor = {
                {(int32_t)0, (int32_t)0},
                {width, height}
        };
        float nearZ = 0.05f;
        float farZ = 100.0f;

        renderLayerInfo.compositionLayerProjectionViews[viewIndex] = {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW};
        renderLayerInfo.compositionLayerProjectionViews[viewIndex].pose = xrViews[viewIndex].pose;
        renderLayerInfo.compositionLayerProjectionViews[viewIndex].fov = xrViews[viewIndex].fov;
        renderLayerInfo.compositionLayerProjectionViews[viewIndex].subImage.swapchain = mColorSwapchainInfos[viewIndex].xrSwapchain;
        renderLayerInfo.compositionLayerProjectionViews[viewIndex].subImage.imageRect.offset.x = 0;
        renderLayerInfo.compositionLayerProjectionViews[viewIndex].subImage.imageRect.offset.y = 0;
        renderLayerInfo.compositionLayerProjectionViews[viewIndex].subImage.imageRect.extent.width = static_cast<int32_t>(width);
        renderLayerInfo.compositionLayerProjectionViews[viewIndex].subImage.imageRect.extent.height = static_cast<int32_t>(height);
        renderLayerInfo.compositionLayerProjectionViews[viewIndex].subImage.imageArrayIndex = 0;

        renderLayerInfo.compositionLayerProjectionViews[viewIndex].next = &renderLayerInfo.compositionLayerDepthInfos[viewIndex];

        renderLayerInfo.compositionLayerDepthInfos[viewIndex] = {XR_TYPE_COMPOSITION_LAYER_DEPTH_INFO_KHR};
        renderLayerInfo.compositionLayerDepthInfos[viewIndex].subImage.swapchain = mDepthSwapchainInfos[viewIndex].xrSwapchain;
        renderLayerInfo.compositionLayerDepthInfos[viewIndex].subImage.imageRect.offset.x = 0;
        renderLayerInfo.compositionLayerDepthInfos[viewIndex].subImage.imageRect.offset.y = 0;
        renderLayerInfo.compositionLayerDepthInfos[viewIndex].subImage.imageRect.extent.width = static_cast<int32_t>(width);
        renderLayerInfo.compositionLayerDepthInfos[viewIndex].subImage.imageRect.extent.height = static_cast<int32_t>(height);
        renderLayerInfo.compositionLayerDepthInfos[viewIndex].minDepth = viewport.minDepth;
        renderLayerInfo.compositionLayerDepthInfos[viewIndex].maxDepth = viewport.maxDepth;
        renderLayerInfo.compositionLayerDepthInfos[viewIndex].nearZ = nearZ;
        renderLayerInfo.compositionLayerDepthInfos[viewIndex].farZ = farZ;

        mGraphicsApi->BeginRendering();

        if (mEnvironmentBlendMode == XR_ENVIRONMENT_BLEND_MODE_OPAQUE) {
            // VR Mode
            Color backgroundColor{0.17f, 0.17f, 0.17f, 1.0f};
            mGraphicsApi->ClearColor(mColorSwapchainInfos[viewIndex].imageViews[colorImageIndex],
                                     EXPAND_RGBA(backgroundColor));
        } else {
            // AR Mode
            Color backgroundColor{0.01f, 0.01f, 0.01f, 1.0f};
            mGraphicsApi->ClearColor(mColorSwapchainInfos[viewIndex].imageViews[colorImageIndex],
                                     EXPAND_RGBA(backgroundColor));
        }

        mGraphicsApi->ClearDepth(mDepthSwapchainInfos[viewIndex].imageViews[depthImageIndex], 1.0f);

        /* TODO: fix crash.
        mGraphicsApi->SetRenderAttachments(
                &mColorSwapchainInfos[viewIndex].imageViews[colorImageIndex],
                1,
                mDepthSwapchainInfos[viewIndex].imageViews[depthImageIndex],
                width, height, mPipelineManager->getPipeline());
        */
        mGraphicsApi->SetViewports(&viewport, 1);
        mGraphicsApi->SetScissors(&scissor, 1);

        // TODO: compute the view-projection transform.

        mPipelineManager->renderFrame();
        mGraphicsApi->EndRendering();

        // submit XrSwapchian
        XrSwapchainImageReleaseInfo xrSwapchainImageReleaseInfo{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
        OPENXR_CHECK(xrReleaseSwapchainImage(
                mColorSwapchainInfos[viewIndex].xrSwapchain,
                &xrSwapchainImageReleaseInfo),
                     "Failed to release color swap chain image!");

        OPENXR_CHECK(xrReleaseSwapchainImage(
                mDepthSwapchainInfos[viewIndex].xrSwapchain,
                &xrSwapchainImageReleaseInfo),
                     "Failed to release depth swap chain image!");
    }

    renderLayerInfo.compositionLayerProjection.layerFlags =
            XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT |
            XR_COMPOSITION_LAYER_CORRECT_CHROMATIC_ABERRATION_BIT;
    renderLayerInfo.compositionLayerProjection.space = mLocalSpace;
    renderLayerInfo.compositionLayerProjection.viewCount = static_cast<uint32_t>(renderLayerInfo.compositionLayerProjectionViews.size());
    renderLayerInfo.compositionLayerProjection.views = renderLayerInfo.compositionLayerProjectionViews.data();
    return true;
}

void XrApp::handleAndroidAppCmd(struct android_app *app, int32_t cmd) {
    auto *appState = (AndroidAppState*)app->userData;
    switch (cmd) {
        case APP_CMD_START: {
            LOGI("activity start.");
        } break;
        case APP_CMD_RESUME: {
            LOGI("activity resume.");
            appState->resumed = true;
        } break;
        case APP_CMD_PAUSE: {
            LOGI("activity pause.");
            appState->resumed = false;
        } break;
        case APP_CMD_STOP: {
            LOGI("activity stop.");
        } break;
        case APP_CMD_DESTROY: {
            LOGI("activity destroy.");
            appState->nativeWindow = nullptr;
        } break;
        case APP_CMD_INIT_WINDOW: {
            LOGI("activity init window.");
            appState->nativeWindow = app->window;
        } break;
        case APP_CMD_TERM_WINDOW: {
            LOGI("activity term window.");
            appState->nativeWindow = nullptr;
        } break;
        default:
            break;
    }
}

void XrApp::createDebugMessenger() {
    if (IsStringInVector(mActiveInstanceExtensions, XR_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
        mDebugUtilsMessengerExt = CreateOpenXRDebugUtilsMessenger(mXrInstance);
    }
}

void XrApp::destroyDebugMessenger() {
    if (mDebugUtilsMessengerExt != XR_NULL_HANDLE) {
        DestroyOpenXRDebugUtilsMessenger(mXrInstance, mDebugUtilsMessengerExt);
    }
}

void XrApp::activeApiLayers() {
    uint32_t apiLayerCount = 0;
    std::vector<XrApiLayerProperties> apiLayerProperties;
    OPENXR_CHECK(xrEnumerateApiLayerProperties(0, &apiLayerCount, nullptr),
                 "Failed to count api layer properties.");
    apiLayerProperties.resize(apiLayerCount, {XR_TYPE_API_LAYER_PROPERTIES});
    OPENXR_CHECK(xrEnumerateApiLayerProperties(apiLayerCount, &apiLayerCount, apiLayerProperties.data()),
                 "Failed to enumerate api layer properties.");

    for (auto &requestApiLayer: mRequestApiLayers) {
        for (auto &layerProperty: apiLayerProperties) {
            if (strcmp(requestApiLayer.c_str(), layerProperty.layerName) != 0) {
                continue;
            } else {
                mActiveApiLayers.emplace_back(requestApiLayer.c_str());
                break;
            }
        }
    }
}

void XrApp::activeInstanceExtensions() {
    uint32_t extensionCount = 0;
    std::vector<XrExtensionProperties> extensionProperties;
    OPENXR_CHECK(xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr),
                 "Failed to count instance extension properties.");
    extensionProperties.resize(extensionCount, {XR_TYPE_EXTENSION_PROPERTIES});
    OPENXR_CHECK(xrEnumerateInstanceExtensionProperties(nullptr, extensionCount, &extensionCount,
                                                        extensionProperties.data()),
                 "Failed to enumerate instance extension properties.");

    for (auto &requestedInstanceExtension: mRequestInstanceExtensions) {
        bool found = false;
        for (auto & extensionProperty: extensionProperties) {
            if (strcmp(requestedInstanceExtension.c_str(), extensionProperty.extensionName) != 0) {
                continue;
            } else {
                mActiveInstanceExtensions.emplace_back(requestedInstanceExtension.c_str());
                found = true;
                break;
            }
        }
        if (!found) {
            LOGE("Failed to find OpenXR instance extension: " << requestedInstanceExtension);
        }
    }
}

void XrApp::pollAndroidSystemEvents() {
    // LOGI("pollAndroidSystemEvents");
    // 1. 首先检查Activity是否被销毁了
    if (mApp->destroyRequested != 0) {
        mApplicationRunning = false;
        return;
    }
    // 2. 处理Android OS System事件
    while (true) {
        struct android_poll_source *source = nullptr;
        int events = 0;
        const int timeout = (!mAndroidAppState.resumed && !mSessionRunning &&
                             mApp->destroyRequested == 0) ? -1 : 0;
        // LOGI("pollAndroidSystemEvents timeout=" << timeout);
        if (ALooper_pollAll(timeout, nullptr, &events, (void **) &source) >= 0) {
            if (source != nullptr) {
                source->process(mApp, source);
            }
        } else {
            break;
        }
    }
}

void XrApp::pollXrEvents() {
    // LOGI("pollXrEvents");
    // 1. 获取XR事件
    XrEventDataBuffer eventData;
    auto xrPollEvents = [&]() -> bool {
        eventData = {XR_TYPE_EVENT_DATA_BUFFER};
        return xrPollEvent(mXrInstance, &eventData) == XR_SUCCESS;
    };
    // 2. 处理获取的XR事件
    while (xrPollEvents()) {
        switch (eventData.type) {
            case XR_TYPE_EVENT_DATA_EVENTS_LOST: {
                handleXrEventsLost(&eventData);
                break;
            }
            case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING: {
                handleXrInstanceLossPending(&eventData);
                break;
            }
            case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED: {
                handleXrInteractionProfileChanged(&eventData);
                break;
            }
            case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING: {
                handleXrReferenceSpaceChangePending(&eventData);
                break;
            }
            case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
                handleXrSessionStateChanged(&eventData);
                break;
            }
            default:break;
        }
    }
}

void XrApp::handleXrEventsLost(XrEventDataBuffer* eventData) {
    auto* eventsLost = reinterpret_cast<XrEventDataEventsLost*>(eventData);
    LOGI("handleXrEventsLost: " << eventsLost->lostEventCount);
}

void XrApp::handleXrInstanceLossPending(XrEventDataBuffer* eventData) {
    auto* instanceLossPending = reinterpret_cast<XrEventDataInstanceLossPending*>(eventData);
    LOGI("handleXrInstanceLossPending: " << instanceLossPending->lossTime);
    mSessionRunning = false;
    mApplicationRunning = false;
}

void XrApp::handleXrInteractionProfileChanged(XrEventDataBuffer* eventData) {
    auto* profileChanged = reinterpret_cast<XrEventDataInteractionProfileChanged*>(eventData);
    LOGI("handleXrInteractionProfileChanged for session: " << profileChanged->session);
    if (profileChanged->session != mSession) {
        return;
    }
    // recordCurrentBindings();
}

void XrApp::handleXrReferenceSpaceChangePending(XrEventDataBuffer* eventData) {
    auto* referenceSpaceChangePending = reinterpret_cast<XrEventDataReferenceSpaceChangePending*>(eventData);
    LOGI("handleXrReferenceSpaceChangePending for session: " << referenceSpaceChangePending->session);
    if (referenceSpaceChangePending->session != mSession) {
        return;
    }
    // TODO:
}

void XrApp::handleXrSessionStateChanged(XrEventDataBuffer* eventData) {
    auto* sessionStateChanged = reinterpret_cast<XrEventDataSessionStateChanged*>(eventData);
    if (sessionStateChanged->session != mSession) {
        return;
    }
    switch (sessionStateChanged->state) {
        case XR_SESSION_STATE_READY: {
            XrSessionBeginInfo sessionBeginInfo {XR_TYPE_SESSION_BEGIN_INFO};
            sessionBeginInfo.primaryViewConfigurationType = mViewConfigurationType;
            OPENXR_CHECK(xrBeginSession(mSession, &sessionBeginInfo), "Failed to begin session!");
            mSessionRunning = true;
        } break;
        case XR_SESSION_STATE_STOPPING: {
            OPENXR_CHECK(xrEndSession(mSession), "Failed to end session!");
            mSessionRunning = false;
        } break;
        case XR_SESSION_STATE_EXITING: {
            mSessionRunning = false;
            mApplicationRunning = false;
        } break;
        case XR_SESSION_STATE_LOSS_PENDING: {
            // Session is loss pending. 可能是需要重启XrInstance或者XrSession，简单退出先。
            mSessionRunning = false;
            mApplicationRunning = false;
        } break;
        default:
            break;
    }
    LOGI("handleXrSessionStateChanged from: " << mSessionState << " to " << sessionStateChanged->state);
    mSessionState = sessionStateChanged->state;
}

void XrApp::createPipelineManager() {
    LOGI("createPipelineManager");
    // std::vector<int64_t> colorFormat = {mColorSwapchainInfos[0].xrSwapchainFormat};
    mPipelineManager = std::make_unique<VkGraphicsPipelineManager>(mGraphicsApi,
                                                                   mApp->activity->assetManager,
                                                                   mColorSwapchainInfos[0].xrSwapchainFormat,
                                                                   mDepthSwapchainInfos[0].xrSwapchainFormat);
    mPipelineManager->prepare();
}

void XrApp::destroyPipeline() {
    mPipelineManager->destroy();
}
