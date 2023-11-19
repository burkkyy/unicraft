/**
 * @file device.cpp
 * @author Caleb Burke
 * @date Nov 18, 2023
 */

#include "device.hpp"
#include "logger.hpp"

#include <cstring>

namespace uni {
namespace eng {

/**
 * @brief Vulkan debug callback function for handling validation layer messages.
 *
 * This function is intended to be used as a callback for Vulkan debug messages generated
 * by the validation layers. It logs the messages with different severity levels.
 *
 * @param m_severity The severity level of the debug message.
 * @param m_type The type of the debug message.
 * @param pCallback_data Pointer to a structure containing debug information.
 * @param pUser_data A pointer to user-defined data, which can be set when registering the callback.
 * @return VK_FALSE to indicate that the Vulkan call that triggered the validation message
 *         should not be aborted, allowing the program to continue execution.
 */
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT m_severity,
    VkDebugUtilsMessageTypeFlagsEXT m_type,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallback_data,
    void* pUser_data){
    (void)m_type;
    (void)pCallback_data;
    (void)pUser_data;
    if(m_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT){
        ERROR("VALIDATION LAYER", pCallback_data->pMessage);
        return VK_SUCCESS;  // Abort program
    } else if(m_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT){
        WARNING("VALIDATION LAYER", pCallback_data->pMessage);
    } else {
        INFO("VALIDATION LAYER", pCallback_data->pMessage);
    }
    return VK_FALSE;
}

/**
 * @brief Constructor
 * @note Only constructor
 * @param[in] window
 */
Device::Device(Window& window) : window{window} {
    initialize();
}

/**
 * @brief Default Deconstructor
 */
Device::~Device(){
    if(enable_validation_layers){
        destroy_debug_utils_messenger_EXT(instance, debug_messenger, nullptr);
        VK_INFO("Destroyed Vulkan Debugger.");
    }

    // Destroy vulkan instance last
    vkDestroyInstance(instance, nullptr);
    VK_INFO("Destroyed VkInstance.");
}

/**
 * @brief Initializes device
 *
 * TODO: window create surface KHR
 * TODO: pick physical device
 * TODO: create logical device
 * TODO: create command pool
 *
 * @note Called by constructor
 * @return void
 */
void Device::initialize(){
    create_vulkan_instance();
    setup_debug_messenger();
}

/**
 * @brief Creates vulkan instance
 * @return void 
 */
void Device::create_vulkan_instance(){
    if(enable_validation_layers && !check_validation_layer_support()){
        VK_ERROR("Validation layers requested but not available.");
        throw std::exception();
    }

    /*
        Tell driver which vulkan api version we are using.
    */
    VkApplicationInfo application_info = {};
    application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application_info.pApplicationName = "Unicraft";
    application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    application_info.pEngineName = "Unicraft Engine";
    application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    application_info.apiVersion = VK_API_VERSION_1_0;
    
    auto extensions = get_required_extensions();

    /*
        Tell vulkan which extensions and validation layers we want to use.
        These extensions are global.
    */
    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &application_info;
    create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();
    create_info.enabledLayerCount = static_cast<uint32_t>(enabled_layers.size());
    create_info.ppEnabledLayerNames = enabled_layers.data();

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {};
    if(enable_validation_layers){
        VK_INFO("Validation Layers enabled.");
        populate_debug_messenger_create_info(debug_create_info);
        create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
    }

    VkResult result = vkCreateInstance(&create_info, nullptr, &instance);
    if(result != VK_SUCCESS){
        VK_ERROR("Failed to create vulkan instance.");
    }
}

/**
 * @brief
 * @return
 */
bool Device::check_validation_layer_support(){
    uint32_t layer_count = 0;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, layers.data());

    // check if properties of each layer is available
    for(auto V : enabled_layers){
        bool layer_found = false;

        for(const auto& LP : layers){
            if(strcmp(V, LP.layerName) == 0){
                layer_found = true;
                break;
            }
        }

        // even if only one validation layer is not available return false
        if(!layer_found){ return false; }
    }
    return true;
}

/**
 * @brief Gets extentions needed
 * @return list of extensions we need enabled
 */
std::vector<const char*> Device::get_required_extensions(){
    /*
        GLFW Extensions

        Extensions for vulkan to interface with GLFW so we use built in 
        glfw function that returns the extension(s)
    */
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    // vector to hold all extensions
    std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

    if(enable_validation_layers){
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    uint32_t available_extension_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &available_extension_count, nullptr); // get # of extentions
    std::vector<VkExtensionProperties> available_extensions(available_extension_count); // set up vector to hold VkExtensionProperties structs
    vkEnumerateInstanceExtensionProperties(nullptr, &available_extension_count, available_extensions.data());   // get VkExtensionProperties structs

    VK_INFO("number of available extensions: " << available_extension_count);
    
#ifndef NDEBUG
    //VK_INFO("available extensions:");
    //for(const auto& e : available_extensions){ std::cout << '\t' << e.extensionName << '\n'; }
    std::cout << "Required extensions:\n";
    for(const auto& e : extensions){ std::cout << "\t" << e << '\n'; }
#endif
    return extensions;
}

/**
 * @brief Creates debugger info
 *
 * Sets up the info struct for creating the debugger
 *
 * @param[out] create_info
 * @return void
 */
void Device::populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info){
    create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = debug_callback;
}

/**
 * @brief Create Debugger
 * @return void
 */
void Device::setup_debug_messenger(){
    if(!enable_validation_layers){ return; }

    VkDebugUtilsMessengerCreateInfoEXT create_info;
    populate_debug_messenger_create_info(create_info);

    VkResult result = create_debug_utils_messenger_EXT(instance, &create_info, nullptr, &debug_messenger);
    if(result != VK_SUCCESS){
        VK_ERROR("Failed to set up debug messenger.");
        VK_ERROR("Error code: " << result);
        throw std::exception();
    }
    VK_INFO("Set up debug messenger.");
}

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
VkResult Device::create_debug_utils_messenger_EXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger){
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if(func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

/**
 * @brief
 * @param instance
 * @param debugMessenger
 * @param pAllocator
 * @return void
 */
void Device::destroy_debug_utils_messenger_EXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator){
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if(func != nullptr){ func(instance, debugMessenger, pAllocator); }
}

}   // namespace eng
}   // namespace uni
