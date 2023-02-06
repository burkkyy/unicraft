#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "device.hpp"
#include "usr.hpp"

namespace gfx {

class SwapChain {
public:
    SwapChain(Device&, VkExtent2D);
    ~SwapChain();

    SwapChain(const SwapChain&) = delete;
    SwapChain& operator=(const SwapChain&) = delete;

private:
    Device& device;
    VkExtent2D window_extent;

    VkSwapchainKHR swapchain;

    std::vector<VkImage> swapchain_images;
    VkFormat swapchain_image_format;
    VkExtent2D swapchain_extent;

    std::vector<VkImageView> swapchain_image_views;

    /* Functions for creating swapchain */
    VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>&);
    VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>&);
    VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR&);
    void create_swap_chain();

    void create_image_views();
};

}
