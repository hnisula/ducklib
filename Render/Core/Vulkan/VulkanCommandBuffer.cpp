#include "VulkanCommandBuffer.h"
#include "VulkanCommon.h"
#include "VulkanFrameBuffer.h"
#include "VulkanPass.h"

namespace DuckLib::Render
{
void* VulkanCommandBuffer::GetApiHandle() const
{
	return vkCommandBuffer;
}

void VulkanCommandBuffer::Reset()
{
	DL_VK_CHECK(vkResetCommandBuffer(vkCommandBuffer, 0), "Failed to reset Vulkan command buffer");
}

void VulkanCommandBuffer::Begin()
{
	VkCommandBufferBeginInfo beginInfo{};

	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;

	DL_VK_CHECK(vkBeginCommandBuffer(vkCommandBuffer, &beginInfo), "Failed to begin Vulkan command buffer");
}

void VulkanCommandBuffer::End()
{
	DL_VK_CHECK(vkEndCommandBuffer(vkCommandBuffer), "Failed to end Vulkan command buffer");
}

void VulkanCommandBuffer::BeginPass(const IPass* pass, const IFrameBuffer* frameBuffer)
{
	const VulkanPass* vulkanPass = (const VulkanPass*)pass;
	const VulkanFrameBuffer* vulkanFrameBuffer = (const VulkanFrameBuffer*)frameBuffer;
	VkRenderPassBeginInfo beginPassInfo{};
	VkClearValue clearValue;

	clearValue.color = VkClearColorValue{ .float32 = { clearColorRgba[0], clearColorRgba[1], clearColorRgba[2], clearColorRgba[3] } };
	clearValue.depthStencil = VkClearDepthStencilValue{ 1.0f, 0 };

	beginPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	beginPassInfo.renderPass = vulkanPass->vkPass;
	beginPassInfo.renderArea.offset = { 0, 0 };
	beginPassInfo.renderArea.extent = vulkanFrameBuffer->vkExtent;
	beginPassInfo.clearValueCount = 1;
	beginPassInfo.pClearValues = &clearValue;
	beginPassInfo.framebuffer = vulkanFrameBuffer->vkFrameBuffer;

	vkCmdBeginRenderPass(vkCommandBuffer, &beginPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandBuffer::EndPass() {}
void VulkanCommandBuffer::SetPipelineState(PipelineState pipelineState) {}
void VulkanCommandBuffer::Transition(ImageBuffer* image, ResourceState from, ResourceState to) {}
void VulkanCommandBuffer::SetRT(ImageBuffer* rt) {}
void VulkanCommandBuffer::SetIndexBuffer(Buffer* buffer) {}
void VulkanCommandBuffer::SetVertexBuffers(Buffer** buffer, uint32 count, uint32 startSlot) {}
void VulkanCommandBuffer::SetInputDeclaration(InputDescription* inputDescription) {}
void VulkanCommandBuffer::SetViewport(const Viewport& viewport) {}
void VulkanCommandBuffer::SetScissorRect(const Rect& rect) {}
void VulkanCommandBuffer::SetPrimitiveTopology(PrimitiveTopology topology) {}
void VulkanCommandBuffer::SetVertexBuffer(uint32_t startIndex, uint32_t numViews, Buffer** buffers) {}
void VulkanCommandBuffer::Draw() {}

VulkanCommandBuffer::VulkanCommandBuffer(VkCommandBuffer vkCommandBuffer)
	: vkCommandBuffer(vkCommandBuffer) {}
}
