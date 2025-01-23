#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "graphicsAPI/common/Device.h"
#include "graphicsAPI/opengl/Device.h"
#include "graphicsAPI/opengl/Context.h"

#include "renderer/ImGuiInstance.h"

#include "imgui/backends/imgui_impl_glfw.h"

int main()
{
    // Window init
    int width = 1920;
    int height = 1080;

    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(width, height, "ImGui renderer demo", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // OpenGL Context init
    auto oglContext = std::make_unique<opengl::Context>();
    std::unique_ptr<IDevice> device = std::make_unique<opengl::Device>(std::move(oglContext));

    // Initialize the ImGui context
    imgui::ImGuiInstance imguiInstance({});
    imguiInstance.initialize(*device, width, height);

    // Note that we are still using ImGui's GLFW implementation because
    // it's not really of our interest to implement our own GLFW implementation
    // as it doesn't really touch any part of the actual rendering code.
    // We might as well just let ImGui handle it.
    // It basically feeds all the needed input to the ImGui context without us having to do anything.
    ImGui_ImplGlfw_InitForOpenGL(window, true);

    glfwGetWindowSize(window, &width, &height);


    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Begin ImGui frame
        ImGui_ImplGlfw_NewFrame();
        imguiInstance.beginFrame();

        // Here we can have some ImGui code that would let the user
        {
            ImGui::Begin("Menu");// Create a window called "Hello, world!" and append into it.
            ImGui::Text("Hello, world!");
            ImGui::ShowDemoWindow();
            ImGui::End();// End of ImGui window
        }

        // End ImGui frame
        imguiInstance.endFrame();

        // After ending the ImGui frame, we render the ImGui frame onto the current frame
        {
            auto commandPool = device->createCommandPool({});
            auto commandBuffer = commandPool->acquireGraphicsCommandBuffer({});

            const RenderPassBeginDesc renderPassDesc = {
                    .renderPass = {
                            .colorAttachments = {
                                    RenderPassDesc::ColorAttachmentDesc{
                                            LoadAction::Clear,
                                            StoreAction::DontCare}}},
                    // Currently opengl's default framebuffer is implemented as just using nullptr in the render pass
                    // However, we could have a framebuffer object that would represent the default framebuffer and we could use it here
                    .framebuffer = nullptr};

            // Execute the imgui rendering commands
            commandBuffer->beginRenderPass(renderPassDesc);
            imguiInstance.renderFrame(*device, *commandBuffer, nullptr, width, height);
            commandBuffer->endRenderPass();

            commandPool->submitCommandBuffer(std::move(commandBuffer));
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}