#include <iostream>
#include <windows.h>
#include <optional>

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imguifilesystem.h"  

#include "file_extension_check.h"
#include "texture_maker.h"
#include "itk_watershed_filter.h"

int main() {

	glfwInit();
    GLFWwindow* window = glfwCreateWindow(800, 600, "Image Artist", NULL, NULL);
    glfwMakeContextCurrent(window);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window \n";
        glfwTerminate();
        return -1;
    }

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD \n";
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    
    ImGui::StyleColorsDark();
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    bool load_pressed{ false };
    GLuint processed_image;
    const char* new_file;
    bool processed{ false };
    int image_width = 0;
    int image_height = 0;
    GLuint image_texture = 0;
    
    double high_val = 0.09;
    double low_val = 0.01;

    // TODO : probably better to have the user reassign
    auto reset_param = [](double& high, double& low) { if (high <= low) { high = 0.0001 + low; } };
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Image Artist");

        bool pressed = ImGui::Button("Load Image");

        static ImGuiFs::Dialog dlg;                                                    
        const char* chosenPath = dlg.chooseFileDialog(pressed);             

        if (strlen(dlg.getChosenPath()) > 0) {
           ImGui::Text("Chosen file: \"%s\"", dlg.getChosenPath());
           std::string path(dlg.getChosenPath());
                //TODO : may not want to use the boost library associated with this, complains when building with debug
           std::string ext = ia::get_file_extension(path);
           
           ImGui::PushItemWidth(75);
           ImGui::InputDouble("high value", &high_val);
           ImGui::SameLine();
           ImGui::InputDouble("low value", &low_val);

           if (ext == ".png" || ext == ".jpeg") {
               ia::load_texture(path.c_str(), image_texture, image_width, image_height);
               ImGui::Image((void*)(intptr_t)image_texture, ImVec2(image_width, image_height));
               ImGui::SameLine();
               if (ImGui::Button("Process"))
               {
                   reset_param(high_val, low_val);
                  new_file = ia::watershed_filter(dlg.getChosenPath(), high_val, low_val);
                  ia::load_texture(new_file, processed_image, image_width, image_height);
                  processed = true;
               }
               if (processed) {
                  ImGui::BeginChildFrame(2, ImVec2(image_width, image_height));
                  ImGui::Image((void*)(intptr_t)(processed_image), ImVec2(image_width, image_height));
                  ImGui::EndChildFrame();
               }
           }
        }
        ImGui::End();

        ImGui::EndFrame();
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

   
}