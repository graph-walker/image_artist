#include "image_button_process.h"
#include <iostream>
#include <windows.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <glfw\glfw3.h>
#include "imguifilesystem.h"  

#include "file_extension_check.h"
#include "texture_maker.h"
#include "itk_watershed_filter.h"


void image_button_processor::load_button_press(bool &pressed)
{
    pressed = ImGui::Button("Load Image");
    static ImGuiFs::Dialog dlg;                                                     // one per dialog (and must be static)
    const char* chosenPath = dlg.chooseFileDialog(pressed);             // see other dialog types and the full list of arguments for advanced usage
    std::cout << chosenPath;
    if (strlen(dlg.getChosenPath()) > 0) {
        if (!path_switch){ path = dlg.getChosenPath(); }
        
        ImGui::Text("Chosen file: \"%s\"", dlg.getChosenPath());
        std::string path_e(dlg.getChosenPath());
        //TODO : may not want to use the boost library associated with this, complains when building with debug
        std::string ext = rad_proj::get_file_extension(path_e);

        if (ext == ".png" || ext == ".jpeg") {
            if (!path_switch) {
                LoadTextureFromFile(path.c_str(), &my_image_texture, &my_image_width, &my_image_height);
                ImGui::Image((void*)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));
            }
            bool process = (ImGui::Button("Process"));
                     
            ImGui::BeginChildFrame(2, ImVec2(my_image_width, my_image_height));
            int my_image_width1 = 0;
            int my_image_height1 = 0;
            GLuint my_image_texture1 = 0;
            auto new_file = watershed1(path.c_str());
            LoadTextureFromFile(new_file, &my_image_texture1, &my_image_width1, &my_image_height1);
            ImGui::Image((void*)(intptr_t)my_image_texture1, ImVec2(my_image_width1, my_image_height1));
            ImGui::EndChildFrame();         
        }
    }
}