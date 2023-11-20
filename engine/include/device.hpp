/**
 * @file device.hpp
 * @author Caleb Burke
 * @date Nov 18, 2023
 */

#pragma once

#include "window.hpp"

#include <vulkan/vulkan.h>

#include <vector>
#include <optional>

namespace uni {
namespace eng {

/**
 * @brief Helper Struct 
 */
struct SwapChainSupportDetails {
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
    VkSurfaceCapabilitiesKHR capabilities;
};

/**
 * @brief Helper Struct
 */
struct QueueFamilyIndices {
    std::optional<uint32_t> graphics;
    std::optional<uint32_t> present;

    bool filled(){
        return graphics.has_value() && present.has_value();
    }
};

/**
 * @brief Creates interface with vulkan device
 */
class Device {
public:
    Device(Window& window);
    ~Device();

    // Prevents copying and moving
    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;
    Device(Device&&) = delete;
    Device& operator=(Device&&) = delete;

private:
    void initialize();
    void create_vulkan_instance();
    bool check_validation_layer_support();
    std::vector<const char*> get_required_extensions();
    
    void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT&);
    void setup_debug_messenger();
    VkResult create_debug_utils_messenger_EXT(VkInstance, const VkDebugUtilsMessengerCreateInfoEXT*, const VkAllocationCallbacks*, VkDebugUtilsMessengerEXT*);
    void destroy_debug_utils_messenger_EXT(VkInstance, VkDebugUtilsMessengerEXT, const VkAllocationCallbacks*);

    void pick_physical_device();
    bool is_physical_device_suitable(VkPhysicalDevice physical_device);
    QueueFamilyIndices find_queue_families(VkPhysicalDevice physical_device);
    bool check_device_extension_support(VkPhysicalDevice physical_device);
    SwapChainSupportDetails query_swapchain_support(VkPhysicalDevice physical_device);

    void create_logical_device();

    VkInstance instance;
    Window& window;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphics_queue;
    VkQueue present_queue;

#ifdef NDEBUG
    const bool enable_validation_layers = false;
    const std::vector<const char*> enabled_layers;
#else
    const bool enable_validation_layers = true;
    const std::vector<const char*> enabled_layers = {"VK_LAYER_KHRONOS_validation"};
#endif
    const std::vector<const char*> enabled_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};

}   // namespace eng
}   // namespace uni
