cmake_minimum_required(VERSION 3.21)

project(PhysicsSim CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable folders for IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(FetchContent)

# Fetch SFML (graphics/window/system only)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
)

# Disable optional SFML components
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)  # Build static libraries instead
if(MINGW)
    set(SFML_USE_STATIC_FREETYPE TRUE CACHE BOOL "" FORCE)
endif()

# Fetch ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.89.9
)

# Fetch ImGui-SFML
FetchContent_Declare(
    imgui-sfml
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
    GIT_TAG 2.6.x
)

# Make SFML and ImGui available first
FetchContent_MakeAvailable(SFML imgui)

# Get the ImGui source directory
FetchContent_GetProperties(imgui)

# Configure ImGui-SFML
set(IMGUI_DIR ${imgui_SOURCE_DIR})
set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_SFML_IMGUI_DEMO ON)

# Make ImGui-SFML available
FetchContent_MakeAvailable(imgui-sfml)

add_executable(physics_sim WIN32
    src/main.cpp
)

# Expose the font header so IDEs and builds can find it
target_sources(physics_sim PRIVATE
    src/fonts/font_data.h
)

# Add the fonts folder to inclue path
target_include_directories(physics_sim PRIVATE
    ${CMAKE_SOURCE_DIR}/src/fonts
)

target_link_libraries(physics_sim PRIVATE
    sfml-graphics
    ImGui-SFML::ImGui-SFML
)

if (MSVC)
    target_compile_options(physics_sim PRIVATE /W4 /permissive-)
else()
    target_compile_options(physics_sim PRIVATE -Wall -Wextra -Wpedantic)
    if(MINGW)
        target_link_libraries(physics_sim PRIVATE mingw32)
    endif()
endif()

if(MINGW)
    # Statically link libstdc++ and libgcc so the produced .exe doesn't depend on MinGW runtime DLLs
    if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.13")
        target_link_options(physics_sim PRIVATE "-static-libgcc" "-static-libstdc++")
    else()
        # Fallback for older CMake
        set_target_properties(physics_sim PROPERTIES LINK_FLAGS "${LINK_FLAGS} -static-libgcc -static-libstdc++")
    endif()
endif()

set_target_properties(physics_sim PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:physics_sim>"
)

# Put exe into seperate folder at project root
set(PHYSICS_SIM_RUNTIME_DIR "${CMAKE_SOURCE_DIR}/physics-sim-build")
set_target_properties(physics_sim PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PHYSICS_SIM_RUNTIME_DIR}"
)