#ifndef ENGINE_PIPELINE_H
#define ENGINE_PIPELINE_H

#include <string>
#include <vector>
#include "Core/Device.h"
#include "Utility/Corvus.h"
#include "Renderer/Shader.h"

namespace Corvus
{
    class Pipeline
    {
    public:
        Pipeline(std::shared_ptr<Device> device, const std::string &vertexShader, const std::string &fragmentShader);
        ~Pipeline();

        [[nodiscard]] VkPipeline getPipeline() const { return m_Pipeline; }
        [[nodiscard]] VkPipelineLayout getPipelineLayout() const { return m_PipelineLayout; }

    private:
        std::shared_ptr<Device> m_Device;
        Shader m_VertexShader;
        Shader m_FragmentShader;

        VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
        VkPipeline m_Pipeline = VK_NULL_HANDLE;

        static std::vector<char> readFile(const std::string &filename);
        void createGraphicsPipeline();
    };

} // Corvus

#endif //ENGINE_PIPELINE_H
