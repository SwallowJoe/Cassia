//
// Created by jh on 2025/4/1.
//

#pragma once
#include "openxr/openxr.h"
#include <vector>

struct SwapchainInfo {
    XrSwapchain xrSwapchain = XR_NULL_HANDLE;
    int64_t xrSwapchainFormat = 0;
    std::vector<void *> imageViews;
};
