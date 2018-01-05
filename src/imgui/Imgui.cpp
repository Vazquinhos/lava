#include "imgui\Imgui.h"

#include <imgui.h>
#include <imgui_internal.h>
#include "imgui/imgui_impl.h"

#include "render/Shader.h"
#include "render/Device.h"
#include "render/Samplers.h"

#include "render/Window.h"

namespace
{
  static uint32_t __glsl_shader_vert_spv[] =
  {
    0x07230203,0x00010000,0x00080001,0x0000002e,0x00000000,0x00020011,0x00000001,0x0006000b,
    0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
    0x000a000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x0000000b,0x0000000f,0x00000015,
    0x0000001b,0x0000001c,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
    0x00000000,0x00030005,0x00000009,0x00000000,0x00050006,0x00000009,0x00000000,0x6f6c6f43,
    0x00000072,0x00040006,0x00000009,0x00000001,0x00005655,0x00030005,0x0000000b,0x0074754f,
    0x00040005,0x0000000f,0x6c6f4361,0x0000726f,0x00030005,0x00000015,0x00565561,0x00060005,
    0x00000019,0x505f6c67,0x65567265,0x78657472,0x00000000,0x00060006,0x00000019,0x00000000,
    0x505f6c67,0x7469736f,0x006e6f69,0x00030005,0x0000001b,0x00000000,0x00040005,0x0000001c,
    0x736f5061,0x00000000,0x00060005,0x0000001e,0x73755075,0x6e6f4368,0x6e617473,0x00000074,
    0x00050006,0x0000001e,0x00000000,0x61635375,0x0000656c,0x00060006,0x0000001e,0x00000001,
    0x61725475,0x616c736e,0x00006574,0x00030005,0x00000020,0x00006370,0x00040047,0x0000000b,
    0x0000001e,0x00000000,0x00040047,0x0000000f,0x0000001e,0x00000002,0x00040047,0x00000015,
    0x0000001e,0x00000001,0x00050048,0x00000019,0x00000000,0x0000000b,0x00000000,0x00030047,
    0x00000019,0x00000002,0x00040047,0x0000001c,0x0000001e,0x00000000,0x00050048,0x0000001e,
    0x00000000,0x00000023,0x00000000,0x00050048,0x0000001e,0x00000001,0x00000023,0x00000008,
    0x00030047,0x0000001e,0x00000002,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,
    0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040017,
    0x00000008,0x00000006,0x00000002,0x0004001e,0x00000009,0x00000007,0x00000008,0x00040020,
    0x0000000a,0x00000003,0x00000009,0x0004003b,0x0000000a,0x0000000b,0x00000003,0x00040015,
    0x0000000c,0x00000020,0x00000001,0x0004002b,0x0000000c,0x0000000d,0x00000000,0x00040020,
    0x0000000e,0x00000001,0x00000007,0x0004003b,0x0000000e,0x0000000f,0x00000001,0x00040020,
    0x00000011,0x00000003,0x00000007,0x0004002b,0x0000000c,0x00000013,0x00000001,0x00040020,
    0x00000014,0x00000001,0x00000008,0x0004003b,0x00000014,0x00000015,0x00000001,0x00040020,
    0x00000017,0x00000003,0x00000008,0x0003001e,0x00000019,0x00000007,0x00040020,0x0000001a,
    0x00000003,0x00000019,0x0004003b,0x0000001a,0x0000001b,0x00000003,0x0004003b,0x00000014,
    0x0000001c,0x00000001,0x0004001e,0x0000001e,0x00000008,0x00000008,0x00040020,0x0000001f,
    0x00000009,0x0000001e,0x0004003b,0x0000001f,0x00000020,0x00000009,0x00040020,0x00000021,
    0x00000009,0x00000008,0x0004002b,0x00000006,0x00000028,0x00000000,0x0004002b,0x00000006,
    0x00000029,0x3f800000,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,
    0x00000005,0x0004003d,0x00000007,0x00000010,0x0000000f,0x00050041,0x00000011,0x00000012,
    0x0000000b,0x0000000d,0x0003003e,0x00000012,0x00000010,0x0004003d,0x00000008,0x00000016,
    0x00000015,0x00050041,0x00000017,0x00000018,0x0000000b,0x00000013,0x0003003e,0x00000018,
    0x00000016,0x0004003d,0x00000008,0x0000001d,0x0000001c,0x00050041,0x00000021,0x00000022,
    0x00000020,0x0000000d,0x0004003d,0x00000008,0x00000023,0x00000022,0x00050085,0x00000008,
    0x00000024,0x0000001d,0x00000023,0x00050041,0x00000021,0x00000025,0x00000020,0x00000013,
    0x0004003d,0x00000008,0x00000026,0x00000025,0x00050081,0x00000008,0x00000027,0x00000024,
    0x00000026,0x00050051,0x00000006,0x0000002a,0x00000027,0x00000000,0x00050051,0x00000006,
    0x0000002b,0x00000027,0x00000001,0x00070050,0x00000007,0x0000002c,0x0000002a,0x0000002b,
    0x00000028,0x00000029,0x00050041,0x00000011,0x0000002d,0x0000001b,0x0000000d,0x0003003e,
    0x0000002d,0x0000002c,0x000100fd,0x00010038
  };

