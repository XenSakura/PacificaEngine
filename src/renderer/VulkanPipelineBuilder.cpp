#include "VulkanPipelineBuilder.h"


VulkanPipelineBuilder::VulkanPipelineBuilder(const vk::Device& device, const VulkanSwapchain& swapchain)
	:m_device(device),
	m_swapchain(swapchain)
{

}

void VulkanPipelineBuilder::BuildPipeline(const std::string& name, VulkanShader& vertShader, VulkanShader& fragShader)
{
	auto vertShaderStageInfo = vk::PipelineShaderStageCreateInfo{ {},
	vk::ShaderStageFlagBits::eVertex, vertShader.module, vertShader.name.c_str()};

	auto fragShaderStageInfo = vk::PipelineShaderStageCreateInfo{ {},
	vk::ShaderStageFlagBits::eFragment,fragShader.module, fragShader.name.c_str()};

	auto pipelineShaderStages = std::vector<vk::PipelineShaderStageCreateInfo>{ vertShaderStageInfo, fragShaderStageInfo };

	auto vertexInputInfo = vk::PipelineVertexInputStateCreateInfo{ {}, 0u, nullptr, 0u, nullptr };

	auto inputAssembly =
		vk::PipelineInputAssemblyStateCreateInfo{ {}, vk::PrimitiveTopology::eTriangleList, false };

	auto viewport = vk::Viewport{ 0.0f, 0.0f, static_cast<float>(m_swapchain.m_width), static_cast<float>(m_swapchain.m_height), 0.0f, 1.0f };

	auto scissor = vk::Rect2D{ {0, 0}, m_swapchain.m_extent };

    auto viewportState = vk::PipelineViewportStateCreateInfo{ {}, 1, &viewport, 1, &scissor };

    auto rasterizer = vk::PipelineRasterizationStateCreateInfo{ {}, /*depthClamp*/ false,
        /*rasterizeDiscard*/ false, vk::PolygonMode::eFill, {},
        /*frontFace*/ vk::FrontFace::eCounterClockwise, {}, {}, {}, {}, 1.0f };

    auto multisampling = vk::PipelineMultisampleStateCreateInfo{ {}, vk::SampleCountFlagBits::e1, false, 1.0 };

    auto colorBlendAttachment = vk::PipelineColorBlendAttachmentState{ {}, /*srcCol*/ vk::BlendFactor::eOne,
        /*dstCol*/ vk::BlendFactor::eZero, /*colBlend*/ vk::BlendOp::eAdd,
        /*srcAlpha*/ vk::BlendFactor::eOne, /*dstAlpha*/ vk::BlendFactor::eZero,
        /*alphaBlend*/ vk::BlendOp::eAdd,
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA };

    auto colorBlending = vk::PipelineColorBlendStateCreateInfo{ {}, /*logicOpEnable=*/false,
        vk::LogicOp::eCopy, /*attachmentCount=*/1, /*colourAttachments=*/&colorBlendAttachment };

    auto pipelineLayout = m_device.createPipelineLayout({}, nullptr);

    auto colorAttachment = vk::AttachmentDescription{ {}, m_swapchain.m_format, vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, {}, {}, {}, vk::ImageLayout::ePresentSrcKHR };

    auto colourAttachmentRef = vk::AttachmentReference{ 0, vk::ImageLayout::eColorAttachmentOptimal };

    auto subpass = vk::SubpassDescription{ {}, vk::PipelineBindPoint::eGraphics,
        /*inAttachmentCount*/ 0, nullptr, 1, &colourAttachmentRef };

    //refactor out into its own thing
    auto semaphoreCreateInfo = vk::SemaphoreCreateInfo{};
    auto imageAvailableSemaphore = m_device.createSemaphoreUnique(semaphoreCreateInfo);
    auto renderFinishedSemaphore = m_device.createSemaphoreUnique(semaphoreCreateInfo);

    auto subpassDependency = vk::SubpassDependency{ VK_SUBPASS_EXTERNAL, 0,
        vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput,
        {}, vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite };

    auto renderPass = m_device.createRenderPassUnique(
        vk::RenderPassCreateInfo{ {}, 1, &colorAttachment, 1, &subpass, 1, &subpassDependency });

    auto pipelineCreateInfo = vk::GraphicsPipelineCreateInfo{ {}, 2, pipelineShaderStages.data(),
        &vertexInputInfo, &inputAssembly, nullptr, &viewportState, &rasterizer, &multisampling,
        nullptr, &colorBlending, nullptr, pipelineLayout, *renderPass, 0 };

    auto pipeline = m_device.createGraphicsPipeline({}, pipelineCreateInfo).value;

    m_pipelineMap[name] = VulkanPipeline{ name, pipeline,  pipelineLayout};
}

VulkanPipeline& VulkanPipelineBuilder::GetPipeline(const std::string& name)
{
	return m_pipelineMap[name];
}
VulkanPipelineBuilder::~VulkanPipelineBuilder()
{
    for (auto& [name, pipeline] : m_pipelineMap)
    {
        m_device.destroyPipeline(pipeline.m_pipeline);
        m_device.destroyPipelineLayout(pipeline.m_layout);
    }
}
