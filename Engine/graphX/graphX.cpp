#include "graphX.h"

#include <iostream>

#include "graphX/vendor/imgui/imgui.h"
#include "graphX/vendor/imgui/imgui_impl_glfw_gl3.h"

#include "graphX/internal/context.h"
#include "graphX/internal/internalCallbacks.h"


bool gx::init(std::string windowName, unsigned int windowWidth, unsigned int windowHeight, int windowMinWidth, int windowMinHeight, bool debugMode)
{
    //init engine context
    ctx.init();
    ctx.debugMode = debugMode;

    //init glfw and window
    if (!glfwInit())
        return false;

    ctx.window = glfwCreateWindow(windowWidth, windowHeight, windowName.c_str(), NULL, NULL);
    if (!ctx.window)
        return false;
    window::setSizeLimits(windowMinWidth, windowMinHeight, windowMinWidth, windowMinHeight);
    glfwMakeContextCurrent(ctx.window);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    //init glew
    if (glewInit() != GLEW_OK)
        return false;

    gx::debug::logInfo((const char*)glGetString(GL_VERSION));
    if (ctx.debugMode)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(callbackOpenGLError, nullptr);
    }

    //init graphic contexts
    ctx.loadResources();
    ctx2d.init();
    ctx3d.init();
    //init audio
    ctx.initAudio();

    //init imgui
    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(ctx.window, true);
    ImGui::GetIO().IniFilename = NULL;
    ImGui::StyleColorsDark();

    //standard settings
    gx::renderer2d::setClearColor({0, 0, 0});


    ctx2d.beginImGuiRendering();//so that there's no looping problem

    return true;
}

bool gx::running()
{
    gx::renderTargetUnbindAll();

    ctx2d.endImGuiRendering();//ImGui End
    glfwPollEvents();
    glfwSwapBuffers(ctx.window);
    glViewport(0, 0, gx::window::getSize().x, gx::window::getSize().y);

    gx::renderer2d::clear();
    ctx2d.beginImGuiRendering();//ImGui Begin



    if (ctx.quitting || glfwWindowShouldClose(ctx.window))
    {
        return false;
    }

    return true;
}

bool gx::terminate()
{
    //destroy contexts
    ctx2d.destroy();
    ctx3d.destroy();
    ctx.destroy();

    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return true;
}