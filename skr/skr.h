/**
 * @file skr.h
 *
 * Single-header 3D graphics engine
 *
 * @copyright (C) 2025 SKR Authors
 *
 * @license SPDX-License-Identifier: GPL-3.0-or-later (see LICENSE).
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SKR_H
#define SKR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Identifies the type of graphics API backend in use.
 */
typedef enum SkrApiBackendType {
	SKR_BACKEND_API_GL, /*!< OpenGL API. */
	SKR_BACKEND_API_VK, /*!< Vulkan API. */
} SkrApiBackendType;

/*
 * You can control which graphics API SKR uses by defining
 * SKR_BACKEND_API before including skr.h.
 *
 * #define SKR_BACKEND_API 0 -> Use OpenGL backend, must include manually
 * #define SKR_BACKEND_API 1 -> Use Vulkan backend, must include manually
 *
 * If SKR_BACKEND_API is not defined, skr.h will automatically try to include
 * both GL/glew.h and <vulkan/vulkan.h>, and will default to OpenGL.
 */

#ifndef SKR_BACKEND_API

/* Try including GLEW */
#ifndef __gl_h_
#include <GL/glew.h>
#endif

/* Try including Vulkan */
#ifndef VULKAN_H_
#include <vulkan/vulkan.h>
#endif

/* Default API if none was specified: GL */
#define SKR_BACKEND_API SKR_BACKEND_API_GL

#else

#if !defined(VULKAN_H_)
typedef struct VkPipeline_T*       VkPipeline;
typedef struct VkPipelineLayout_T* VkPipelineLayout;
#endif

#endif

/**
 * @brief Identifies the type of window backend in use.
 */
typedef enum SkrWindowBackendType {
	SKR_BACKEND_WINDOW_GLFW, /*!< Window created using GLFW. */
	SKR_BACKEND_WINDOW_SDL,  /*!< Window created using SDL. */
} SkrWindowBackendType;

/*
 * You can control which windowing backend SKR uses by defining
 * SKR_BACKEND_WINDOW before including skr.h.
 *
 * #define SKR_BACKEND_WINDOW 0 -> Use GLFW backend, must include manually
 * #define SKR_BACKEND_WINDOW 1 -> Use SDL backend, must include manually
 *
 * If SKR_BACKEND_WINDOW is not defined, skr.h will automatically try to include
 * both GLFW/glfw3.h and SDL2/SDL.h, and will default to GLFW.
 */

#ifndef SKR_BACKEND_WINDOW

/* Try including GLFW */
#ifndef _glfw3_h_
#include <GLFW/glfw3.h>
#endif

/* Try including SDL */
#ifndef SDL_h_
#include <SDL2/SDL.h>
#endif

/* Default backend if none was specified: GLFW */
#define SKR_BACKEND_WINDOW SKR_BACKEND_WINDOW_GLFW

#endif

#ifndef cglm_h
#include <cglm/cglm.h>
#endif

/**
 * @internal
 * @brief Size of the global error buffer (in bytes).
 *
 * This constant defines the maximum length of the error string stored in
 * `SKR_LAST_ERROR`, including the null terminator.
 */
#ifndef SKR_LAST_ERROR_SIZE
#define SKR_LAST_ERROR_SIZE 1044
#endif

/**
 * @brief Global error buffer.
 *
 * Holds the most recent error message set by the error handling system. Its
 * size is fixed at `SKR_LAST_ERROR_SIZE`.
 *
 * @details
 * If the buffer is empty (first character == '\0'),
 * then no error is currently set.
 *
 * @see m_skr_last_error_clear
 * @see m_skr_last_error_set
 * @see SKR_OK
 */
static char SKR_LAST_ERROR[SKR_LAST_ERROR_SIZE];

/**
 * @brief Checks if there is no error set.
 *
 * Returns true (nonzero) if `SKR_LAST_ERROR` is empty, false (0) otherwise.
 *
 * @return int 1 if no error, 0 if an error exists.
 *
 * @usage
 * if (!SKR_OK) {
 *     fprintf(stderr, "Error: %s\n", SKR_LAST_ERROR);
 * }
 */
#define SKR_OK (SKR_LAST_ERROR[0] == '\0')

/**
 * @brief Maximum number of bone influences per vertex.
 *
 * Each vertex can be affected by up to this many bones during skeletal
 * animation. Commonly set to 4, since most real-time rendering pipelines
 * balance flexibility with performance by limiting to four weights per vertex.
 */
#define MAX_BONE_INFLUENCE 4

/**
 * @brief Shader object definition.
 *
 * Represents a GPU shader in the engine. A shader can be defined either
 * directly from source code in memory or by referencing a file path.
 */
