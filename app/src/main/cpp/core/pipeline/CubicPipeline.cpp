//
// Created by jh on 2025/4/2.
//

#include "CubicPipeline.h"
#include <utility>

CubicPipeline::CubicPipeline(std::string name, std::shared_ptr<GraphicsAPI_Vulkan> api,
                             int64_t colorFormat, int64_t depthFormat)
        : VkGraphicsPipeline(std::move(name), std::move(api), colorFormat, depthFormat) {

}
