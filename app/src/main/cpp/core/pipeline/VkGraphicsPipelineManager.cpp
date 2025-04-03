//
// Created by jh on 2025/4/2.
//
#include "VkGraphicsPipelineManager.h"
#include "CubicPipeline.h"
#include "TrianglePipeline.h"

#include <utility>

VkGraphicsPipelineManager::VkGraphicsPipelineManager(
        std::shared_ptr<GraphicsAPI_Vulkan> api,
        AAssetManager *assetManager,
        int64_t colorFormat,
        int64_t depthFormat)
    : mGraphicsApi(std::move(api)), mAssetManager(assetManager) {

    mColorFormat = colorFormat;
    mDepthFormat = depthFormat;
}

VkGraphicsPipelineManager::~VkGraphicsPipelineManager() {
    mPipelineCache.clear();
}

void VkGraphicsPipelineManager::destroy() {
    mPipelineCache.foreach([](const std::string& name,
            const std::shared_ptr<VkGraphicsPipeline>& pipeline)->void {
        pipeline->destroy();
    });
}

void VkGraphicsPipelineManager::prepare() {
    std::shared_ptr<VkGraphicsPipeline> triangle =
            std::make_shared<TrianglePipeline>("Triangle",
                                            mGraphicsApi,
                                            mColorFormat,
                                            mDepthFormat);
    triangle->prepare();
    mPipelineCache.put(triangle->getName(), triangle);
}

void VkGraphicsPipelineManager::renderFrame() {
    mPipelineCache.foreach([](const std::string& name,
                              const std::shared_ptr<VkGraphicsPipeline>& pipeline)->void {
        pipeline->renderFrame();
    });
}
