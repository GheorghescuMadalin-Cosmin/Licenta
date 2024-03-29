#pragma once

// ImGui library
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <stdio.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <imgui_stdlib.h>

#define VERSION_MAJOR 0
#define VERSION_MINOR 2

#if DEBUG
#define PROJECT_TYPE "Debug"
#else
#define PROJECT_TYPE "Release"
#endif