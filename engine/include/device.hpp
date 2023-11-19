/**
 * @file device.hpp
 * @author Caleb Burke
 * @date Nov 18, 2023
 */

#pragma once

#include "window.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace uni {
namespace eng {

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

    VkInstance instance;
    Window& window;
    VkDebugUtilsMessengerEXT debug_messenger;


#ifdef NDEBUG
    const bool enable_validation_layers = false;
    std::vector<const char*> enabled_layers;
#else
    const bool enable_validation_layers = true;
    std::vector<const char*> enabled_layers = { "VK_LAYER_KHRONOS_validation" };
#endif

};

}   // namespace eng
}   // namespace uni
