//
// Created by jh on 2025/4/2.
//

#pragma once

#include <string>
#include "VkGraphicsPipeline.h"

class CubicPipeline : public VkGraphicsPipeline {
public:
    explicit CubicPipeline(std::string name,
                           std::shared_ptr<GraphicsAPI_Vulkan> api,
                           const std::vector<int64_t>& colorFormats,
                           int64_t depthFormat);
    const std::string& getVertexShaderSource() override;
    const std::string& getFragmentShaderSource() override;
};
