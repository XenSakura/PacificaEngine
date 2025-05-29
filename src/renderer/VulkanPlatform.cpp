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
