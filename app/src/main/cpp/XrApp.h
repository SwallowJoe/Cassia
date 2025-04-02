//
// Created by jh on 2025/3/26.
//

#pragma once

#include "base/include/IOpenXrProgram.h"
#include "base/include/SwapchainInfo.h"
#include "base/include/RenderLayerInfo.h"
#include "base/include/AndroidAppState.h"
#include "xrcommon/GraphicsAPI.h"
#include "xrcommon/GraphicsAPI_Vulkan.h"
#include "pipeline/VkGraphicsPipelineManager.h"

class XrApp : private IOpenXrProgram {
public:
    explicit XrApp(GraphicsAPI_Type type, android_app* app);
    ~XrApp() override;

    void run();

private:
    void createXrInstance() override;
    void destroyXrInstance() override;
    void initializeSystemId() override;
    void initializeDevice() override;
    void createSession() override;
    void destroySession() override;
    void createReferenceSpace() override;
    void destroyReferenceSpace() override;
    void createXrSwapChains() override;
    void destroyXrSwapChains() override;
    void pollXrEvents() override;
    bool renderFrame() override;

private:
    void initializeXrEnvironment();
    void initViewConfigurationViews();
    void selectViewConfiguration();
    void initEnvironmentBlendModes();
    void queryAllSupportXrSwapchainFormats();
    void createXrSwapchain(SwapchainInfo& info, GraphicsAPI::SwapchainType swapchainType,
                           XrSwapchainUsageFlags usageFlags, int64_t format,
                           uint32_t sampleCount, uint32_t width, uint32_t height);
    void releaseResources();
    void createDebugMessenger();
    void destroyDebugMessenger();

    void activeApiLayers();
    void activeInstanceExtensions();
    void createPipelineManager();
    void destroyPipeline();
    bool renderLayers(RenderLayerInfo& renderLayerInfo);

    void pollAndroidSystemEvents();

    void handleXrEventsLost(XrEventDataBuffer* eventData);
    void handleXrInstanceLossPending(XrEventDataBuffer* eventData);
    void handleXrInteractionProfileChanged(XrEventDataBuffer* eventData);
    void handleXrReferenceSpaceChangePending(XrEventDataBuffer* eventData);
    void handleXrSessionStateChanged(XrEventDataBuffer* eventData);
private:
    static void handleAndroidAppCmd(struct android_app* app, int32_t cmd);
private:
    GraphicsAPI_Type mGraphicsApiType;
    android_app* mApp;
    AndroidAppState mAndroidAppState{};

    volatile bool mApplicationRunning = true;
    // TODO:
    volatile bool mSessionRunning = false;

    XrInstance mXrInstance = {};
    std::vector<std::string> mRequestApiLayers = {};
    std::vector<const char*> mActiveApiLayers = {};
    std::vector<std::string> mRequestInstanceExtensions = {};
    std::vector<const char*> mActiveInstanceExtensions = {};

    XrDebugUtilsMessengerEXT  mDebugUtilsMessengerExt = {};

    // SystemID:
    XrFormFactor mFormFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    XrSystemId mSystemId = {};
    XrSystemProperties mXrSystemProperties = {XR_TYPE_SYSTEM_PROPERTIES};

    // ViewConfigurations
    std::vector<XrViewConfigurationType> mAppViewConfigurationTypes = {
            XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
            XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO
    };
    std::vector<XrViewConfigurationType> mViewConfigurationTypes;
    XrViewConfigurationType mViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM;
    std::vector<XrViewConfigurationView> mViewConfigurationViews;

    // BlendMode
    std::vector<XrEnvironmentBlendMode> mAppEnvironmentBlendModes = {
            XR_ENVIRONMENT_BLEND_MODE_OPAQUE,
            XR_ENVIRONMENT_BLEND_MODE_ADDITIVE
    };
    std::vector<XrEnvironmentBlendMode> mEnvironmentBlendModes = {};
    XrEnvironmentBlendMode mEnvironmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_MAX_ENUM;

    // XrSwapChain
    std::vector<int64_t> mXrSwapChainFormats = {};
    std::vector<SwapchainInfo> mColorSwapchainInfos = {};
    std::vector<SwapchainInfo> mDepthSwapchainInfos = {};

    // XrSession
    std::shared_ptr<GraphicsAPI_Vulkan> mGraphicsApi;
    XrSession mSession = XR_NULL_HANDLE;
    XrSessionState mSessionState = XR_SESSION_STATE_UNKNOWN;

    // XrSpace
    XrSpace mLocalSpace = XR_NULL_HANDLE;

    // TODO: hand tracking properties.
    XrSystemHandTrackingPropertiesEXT mXrSystemHandTrackingPropertiesExt = {
            XR_TYPE_SYSTEM_HAND_TRACKING_PROPERTIES_EXT
    };
    struct Hand {
        XrHandJointLocationEXT handJointLocations[XR_HAND_JOINT_COUNT_EXT];
        XrHandTrackerEXT xrHandTrackerExt = 0;
    };
    Hand mHands[2];

    // PipelineManager
    std::unique_ptr<VkGraphicsPipelineManager> mPipelineManager;
};
