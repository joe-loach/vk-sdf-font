#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VkBootstrap.h"
#include <iostream>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

GLFWwindow* create_window_glfw(const char* window_name = "", bool resize = true) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    if (!resize) glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    return glfwCreateWindow(1024, 1024, window_name, NULL, NULL);
}

void destroy_window_glfw(GLFWwindow* window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool init_vulkan() {
    auto window = create_window_glfw("vk-sdf-font");

    vkb::InstanceBuilder builder;
    auto inst_ret = builder.set_app_name("vk-sdf-font")
                        .request_validation_layers()
                        .use_default_debug_messenger()
                        .build();
    if (!inst_ret) {
        std::cerr << "Failed to create Vulkan instance. Error: "
                  << inst_ret.error().message() << "\n";
        return false;
    }

    vkb::Instance vkb_inst = inst_ret.value();

    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkResult err = glfwCreateWindowSurface(vkb_inst.instance, window, nullptr, &surface);
    if (err != VK_SUCCESS) {
        return false;
    }

    vkb::PhysicalDeviceSelector selector{vkb_inst};
    auto phys_ret =
        selector.set_surface(surface)
            .set_minimum_version(1, 1) // require a vulkan 1.1 capable device
            .require_dedicated_transfer_queue()
            .select();
    if (!phys_ret) {
        std::cerr << "Failed to select Vulkan Physical Device. Error: "
                  << phys_ret.error().message() << "\n";
        return false;
    }

    vkb::DeviceBuilder device_builder{phys_ret.value()};
    // automatically propagate needed data from instance & physical device
    auto dev_ret = device_builder.build();
    if (!dev_ret) {
        std::cerr << "Failed to create Vulkan device. Error: "
                  << dev_ret.error().message() << "\n";
        return false;
    }
    vkb::Device vkb_device = dev_ret.value();

    // Get the VkDevice handle used in the rest of a vulkan application
    VkDevice device = vkb_device.device;

    // Get the graphics queue with a helper function
    auto graphics_queue_ret = vkb_device.get_queue(vkb::QueueType::graphics);
    if (!graphics_queue_ret) {
        std::cerr << "Failed to get graphics queue. Error: "
                  << graphics_queue_ret.error().message() << "\n";
        return false;
    }
    VkQueue graphics_queue = graphics_queue_ret.value();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    destroy_window_glfw(window);

    return true;
}