typedef struct SkrShader {
	/**
	 * @brief Shader type.
	 *
	 * OpenGL shader type enum (e.g. GL_VERTEX_SHADER, GL_FRAGMENT_SHADER).
	 */
	const unsigned int Type;

	/**
	 * @brief GLSL source code (optional).
	 *
	 * If provided, the shader will be compiled directly from this string in
	 * memory. May be NULL if the shader is loaded from a file.
	 */
	const char* Source;

	/**
	 * @brief Path to shader file (optional).
	 *
	 * If provided, the shader source will be loaded from this file. May be
	 * NULL if the shader is provided directly via @ref Source.
	 */
	const char* Path;
} SkrShader;

static SkrShader skr_fps_camera_vert = {
        GL_VERTEX_SHADER,
        (char*){"#version 330 core\n"
                "layout (location = 0) in vec3 aPos;\n"
                "layout (location = 1) in vec2 aTexCoord;\n"
                "out vec2 TexCoord;\n"
                "uniform mat4 model;\n"
                "uniform mat4 view;\n"
                "uniform mat4 projection;\n"
                "void main() {\n"
                "gl_Position = projection * view * model * vec4(aPos, "
                "1.0f);\n"
                "TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
                "}\n"},
        NULL};

/**
 * @brief Vertex structure used by the rendering engine.
 *
 * Encapsulates all per-vertex attributes commonly required in 3D rendering,
 * including position, normals, texture coordinates, tangent space, and skeletal
 * animation data.
 */
typedef struct SkrVertex {
	/**
	 * @brief Vertex position in object space.
	 *
	 * Three-component vector (x, y, z).
	 */
	vec3 Position;

	/**
	 * @brief Vertex normal vector.
	 *
	 * Used for lighting calculations. Three components (x, y, z).
	 */
	vec3 Normal;

	/**
	 * @brief Texture coordinates (UV).
	 *
	 * Two-component vector (u, v), typically in the range [0, 1].
	 */
	vec2 UV;

	/**
	 * @brief Tangent vector.
	 *
	 * Defines the direction of increasing U in the tangent space.
	 * Used for normal mapping. Three components (x, y, z).
	 */
	vec3 Tangent;

	/**
	 * @brief Bitangent vector.
	 *
	 * Defines the direction of increasing V in the tangent space.
	 * Orthogonal to both the normal and tangent. Three components (x, y,
	 * z).
	 */
	vec3 Bitangent;

	/**
	 * @brief Indices of influencing bones.
	 *
	 * Array of up to @ref MAX_BONE_INFLUENCE integers that reference bones
	 * in the skeleton.
	 * Used for skeletal animation.
	 */
	int BoneIDs[MAX_BONE_INFLUENCE];

	/**
	 * @brief Weights of influencing bones.
	 *
	 * Parallel array to @ref BoneIDs, with the corresponding influence
	 * weights.
	 * Values typically normalized so they sum to 1.0.
	 */
	int BoneWeights[MAX_BONE_INFLUENCE];
} SkrVertex;

/**
 * @brief Supported texture roles.
 *
 * Defines the semantic role of a texture in a material or shader.
 */
typedef enum SkrTextureType {
	SKR_TEXTURE_DIFFUSE,    /*!< Base color / albedo map. */
	SKR_TEXTURE_SPECULAR,   /*!< Specular intensity map. */
	SKR_TEXTURE_NORMAL,     /*!< Normal map (tangent-space). */
	SKR_TEXTURE_HEIGHT,     /*!< Height/displacement map. */
	SKR_TEXTURE_EMISSIVE,   /*!< Emissive (glow) map. */
	SKR_TEXTURE_AMBIENT,    /*!< Ambient occlusion map. */
	SKR_TEXTURE_METALLIC,   /*!< Metallic map (PBR). */
	SKR_TEXTURE_ROUGHNESS,  /*!< Roughness map (PBR). */
	SKR_TEXTURE_REFLECTION, /*!< Reflection/environment map. */
	SKR_TEXTURE_UNKNOWN     /*!< Unknown/unsupported type. */
} SkrTextureType;

/**
 * @brief Texture object used by the rendering engine.
 *
 * Encapsulates GPU texture data and the raw image source from which it was
 * created. The texture may represent diffuse color, normals, specular, or other
 * material properties (see @ref SkrTextureType).
 */
typedef struct SkrTexture {
	/**
	 * @brief OpenGL texture object ID.
	 *
	 * Assigned by glGenTextures() and used to bind this texture to the GPU.
	 */
	unsigned int ID;

	/**
	 * @brief Texture semantic type.
	 *
	 * Indicates the role this texture plays in a material/shader.
	 * See @ref SkrTextureType for the list of supported values.
	 */
	SkrTextureType Type;

	/**
	 * @brief Raw image data in memory.
	 *
	 * Pointer to pixel data (e.g. from `stbi_load`). Used to upload the
	 * texture to the GPU. The format (channels, bit depth, layout) depends
	 * on the loader. May be NULL once the texture has been uploaded and the
	 * CPU copy freed.
	 */
	char* Source;

	/**
	 * @brief Filesystem path to the texture image.
	 *
	 * Original file location of the texture (e.g. "assets/diffuse.png").
	 */
	char* Path;
} SkrTexture;

