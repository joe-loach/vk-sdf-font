#pragma once

#include "io.hpp"
#include "vulkan.hpp"

#include <expected>

auto compute_pipeline(const Init& init) -> std::expected<VkPipeline, io::io_error> {
    auto device = init.device.device;

    auto shader_bytes_res = io::read_file_bytes("shaders/comp.spv");

    if (!shader_bytes_res) {
        return std::unexpected(shader_bytes_res.error());
    }

    VkShaderModule compute_shader = create_shader_module(init, shader_bytes_res.value());

    VkPipelineShaderStageCreateInfo compute_stage_info{};
    compute_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    compute_stage_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    compute_stage_info.module = compute_shader;
    compute_stage_info.pName = "main";

    // Bindings and set layout
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    // TODO: layout bindings from compute shader
    // layoutInfo.bindingCount = 3;
    // layoutInfo.pBindings = layoutBindings.data();

    VkDescriptorSetLayout set_layout;

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &set_layout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute descriptor set layout!");
    }

    // Compute Pipeline info
    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 1;
    pipeline_layout_info.pSetLayouts = &set_layout;

    VkPipelineLayout pipeline_layout;

    if (vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute pipeline layout!");
    }

    // Compute Pipeline
    VkComputePipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipeline_info.layout = pipeline_layout;
    pipeline_info.stage = compute_stage_info;

    VkPipeline compute_pipeline;
    if (vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &compute_pipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute pipeline!");
    }

    vkDestroyShaderModule(device, compute_shader, nullptr);

    return compute_pipeline;
}