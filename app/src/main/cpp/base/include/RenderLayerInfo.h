//
// Created by jh on 2025/4/2.
//

#pragma once

#include "openxr/openxr.h"
#include <vector>

/***
 * XrLayer基本信息，包含：
 * 1. 预计上屏显示时间戳
 * 2. xr合成Layer集合
 * 3.
 */
struct RenderLayerInfo {
    XrTime predictedDisplayTime;
    std::vector<XrCompositionLayerBaseHeader*> compositionLayers;

    XrCompositionLayerProjection compositionLayerProjection = {XR_TYPE_COMPOSITION_LAYER_PROJECTION};
    std::vector<XrCompositionLayerProjectionView> compositionLayerProjectionViews;

    std::vector<XrCompositionLayerDepthInfoKHR> compositionLayerDepthInfos;
};