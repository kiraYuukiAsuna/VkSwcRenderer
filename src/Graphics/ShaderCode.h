#pragma once

#include <fstream>
#include <vector>
#include <filesystem>
#include <vulkan/vulkan.hpp>

class ShaderCode {
public:

    static std::vector<char> readShaderFile(const std::filesystem::path& filePath);

    static vk::ShaderModule createShaderModule(const std::vector<char>& code, vk::Device device);
};
