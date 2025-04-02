//
// Created by jh on 2025/4/2.
//
#include "VkGraphicsPipelineManager.h"
#include "CubicPipeline.h"

#include <utility>

VkGraphicsPipelineManager::VkGraphicsPipelineManager(
        std::shared_ptr<GraphicsAPI_Vulkan> api,
        AAssetManager *assetManager,
        const std::vector<int64_t>& colorFormats,
        int64_t depthFormat)
    : mGraphicsApi(std::move(api)), mAssetManager(assetManager) {

    mColorFormats.resize(colorFormats.size());
    std::for_each(colorFormats.begin(), colorFormats.end(), [&](int64_t colorFormat)->void {
        mColorFormats.emplace_back(colorFormat);
    });

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
    std::shared_ptr<VkGraphicsPipeline> cubicPipeline = std::make_shared<CubicPipeline>("Cubic", mGraphicsApi, mColorFormats, mDepthFormat);
    mPipelineCache.put(cubicPipeline->getName(), cubicPipeline);
}

void VkGraphicsPipelineManager::renderFrame() {
    mPipelineCache.foreach([](const std::string& name,
                              const std::shared_ptr<VkGraphicsPipeline>& pipeline)->void {
        pipeline->renderFrame();
    });
}
