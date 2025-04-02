//
// Created by jh on 2025/4/2.
//

#include "CubicPipeline.h"

#include <utility>

const std::string &CubicPipeline::getVertexShaderSource() {
    return "#version 450"
           "#extension GL_KHR_vulkan_glsl : enable"
           "layout(std140, binding = 0) uniform CameraConstants {"
           "    mat4 viewProj;"
           "    mat4 modelViewProj;"
           "    mat4 model;"
           "    vec4 color;"
           "    vec4 pad1;"
           "    vec4 pad2;"
           "    vec4 pad3;"
           "}"
           "layout(std140, binding = 1) uniform Normals {"
           "    vec4 normals[6];"
           "}"
           "layout(location = 0) in vec4 a_Positions;"
           "layout(location = 0) out flat uvec2 o_TexCoord;"
           "layout(location = 1) out flat vec3 o_Normal;"
           "layout(location = 2) out flat vec3 o_Color;"
           "void main() {"
           "    gl_Position = modelViewProj * a_Positions;"
           "    int face = gl_VertexIndex / 6;"
           "    o_TexCoord = uvec2(face, 0);"
           "    o_Normal = (model * normals[face]).xyz;"
           "    o_Color =  color.rgb;"
           "}";
}

const std::string &CubicPipeline::getFragmentShaderSource() {
    return "#version 450"
           "layout(location = 0) in flat uvec2 i_TexCoord;"
           "layout(location = 1) in vec3 i_Normal;"
           "layout(location = 2) in flat vec3 i_Color;"
           "layout(location = 0) out vec4 o_Color;"
           "layout(std140, binding = 2) uniform Data {"
           "    vec4 colors[6];"
           "}"
           "d_Data;"
           "layout(binding = 3) uniform sampler2D texSampler;"
           ""
           "void main() {"
           "    float light = 0.1 + 0.9 * clamp(i_Normal.g, 0.0, 1.0);"
           "    vec4 textureColor = texture(texSampler, i_TexCoord);"
           "    o_Color = vec4(light * textureColor.rgb, 1.0);"
           "}"
           "";
}

CubicPipeline::CubicPipeline(std::string name, std::shared_ptr<GraphicsAPI_Vulkan> api,
                             const std::vector<int64_t> &colorFormats, int64_t depthFormat)
        : VkGraphicsPipeline(std::move(name), std::move(api), colorFormats, depthFormat) {

}
