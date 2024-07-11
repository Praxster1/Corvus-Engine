
#include "Pipeline.h"
#include <fstream>
#include "Utility/Log.h"

Pipeline::Pipeline(const std::string &vertexShader, const std::string &fragmentShader) {
    createGraphicsPipeline(vertexShader, fragmentShader);
}

Pipeline::~Pipeline() = default;

std::vector<char> Pipeline::readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (not file.is_open()) {
        CORVUS_LOG(error, "Failed to open file: {}", filename);
        return {};
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), (long long)fileSize);
    file.close();
    return buffer;
}

void Pipeline::createGraphicsPipeline(const std::string &vertexShader, const std::string &fragmentShader) {
    auto vertShaderCode = readFile(vertexShader);
    auto fragShaderCode = readFile(fragmentShader);

    if (vertShaderCode.empty() or fragShaderCode.empty()) {
        CORVUS_LOG(error, "Failed to load shaders!");
        return;
    }

    CORVUS_LOG(info, "Vertex shader loaded successfully! Size: {}", vertShaderCode.size());
    CORVUS_LOG(info, "Fragment shader loaded successfully! Size: {}", fragShaderCode.size());
}
