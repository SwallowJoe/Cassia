//
// Created by jh on 2025/3/26.
//

#pragma once

#include "android/native_window.h"

struct AndroidAppState {
    ANativeWindow* nativeWindow = nullptr;
    bool resumed = false;
};