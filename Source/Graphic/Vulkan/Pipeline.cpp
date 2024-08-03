#include "Pipeline.h"
#include <fstream>
#include <utility>
#include "Utility/Log.h"
#include "Vertex.h"

namespace Corvus
{
    Pipeline::Pipeline(
        std::shared_ptr<Device> device, const std::string& vertexShader,
        const std::string& fragmentShader
    )
        : m_Device(std::move(device)),
          m_VertexShader("Vertex", readFile(vertexShader), m_Device),
          m_FragmentShader("Fragment", readFile(fragmentShader), m_Device)
    {
        createDescriptorSetLayout();
        createGraphicsPipeline();
    }

    Pipeline::~Pipeline()
    {
        auto device = m_Device->getDevice();
        vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);
        vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);
        vkDestroyPipeline(device, m_Pipeline, nullptr);
    }

    std::vector<char> Pipeline::readFile(const std::string& filename)
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
        file.read(buffer.data(), (long long)fileSize);
        file.close();
        return buffer;
    }

    void Pipeline::createDescriptorSetLayout()
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding = {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = nullptr
        };

        VkDescriptorSetLayoutCreateInfo layoutInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = 1,
            .pBindings = &uboLayoutBinding
        };

        auto success = vkCreateDescriptorSetLayout(m_Device->getDevice(), &layoutInfo, nullptr, &m_DescriptorSetLayout);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create descriptor set layout!")
    }

    void Pipeline::createGraphicsPipeline()
    {
        std::vector dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
            .pDynamicStates = dynamicStates.data()
        };

        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = 1,
            .pVertexBindingDescriptions = &bindingDescription,
            .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
            .pVertexAttributeDescriptions = attributeDescriptions.data()
        };

        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE
        };

        VkPipelineViewportStateCreateInfo viewportState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = 1,
            .scissorCount = 1,
        };

        VkPipelineRasterizationStateCreateInfo rasterizer = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .frontFace = VK_FRONT_FACE_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .lineWidth = 1.0f,
        };

        VkPipelineMultisampleStateCreateInfo multisampling = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 1.0f,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE
        };

        VkPipelineColorBlendAttachmentState colorBlendAttachment = {
            .blendEnable = VK_FALSE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD,

            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT
        };

        VkPipelineColorBlendStateCreateInfo colorBlending = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments = &colorBlendAttachment,
            .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
        };

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = 1,
            .pSetLayouts = &m_DescriptorSetLayout,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr
        };

        auto success = vkCreatePipelineLayout(m_Device->getDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create pipeline layout!")
        CORVUS_LOG(info, "Pipeline layout created successfully!");

        VkPipelineShaderStageCreateInfo shaderStages[] = {
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = m_VertexShader.getModule(),
                .pName = "main"
            },
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = m_FragmentShader.getModule(),
                .pName = "main"
            }
        };

        VkGraphicsPipelineCreateInfo pipelineInfo = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .stageCount = 2,
            .pStages = shaderStages,
            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &inputAssembly,
            .pViewportState = &viewportState, // viewport and scissor are dynamic
            .pRasterizationState = &rasterizer,
            .pMultisampleState = &multisampling,
            .pColorBlendState = &colorBlending,
            .pDynamicState = &dynamicState,
            .layout = m_PipelineLayout,
            .renderPass = m_Device->getRenderPass(),
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1
        };

        success = vkCreateGraphicsPipelines(m_Device->getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr,
                                            &m_Pipeline);
        CORVUS_ASSERT(success == VK_SUCCESS, "Failed to create graphics pipeline!")
        CORVUS_LOG(info, "Graphics pipeline created successfully!");
    }
} // Corvus