  static uint32_t __glsl_shader_frag_spv[] =
  {
    0x07230203,0x00010000,0x00080001,0x0000001e,0x00000000,0x00020011,0x00000001,0x0006000b,
    0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
    0x0007000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000009,0x0000000d,0x00030010,
    0x00000004,0x00000007,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
    0x00000000,0x00040005,0x00000009,0x6c6f4366,0x0000726f,0x00030005,0x0000000b,0x00000000,
    0x00050006,0x0000000b,0x00000000,0x6f6c6f43,0x00000072,0x00040006,0x0000000b,0x00000001,
    0x00005655,0x00030005,0x0000000d,0x00006e49,0x00050005,0x00000016,0x78655473,0x65727574,
    0x00000000,0x00040047,0x00000009,0x0000001e,0x00000000,0x00040047,0x0000000d,0x0000001e,
    0x00000000,0x00040047,0x00000016,0x00000022,0x00000000,0x00040047,0x00000016,0x00000021,
    0x00000000,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,
    0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040020,0x00000008,0x00000003,
    0x00000007,0x0004003b,0x00000008,0x00000009,0x00000003,0x00040017,0x0000000a,0x00000006,
    0x00000002,0x0004001e,0x0000000b,0x00000007,0x0000000a,0x00040020,0x0000000c,0x00000001,
    0x0000000b,0x0004003b,0x0000000c,0x0000000d,0x00000001,0x00040015,0x0000000e,0x00000020,
    0x00000001,0x0004002b,0x0000000e,0x0000000f,0x00000000,0x00040020,0x00000010,0x00000001,
    0x00000007,0x00090019,0x00000013,0x00000006,0x00000001,0x00000000,0x00000000,0x00000000,
    0x00000001,0x00000000,0x0003001b,0x00000014,0x00000013,0x00040020,0x00000015,0x00000000,
    0x00000014,0x0004003b,0x00000015,0x00000016,0x00000000,0x0004002b,0x0000000e,0x00000018,
    0x00000001,0x00040020,0x00000019,0x00000001,0x0000000a,0x00050036,0x00000002,0x00000004,
    0x00000000,0x00000003,0x000200f8,0x00000005,0x00050041,0x00000010,0x00000011,0x0000000d,
    0x0000000f,0x0004003d,0x00000007,0x00000012,0x00000011,0x0004003d,0x00000014,0x00000017,
    0x00000016,0x00050041,0x00000019,0x0000001a,0x0000000d,0x00000018,0x0004003d,0x0000000a,
    0x0000001b,0x0000001a,0x00050057,0x00000007,0x0000001c,0x00000017,0x0000001b,0x00050085,
    0x00000007,0x0000001d,0x00000012,0x0000001c,0x0003003e,0x00000009,0x0000001d,0x000100fd,
    0x00010038
  };

