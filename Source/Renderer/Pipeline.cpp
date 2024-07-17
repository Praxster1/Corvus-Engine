
#include "Pipeline.h"
#include <fstream>
#include <utility>
#include "Utility/Log.h"

namespace Corvus
{

    Pipeline::Pipeline(std::shared_ptr<Device> device, const std::string &vertexShader,
                       const std::string &fragmentShader)
            : m_Device(std::move(device)),
                m_VertexShader("Vertex", readFile(vertexShader), m_Device),
                m_FragmentShader("Fragment", readFile(fragmentShader), m_Device)
    {
    }

    Pipeline::~Pipeline() = default;

    std::vector<char> Pipeline::readFile(const std::string &filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (not file.is_open())
        {
            CORVUS_LOG(error, "Failed to open file: {}", filename);
            return {};
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), (long long) fileSize);
        file.close();
        return buffer;
    }

    void Pipeline::createGraphicsPipeline()
    {
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = m_VertexShader.getModule(),
                .pName = "main"
        };

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = m_FragmentShader.getModule(),
                .pName = "main"
        };

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        std::vector<VkDynamicState> dynamicStates = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();
    }


} // Corvus
