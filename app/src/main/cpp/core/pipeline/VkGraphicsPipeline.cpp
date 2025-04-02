//
// Created by jh on 2025/4/2.
//

#include "VkGraphicsPipeline.h"

#include <utility>

VkGraphicsPipeline::VkGraphicsPipeline(std::string name,
                                       std::shared_ptr<GraphicsAPI_Vulkan> api,
                                       const std::vector<int64_t>& colorFormats,
                                       int64_t depthFormat)
    : mName(std::move(name)), mGraphicsApi(std::move(api)) {

    mColorFormats.resize(colorFormats.size());
    std::for_each(colorFormats.begin(), colorFormats.end(), [&](int64_t colorFormat)->void {
        mColorFormats.emplace_back(colorFormat);
    });

    mDepthFormat = depthFormat;
}

VkGraphicsPipeline::~VkGraphicsPipeline() {
    mGraphicsApi->DestroyPipeline(mPipeline);
    mGraphicsApi->DestroyShader(mVertexShader);
    mGraphicsApi->DestroyShader(mFragmentShader);
}

void VkGraphicsPipeline::destroy() {

}

void VkGraphicsPipeline::prepare() {
    compileShaders();
    createVkPipeline();
}

void VkGraphicsPipeline::compileShaders() {
    auto vertexShaderSource = getVertexShaderSource();
    mVertexShader = mGraphicsApi->CreateShader({
        GraphicsAPI::ShaderCreateInfo::Type::VERTEX,
        vertexShaderSource.c_str(),
        vertexShaderSource.size()
    });

    auto fragmentShaderSource = getFragmentShaderSource();
    mFragmentShader = mGraphicsApi->CreateShader({
        GraphicsAPI::ShaderCreateInfo::Type::FRAGMENT,
        fragmentShaderSource.c_str(),
        fragmentShaderSource.size()
    });
}

void VkGraphicsPipeline::createVkPipeline() {
    GraphicsAPI::PipelineCreateInfo pipelineCreateInfo;
    pipelineCreateInfo.shaders = {mVertexShader, mFragmentShader};
    pipelineCreateInfo.vertexInputState.attributes = {
            {0, 0, GraphicsAPI::VertexType::VEC4, 0, "TEXCOORD"}
    };
    pipelineCreateInfo.vertexInputState.bindings = {
            {0, 0, 4 * sizeof (float)}
    };
    pipelineCreateInfo.inputAssemblyState = {
            GraphicsAPI::PrimitiveTopology::TRIANGLE_LIST, false};
    pipelineCreateInfo.rasterisationState = {
            false,
            false,
            GraphicsAPI::PolygonMode::FILL,
            GraphicsAPI::CullMode::BACK,
            GraphicsAPI::FrontFace::COUNTER_CLOCKWISE,
            false,
            0.0f,
            0.0f,
            0.0f,
            1.0f
    };
    pipelineCreateInfo.multisampleState = {
            1,
            false,
            1.0f,
            0xFFFFFFFF,
            false,
            false
    };
    pipelineCreateInfo.depthStencilState = {
            true, true,
            GraphicsAPI::CompareOp::LESS_OR_EQUAL,
            false, false,
            {}, {}, 0.0f, 1.0f
    };
    pipelineCreateInfo.colorBlendState = {
            false, GraphicsAPI::LogicOp::NO_OP,
            {
                {
                    true,
                    GraphicsAPI::BlendFactor::SRC_ALPHA,
                    GraphicsAPI::BlendFactor::ONE_MINUS_DST_ALPHA,
                    GraphicsAPI::BlendOp::ADD,
                    GraphicsAPI::BlendFactor::ONE,
                    GraphicsAPI::BlendFactor::ZERO,
                    GraphicsAPI::BlendOp::ADD,
                    (GraphicsAPI::ColorComponentBit)15
                }
            },
            {0.0f, 0.0f, 0.0f, 0.0f}
    };
    pipelineCreateInfo.colorFormats = std::move(mColorFormats);
    pipelineCreateInfo.depthFormat = mDepthFormat;
    pipelineCreateInfo.layout = {
            {0, nullptr, GraphicsAPI::DescriptorInfo::Type::BUFFER, GraphicsAPI::DescriptorInfo::Stage::VERTEX},
            {1, nullptr, GraphicsAPI::DescriptorInfo::Type::BUFFER, GraphicsAPI::DescriptorInfo::Stage::VERTEX},
            {2, nullptr, GraphicsAPI::DescriptorInfo::Type::BUFFER, GraphicsAPI::DescriptorInfo::Stage::FRAGMENT},
            {3, nullptr, GraphicsAPI::DescriptorInfo::Type::SAMPLER, GraphicsAPI::DescriptorInfo::Stage::FRAGMENT},
    };
    mPipeline = mGraphicsApi->CreatePipeline(pipelineCreateInfo);
}

void VkGraphicsPipeline::renderFrame() {

}

