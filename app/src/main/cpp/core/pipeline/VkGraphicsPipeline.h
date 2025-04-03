//
// Created by jh on 2025/4/2.
//

#pragma once

#include "GraphicsAPI_Vulkan.h"
#include <shaderc/shaderc.hpp>

class VkGraphicsPipeline {

public:
    explicit VkGraphicsPipeline(std::string name,
                                std::shared_ptr<GraphicsAPI_Vulkan> api,
                                int64_t colorFormat,
                                int64_t depthFormat);
    ~VkGraphicsPipeline();

    void prepare();
    void destroy();
    void renderFrame();

    virtual const std::string& getVertexShaderSource() = 0;
    virtual const std::string& getFragmentShaderSource() = 0;

    [[nodiscard]] const std::string& getName() const { return mName; }
    void* getPipeline() { return mPipeline; }
private:
    void compileShaders();
    std::vector<uint32_t> compileGlslToSPV(const std::string& name,
                                 const std::string& source,
                                 shaderc_shader_kind type);
    void createVkPipeline();
private:
    const std::string mName;
    std::shared_ptr<GraphicsAPI_Vulkan> mGraphicsApi;
    int64_t mColorFormat;
    int64_t mDepthFormat;
    void* mPipeline = nullptr;
    void* mVertexShader = nullptr;
    void* mFragmentShader = nullptr;
};