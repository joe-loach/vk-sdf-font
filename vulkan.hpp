#pragma once

// #define GLFW_INCLUDE_VULKAN
// #include <GLFW/glfw3.h>

#include "VkBootstrap.h"
#include <format>
#include <iostream>
#include <optional>
#include <string>

struct Init {
    vkb::Instance instance;
    vkb::Device device;
    VkQueue compute_queue;
};

auto device_initalisation(Init &init) -> std::optional<std::string> {
    vkb::InstanceBuilder builder;
    auto inst_ret = builder.set_app_name("vk-sdf-font")
                        .request_validation_layers()
                        .use_default_debug_messenger()
                        .build();
    if (!inst_ret) {
        return std::format("Failed to create Vulkan instance. Error: {}",
                           inst_ret.error().message());
    }

    init.instance = inst_ret.value();

    vkb::PhysicalDeviceSelector selector{init.instance};
    auto phys_ret =
        selector
            .set_minimum_version(1, 1) // require a vulkan 1.1 capable device
            .require_dedicated_transfer_queue()
            .select();
    if (!phys_ret) {
        return std::format("Failed to select Vulkan Physical Device. Error: {}",
                           phys_ret.error().message());
    }

    vkb::DeviceBuilder device_builder{phys_ret.value()};

    auto dev_ret = device_builder.build();
    if (!dev_ret) {
        return std::format("Failed to create Vulkan device. Error: {}",
                           dev_ret.error().message());
    }
    vkb::Device vkb_device = dev_ret.value();

    init.device = vkb_device;

    // get the compute queue with a helper function
    auto compute_queue_ret = vkb_device.get_queue(vkb::QueueType::compute);
    if (!compute_queue_ret) {
        return std::format("Failed to get compute queue. Error: {}",
                           compute_queue_ret.error().message());
    }
    init.compute_queue = compute_queue_ret.value();

    return {};
}