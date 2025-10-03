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

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
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
 * @see SKR_LAST_ERROR_CLEAR
 * @see SKR_LAST_ERROR_SET
 * @see SKR_OK
 */
static char SKR_LAST_ERROR[SKR_LAST_ERROR_SIZE];

/**
 * @internal
 * @brief Clears the global error buffer.
 *
 * Sets the first character of `SKR_LAST_ERROR` to '\0', effectively removing
 * any previously stored error message.
 *
 * @usage
 * SKR_LAST_ERROR_CLEAR();
 */
#define SKR_LAST_ERROR_CLEAR() (SKR_LAST_ERROR[0] = '\0')

/**
 * @internal
 * @brief Sets the global error message.
 *
 * Formats and writes a message into `SKR_LAST_ERROR` using `snprintf()`
 * semantics. Any previous error is overwritten.
 *
 * @param fmt Format string (like printf).
 * @param ... Optional arguments for the format string.
 *
 * @details
 * This macro stores the last error encountered by the application in a global
 * buffer of size `SKR_LAST_ERROR_SIZE`. It is safe and bounded by the buffer
 * size.
 *
 * @usage
 * if (!init_graphics()) {
 *     SKR_LAST_ERROR_SET("Graphics init failed: %s", reason);
 *     return false;
 * }
 */
#define SKR_LAST_ERROR_SET(fmt, ...)                                           \
	snprintf(SKR_LAST_ERROR, SKR_LAST_ERROR_SIZE, fmt, ##__VA_ARGS__)

/**
 * @brief Checks if there is no error set.
 *
 * Returns true (nonzero) if `SKR_LAST_ERROR` is empty, false (0) otherwise.
 *
 * @return int 1 if no error, 0 if an error exists.
 *
 * @usage
 * if (!SKR_OK()) {
 *     fprintf(stderr, "Error: %s\n", SKR_LAST_ERROR);
 * }
 */
#define SKR_OK() (SKR_LAST_ERROR[0] == '\0')

/**
 * @brief Maximum number of bone influences per vertex.
 *
 * Each vertex can be affected by up to this many bones during skeletal
 * animation. Commonly set to 4, since most real-time rendering pipelines
 * balance flexibility with performance by limiting to four weights per vertex.
 */
#define MAX_BONE_INFLUENCE 4

/**
 * @brief Identifies the type of window backend in use.
 */
typedef enum SkrWindowBackendType {
	SkrGLFW, /*!< Window created using GLFW. */
	SkrSDL   /*!< Window created using SDL. */
} SkrWindowBackendType;

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
	float Position[3];

	/**
	 * @brief Vertex normal vector.
	 *
	 * Used for lighting calculations. Three components (x, y, z).
	 */
	float Normal[3];

	/**
	 * @brief Texture coordinates (UV).
	 *
	 * Two-component vector (u, v), typically in the range [0, 1].
	 */
	float UV[2];

	/**
	 * @brief Tangent vector.
	 *
	 * Defines the direction of increasing U in the tangent space.
	 * Used for normal mapping. Three components (x, y, z).
	 */
	float Tangent[3];

	/**
	 * @brief Bitangent vector.
	 *
	 * Defines the direction of increasing V in the tangent space.
	 * Orthogonal to both the normal and tangent. Three components (x, y,
	 * z).
	 */
	float Bitangent[3];

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
	SkrVertex* Vertices;

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
	SkrTexture* Textures;
} SkrMesh;

/**
 * @brief 3D model representation.
 *
 * A model consists of one or more meshes, each with its own vertices, indices,
 * and material textures. The model may also reference textures that are shared
 * across meshes.
 */
typedef struct SkrModel {
	SkrTexture* Textures; /*!< Array of textures used by model’s meshes. */
	SkrMesh*    Meshes;   /*!< Array of meshes that compose the model. */
	char*       Path;     /*!< Filesystem path of the model file. */
} SkrModel;

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
 *         RGB vs. RGBA) is determined by the user’s implementation. Returns
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
		SKR_LAST_ERROR_SET("failed to open");
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	long len = ftell(file);
	rewind(file);

	char* buffer = (char*)malloc(len + 1);
	if (!buffer) {
		fclose(file);
		SKR_LAST_ERROR_SET("failed to open");
		return NULL;
	}

	fread(buffer, 1, len, file);
	buffer[len] = '\0';
	fclose(file);

	SKR_LAST_ERROR_CLEAR();
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
	SKR_LAST_ERROR_CLEAR();
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
	m_skr_gl_framebuffer_size_callback(width, height);
	SKR_LAST_ERROR_CLEAR();
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
		SKR_LAST_ERROR_SET("either glfwInit != 1 or SkrWindow == NULL");
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
		SKR_LAST_ERROR_SET("window backend is NULL");
		glfwTerminate();
		return 0;
	}

	glfwSetFramebufferSizeCallback(w->Backend.Handler.GLFW,
	                               m_skr_gl_glfw_framebuffer_size_callback);
	glfwMakeContextCurrent(w->Backend.Handler.GLFW);

	SKR_LAST_ERROR_CLEAR();
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
			SKR_LAST_ERROR_SET("failed to link %s: %s", type,
			                   infoLog);
			return 0;
		}
	} else {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, sizeof(infoLog), NULL,
			                   infoLog);
			SKR_LAST_ERROR_SET("failed to compile %s: %s", type,
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
	case GL_COMPUTE_SHADER:
		type_str = "comp";
		break;
	case GL_TESS_CONTROL_SHADER:
		type_str = "tesc";
		break;
	case GL_TESS_EVALUATION_SHADER:
		type_str = "tese";
		break;
	default:
		type_str = "unknown";
		break;
	}

	if (!m_skr_gl_check_compile_errors(shader, type_str)) {
		glDeleteShader(shader);
		return 0;
	}

	SKR_LAST_ERROR_CLEAR();
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

	SKR_LAST_ERROR_CLEAR();
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

	SKR_LAST_ERROR_CLEAR();
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
		SKR_LAST_ERROR_SET("either shaders_input != 1 or count == 0");
		return 0;
	}

	GLuint* shaders = (GLuint*)malloc(sizeof(GLuint) * count);
	if (!shaders) {
		SKR_LAST_ERROR_SET("shaders_input == NULL");
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
			SKR_LAST_ERROR_SET(
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

	SKR_LAST_ERROR_CLEAR();
	return program;
}

/**
 * @internal
 * @brief GL use an OpenGL shader program.
 */
static inline void m_skr_gl_shader_use(const GLuint program) {
	glUseProgram(program);
	SKR_LAST_ERROR_CLEAR();
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
		SKR_LAST_ERROR_CLEAR();
	}
}

