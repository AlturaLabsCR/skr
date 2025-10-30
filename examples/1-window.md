# Empty window

```c
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#define SKR_BACKEND_API 0    // opengl
#define SKR_BACKEND_WINDOW 0 // glfw
#include <skr/skr.h>

int main(void) {
	SkrState state = SkrInit(&(SkrWindow){
	        .Title = "Hello SKR",
	        .Width = 800,
	        .Height = 600,
	});

	if (!SKR_OK) {
		fprintf(stderr, "Failed to init window: %s\n", SKR_LAST_ERROR);
		return 1;
	}

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to init GLEW\n");
		return 1;
	}

	// SkrTriangle(&state);

	while (!SkrWindowShouldClose(state.Window)) {
		SkrRendererRender(&state);
	}

	SkrFinalize(&state);
	return 0;
}
```
