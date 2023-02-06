#include "swapchain.hpp"

namespace gfx {

SwapChain::SwapChain(Device& d, VkExtent2D e) : device{d}, window_extent{e} {
    create_swap_chain();
    create_image_views();
}

SwapChain::~SwapChain(){
    for(auto i : swapchain_image_views){
        vkDestroyImageView(device.get_device(), i, nullptr);
    }

    vkDestroySwapchainKHR(device.get_device(), swapchain, nullptr);
    VK_INFO("destroyed swapchain");
}

VkSurfaceFormatKHR SwapChain::choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& formats){
    /* We want to return the format with the 'best' color space */
    for(const auto& f : formats){
        if(f.format == VK_FORMAT_B8G8R8A8_SRGB && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR){
            return f;
        }
    }
    VK_WARNING("choosing poor swapchain surface format");
    return formats[0];
}

VkPresentModeKHR SwapChain::choose_swap_present_mode(const std::vector<VkPresentModeKHR>& modes){
    /* 
    The presentation mode is most important setting for the swapchain.
    
    Possible presentation modes:
        VK_PRESENT_MODE_IMMEDIATE_KHR: 
            Images submitted by your application are transferred to 
            the screen right away, which may result in tearing.

        VK_PRESENT_MODE_FIFO_KHR: 
            The swap chain is a queue where the 
            display takes an image from the front of the queue when the 
            display is refreshed and the program inserts rendered images 
            at the back of the queue. If the queue is full then the program 
            has to wait. This is most similar to vertical sync as found 
            in modern games. The moment that the display is refreshed is 
            known as "vertical blank".

        VK_PRESENT_MODE_FIFO_RELAXED_KHR:
            This mode only differs from the 
            previous one if the application is late and the queue was empty at 
            the last vertical blank. Instead of waiting for the next vertical 
            blank, the image is transferred right away when it finally arrives. 
            This may result in visible tearing.

        VK_PRESENT_MODE_MAILBOX_KHR: 
            This is another variation of the second mode. 
            Instead of blocking the application when the 
            queue is full, the images that are already queued are simply 
            replaced with the newer ones. This mode can be used to render 
            frames as fast as possible while still avoiding tearing, 
            resulting in fewer latency issues than standard vertical sync. 
            This is commonly known as "triple buffering", although the 
            existence of three buffers alone does not necessarily mean 
            that the framerate is unlocked.
    */

    /*
    for(const auto& m : modes){
        if(m == VK_PRESENT_MODE_IMMEDIATE_KHR){
            VK_INFO("present mode: immediate");
            return m;
        }
    }
    */

    for(const auto& m : modes){
        if(m == VK_PRESENT_MODE_MAILBOX_KHR){
            VK_INFO("present mode: mailbox");
            return m;
        }
    }

    /* FIFO_KHR is guaranteed to be avalible */
    VK_INFO("present mode: V-Sync");
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::choose_swap_extent(const VkSurfaceCapabilitiesKHR& capa){
    if(capa.currentExtent.width != 0xFFFFFFFF){
        return capa.currentExtent;
    } else {
        VkExtent2D extent = window_extent;
        extent.width = std::max(capa.minImageExtent.width, std::min(capa.maxImageExtent.width, extent.width));
        extent.height = std::max(capa.minImageExtent.height, std::min(capa.maxImageExtent.height, extent.height));
        return extent;
    }
}

void SwapChain::create_swap_chain(){
    SCSD ss = device.get_swapchain_support();
    VkSurfaceFormatKHR surface_format = choose_swap_surface_format(ss.formats);
    VkPresentModeKHR present_mode = choose_swap_present_mode(ss.present_modes);
    VkExtent2D extent = choose_swap_extent(ss.capabilities);

    uint32_t image_count = ss.capabilities.minImageCount + 1;
    if(ss.capabilities.maxImageCount > 0 && image_count > ss.capabilities.maxImageCount){
        image_count = ss.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = device.get_surface();
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QFI indices = device.find_physical_que_families();
    uint32_t qfi[] = {indices.graphics_family.value(), indices.present_family.value()};
    if(indices.graphics_family != indices.present_family){
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = qfi;
    } else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices = nullptr;
    }

    create_info.preTransform = ss.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = nullptr;
    // create_info.oldSwapchain = oldSwapchain == nullptr ? VK_NULL_HANDLE : oldSwapchain->swapChain;
    
    if(vkCreateSwapchainKHR(device.get_device(), &create_info, nullptr, &swapchain) != VK_SUCCESS){
        VK_ERROR("failed to create swapchain");
    }
    VK_INFO("created swapchain");

    vkGetSwapchainImagesKHR(device.get_device(), swapchain, &image_count, nullptr);
    swapchain_images.resize(image_count);
    vkGetSwapchainImagesKHR(device.get_device(), swapchain, &image_count, swapchain_images.data());

    swapchain_image_format = surface_format.format;
    swapchain_extent = extent;
}

void SwapChain::create_image_views(){
    swapchain_image_views.resize(swapchain_images.size());
    
    for(size_t i = 0; i < swapchain_images.size(); i++){
        VkImageViewCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        create_info.image = swapchain_images[i];
        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = swapchain_image_format;
        create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;

        if(vkCreateImageView(device.get_device(), &create_info, nullptr, &swapchain_image_views[i]) != VK_SUCCESS){
            VK_ERROR("failed to create image views");
        }
    }
}

}
