/**
 * @file engine/src/swapchain.hpp
 * @author Caleb Burke
 * @date Nov 20, 2023
 */

#include "swapchain.hpp"

#include "logger.hpp"

namespace uni {
namespace eng {

/**
 * @brief
 * @param[in] device
 */
SwapChain::SwapChain(Device& device) : device{device} {
    initialize();
}

/**
 * @brief Default Deconstructor
 */
SwapChain::~SwapChain(){

}

/**
 * @brief
 */
void SwapChain::initialize(){
    create_swapchain();
}

/**
 * @brief
 */
void SwapChain::create_swapchain(){
    VkSwapchainCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = device.get_surface();
    create_info.minImageCount = 4;
    create_info.imageFormat = VK_FORMAT_R8G8B8A8_SRGB;
    create_info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;    // vkGetPhysicalDeviceSurfaceFormatsKHR
    create_info.imageExtent = VkExtent2D{1920, 1080};
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    /*
    create_info.imageSharingMode = 0;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = 0;
    create_info.preTransform = 0;
    create_info.compositeAlpha = 0;
    */
    create_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    //create_info.clipped = 0;
    //create_info.oldSwapchain = 0;

    VkResult result = vkCreateSwapchainKHR(device.get_device(), &create_info, nullptr, &swapchain);
    if(result != VK_SUCCESS){
        VK_ERROR("Failed to create swapchain");
        throw std::exception();
    }
    VK_INFO("Created Swapchain");
}

}   // namespace eng
}   // namespace uni
