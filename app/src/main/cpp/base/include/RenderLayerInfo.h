//
// Created by jh on 2025/4/2.
//

#pragma once

#include "openxr/openxr.h"
#include <vector>

struct RenderLayerInfo {
    XrTime predictedDisplayTime;
    std::vector<XrCompositionLayerBaseHeader*> compositionLayers;

    XrCompositionLayerProjection compositionLayerProjection = {XR_TYPE_COMPOSITION_LAYER_PROJECTION};
    std::vector<XrCompositionLayerProjectionView> compositionLayerProjectionViews;

    std::vector<XrCompositionLayerDepthInfoKHR> compositionLayerDepthInfos;
};