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
                           int64_t colorFormat,
                           int64_t depthFormat);
    const std::string& getVertexShaderSource() override { return mVertexShaderSource; };
    const std::string& getFragmentShaderSource() override { return mFragmentShaderSource; };

private:
    const std::string mVertexShaderSource =
            "#version 450\n"
            "//#extension GL_KHR_vulkan_glsl : enable\n"
            "layout(std140, binding = 0) uniform CameraConstants {\n"
            "    mat4 viewProj;\n"
            "    mat4 modelViewProj;\n"
            "    mat4 model;\n"
            "    vec4 color;\n"
            "    vec4 pad1;\n"
            "    vec4 pad2;\n"
            "    vec4 pad3;\n"
            "};\n"
            "layout(std140, binding = 1) uniform Normals {\n"
            "    vec4 normals[6];\n"
            "};\n"
            "layout(location = 0) in vec4 a_Positions;\n"
            "layout(location = 0) out flat uvec2 o_TexCoord;\n"
            "layout(location = 1) out flat vec3 o_Normal;\n"
            "layout(location = 2) out flat vec3 o_Color;\n"
            "void main() {\n"
            "    gl_Position = modelViewProj * a_Positions;\n"
            "    int face = gl_VertexIndex / 6;\n"
            "    o_TexCoord = uvec2(face, 0);\n"
            "    o_Normal = (model * normals[face]).xyz;\n"
            "    o_Color =  color.rgb;\n"
            "}\n";

    const std::string mFragmentShaderSource =
            "#version 450\n"
            "layout(location = 0) in flat uvec2 i_TexCoord;\n"
            "layout(location = 1) in vec3 i_Normal;\n"
            "layout(location = 2) in flat vec3 i_Color;\n"
            "layout(location = 0) out vec4 o_Color;\n"
            "layout(std140, binding = 2) uniform Data {\n"
            "    vec4 colors[6];\n"
            "}\n"
            "d_Data;\n"
            "layout(binding = 3) uniform sampler2D texSampler;\n"
            "\n"
            "void main() {\n"
            "    float light = 0.1 + 0.9 * clamp(i_Normal.g, 0.0, 1.0);\n"
            "    vec4 textureColor = texture(texSampler, i_TexCoord);\n"
            "    o_Color = vec4(light * textureColor.rgb, 1.0);\n"
            "}\n"
            "\n";
};
