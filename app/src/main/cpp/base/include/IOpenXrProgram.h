//
// Created by jh on 2025/3/26.
//

#pragma once

class IOpenXrProgram {
public:
    virtual ~IOpenXrProgram() = default;

    virtual void createXrInstance() = 0;
    virtual void destroyXrInstance() = 0;
    virtual void initializeSystemId() = 0;
    virtual void initializeDevice() = 0;
    virtual void createSession() = 0;
    virtual void destroySession() = 0;
    virtual void createReferenceSpace() = 0;
    virtual void destroyReferenceSpace() = 0;
    virtual void createXrSwapChains() = 0;
    virtual void destroyXrSwapChains() = 0;
    virtual void pollXrEvents() = 0;
    virtual bool renderFrame() = 0;
};