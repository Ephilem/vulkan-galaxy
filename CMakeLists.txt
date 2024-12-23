cmake_minimum_required(VERSION 3.29)
project(VulkanGalaxy)

set(CMAKE_CXX_STANDARD 20)

find_program(GLSL_COMPILER glslc)

if(NOT GLSL_COMPILER)
    message(FATAL_ERROR "glslc not found!")
endif()

function(compile_shader TARGET SHADER)
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(SHADER_OUTPUT "${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME}.spv")

    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)

    add_custom_command(
            OUTPUT ${SHADER_OUTPUT}
            COMMAND ${GLSL_COMPILER} ${SHADER} -o ${SHADER_OUTPUT}
            DEPENDS ${SHADER}
            COMMENT "Compiling shader ${SHADER_NAME}"
    )

    set_property(
            TARGET ${TARGET}
            APPEND PROPERTY SHADER_OUTPUTS ${SHADER_OUTPUT}
    )
endfunction()

add_executable(VulkanGalaxy src/main.cpp
        src/core/Logger.cpp
        src/core/Logger.h
        src/renderer/VulkanContext.cpp
        src/renderer/VulkanContext.h
        src/renderer/SwapChain.cpp
        src/renderer/SwapChain.h
        src/renderer/Pipeline.cpp
        src/renderer/Pipeline.h
        src/renderer/CommandManager.cpp
        src/renderer/CommandManager.h
        src/renderer/Shader.cpp
        src/renderer/Shader.h
        src/renderer/Synchronization.cpp
        src/renderer/Synchronization.h
        src/core/Window.cpp
        src/core/Window.h
        src/core/Application.cpp
        src/core/Application.h
        src/renderer/VulkanProxy.cpp
        src/renderer/VulkanProxy.h
        src/renderer/PipelineManager.cpp
        src/renderer/PipelineManager.h
        src/renderer/Buffer.cpp
        src/renderer/Buffer.h
)


# Compile shaders
file(GLOB SHADER_SOURCES "src/shaders/*.vert" "src/shaders/*.frag")
foreach(SHADER ${SHADER_SOURCES})
    compile_shader(VulkanGalaxy ${SHADER})
endforeach()

# Create custom target for shaders
get_target_property(SHADER_OUTPUTS VulkanGalaxy SHADER_OUTPUTS)
add_custom_target(Shaders DEPENDS ${SHADER_OUTPUTS})
add_dependencies(VulkanGalaxy Shaders)

# link to -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
target_link_libraries(VulkanGalaxy glfw vulkan dl pthread X11 Xxf86vm Xrandr Xi)

# set to O2 optimization
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
