#pragma once

#include <vulkan/vulkan.h>

#include "std.hpp"
#include "usr.hpp"

#include "window.hpp"

namespace gfx {

/* SwapChainSupportDetails helper struct */
struct SCSD {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};

/* QueFamilyIndices helper struct */
struct QFI {
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    bool operator~(){
        return graphics_family.has_value() && present_family.has_value();
    }
};

class Device {
public:
#ifdef NDEBUG
    const bool enable_validation_layers = false;
#else
    const bool enable_validation_layers = true;
#endif    

    Device(Window&);
    ~Device();

    VkDevice get_device(){ return device; }
    VkSurfaceKHR get_surface(){ return surface; }
    VkQueue get_graphics_que(){ return gfx_queue; }
    VkQueue get_present_que(){ return present_queue; }

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;
    Device(Device&&) = delete;
    Device& operator=(Device&&) = delete;

    SCSD get_swapchain_support(){ return query_swapchain_support(physical_device); }
    QFI find_physical_que_families(){ return find_que_families(physical_device); }

private:
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    Window& window;
    VkDevice device;
    VkQueue gfx_queue;
    VkQueue present_queue;

    const std::vector<const char*> validation_layers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char*> device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT&);
    VkResult create_debug_utils_messenger_EXT(VkInstance, const VkDebugUtilsMessengerCreateInfoEXT*, const VkAllocationCallbacks*, VkDebugUtilsMessengerEXT*);
    void setup_debug_messenger();
    void destroy_debug_utils_messenger_EXT(VkInstance, VkDebugUtilsMessengerEXT, const VkAllocationCallbacks*);

    /* Functions for creating instance */
    void create_instance();
    bool check_validation_layer_support();
    std::vector<const char*> get_required_extensions();

    /* Functions for picking and setting up physical device */
    void pick();
    bool is_device_suitable(VkPhysicalDevice);
    bool check_device_extension_support(VkPhysicalDevice);
    QFI find_que_families(VkPhysicalDevice);

    /* Functions for creating and setting up logical device*/
    void create_logical_device();

    /* Functions for checking swapchain support in device*/
    SCSD query_swapchain_support(VkPhysicalDevice);
};

}