/**
 * @brief Renderable mesh data.
 *
 * Represents a single drawable mesh in the engine. A mesh contains its own
 * GPU buffer objects (VAO, VBO, EBO) and CPU-side data for vertices, indices,
 * and associated textures.
 */
typedef struct SkrMesh {
	/**
	 * @brief Vertex Array Object (VAO).
	 *
	 * Stores the vertex attribute configuration and references to VBO/EBO.
	 */
	unsigned int VAO;

	/**
	 * @brief Vertex Buffer Object (VBO).
	 *
	 * Stores vertex data (positions, normals, UVs, etc.) in GPU memory.
	 */
	unsigned int VBO;

	/**
	 * @brief Element Buffer Object (EBO).
	 *
	 * Stores mesh indices in GPU memory for indexed drawing.
	 */
	unsigned int EBO;

	/**
	 * @brief Vertex data.
	 *
	 * Pointer to an array of @ref SkrVertex structs stored on the CPU.
	 * Uploaded to GPU via the VBO. May be freed after upload if not needed.
	 */
	SkrVertex*   Vertices;
	unsigned int VertexCount;

	/**
	 * @brief Index data count.
	 *
	 * Number of indices used to draw this mesh.
	 */
	unsigned int Indices;

	/**
	 * @brief Associated textures.
	 *
	 * Pointer to an array of @ref SkrTexture objects that define the
	 * materials of this mesh.
	 */
	SkrTexture*  Textures;
	unsigned int TextureCount;

	/**
	 * @brief Backend-specific rendering data.
	 *
	 * Tagged union that stores handles or objects specific to the graphics
	 * API backend in use. Only the member corresponding to the current
	 * backend type is valid.
	 */
	union {
		struct {
			GLuint Program;
		} GL;

		struct {
			VkPipeline       Pipeline;
			VkPipelineLayout Layout;
		} VK;
	} Backend;
} SkrMesh;

/**
 * @brief 3D model representation.
 *
 * A model consists of one or more meshes, each with its own vertices, indices,
 * and material textures. The model may also reference textures that are shared
 * across meshes.
 */
typedef struct SkrModel {
	SkrMesh*     Meshes; /*!< Array of meshes that compose the model. */
	unsigned int MeshCount;

	SkrTexture*  Textures; /*!< Array of textures used by model's meshes. */
	unsigned int TextureCount;

	char* Path; /*!< Filesystem path of the model file. */
} SkrModel;

/**
 * @brief Tagged union that wraps a backend window handle.
 *
 * Contains both the backend type (`Tag`) and the backend-specific handle
 * (`Handler`). Only the member corresponding to the active backend type is
 * valid.
 */
typedef struct SkrWindowBackend {
	const SkrWindowBackendType Type; /*!< Backend type. */

	union {
		struct GLFWwindow* GLFW; /*!< Handle to a GLFW window. */
		                         /* TODO: more backends */
	} Handler;                       /*!< Backend-specific handle. */
} SkrWindowBackend;

struct SkrWindow;

/**
 * @brief Function type for input event callbacks.
 */
typedef void SkrInputHandler(struct SkrWindow* w);

/**
 * @brief Generic engine window.
 *
 * A portable window structure that abstracts over different windowing backends.
 * Encapsulates title, size, optional input handler, and backend data.
 */
typedef struct SkrWindow {
	char* Title;  /*!< Window title (UTF-8 string). */
	int   Width;  /*!< Width of the window in pixels. */
	int   Height; /*!< Height of the window in pixels. */

	SkrInputHandler* InputHandler; /*!< Pointer to input handler. */
	SkrWindowBackend Backend;      /*!< Backend type and handle. */
} SkrWindow;

typedef struct SkrShaderProgram {
	GLuint       ID;
	char*        Name;
	SkrShader*   Shaders;
	unsigned int ShaderCount;
} SkrShaderProgram;

typedef struct SkrState {
	SkrWindow* Window;

	SkrModel*    Models;
	unsigned int ModelCount;

	SkrShaderProgram* Programs;
	unsigned int      ProgramCount;
} SkrState;

/**
 * @brief First-person camera structure.
 *
 * Stores position, orientation, and field-of-view.
 * Can be controlled by mouse and keyboard input.
 */
typedef struct SkrCamera {
	vec3 Position; /*!< Camera position in world space. */
	vec3 Front;    /*!< Normalized forward vector. */
	vec3 Up;       /*!< Normalized up vector. */
	vec3 Right;    /*!< Normalized right vector. */
	vec3 WorldUp;  /*!< Global up direction, usually {0, 1, 0}. */

	float Yaw;
	float Pitch;
	float FOV;

	float Sensitivity;
	float LastX;      /*!< Last cursor X position. */
	float LastY;      /*!< Last cursor Y position. */
	bool  FirstMouse; /*!< Whether the first mouse movement has been
	                     captured. */
	bool  Initialized;
} SkrCamera;