static inline void m_skr_gl_shader_set_bool(const GLuint program,
                                            const char* name, const int value) {
	glUniform1i(glGetUniformLocation(program, name), value);
	SKR_LAST_ERROR_CLEAR();
}

static inline void m_skr_gl_shader_set_int(const GLuint program,
                                           const char* name, const int value) {
	glUniform1i(glGetUniformLocation(program, name), value);
	SKR_LAST_ERROR_CLEAR();
}

static inline void m_skr_gl_shader_set_float(const GLuint program,
                                             const char*  name,
                                             const float  value) {
	glUniform1f(glGetUniformLocation(program, name), value);
	SKR_LAST_ERROR_CLEAR();
}

static inline void m_skr_gl_shader_set_vec2(const GLuint program,
                                            const char*  name,
                                            const vec2   value) {
	glUniform2fv(glGetUniformLocation(program, name), 1, value);
	SKR_LAST_ERROR_CLEAR();
}

static inline void m_skr_gl_shader_set_vec3(const GLuint program,
                                            const char*  name,
                                            const vec3   value) {
	glUniform3fv(glGetUniformLocation(program, name), 1, value);
	SKR_LAST_ERROR_CLEAR();
}

static inline void m_skr_gl_shader_set_vec4(const GLuint program,
                                            const char*  name,
                                            const vec4   value) {
	glUniform4fv(glGetUniformLocation(program, name), 1, value);
	SKR_LAST_ERROR_CLEAR();
}

static inline void m_skr_gl_shader_set_mat2(const GLuint program,
                                            const char*  name,
                                            const mat2   value) {
	glUniformMatrix2fv(glGetUniformLocation(program, name), 1, GL_FALSE,
	                   (const float*)value);
	SKR_LAST_ERROR_CLEAR();
}

static inline void m_skr_gl_shader_set_mat3(const GLuint program,
                                            const char*  name,
                                            const mat3   value) {
	glUniformMatrix3fv(glGetUniformLocation(program, name), 1, GL_FALSE,
	                   (const float*)value);
	SKR_LAST_ERROR_CLEAR();
}

static inline void m_skr_gl_shader_set_mat4(const GLuint program,
                                            const char*  name,
                                            const mat4   value) {
	glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE,
	                   (const float*)value);
	SKR_LAST_ERROR_CLEAR();
}

static inline void m_skr_gl_renderer_init() {}

/**
 * @internal
 * @brief GL clear screen (color + depth).
 */
static inline void m_skr_gl_renderer_render(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/**
 * @internal
 * @brief GL free VAO/VBO/EBO of a mesh.
 *
 * Usually called at shutdown, not per-frame.
 */
static inline void m_skr_gl_renderer_finalize(SkrMesh* m) {
	glDeleteVertexArrays(1, &m->VAO);
	glDeleteBuffers(1, &m->VBO);
	glDeleteBuffers(1, &m->EBO);
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
static inline void m_skr_gl_glfw_renderer_render(SkrWindow* w, SkrMesh* m) {
	if (w->InputHandler) {
		w->InputHandler(w);
	}

	glfwPollEvents();
	glfwGetFramebufferSize(w->Backend.Handler.GLFW, &w->Width, &w->Height);

	m_skr_gl_renderer_render();

	glfwSwapBuffers(w->Backend.Handler.GLFW);
}

/**
 * @internal
 * @brief GLFW Shutdown OpenGL renderer and GLFW.
 */
static inline void m_skr_gl_glfw_renderer_finalize(SkrMesh* m) {
	m_skr_gl_renderer_finalize(m);

	glfwTerminate();
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
		SKR_LAST_ERROR_SET("failed to load texture");
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

	SKR_LAST_ERROR_CLEAR();
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

	SKR_LAST_ERROR_CLEAR();
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
		SKR_LAST_ERROR_CLEAR();
	}
}

#ifdef __cplusplus
}
#endif

#endif // SKR_H
