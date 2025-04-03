//
// Created by jh on 2025/4/2.
//
#define LOG_TAG "VkGraphicsPipeline"

#include <utility>
#include <shaderc/shaderc.hpp>
#include "VkGraphicsPipeline.h"
#include "Log.h"

VkGraphicsPipeline::VkGraphicsPipeline(std::string name,
                                       std::shared_ptr<GraphicsAPI_Vulkan> api,
                                       int64_t colorFormat,
                                       int64_t depthFormat)
    : mName(std::move(name)), mGraphicsApi(std::move(api)) {

    mColorFormat = colorFormat;
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
    LOGI("prepare");
    compileShaders();
    createVkPipeline();
}

std::vector<uint32_t> VkGraphicsPipeline::compileGlslToSPV(const std::string& name,
                                                 const std::string& source,
                                                 shaderc_shader_kind kind) {
    LOGI("compileGlslToSPV: "<<name<<", source:\n"<<source);
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    // Like -DMY_DEFINE=1
    // options.AddMacroDefinition("MY_DEFINE", "1");

    // const char* test = "void main() {}";
    shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(
            source.c_str(),
            source.size(),
            kind,
            name.c_str(),
            options);

    if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
        LOGE(module.GetErrorMessage());
    }

    return {module.cbegin(), module.cend()};
}

void VkGraphicsPipeline::compileShaders() {
    LOGI("compileShaders");
    auto vertexShaderSource = compileGlslToSPV("vertex",
                                               getVertexShaderSource(),
                                               shaderc_shader_kind::shaderc_glsl_vertex_shader);
    LOGI("vertex binary size: "<<vertexShaderSource.size());
    mVertexShader = mGraphicsApi->CreateShader({
        GraphicsAPI::ShaderCreateInfo::Type::VERTEX,
        reinterpret_cast<const char *>(vertexShaderSource.data()),
        vertexShaderSource.size()
    });

    auto fragmentShaderSource = compileGlslToSPV("fragment",
                                                 getFragmentShaderSource(),
                                                 shaderc_shader_kind::shaderc_glsl_fragment_shader);
    LOGI("fragment binary size: "<<fragmentShaderSource.size());
    mFragmentShader = mGraphicsApi->CreateShader({
        GraphicsAPI::ShaderCreateInfo::Type::FRAGMENT,
        reinterpret_cast<const char *>(fragmentShaderSource.data()),
        fragmentShaderSource.size()
    });
}

void VkGraphicsPipeline::createVkPipeline() {
    GraphicsAPI::PipelineCreateInfo pipelineCreateInfo{};
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
    pipelineCreateInfo.colorFormats = {mColorFormat};
    pipelineCreateInfo.depthFormat = mDepthFormat;
    pipelineCreateInfo.layout = {
            {0, nullptr, GraphicsAPI::DescriptorInfo::Type::BUFFER, GraphicsAPI::DescriptorInfo::Stage::VERTEX},
            {1, nullptr, GraphicsAPI::DescriptorInfo::Type::BUFFER, GraphicsAPI::DescriptorInfo::Stage::VERTEX},
            {2, nullptr, GraphicsAPI::DescriptorInfo::Type::BUFFER, GraphicsAPI::DescriptorInfo::Stage::FRAGMENT},
            // {3, nullptr, GraphicsAPI::DescriptorInfo::Type::SAMPLER, GraphicsAPI::DescriptorInfo::Stage::FRAGMENT},
    };
    mPipeline = mGraphicsApi->CreatePipeline(pipelineCreateInfo);
}

void VkGraphicsPipeline::renderFrame() {
    // LOGI("renderFrame");
}