#define SkrDefaultFPSCamera                                                    \
	(&(SkrCamera){                                                         \
	        .Position = {0.0f, 0.0f, 3.0f},                                \
	        .Front = {0.0f, 0.0f, -1.0f},                                  \
	        .Up = {0.0f, 1.0f, 0.0f},                                      \
	        .Yaw = -90.0f,                                                 \
	        .Pitch = 0.0f,                                                 \
	        .FOV = 70.0f,                                                  \
	        .Sensitivity = 0.1f,                                           \
	        .LastX = 400.0f,                                               \
	        .LastY = 300.0f,                                               \
	        .FirstMouse = true,                                            \
	})

extern SkrCamera* g_skr_camera;

/**
 * @internal
 * @brief Set the last error message with source metadata.
 *
 * Formats a string with printf-style arguments and stores it into the global
 * error buffer 'SKR_LAST_ERROR'. The message will typically be set through the
 * @ref SKR_LAST_ERROR_SET macro, which automatically includes the file, line,
 * and function name where the error occurred.
 *
 * @param file  Source file where the error occurred (use __FILE__).
 * @param line  Line number where the error occurred (use __LINE__).
 * @param func  Function name where the error occurred (use __func__).
 * @param fmt   Format string (printf-style).
 * @param ...   Arguments corresponding to the format specifiers in `fmt`.
 */
static inline void m_skr_last_error_set_with_meta(const char* file, int line,
                                                  const char* func,
                                                  const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	int written = snprintf(SKR_LAST_ERROR, SKR_LAST_ERROR_SIZE,
	                       "[SKR] ERROR %s:%d:%s: ", file, line, func);

	if (written < 0 || written >= SKR_LAST_ERROR_SIZE)
		written = 0;

	vsnprintf(SKR_LAST_ERROR + written, SKR_LAST_ERROR_SIZE - written, fmt,
	          args);

	va_end(args);
}

/**
 * @brief Set the last error message with automatic source metadata.
 *
 * Use this macro instead of calling 'm_skr_last_error_set_with_meta()'
 * directly. It automatically includes the source file, line number, and
 * function name in the error message prefix.
 *
 * @usage
 * m_skr_last_error_set("Failed to load texture: %s", path);
 */
#define m_skr_last_error_set(...)                                              \
	m_skr_last_error_set_with_meta(__FILE__, __LINE__, __func__,           \
	                               __VA_ARGS__)

/**
 * @internal
 * @brief Clears the global error buffer.
 *
 * Sets the first character of `SKR_LAST_ERROR` to '\0', effectively removing
 * any previously stored error message.
 *
 * @usage
 * m_skr_last_error_clear();
 */
static inline void m_skr_last_error_clear() { SKR_LAST_ERROR[0] = '\0'; }

/**
 * @internal
 * @brief Load an image from a file into raw pixel memory.
 *
 * This function **must be implemented by the user** of the engine. The engine
 * will call it when loading textures, but does not provide its own image
 * loading backend.
 *
 * @param path     Filesystem path to the image file.
 * @param width    Output pointer to receive image width in pixels.
 * @param height   Output pointer to receive image height in pixels.
 * @param channels Output pointer to receive the number of color channels.
 *
 * @return Pointer to raw pixel data (heap-allocated). The exact format (e.g.
 *         RGB vs. RGBA) is determined by the user's implementation. Returns
 *         NULL if the image could not be loaded.
 *
 * @note The returned memory must be freed with ::skr_free_image.
 */
extern unsigned char* m_skr_load_image_from_file(const char* path, int* width,
                                                 int* height, int* channels);

/**
 * @internal
 * @brief Free pixel memory previously allocated by
 * ::m_skr_load_image_from_file.
 *
 * This function **must be implemented by the user** of the engine to match the
 * allocation strategy used in their m_skr_load_image_from_file.
 *
 * @param image_data Pointer to the pixel data to free. Safe to pass NULL.
 */
extern void m_skr_free_image(unsigned char* image_data);

/**
 * @internal
 * @brief Read a whole file into memory.
 *
 * Opens the file in binary mode, reads its contents into a null-terminated
 * buffer, and returns it. Caller must free the buffer with `free()`.
 *
 * @param path File path to read.
 * @return Newly allocated buffer containing file contents, or NULL on error.
 */
