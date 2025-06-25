//
// Created by jh on 2025/4/3.
//

#pragma once

#include <string>
#include "VkGraphicsPipeline.h"

class TrianglePipeline: public VkGraphicsPipeline {
public:
    explicit TrianglePipeline(std::string name,
                              std::shared_ptr<GraphicsAPI_Vulkan> api,
                              int64_t colorFormat,
                              int64_t depthFormat)
              : VkGraphicsPipeline(std::move(name), std::move(api), colorFormat, depthFormat) {

    }

    const std::string& getVertexShaderSource() override { return mVertexShaderSource; };
    const std::string& getFragmentShaderSource() override { return mFragmentShaderSource; };

private:
    const std::string mVertexShaderSource = R"(
#version 450
layout(location = 0) in vec4 a_Positions;
layout(location = 0) out vec3 v_Color;
void main() {
    gl_Position = a_Positions;
    v_Color = vec3(0.5, 0.5, 0.5);
}
    )";

    const std::string mFragmentShaderSource =
            "#version 450\n"
            "layout(location = 0) in vec3 v_Color;\n"  // 与顶点着色器匹配
            "layout(location = 1) out vec4 o_Color;\n"
            "void main() {\n"
            "    float light = 0.5;\n"
            "    o_Color = vec4(light * v_Color.rgb, 1.0);\n"
            "}\n";
};