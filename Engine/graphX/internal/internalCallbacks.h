#pragma once

#include <sstream>

#include "graphX/vendor/openglIncludes.h"
#include "graphX/core/debug.h"


void GLAPIENTRY callbackOpenGLError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    std::stringstream errorMsg;

    int severityCode = -1;

    // Source
    errorMsg << "Source: ";
    switch (source) {
    case GL_DEBUG_SOURCE_API:
        errorMsg << "API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        errorMsg << "Window System";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        errorMsg << "Shader Compiler";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        errorMsg << "Third Party";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        errorMsg << "Application";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        errorMsg << "Other";
        break;
    }
    errorMsg << ", ";

    // Type
    errorMsg << "Type: ";
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        errorMsg << "Error";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        errorMsg << "Deprecated Behavior";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        errorMsg << "Undefined Behavior";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        errorMsg << "Portability";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        errorMsg << "Performance";
        break;
    case GL_DEBUG_TYPE_MARKER:
        errorMsg << "Marker";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        errorMsg << "Push Group";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        errorMsg << "Pop Group";
        break;
    case GL_DEBUG_TYPE_OTHER:
        errorMsg << "Other";
        break;
    }
    errorMsg << ", ";

    // Severity
    errorMsg << "Severity: ";
    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        errorMsg << "High";
        severityCode = 3;
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        errorMsg << "Medium";
        severityCode = 2;
        break;
    case GL_DEBUG_SEVERITY_LOW:
        errorMsg << "Low";
        severityCode = 1;
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        errorMsg << "Notification";
        severityCode = 0;
        break;
    }
    errorMsg << "\n";

    // Message
    errorMsg << "Message: " << message;

    if (severityCode == 3)
        gx::debug::logError(errorMsg.str());
    else if (severityCode == 2 || severityCode == 1)
        gx::debug::logWarning(errorMsg.str());
    //else if (severityCode == 0)
    //    gx::debug::logInfo(errorMsg.str());
}