static inline char* m_skr_read_file(const char* path) {
	FILE* file = fopen(path, "rb");
	if (!file) {
		m_skr_last_error_set("failed to open");
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	long len = ftell(file);
	rewind(file);

	char* buffer = (char*)malloc(len + 1);
	if (!buffer) {
		fclose(file);
		m_skr_last_error_set("failed to open");
		return NULL;
	}

	fread(buffer, 1, len, file);
	buffer[len] = '\0';
	fclose(file);

	m_skr_last_error_clear();
	return buffer;
}

/**
 * @internal
 * @brief GL framebuffer resize callback
 *
 * Adjusts the OpenGL viewport when the window is resized.
 *
 * @param width  New framebuffer width.
 * @param height New framebuffer height.
 */
static inline void m_skr_gl_framebuffer_size_callback(const int width,
                                                      const int height) {
	glViewport(0, 0, width, height);
	m_skr_last_error_clear();
}

/**
 * @internal
 * @brief GLFW framebuffer resize callback wrapper.
 *
 * @param window GLFW window handle.
 * @param width  New framebuffer width.
 * @param height New framebuffer height.
 */
static inline void m_skr_gl_glfw_framebuffer_size_callback(GLFWwindow* window,
                                                           const int   width,
                                                           const int   height) {
	(void)window;
	m_skr_gl_framebuffer_size_callback(width, height);
	m_skr_last_error_clear();
}

static inline void m_skr_gl_glfw_mouse_callback(GLFWwindow* window,
                                                double xposIn, double yposIn) {
	float xpos = xposIn;
	float ypos = yposIn;

	if (g_skr_camera->FirstMouse) {
		g_skr_camera->LastX = xpos;
		g_skr_camera->LastY = ypos;
		g_skr_camera->FirstMouse = false;
	}

	float xoffset = xpos - g_skr_camera->LastX;
	float yoffset =
	        g_skr_camera->LastY -
	        ypos; // reversed since y-coordinates go from bottom to top
	g_skr_camera->LastX = xpos;
	g_skr_camera->LastY = ypos;

	float sensitivity = g_skr_camera->Sensitivity;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	g_skr_camera->Yaw += xoffset;
	g_skr_camera->Pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get
	// flipped
	if (g_skr_camera->Pitch > 89.0f)
		g_skr_camera->Pitch = 89.0f;
	if (g_skr_camera->Pitch < -89.0f)
		g_skr_camera->Pitch = -89.0f;

	vec3 front = {0.0f, 0.0f, 0.0f};

	front[0] = cosf(glm_rad(g_skr_camera->Yaw)) *
	           cosf(glm_rad(g_skr_camera->Pitch));
	front[1] = sinf(glm_rad(g_skr_camera->Pitch));
	front[2] = sinf(glm_rad(g_skr_camera->Yaw)) *
	           cosf(glm_rad(g_skr_camera->Pitch));

	glm_vec3_normalize_to(front, g_skr_camera->Front);
}

/**
 * @internal
 * @brief Initialize a GLFW window for OpenGL rendering.
 *
 * @param w Pointer to SkrWindow to initialize (must not be NULL).
 * @return 1 on success, 0 on failure.
 */
static inline int m_skr_gl_glfw_init(SkrWindow* w) {
	if (!glfwInit() || !w) {
		m_skr_last_error_set(
		        "either glfwInit != 1 or SkrWindow == NULL");
		return 0;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	w->Backend.Handler.GLFW = NULL;

	w->Backend.Handler.GLFW =
	        glfwCreateWindow(w->Width, w->Height, w->Title, NULL, NULL);

	if (!w->Backend.Handler.GLFW) {
		m_skr_last_error_set("window backend is NULL");
		glfwTerminate();
		return 0;
	}

	glfwSetFramebufferSizeCallback(w->Backend.Handler.GLFW,
	                               m_skr_gl_glfw_framebuffer_size_callback);
	glfwMakeContextCurrent(w->Backend.Handler.GLFW);

	if (g_skr_camera) {
		glfwSetCursorPosCallback(w->Backend.Handler.GLFW,
		                         m_skr_gl_glfw_mouse_callback);
	}

	m_skr_last_error_clear();
	return 1;
}

/**
 * @internal
 * @brief GL check shader or program compile/link status.
 *
 * @param shader OpenGL shader or program ID.
 * @param type   String ("vert","frag","prog", etc.).
 *
 * @return 1 if compilation/link succeeded, 0 otherwise.
 */
static inline int m_skr_gl_check_compile_errors(const GLuint shader,
                                                const char*  type) {
	GLint  success;
	GLchar infoLog[1024];

	if (strcmp(type, "prog") == 0) {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, sizeof(infoLog), NULL,
			                    infoLog);
			m_skr_last_error_set("failed to link %s: %s", type,
			                     infoLog);
			return 0;
		}
	} else {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, sizeof(infoLog), NULL,
			                   infoLog);
			m_skr_last_error_set("failed to compile %s: %s", type,
			                     infoLog);
			return 0;
		}
	}

	return 1;
}

/**
 * @internal
 * @brief GL create an OpenGL shader from source.
 *
 * @param type   Shader type (GL_VERTEX_SHADER, etc.).
 * @param source Null-terminated GLSL source code.
 *
 * @return Shader ID, or 0 on failure.
 */
