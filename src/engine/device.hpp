/**
 * @file src/engine/device.hpp
 * @author Caleb Burke
 * @date Nov 18, 2023
 */

#pragma once

#include "engine/window.hpp"

#include <vulkan/vulkan.h>

#include <vector>
#include <optional>
#include <cstring>
#include <set>

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
    // Prevents copying and moving
    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;
    Device(Device&&) = delete;
    Device& operator=(Device&&) = delete;
	
	/**
 	* @brief Constructor
 	* @note Only constructor
 	* @param[in] window
 	*/
	Device(Window& window);

	/**
 	* @brief Default Deconstructor
 	* 
 	* Cleans up:
 	*  1. Vulkan Debugger
 	*  2. Command Buffers
 	*  3. Logical Device / Queues
 	*  4. Vulkan Instance
 	*
 	* TODO: Destroy command buffers
 	*/
	~Device();

    VkDevice get_device() const { return device; }
    VkSurfaceKHR get_surface() const { return surface; }
    VkQueue get_graphics_queue() const { return graphics_queue; }
    VkQueue get_present_queue() const { return present_queue; }
    SwapChainSupportDetails get_swapchain_support() { return query_swapchain_support(physical_device); }

private:
	/**
	 * @brief Initializes device
	 *
	 * TODO create logical device
	 * TODO create command pool
	 *
	 * @note Called by constructor
	 * @return void
	 */
    void initialize();

	/**
	 * @brief Creates vulkan instance
	 * @return void
	 */
    void create_vulkan_instance();

	/**
	 * @brief Checks if the layers we enabled are available
	 * @return If layers are available
	 */
    bool check_validation_layer_support();

	/**
	 * @brief Gets extentions needed
	 * @return list of extensions we need enabled
	 */
    std::vector<const char*> get_required_extensions();

    /**
 	 * @brief Creates debugger info
 	 *
 	 * Sets up the info struct for creating the debugger
 	 *
 	 * @param[out] create_info
 	 * @return void
 	 */
	void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info);

    /**
 	 * @brief Create Debugger
 	 * @return void
 	 */
	void setup_debug_messenger();

    /**
 	 * @brief 
	 *
 	 * Not sure what this does but its required for vulkan debugger
 	 *
 	 * @param instance
 	 * @param pCreateInfo
 	 * @param pAllocator
 	 * @param pDebugMessenger
 	 * @return
 	 */
	VkResult create_debug_utils_messenger_EXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

	/**
 	 * @brief
 	 * @param instance
 	 * @param debugMessenger
 	 * @param pAllocator
 	 * @return void
 	 */
	void destroy_debug_utils_messenger_EXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

	/**
	 * @brief Picks physical device to use
	 * 
	 * Picks the physical device vulkan will use to render with
	 *
	 * @return void
	 */
    void pick_physical_device();

	/**
 	 * @brief Checks if device is suitable
 	 *
 	 * Checks if a physical device is suitable to use for our program.
 	 *
 	 * @param[in] physical_device The device we are checking
 	 * @return If device is suitable
 	 */
    bool is_physical_device_suitable(VkPhysicalDevice physical_device);

	/**
 	 * @brief Find the queue families of a physical device
 	 *
 	 * Find the queue families of a physical device. 
 	 *
 	 * @param[in] physical_device The physical device we search in
 	 * @return void
 	 */
    QueueFamilyIndices find_queue_families(VkPhysicalDevice physical_device);

	/**
 	 * @brief Check if physical device supports extensions
 	 *
 	 * Checks if a physical device supports vulkan extensions for this program
 	 *
 	 * @param[in] physical_device The physical device we search in
 	 * @return If physical device supports extensions
 	 */
	bool check_device_extension_support(VkPhysicalDevice physical_device);

	/**
 	 * @brief Get details on physical device
 	 *
 	 * Get details on physical device, details that can be used to determine
 	 * if a physical device supports swapchain
 	 *
 	 * @param[in] physical_device The physical device we search in
 	 * @return The support details of physical device
 	 */
	SwapChainSupportDetails query_swapchain_support(VkPhysicalDevice physical_device);

	/**
 	 * @brief Creates the vulkan logical device
 	 *
 	 * Creates a logical device with 2 queues. A graphics
 	 * queue and a present queue. Before calling this function 
 	 * a physical device must have been picked.
 	 *
 	 * TODO Enable needed features
 	 * 
 	 * @note Stores created logical device in `device`
 	 * @return void
 	 */
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
    const std::vector<const char*> enabled_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
};

}   // namespace eng
}   // namespace uni

