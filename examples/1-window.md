# Empty window

```c
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#define SKR_BACKEND_API 0    // opengl
#define SKR_BACKEND_WINDOW 0 // glfw
#include <skr/skr.h>

int main(void) {
	SkrState state = SkrInit(
	        &(SkrWindow){
	                .Title = "Hello SKR",
	                .Width = 800,
	                .Height = 600,
	        },
	        SKR_BACKEND_API_GL);

	glewInit();

	SkrTriangle(&state);

	while (!SkrShouldClose(&state)) {
		SkrRendererRender(&state);
	}

	return 0;
}
```
