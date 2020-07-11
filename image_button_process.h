#pragma once
#include <string>
#include <optional>
#include <glad/glad.h>
#include <glfw\glfw3.h>


struct image_button_processor {
	mutable std::string path;
	mutable std::optional<std::string> processed_path;
	mutable bool path_switch{ false };
	mutable int my_image_width = 0;
	mutable int my_image_height = 0;
	mutable GLuint my_image_texture = 0;
	void load_button_press(bool &pressed);
};