  static INT64                    g_Time = 0;
  static INT64                    g_TicksPerSecond = 0;

  // Vulkan Data
  static VkAllocationCallbacks* g_Allocator = NULL;
  static VkPhysicalDevice       g_Gpu = VK_NULL_HANDLE;
  static VkDevice               g_Device = VK_NULL_HANDLE;
  static VkRenderPass           g_RenderPass = VK_NULL_HANDLE;
  static VkPipelineCache        g_PipelineCache = VK_NULL_HANDLE;
  static void(*g_CheckVkResult)(VkResult err) = NULL;

  static VkCommandBuffer        g_CommandBuffer = VK_NULL_HANDLE;
  static size_t                 g_BufferMemoryAlignment = 256;
  static VkPipelineCreateFlags  g_PipelineCreateFlags = 0;
  static int                    g_FrameIndex = 0;

  static VkSampler              g_FontSampler = VK_NULL_HANDLE;
  static VkDeviceMemory         g_FontMemory = VK_NULL_HANDLE;
  static VkImage                g_FontImage = VK_NULL_HANDLE;
  static VkImageView            g_FontView = VK_NULL_HANDLE;

  static VkDeviceMemory         g_VertexBufferMemory[IMGUI_VK_QUEUED_FRAMES] = {};
  static VkDeviceMemory         g_IndexBufferMemory[IMGUI_VK_QUEUED_FRAMES] = {};
  static size_t                 g_VertexBufferSize[IMGUI_VK_QUEUED_FRAMES] = {};
  static size_t                 g_IndexBufferSize[IMGUI_VK_QUEUED_FRAMES] = {};
  static VkBuffer               g_VertexBuffer[IMGUI_VK_QUEUED_FRAMES] = {};
  static VkBuffer               g_IndexBuffer[IMGUI_VK_QUEUED_FRAMES] = {};

  static VkDeviceMemory         g_UploadBufferMemory = VK_NULL_HANDLE;
  static VkBuffer               g_UploadBuffer = VK_NULL_HANDLE;

  
}