static inline GLuint m_skr_gl_create_shader(const GLenum type,
                                            const char*  source) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	const char* type_str = NULL;
	switch (type) {
	case GL_VERTEX_SHADER:
		type_str = "vert";
		break;
	case GL_FRAGMENT_SHADER:
		type_str = "frag";
		break;
	case GL_GEOMETRY_SHADER:
		type_str = "geom";
		break;
	default:
		type_str = "unknown";
		break;
	}

	if (!m_skr_gl_check_compile_errors(shader, type_str)) {
		glDeleteShader(shader);
		return 0;
	}

	m_skr_last_error_clear();
	return shader;
}

/**
 * @internal
 * @brief GL create an OpenGL shader from a file.
 *
 * Reads the file contents and compiles it as a shader of the given type.
 *
 * @param type Shader type.
 * @param path File path to GLSL source.
 *
 * @return Shader ID, or 0 on failure.
 */
static inline GLuint m_skr_gl_create_shader_from_file(const GLenum type,
                                                      const char*  path) {
	char* source = m_skr_read_file(path);
	if (!source) {
		return 0;
	}

	GLuint shader = m_skr_gl_create_shader(type, source);
	free(source);

	m_skr_last_error_clear();
	return shader;
}

/**
 * @internal
 * @brief GL link multiple shaders into a program.
 *
 * Attaches all shaders, links, deletes them, and returns the program.
 *
 * @param shaders Array of shader IDs.
 * @param count   Number of shaders.
 *
 * @return Program ID, or 0 on failure.
 */
static inline GLuint m_skr_gl_create_program(const GLuint* shaders,
                                             const size_t  count) {
	GLuint program = glCreateProgram();
	for (size_t i = 0; i < count; ++i) {
		glAttachShader(program, shaders[i]);
	}

	glLinkProgram(program);
	if (!m_skr_gl_check_compile_errors(program, "prog")) {
		glDeleteProgram(program);
		return 0;
	}

	for (size_t i = 0; i < count; ++i) {
		glDetachShader(program, shaders[i]);
		glDeleteShader(shaders[i]);
	}

	m_skr_last_error_clear();
	return program;
}

/**
 * @internal
 * @brief GL create program from SkrShader array (source or file).
 *
 * @param shaders_input Array of SkrShader descriptors.
 * @param count         Number of shaders.
 *
 * @return Program ID, or 0 on failure.
 */
static inline GLuint
m_skr_gl_create_program_from_shaders(const SkrShader* shaders_input,
                                     const size_t     count) {
	if (!shaders_input || count == 0) {
		m_skr_last_error_set("either shaders_input != 1 or count == 0");
		return 0;
	}

	GLuint* shaders = (GLuint*)malloc(sizeof(GLuint) * count);
	if (!shaders) {
		m_skr_last_error_set("shaders_input == NULL");
		return 0;
	}

	for (size_t i = 0; i < count; ++i) {
		const SkrShader* s = &shaders_input[i];
		GLuint           shader = 0;

		if (s->Source) {
			shader = m_skr_gl_create_shader(s->Type, s->Source);
		} else if (s->Path) {
			shader = m_skr_gl_create_shader_from_file(s->Type,
			                                          s->Path);
		} else {
			for (size_t j = 0; j < i; ++j) {
				glDeleteShader(shaders[j]);
			}
			free(shaders);
			m_skr_last_error_set(
			        "shader.Source and shader.Path are NULL");
			return 0;
		}

		if (!shader) {
			for (size_t j = 0; j < i; ++j) {
				glDeleteShader(shaders[j]);
			}
			free(shaders);
			return 0;
		}

		shaders[i] = shader;
	}

	GLuint program = m_skr_gl_create_program(shaders, count);
	free(shaders);

	if (!program) {
		return 0;
	}

	m_skr_last_error_clear();
	return program;
}

/**
 * @internal
 * @brief GL use an OpenGL shader program.
 */
static inline void m_skr_gl_shader_use(const GLuint program) {
	glUseProgram(program);
	m_skr_last_error_clear();
}

/**
 * @internal
 * @brief GL destroy an OpenGL shader program.
 *
 * @param program Pointer to program ID. Resets to 0 on success.
 */
static inline void m_skr_gl_shader_destroy(GLuint* program) {
	if (program && *program) {
		glDeleteProgram(*program);
		*program = 0;
		m_skr_last_error_clear();
	}
}

static inline void m_skr_gl_shader_set_bool(const GLuint program,
                                            const char* name, const int value) {
	glUniform1i(glGetUniformLocation(program, name), value);
	m_skr_last_error_clear();
}

static inline void m_skr_gl_shader_set_int(const GLuint program,
                                           const char* name, const int value) {
	glUniform1i(glGetUniformLocation(program, name), value);
	m_skr_last_error_clear();
}

static inline void m_skr_gl_shader_set_float(const GLuint program,
                                             const char*  name,
                                             const float  value) {
	glUniform1f(glGetUniformLocation(program, name), value);
	m_skr_last_error_clear();
}

