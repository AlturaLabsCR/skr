#include <stdio.h>

#include "skr.h"

#include <GL/glew.h>

int main(void) {
	SkrWindow window = {
	        .Title = "Hello SKR",
	        .Width = 800,
	        .Height = 600,
	};

	if (!m_skr_gl_glfw_init(&window)) {
		fprintf(stderr, "Failed to init GLFW: %s\n", SKR_LAST_ERROR);
		return 1;
	}

	// Initialize GLEW after creating context
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to init GLEW\n");
		return 1;
	}

	while (!m_skr_gl_glfw_should_close(&window)) {
		m_skr_gl_renderer_render();
		glfwSwapBuffers(window.Backend.Handler.GLFW);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
