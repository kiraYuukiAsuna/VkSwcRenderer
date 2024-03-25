#include "ShaderCode.h"

std::vector<char> ShaderCode::readShaderFile(const std::filesystem::path &filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if(!file.is_open()){
       throw std::runtime_error("Open Shader File Failed!");
    }

    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

vk::ShaderModule ShaderCode::createShaderModule(const std::vector<char> &code, vk::Device device) {
    vk::ShaderModuleCreateInfo createInfo;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    return device.createShaderModule(createInfo);
}
