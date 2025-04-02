//
// Created by jh on 2025/4/2.
//

#pragma once

#include "GraphicsAPI_Vulkan.h"
#include "LRUCache.h"
#include "VkGraphicsPipeline.h"
#include <algorithm>

class VkGraphicsPipelineManager {
public:
    explicit VkGraphicsPipelineManager(std::shared_ptr<GraphicsAPI_Vulkan> api,
                                       AAssetManager *assetManager,
                                       const std::vector<int64_t>& colorFormats,
                                       int64_t depthFormat);
    ~VkGraphicsPipelineManager();

    void destroy();
    void prepare();
    void renderFrame();
    void* getPipeline() { return mPipelineCache.get("Cubic")->get(); }

private:
    std::shared_ptr<GraphicsAPI_Vulkan> mGraphicsApi;
    AAssetManager *mAssetManager;
    std::vector<int64_t> mColorFormats;
    int64_t mDepthFormat;
    static constexpr size_t MAX_PIPELINE_CACHE = 64;
    LRUCache<std::string, std::shared_ptr<VkGraphicsPipeline>> mPipelineCache{MAX_PIPELINE_CACHE};
};