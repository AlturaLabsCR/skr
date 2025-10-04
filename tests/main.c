#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define SKR_BACKEND_API 0    // using opengl
#define SKR_BACKEND_WINDOW 0 // using glfw
#include "../skr/skr.h"

int main(void) {
	SkrWindow window = {
	        .Title = "Hello SKR",
	        .Width = 800,
	        .Height = 600,
	};

	SkrState state = SkrInit(&window);
	if (!SKR_OK) {
		fprintf(stderr, "Failed to init window: %s\n", SKR_LAST_ERROR);
		return 1;
	}

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to init GLEW\n");
		return 1;
	}

	while (!SkrWindowShouldClose(&window)) {
		SkrRendererRender(&state);
	}

	SkrFinalize(&state);
	return 0;
}
