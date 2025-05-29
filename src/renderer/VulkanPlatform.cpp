#include "VulkanPlatform.h"
#include <memory>

#include <set>
#include <shaderc/shaderc.hpp>




VulkanPlatform::VulkanPlatform()
    :m_window(),
    m_instance(),
    m_debugMessenger(m_instance.m_instance, m_instance.m_dldi),
    m_surface(m_instance.m_instance, m_window.window)
{

    

    auto vertShaderStageInfo = vk::PipelineShaderStageCreateInfo{ {},
        vk::ShaderStageFlagBits::eVertex, *vertexShaderModule, "main" };

    auto fragShaderStageInfo = vk::PipelineShaderStageCreateInfo{ {},
        vk::ShaderStageFlagBits::eFragment, *fragmentShaderModule, "main" };

    auto pipelineShaderStages =
        std::vector<vk::PipelineShaderStageCreateInfo>{ vertShaderStageInfo, fragShaderStageInfo };

    auto vertexInputInfo = vk::PipelineVertexInputStateCreateInfo{ {}, 0u, nullptr, 0u, nullptr };

    auto inputAssembly =
        vk::PipelineInputAssemblyStateCreateInfo{ {}, vk::PrimitiveTopology::eTriangleList, false };

    auto viewport =
        vk::Viewport{ 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };

    auto scissor = vk::Rect2D{ { 0, 0 }, extent };

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

    auto pipelineLayout = device->createPipelineLayoutUnique({}, nullptr);

    auto colorAttachment = vk::AttachmentDescription{ {}, format, vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, {}, {}, {}, vk::ImageLayout::ePresentSrcKHR };

    auto colourAttachmentRef = vk::AttachmentReference{ 0, vk::ImageLayout::eColorAttachmentOptimal };

    auto subpass = vk::SubpassDescription{ {}, vk::PipelineBindPoint::eGraphics,
        /*inAttachmentCount*/ 0, nullptr, 1, &colourAttachmentRef };

    auto semaphoreCreateInfo = vk::SemaphoreCreateInfo{};
    auto imageAvailableSemaphore = device->createSemaphoreUnique(semaphoreCreateInfo);
    auto renderFinishedSemaphore = device->createSemaphoreUnique(semaphoreCreateInfo);

    auto subpassDependency = vk::SubpassDependency{ VK_SUBPASS_EXTERNAL, 0,
        vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput,
        {}, vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite };

    auto renderPass = device->createRenderPassUnique(
        vk::RenderPassCreateInfo{ {}, 1, &colorAttachment, 1, &subpass, 1, &subpassDependency });

    auto pipelineCreateInfo = vk::GraphicsPipelineCreateInfo{ {}, 2, pipelineShaderStages.data(),
        &vertexInputInfo, &inputAssembly, nullptr, &viewportState, &rasterizer, &multisampling,
        nullptr, &colorBlending, nullptr, *pipelineLayout, *renderPass, 0 };

    auto pipeline = device->createGraphicsPipelineUnique({}, pipelineCreateInfo).value;

    auto framebuffers = std::vector<vk::UniqueFramebuffer>(imageCount);
    for (size_t i = 0; i < imageViews.size(); i++) {
        framebuffers[i] = device->createFramebufferUnique(vk::FramebufferCreateInfo{
            {}, *renderPass, 1, &(*imageViews[i]), extent.width, extent.height, 1 });
    }
    auto commandPoolUnique =
        device->createCommandPoolUnique({ {}, static_cast<uint32_t>(graphicsQueueFamilyIndex) });

    std::vector<vk::UniqueCommandBuffer> commandBuffers =
        device->allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo(commandPoolUnique.get(),
            vk::CommandBufferLevel::ePrimary, static_cast<uint32_t>(framebuffers.size())));


    for (size_t i = 0; i < commandBuffers.size(); i++) {

        auto beginInfo = vk::CommandBufferBeginInfo{};
        commandBuffers[i]->begin(beginInfo);
        vk::ClearValue clearValues{};
        auto renderPassBeginInfo = vk::RenderPassBeginInfo{ renderPass.get(), framebuffers[i].get(),
            vk::Rect2D{ { 0, 0 }, extent }, 1, &clearValues };

        commandBuffers[i]->beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
        commandBuffers[i]->bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline);
        commandBuffers[i]->draw(3, 1, 0, 0);
        commandBuffers[i]->endRenderPass();
        commandBuffers[i]->end();
    }

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        auto imageIndex = device->acquireNextImageKHR(swapChain.get(),
            std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore.get(), {});

        vk::PipelineStageFlags waitStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;

        auto submitInfo = vk::SubmitInfo{ 1, &imageAvailableSemaphore.get(), &waitStageMask, 1,
            &commandBuffers[imageIndex.value].get(), 1, &renderFinishedSemaphore.get() };

        deviceQueue.submit(submitInfo, {});

        auto presentInfo = vk::PresentInfoKHR{ 1, &renderFinishedSemaphore.get(), 1,
            &swapChain.get(), &imageIndex.value };
        auto result = presentQueue.presentKHR(presentInfo);

        device->waitIdle();
    }

}

VulkanPlatform::~VulkanPlatform()
{
	
}
