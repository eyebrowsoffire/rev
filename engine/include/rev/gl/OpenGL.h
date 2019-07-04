#pragma once

#define GL_SILENCE_DEPRECATION

// Make sure to include this first.
#include <glad/glad.h>

#include <GLFW/glfw3.h>

namespace rev::gl {
// In glad, all the OpenGL functions are actually non-const function pointers
// that dynamically are loaded at runtime. So we declare a bunch of wrappers
// for each of these functions in order to be able to use them as template
// parameters.

inline void genBuffers(GLsizei n, GLuint* buffers) { glGenBuffers(n, buffers); }
inline void deleteBuffers(GLsizei n, const GLuint* buffers) { glDeleteBuffers(n, buffers); }
inline void bindBuffer(GLenum target, GLuint buffer) { glBindBuffer(target, buffer); }

inline void genFramebuffers(GLsizei n, GLuint* fBuffers) { glGenFramebuffers(n, fBuffers); }
inline void deleteFramebuffers(GLsizei n, const GLuint* fBuffers) { glDeleteFramebuffers(n, fBuffers); }
inline void bindFramebuffer(GLenum target, GLuint fBuffer) { glBindFramebuffer(target, fBuffer); }

inline void genTextures(GLsizei n, GLuint* textures) { glGenTextures(n, textures); }
inline void deleteTextures(GLsizei n, const GLuint* textures) { glDeleteTextures(n, textures); }
inline void bindTexture(GLenum target, GLuint texture) { glBindTexture(target, texture); }

inline void genVertexArrays(GLsizei n, GLuint* vArrays) { glGenVertexArrays(n, vArrays); }
inline void deleteVertexArrays(GLsizei n, const GLuint* vArrays) { glDeleteVertexArrays(n, vArrays); }
inline void bindVertexArray(GLuint vArray) { glBindVertexArray(vArray); }

inline void deleteShader(GLuint shader) { glDeleteShader(shader); }

inline GLuint createProgram() { return glCreateProgram(); }
inline void deleteProgram(GLuint program) { glDeleteProgram(program); }
inline void useProgram(GLuint program) { glUseProgram(program); }

inline void getShaderiv(GLuint shader, GLenum name, GLint* params) { glGetShaderiv(shader, name, params); }

inline void getShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog)
{
    glGetShaderInfoLog(shader, maxLength, length, infoLog);
}

inline void getProgramiv(GLuint program, GLenum name, GLint* params)
{
    glGetProgramiv(program, name, params);
}

inline void getProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog)
{
    glGetProgramInfoLog(program, maxLength, length, infoLog);
}
}