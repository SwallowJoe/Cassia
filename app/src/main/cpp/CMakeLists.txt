# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
set(PROJECT_NAME "XRGraphics")
project("${PROJECT_NAME}")

set(PROJECT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Additional Directories for find_package() to search within.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# For FetchContent_Declare() and FetchContent_MakeAvailable()
include(FetchContent)

# openxr_loader - From github.com/KhronosGroup
set(BUILD_TESTS
        OFF
        CACHE INTERNAL "Build tests"
)
set(BUILD_API_LAYERS
        ON
        CACHE INTERNAL "Use OpenXR layers"
)
FetchContent_Declare(
        OpenXR
        URL_HASH MD5=924a94a2da0b5ef8e82154c623d88644
        URL https://github.com/KhronosGroup/OpenXR-SDK-Source/archive/refs/tags/release-1.0.34.zip
        SOURCE_DIR
        openxr
)
FetchContent_MakeAvailable(OpenXR)

# Files
set(SOURCES
        "main_graphics.cpp"
        "xrcommon/GraphicsAPI.cpp"
        "xrcommon/GraphicsAPI_Vulkan.cpp"
        "xrcommon/OpenXRDebugUtils.cpp")

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
target_include_directories(
        ${PROJECT_NAME}
        PRIVATE
        # In this repo
        xrcommon/
        # From OpenXR repo
        "${openxr_SOURCE_DIR}/src/common"
        "${openxr_SOURCE_DIR}/external/include"
)

# export ANativeActivity_onCreate for java to call.
set_property(
        TARGET ${PROJECT_NAME}
        APPEND_STRING
        PROPERTY LINK_FLAGS " -u ANativeActivity_onCreate"
)

# native_app_glue
include(AndroidNdkModules)
android_ndk_import_module_native_app_glue()

# Specifies libraries CMake should link to your target library. You
# can link libraries from various origins, such as libraries defined in this
# build script, prebuilt third-party libraries, or Android system libraries.
target_link_libraries(${PROJECT_NAME}
        android
        native_app_glue
        openxr_loader
        log
        jnigraphics)
target_compile_options(${PROJECT_NAME} PRIVATE -Wno-cast-calling-convention)

# VulkanNDK
find_library(vulkan-lib vulkan)
if(vulkan-lib)
    message("find vulkan sdk....")
    target_include_directories(
            ${PROJECT_NAME}
            PUBLIC ${ANDROID_NDK}/sources/third_party/vulkan/src/include
    )
    target_link_libraries(${PROJECT_NAME} ${vulkan-lib})
    target_compile_definitions(
            ${PROJECT_NAME} PUBLIC XR_TUTORIAL_USE_VULKAN
    )
endif()