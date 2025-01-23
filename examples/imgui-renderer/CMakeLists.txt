cmake_minimum_required(VERSION 3.26)
project(imgui-renderer VERSION 0.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(
        ${PROJECT_NAME}
        src/main.cpp
        src/imgui/imgui.cpp
        src/imgui/imgui_demo.cpp
        src/imgui/imgui_draw.cpp
        src/imgui/imgui_tables.cpp
        src/imgui/imgui_widgets.cpp
        src/imgui/backends/imgui_impl_glfw.cpp

        src/renderer/ImGuiInstance.cpp
        src/renderer/ImGuiRenderer.cpp
)

target_include_directories(
        ${PROJECT_NAME}
        PRIVATE
        src
        include/imgui
)

if(EMSCRIPTEN)
    target_link_options(${PROJECT_NAME} PRIVATE "-s WASM=1 -sUSE_GLFW=3 -sUSE_WEBGL2=1 -sEMULATE_FUNCTION_POINTER_CASTS=1 -sALLOW_MEMORY_GROWTH=1 -sEXPORTED_RUNTIME_METHODS=\"['ccall', 'cwrap']\"")
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX .html)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/res")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --preload-file res")
    endif()
endif()

# GLFW
if(NOT EMSCRIPTEN)
    CPMAddPackage(
            NAME "GLFW"
            GIT_TAG "3.3.8"
            GITHUB_REPOSITORY "glfw/glfw"
            OPTIONS
            "GLFW_BUILD_DOCS OFF"
            "GLFW_BUILD_TESTS OFF"
            "GLFW_BUILD_EXAMPLES OFF"
    )
    target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
endif()

# graphicsAPI
target_link_libraries(${PROJECT_NAME} PRIVATE graphicsAPI)