namespace lava
{ 
  // This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
  void CImGui::DrawLists(ImDrawData* draw_data)
  {
    VkResult err;
    ImGuiIO& io = ImGui::GetIO();

    CDevice& lDevice = CDevice::getInstance();

    g_FrameIndex = lDevice.GetCurrentImageIndex();

    // Create the Vertex Buffer:
    size_t vertex_size = draw_data->TotalVtxCount * sizeof(ImDrawVert);
    if (!g_VertexBuffer[g_FrameIndex] || g_VertexBufferSize[g_FrameIndex] < vertex_size)
    {
      if (g_VertexBuffer[g_FrameIndex])
        vkDestroyBuffer(g_Device, g_VertexBuffer[g_FrameIndex], g_Allocator);
      if (g_VertexBufferMemory[g_FrameIndex])
        vkFreeMemory(g_Device, g_VertexBufferMemory[g_FrameIndex], g_Allocator);
      size_t vertex_buffer_size = ((vertex_size - 1) / g_BufferMemoryAlignment + 1) * g_BufferMemoryAlignment;
      VkBufferCreateInfo buffer_info = {};
      buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      buffer_info.size = vertex_buffer_size;
      buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
      buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
      vkCall(vkCreateBuffer(g_Device, &buffer_info, g_Allocator, &g_VertexBuffer[g_FrameIndex]));
      VkMemoryRequirements req;
      vkGetBufferMemoryRequirements(g_Device, g_VertexBuffer[g_FrameIndex], &req);
      g_BufferMemoryAlignment = (g_BufferMemoryAlignment > req.alignment) ? g_BufferMemoryAlignment : req.alignment;
      VkMemoryAllocateInfo alloc_info = {};
      alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      alloc_info.allocationSize = req.size;
      alloc_info.memoryTypeIndex = lava::CDevice::getInstance().FindMemoryType(req.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
      vkCall(vkAllocateMemory(g_Device, &alloc_info, g_Allocator, &g_VertexBufferMemory[g_FrameIndex]));
      vkCall(vkBindBufferMemory(g_Device, g_VertexBuffer[g_FrameIndex], g_VertexBufferMemory[g_FrameIndex], 0));
      g_VertexBufferSize[g_FrameIndex] = vertex_buffer_size;
    }

    // Create the Index Buffer:
    size_t index_size = draw_data->TotalIdxCount * sizeof(ImDrawIdx);
    if (!g_IndexBuffer[g_FrameIndex] || g_IndexBufferSize[g_FrameIndex] < index_size)
    {
      if (g_IndexBuffer[g_FrameIndex])
        vkDestroyBuffer(g_Device, g_IndexBuffer[g_FrameIndex], g_Allocator);
      if (g_IndexBufferMemory[g_FrameIndex])
        vkFreeMemory(g_Device, g_IndexBufferMemory[g_FrameIndex], g_Allocator);
      size_t index_buffer_size = ((index_size - 1) / g_BufferMemoryAlignment + 1) * g_BufferMemoryAlignment;
      VkBufferCreateInfo buffer_info = {};
      buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      buffer_info.size = index_buffer_size;
      buffer_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
      buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
      vkCall(vkCreateBuffer(g_Device, &buffer_info, g_Allocator, &g_IndexBuffer[g_FrameIndex]));
      VkMemoryRequirements req;
      vkGetBufferMemoryRequirements(g_Device, g_IndexBuffer[g_FrameIndex], &req);
      g_BufferMemoryAlignment = (g_BufferMemoryAlignment > req.alignment) ? g_BufferMemoryAlignment : req.alignment;
      VkMemoryAllocateInfo alloc_info = {};
      alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      alloc_info.allocationSize = req.size;
      alloc_info.memoryTypeIndex = lava::CDevice::getInstance().FindMemoryType(req.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
      vkCall(vkAllocateMemory(g_Device, &alloc_info, g_Allocator, &g_IndexBufferMemory[g_FrameIndex]));
      vkCall(vkBindBufferMemory(g_Device, g_IndexBuffer[g_FrameIndex], g_IndexBufferMemory[g_FrameIndex], 0));
      g_IndexBufferSize[g_FrameIndex] = index_buffer_size;
    }

    // Upload Vertex and index Data:
    {
      ImDrawVert* vtx_dst;
      ImDrawIdx* idx_dst;
      vkCall(vkMapMemory(g_Device, g_VertexBufferMemory[g_FrameIndex], 0, vertex_size, 0, (void**)(&vtx_dst)));
      vkCall(vkMapMemory(g_Device, g_IndexBufferMemory[g_FrameIndex], 0, index_size, 0, (void**)(&idx_dst)));
      for (int n = 0; n < draw_data->CmdListsCount; n++)
      {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
        vtx_dst += cmd_list->VtxBuffer.Size;
        idx_dst += cmd_list->IdxBuffer.Size;
      }
      VkMappedMemoryRange range[2] = {};
      range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
      range[0].memory = g_VertexBufferMemory[g_FrameIndex];
      range[0].size = VK_WHOLE_SIZE;
      range[1].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
      range[1].memory = g_IndexBufferMemory[g_FrameIndex];
      range[1].size = VK_WHOLE_SIZE;
      vkCall(vkFlushMappedMemoryRanges(g_Device, 2, range));
      vkUnmapMemory(g_Device, g_VertexBufferMemory[g_FrameIndex]);
      vkUnmapMemory(g_Device, g_IndexBufferMemory[g_FrameIndex]);
    }

    // Bind pipeline and descriptor sets:
    {
      vkCmdBindPipeline(g_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
      VkDescriptorSet desc_set[1] = { mDescriptorSet };
      vkCmdBindDescriptorSets(g_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1, desc_set, 0, NULL);
    }

    // Bind Vertex And Index Buffer:
    {
      VkBuffer vertex_buffers[1] = { g_VertexBuffer[g_FrameIndex] };
      VkDeviceSize vertex_offset[1] = { 0 };
      vkCmdBindVertexBuffers(g_CommandBuffer, 0, 1, vertex_buffers, vertex_offset);
      vkCmdBindIndexBuffer(g_CommandBuffer, g_IndexBuffer[g_FrameIndex], 0, VK_INDEX_TYPE_UINT16);
    }

    // Setup viewport:
    {
      VkViewport viewport;
      viewport.x = 0;
      viewport.y = 0;
      viewport.width = ImGui::GetIO().DisplaySize.x;
      viewport.height = ImGui::GetIO().DisplaySize.y;
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;
      vkCmdSetViewport(g_CommandBuffer, 0, 1, &viewport);
    }

    // Setup scale and translation:
    {
      float scale[2];
      scale[0] = 2.0f / io.DisplaySize.x;
      scale[1] = 2.0f / io.DisplaySize.y;
      float translate[2];
      translate[0] = -1.0f;
      translate[1] = -1.0f;
      vkCmdPushConstants(g_CommandBuffer, mPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 0, sizeof(float) * 2, scale);
      vkCmdPushConstants(g_CommandBuffer, mPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 2, sizeof(float) * 2, translate);
    }

    // Render the command lists:
    int vtx_offset = 0;
    int idx_offset = 0;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
      const ImDrawList* cmd_list = draw_data->CmdLists[n];
      for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
      {
        const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
        if (pcmd->UserCallback)
        {
          pcmd->UserCallback(cmd_list, pcmd);
        }
        else
        {
          VkRect2D scissor;
          scissor.offset.x = (int32_t)(pcmd->ClipRect.x) > 0 ? (int32_t)(pcmd->ClipRect.x) : 0;
          scissor.offset.y = (int32_t)(pcmd->ClipRect.y) > 0 ? (int32_t)(pcmd->ClipRect.y) : 0;
          scissor.extent.width = (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x);
          scissor.extent.height = (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y + 1); // FIXME: Why +1 here?
          vkCmdSetScissor(g_CommandBuffer, 0, 1, &scissor);
          vkCmdDrawIndexed(g_CommandBuffer, pcmd->ElemCount, 1, idx_offset, vtx_offset, 0);
        }
        idx_offset += pcmd->ElemCount;
      }
      vtx_offset += cmd_list->VtxBuffer.Size;
    }
  }

  void CImGui::Create(Window * aWindow)
  {
    mWindow = aWindow;
    CDevice& lDevice = CDevice::getInstance();
    VkDevice lLogicalDevice = lDevice.GetLogicalDevice();

    ImGuiIO& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Tab] = VK_TAB;                       // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array that we will update during the application lifetime.
    io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
    io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
    io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
    io.KeyMap[ImGuiKey_Home] = VK_HOME;
    io.KeyMap[ImGuiKey_End] = VK_END;
    io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
    io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
    io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
    io.KeyMap[ImGuiKey_A] = 'A';
    io.KeyMap[ImGuiKey_C] = 'C';
    io.KeyMap[ImGuiKey_V] = 'V';
    io.KeyMap[ImGuiKey_X] = 'X';
    io.KeyMap[ImGuiKey_Y] = 'Y';
    io.KeyMap[ImGuiKey_Z] = 'Z';

    io.ClipboardUserData = mWindow;
#ifdef _WIN32
    io.ImeWindowHandle = mWindow->GetHWND();
#endif
    io.MouseDrawCursor = false;

    // Create the shaders that will render the gui
    mVertexShader = Handle(CShader);
    mVertexShader->Create(__glsl_shader_vert_spv, sizeof(__glsl_shader_vert_spv));
    mFragmentShader = Handle(CShader);
    mFragmentShader->Create(__glsl_shader_frag_spv, sizeof(__glsl_shader_frag_spv));

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    size_t upload_size = width*height * 4 * sizeof(char);

    mImage = Handle(CImage);
    mImage->Create(width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    mImage->SetPixels(pixels);

    VkSamplerCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    info.magFilter = VK_FILTER_LINEAR;
    info.minFilter = VK_FILTER_LINEAR;
    info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    info.minLod = -1000;
    info.maxLod = 1000;
    info.maxAnisotropy = 1.0f;
    vkCall(vkCreateSampler(lLogicalDevice, &info, nullptr, &mSampler));

    if (!mDescriptorSetLayout)
    {
      VkSampler sampler[1] = { g_FontSampler };
      VkDescriptorSetLayoutBinding binding[1] = {};
      binding[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      binding[0].descriptorCount = 1;
      binding[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
      binding[0].pImmutableSamplers = &mSampler;
      VkDescriptorSetLayoutCreateInfo info = {};
      info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
      info.bindingCount = 1;
      info.pBindings = binding;
      vkCall(vkCreateDescriptorSetLayout(lLogicalDevice, &info, g_Allocator, &mDescriptorSetLayout));

    }

    // Create Descriptor Pool
    {
      VkDescriptorPoolSize pool_size[11] =
      {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
      };
      VkDescriptorPoolCreateInfo pool_info = {};
      pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
      pool_info.maxSets = 1000 * 11;
      pool_info.poolSizeCount = 11;
      pool_info.pPoolSizes = pool_size;
      vkCall(vkCreateDescriptorPool(lLogicalDevice, &pool_info, g_Allocator, &mDescriptorPool));
    }

    // Create Descriptor Set:
    {
      VkDescriptorSetAllocateInfo alloc_info = {};
      alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
      alloc_info.descriptorPool = mDescriptorPool;
      alloc_info.descriptorSetCount = 1;
      alloc_info.pSetLayouts = &mDescriptorSetLayout;
      vkCall( vkAllocateDescriptorSets(lLogicalDevice, &alloc_info, &mDescriptorSet));
    }

    if (!mPipelineLayout)
    {
      VkPushConstantRange push_constants[1] = {};
      push_constants[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
      push_constants[0].offset = sizeof(float) * 0;
      push_constants[0].size = sizeof(float) * 4;
      VkDescriptorSetLayout set_layout[1] = { mDescriptorSetLayout };
      VkPipelineLayoutCreateInfo layout_info = {};
      layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      layout_info.setLayoutCount = 1;
      layout_info.pSetLayouts = set_layout;
      layout_info.pushConstantRangeCount = 1;
      layout_info.pPushConstantRanges = push_constants;
      vkCall(vkCreatePipelineLayout(lLogicalDevice, &layout_info, g_Allocator, &mPipelineLayout));
    }

    VkPipelineShaderStageCreateInfo stage[2] = {};
    stage[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    //stage[0].module = mVertexShader->shaderModule();
    stage[0].pName = "main";
    stage[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    //stage[1].module = mFragmentShader->shaderModule();
    stage[1].pName = "main";

    VkVertexInputBindingDescription binding_desc[1] = {};
    binding_desc[0].stride = sizeof(ImDrawVert);
    binding_desc[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription attribute_desc[3] = {};
    attribute_desc[0].location = 0;
    attribute_desc[0].binding = binding_desc[0].binding;
    attribute_desc[0].format = VK_FORMAT_R32G32_SFLOAT;
    attribute_desc[0].offset = (size_t)(&((ImDrawVert*)0)->pos);
    attribute_desc[1].location = 1;
    attribute_desc[1].binding = binding_desc[0].binding;
    attribute_desc[1].format = VK_FORMAT_R32G32_SFLOAT;
    attribute_desc[1].offset = (size_t)(&((ImDrawVert*)0)->uv);
    attribute_desc[2].location = 2;
    attribute_desc[2].binding = binding_desc[0].binding;
    attribute_desc[2].format = VK_FORMAT_R8G8B8A8_UNORM;
    attribute_desc[2].offset = (size_t)(&((ImDrawVert*)0)->col);

    VkPipelineVertexInputStateCreateInfo vertex_info = {};
    vertex_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_info.vertexBindingDescriptionCount = 1;
    vertex_info.pVertexBindingDescriptions = binding_desc;
    vertex_info.vertexAttributeDescriptionCount = 3;
    vertex_info.pVertexAttributeDescriptions = attribute_desc;

    VkPipelineInputAssemblyStateCreateInfo ia_info = {};
    ia_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    ia_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineViewportStateCreateInfo viewport_info = {};
    viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_info.viewportCount = 1;
    viewport_info.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo raster_info = {};
    raster_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    raster_info.polygonMode = VK_POLYGON_MODE_FILL;
    raster_info.cullMode = VK_CULL_MODE_NONE;
    raster_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    raster_info.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo ms_info = {};
    ms_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    ms_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState color_attachment[1] = {};
    color_attachment[0].blendEnable = VK_TRUE;
    color_attachment[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_attachment[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_attachment[0].colorBlendOp = VK_BLEND_OP_ADD;
    color_attachment[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_attachment[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_attachment[0].alphaBlendOp = VK_BLEND_OP_ADD;
    color_attachment[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineDepthStencilStateCreateInfo depth_info = {};
    depth_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    VkPipelineColorBlendStateCreateInfo blend_info = {};
    blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blend_info.attachmentCount = 1;
    blend_info.pAttachments = color_attachment;

    VkDynamicState dynamic_states[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamic_state = {};
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount = 2;
    dynamic_state.pDynamicStates = dynamic_states;

    {
      VkGraphicsPipelineCreateInfo info = {};
      info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
      info.flags = g_PipelineCreateFlags;
      info.stageCount = 2;
      info.pStages = stage;
      info.pVertexInputState = &vertex_info;
      info.pInputAssemblyState = &ia_info;
      info.pViewportState = &viewport_info;
      info.pRasterizationState = &raster_info;
      info.pMultisampleState = &ms_info;
      info.pDepthStencilState = &depth_info;
      info.pColorBlendState = &blend_info;
      info.pDynamicState = &dynamic_state;
      info.layout = mPipelineLayout;
      info.renderPass = CDevice::getInstance().GetSwapChain().GetRenderPass();
      vkCall(vkCreateGraphicsPipelines(lLogicalDevice, g_PipelineCache, 1, &info, g_Allocator, &mPipeline));
    }
  }

  void CImGui::NewFrame()
  {
    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    RECT rect;
    GetClientRect(mWindow->GetHWND(), &rect);
    io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));

    // Setup time step
    INT64 current_time;
    QueryPerformanceCounter((LARGE_INTEGER *)&current_time);
    io.DeltaTime = (float)(current_time - g_Time) / g_TicksPerSecond;
    g_Time = current_time;

    // Read keyboard modifiers inputs
    io.KeyCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    io.KeyShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
    io.KeyAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
    io.KeySuper = false;
    // io.KeysDown : filled by WM_KEYDOWN/WM_KEYUP events
    // io.MousePos : filled by WM_MOUSEMOVE events
    // io.MouseDown : filled by WM_*BUTTON* events
    // io.MouseWheel : filled by WM_MOUSEWHEEL events

    // Set OS mouse position if requested last frame by io.WantMoveMouse flag (used when io.NavMovesTrue is enabled by user and using directional navigation)
    if (io.WantMoveMouse)
    {
      POINT pos = { (int)io.MousePos.x, (int)io.MousePos.y };
      ClientToScreen(mWindow->GetHWND(), &pos);
      SetCursorPos(pos.x, pos.y);
    }

    // Hide OS mouse cursor if ImGui is drawing it
    if (io.MouseDrawCursor)
      SetCursor(NULL);

    // Start the frame
    ImGui::NewFrame();
  }

  void CImGui::Render()
  {
    ImGui::Render();
    DrawLists(ImGui::GetDrawData());
  }

  void CImGui::SetStyle()
  {
    ImGuiStyle& style = ImGui::GetStyle();

    style.ChildWindowRounding = 3.f;
    style.GrabRounding = 0.f;
    style.WindowRounding = 0.f;
    style.ScrollbarRounding = 3.f;
    style.FrameRounding = 3.f;
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

    style.Colors[ImGuiCol_Text] = ImVec4(0.73f, 0.73f, 0.73f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.1f, 0.1f, 0.1f, 0.7f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.26f, 0.26f, 0.26f, 0.95f);
    style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_ComboBg] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_CloseButton] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
    style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.32f, 0.52f, 0.65f, 1.00f);
    style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);
  }

  void CImGui::Destroy()
  {
    CDevice& lDevice = CDevice::getInstance();
    VkDevice lLogicalDevice = lDevice.GetLogicalDevice();

    if (g_UploadBuffer)
    {
      vkDestroyBuffer(g_Device, g_UploadBuffer, g_Allocator);
      g_UploadBuffer = VK_NULL_HANDLE;
    }
    if (g_UploadBufferMemory)
    {
      vkFreeMemory(g_Device, g_UploadBufferMemory, g_Allocator);
      g_UploadBufferMemory = VK_NULL_HANDLE;
    }

    for (int i = 0; i < IMGUI_VK_QUEUED_FRAMES; i++)
    {
      if (g_VertexBuffer[i]) { vkDestroyBuffer(g_Device, g_VertexBuffer[i], g_Allocator); g_VertexBuffer[i] = VK_NULL_HANDLE; }
      if (g_VertexBufferMemory[i]) { vkFreeMemory(g_Device, g_VertexBufferMemory[i], g_Allocator); g_VertexBufferMemory[i] = VK_NULL_HANDLE; }
      if (g_IndexBuffer[i]) { vkDestroyBuffer(g_Device, g_IndexBuffer[i], g_Allocator); g_IndexBuffer[i] = VK_NULL_HANDLE; }
      if (g_IndexBufferMemory[i]) { vkFreeMemory(g_Device, g_IndexBufferMemory[i], g_Allocator); g_IndexBufferMemory[i] = VK_NULL_HANDLE; }
    }

    if (g_FontView) { vkDestroyImageView(g_Device, g_FontView, g_Allocator); g_FontView = VK_NULL_HANDLE; }
    if (g_FontImage) { vkDestroyImage(g_Device, g_FontImage, g_Allocator); g_FontImage = VK_NULL_HANDLE; }
    if (g_FontMemory) { vkFreeMemory(g_Device, g_FontMemory, g_Allocator); g_FontMemory = VK_NULL_HANDLE; }
    if (mSampler) { vkDestroySampler(g_Device, mSampler, g_Allocator); mSampler = VK_NULL_HANDLE; }
    if (mDescriptorSetLayout) { vkDestroyDescriptorSetLayout(g_Device, mDescriptorSetLayout, g_Allocator); mDescriptorSetLayout = VK_NULL_HANDLE; }
    if (mPipelineLayout) { vkDestroyPipelineLayout(g_Device, mPipelineLayout, g_Allocator); mPipelineLayout = VK_NULL_HANDLE; }
    if (mPipeline) { vkDestroyPipeline(g_Device, mPipeline, g_Allocator); mPipeline = VK_NULL_HANDLE; }

    //mVertexShader->destroy(lLogicalDevice);
    //mFragmentShader->destroy(lLogicalDevice);

    mImage->Destroy();

    ImGui::Shutdown();
  }
}