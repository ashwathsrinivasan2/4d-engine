cmake_minimum_required(VERSION 3.27)
project(4DEngine)
set(CMAKE_CXX_STANDARD 20)

# Vulkan SDK
if(DEFINED ENV{VULKAN_SDK})
    set(VULKAN_SDK $ENV{VULKAN_SDK})
else()
    message(FATAL_ERROR "VULKAN_SDK not set. Please source the Vulkan SDK.")
endif()

# GLM include directory
set(GLM_INCLUDE_DIR "/opt/homebrew/opt/glm/include")
include_directories(${GLM_INCLUDE_DIR})

include_directories(${VULKAN_SDK}/include ${PROJECT_SOURCE_DIR}/include)
link_directories(${VULKAN_SDK}/lib)

# GLFW (Homebrew paths)
include_directories(/opt/homebrew/Cellar/glfw/3.4/include)
link_directories(/opt/homebrew/Cellar/glfw/3.4/lib)

# Source
add_executable(4DEngine src/main.cpp src/Buffer.cpp src/Entity.cpp src/Mesh.cpp src/Scene.cpp src/Camera.cpp src/Application.cpp src/Renderer.cpp src/Transform.cpp)

# Link libraries
target_link_libraries(4DEngine
    ${VULKAN_SDK}/lib/libvulkan.dylib
    glfw
)

# Make sure runtime can find Vulkan dylibs
set_target_properties(4DEngine PROPERTIES
    BUILD_RPATH "${VULKAN_SDK}/lib"
)
