#pragma once
#include <glad/glad.h>
#include <glfw\glfw3.h>

namespace ia {
	bool load_texture(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
}