static inline void m_skr_gl_shader_set_vec2(const GLuint program,
                                            const char*  name,
                                            const vec2   value) {
	glUniform2fv(glGetUniformLocation(program, name), 1, value);
	m_skr_last_error_clear();
}

static inline void m_skr_gl_shader_set_vec3(const GLuint program,
                                            const char*  name,
                                            const vec3   value) {
	glUniform3fv(glGetUniformLocation(program, name), 1, value);
	m_skr_last_error_clear();
}

static inline void m_skr_gl_shader_set_vec4(const GLuint program,
                                            const char*  name,
                                            const vec4   value) {
	glUniform4fv(glGetUniformLocation(program, name), 1, value);
	m_skr_last_error_clear();
}

static inline void m_skr_gl_shader_set_mat2(const GLuint program,
                                            const char*  name,
                                            const mat2   value) {
	glUniformMatrix2fv(glGetUniformLocation(program, name), 1, GL_FALSE,
	                   (const float*)value);
	m_skr_last_error_clear();
}

static inline void m_skr_gl_shader_set_mat3(const GLuint program,
                                            const char*  name,
                                            const mat3   value) {
	glUniformMatrix3fv(glGetUniformLocation(program, name), 1, GL_FALSE,
	                   (const float*)value);
	m_skr_last_error_clear();
}

static inline void m_skr_gl_shader_set_mat4(const GLuint program,
                                            const char*  name,
                                            const mat4   value) {
	glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE,
	                   (const float*)value);
	m_skr_last_error_clear();
}

static inline void m_skr_gl_renderer_init() {}

/**
 * @internal
 * @brief GL clear screen (color + depth).
 */
static inline void m_skr_gl_renderer_render(SkrState* s) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (unsigned int i = 0; i < s->ModelCount; ++i) {
		SkrModel* model = &s->Models[i];
		if (!model->Meshes)
			continue;

		for (unsigned int j = 0; j < model->MeshCount; ++j) {
			SkrMesh* mesh = &model->Meshes[j];

			if (mesh->VAO == 0 || mesh->VertexCount == 0)
				continue;

			glUseProgram(mesh->Backend.GL.Program);
			glBindVertexArray(mesh->VAO);
			glDrawArrays(GL_TRIANGLES, 0, mesh->VertexCount);
		}
	}

	glBindVertexArray(0);
}

/**
 * @internal
 * @brief GL free VAO/VBO/EBO of a mesh.
 *
 * Usually called at shutdown, not per-frame.
 */
static inline void m_skr_gl_renderer_finalize(SkrState* s) {
	if (!s)
		return;

	for (unsigned int i = 0; i < s->ModelCount; ++i) {
		SkrModel* model = &s->Models[i];
		if (!model->Meshes)
			continue;

		for (unsigned int j = 0; j < model->MeshCount; ++j) {
			SkrMesh* mesh = &model->Meshes[j];

			if (mesh->Textures && mesh->TextureCount > 0) {
				for (unsigned int t = 0; t < mesh->TextureCount;
				     ++t) {
					glDeleteTextures(1,
					                 &mesh->Textures[t].ID);
				}
			}

			if (mesh->VAO)
				glDeleteVertexArrays(1, &mesh->VAO);
			if (mesh->VBO)
				glDeleteBuffers(1, &mesh->VBO);
			if (mesh->EBO)
				glDeleteBuffers(1, &mesh->EBO);

			mesh->VAO = mesh->VBO = mesh->EBO = 0;
			mesh->Backend.GL.Program = 0;
		}
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);
}

/**
 * @internal
 * @brief GLFW check if a GLFW window should close.
 */
static inline int m_skr_gl_glfw_should_close(SkrWindow* w) {
	return glfwWindowShouldClose(w->Backend.Handler.GLFW);
}

/**
 * @internal
 * @brief GLFW Render a frame with GLFW with OpenGL.
 *
 * Calls input handler, polls events, renders, swaps buffers.
 */
static inline void m_skr_gl_glfw_renderer_render(SkrState* s) {
	if (s->Window->InputHandler) {
		s->Window->InputHandler(s->Window);
	}

	glfwGetFramebufferSize(s->Window->Backend.Handler.GLFW,
	                       &s->Window->Width, &s->Window->Height);

	m_skr_gl_renderer_render(s);

	glfwSwapBuffers(s->Window->Backend.Handler.GLFW);

	glfwPollEvents();
}

/**
 * @internal
 * @brief GL load a 2D texture from file path.
 *
 * @param path    Path to image file.
 * @param texture Output texture ID.
 *
 * @return 1 on success, 0 on failure.
 */
static inline int m_skr_gl_load_texture_2d_from_path(const char*   path,
                                                     unsigned int* texture) {
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
	                GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int            width, height, nrChannels;
	unsigned char* data =
	        m_skr_load_image_from_file(path, &width, &height, &nrChannels);
	if (!data) {
		m_skr_last_error_set("failed to load texture");
		return 0;
	}

	GLenum format = GL_RGB;
	if (nrChannels == 1)
		format = GL_RED;
	else if (nrChannels == 3)
		format = GL_RGB;
	else if (nrChannels == 4)
		format = GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
	             GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	m_skr_free_image(data);

	m_skr_last_error_clear();
	return 1;
}

/**
 * @internal
 * @brief GL load multiple 2D textures from file paths.
 */
static inline int m_skr_gl_load_textures_2d_from_paths(const char**  paths,
                                                       unsigned int* textures,
                                                       const int     count) {
	for (int i = 0; i < count; i++) {
		if (!m_skr_gl_load_texture_2d_from_path(paths[i],
		                                        &textures[i])) {
			return 0;
		}
	}

	m_skr_last_error_clear();
	return 1;
}

/**
 * @internal
 * @brief GL free an array of textures.
 */
static inline void m_skr_free_textures_2d(unsigned int* textures,
                                          const int     count) {
	if (count > 0 && textures) {
		glDeleteTextures(count, textures);
		m_skr_last_error_clear();
	}
}

static inline int SkrWindowInit(SkrWindow* w) {
	if (SKR_BACKEND_API == SKR_BACKEND_API_GL) {
		if (SKR_BACKEND_WINDOW == SKR_BACKEND_WINDOW_GLFW) {
			if (!m_skr_gl_glfw_init(w)) {
				return 0;
			}
		}
	}

	return 1;
}

static inline SkrState SkrInit(SkrWindow* w) {
	SkrState s = {0};

	if (!SkrWindowInit(w))
		return (SkrState){0};

	s.Window = w;
	return s;
}

static inline int SkrWindowShouldClose(SkrWindow* w) {
	if (SKR_BACKEND_WINDOW == SKR_BACKEND_WINDOW_GLFW) {
		return m_skr_gl_glfw_should_close(w);
	}

	return 0;
}

static inline void SkrRendererRender(SkrState* s) {
	if (!s || !s->Window)
		return;

	if (SKR_BACKEND_API == SKR_BACKEND_API_GL) {
		if (SKR_BACKEND_WINDOW == SKR_BACKEND_WINDOW_GLFW) {
			m_skr_gl_glfw_renderer_render(s);
		}
	}
}

static inline void SkrFinalize(SkrState* s) {
	if (SKR_BACKEND_API == SKR_BACKEND_API_GL) {
		m_skr_gl_renderer_finalize(s);

		if (s->Window->Backend.Type == SKR_BACKEND_WINDOW_GLFW) {
			glfwTerminate();
		}
	}

	s->Models = NULL;
	s->ModelCount = 0;
	s->Window = NULL;
}

static inline GLuint m_skr_gl_triangle(SkrState* s) {
	static SkrMesh  mesh = {0};
	static SkrModel model = {0};

	const char* triangle_vert = "#version 330 core\n"
	                            "layout (location = 0) in vec3 aPos;\n"
	                            "layout (location = 1) in vec3 aColor;\n"
	                            "out vec3 ourColor;\n"
	                            "void main() {\n"
	                            "  gl_Position = vec4(aPos, 1.0);\n"
	                            "  ourColor = aColor;\n"
	                            "}\n";

	const char* triangle_frag = "#version 330 core\n"
	                            "out vec4 FragColor;\n"
	                            "in vec3 ourColor;\n"
	                            "void main() {\n"
	                            "  FragColor = vec4(ourColor, 1.0f);\n"
	                            "}\n";

	SkrShader shaders[] = {
	        {GL_VERTEX_SHADER, triangle_vert, NULL},
	        {GL_FRAGMENT_SHADER, triangle_frag, NULL},
	};

	mesh.Backend.GL.Program =
	        m_skr_gl_create_program_from_shaders(shaders, 2);
	glUseProgram(mesh.Backend.GL.Program);

	glGenVertexArrays(1, &mesh.VAO);
	glGenBuffers(1, &mesh.VBO);
	glBindVertexArray(mesh.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);

	float vertices[] = {0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
	                    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
	                    0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f};

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
	             GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
	                      (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
	                      (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	mesh.VertexCount = 3;
	model.Meshes = &mesh;
	model.MeshCount = 1;

	s->Models = &model;
	s->ModelCount = 1;

	return mesh.Backend.GL.Program;
}

static inline void SkrTriangle(SkrState* s) {
	if (SKR_BACKEND_API == SKR_BACKEND_API_GL) {
		m_skr_gl_triangle(s);
	}
}

static inline void SkrInitCamera(SkrState* s, SkrShader vert) {}

static inline void SkrCaptureCursor(SkrState* s) {
	if (s->Window->Backend.Type == SKR_BACKEND_WINDOW_GLFW) {
		glfwSetInputMode(s->Window->Backend.Handler.GLFW, GLFW_CURSOR,
		                 GLFW_CURSOR_DISABLED);
	}
}

#ifdef __cplusplus
}
#endif

#endif /* SKR_H */
