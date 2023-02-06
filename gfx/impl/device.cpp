#include "device.hpp"

namespace gfx {

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT m_severity,
    VkDebugUtilsMessageTypeFlagsEXT m_type,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallback_data,
    void* pUser_data){
        if(m_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT){
            ERROR("VALIDATION LAYER", pCallback_data->pMessage);
        } else if(m_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT){
            WARNING("VALIDATION LAYER", pCallback_data->pMessage);
        } else {
            INFO("VALIDATION LAYER", pCallback_data->pMessage);
        }
        return VK_FALSE;
}

Device::Device(Window& win) : window{win} {
    create_instance();
    setup_debug_messenger();
    window.create_surface(instance, &surface);
    pick();
    create_logical_device();
}

Device::~Device(){
    if(enable_validation_layers){
        destroy_debug_utils_messenger_EXT(instance, debug_messenger, nullptr);
    }

    vkDestroyDevice(device, nullptr);
    VK_INFO("destroyed logical device");

    vkDestroySurfaceKHR(instance, surface, nullptr);
    VK_INFO("destroyed VkSurfaceKHR");

    /* Note that all other vulkan resources should be destroyed before VkInstance */
    vkDestroyInstance(instance, nullptr);
    VK_INFO("destroyed VkInstance");
}

void Device::populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info){
    create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = debug_callback;
}

void Device::setup_debug_messenger(){
    if(!enable_validation_layers){ return; }

    VkDebugUtilsMessengerCreateInfoEXT create_info;
    populate_debug_messenger_create_info(create_info);

    if(create_debug_utils_messenger_EXT(instance, &create_info, nullptr, &debug_messenger) != VK_SUCCESS){
        VK_ERROR("failed to set up debug messenger");
    }
    VK_INFO("set up debug messenger");
}

VkResult Device::create_debug_utils_messenger_EXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger){
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if(func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void Device::destroy_debug_utils_messenger_EXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator){
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if(func != nullptr){ func(instance, debugMessenger, pAllocator); }
}

void Device::create_instance(){
    if(enable_validation_layers && !check_validation_layer_support()){ VK_ERROR("validation layers requested but not available"); }
    if(enable_validation_layers){ VK_INFO("validation layers were requested successfully"); }
    
    /*
    VkApplicationInfo

    Infomation struct about our application. This is used
    by the driver to optimize our application (optional)
    */
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Hello Triangle";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "No Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    /*
    VkInstanceCreateInfo

    This struct tells the vulkan driver which extensions
    and validation layers we want to use. These extensions
    are global meaning they apply to the entire program
    */
    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    /*
    Extensions
    */
    auto extensions = get_required_extensions();
    create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();

    /*
    Validation layers and debug messenger
    */
    VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
    if(enable_validation_layers){
        create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        create_info.ppEnabledLayerNames = validation_layers.data();

        populate_debug_messenger_create_info(debug_create_info);
        create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
    } else {
        create_info.enabledLayerCount = 0;
        create_info.pNext = nullptr;
    }

    /*
    Note that most object creation functions in vulkan follow the same parameter structure:
    1. Pointer to struct with the object creation info
    2. Pointer to custom allocator callback (we just set this to nullptr)
    3. Pointer to the variable that stores the handle to the new object

    Object creation functions usally return a VkResult with value VK_SUCCESS or an error code
    such as VK_ERROR_INCOMPATIBLE_DRIVER
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    */
    if(vkCreateInstance(&create_info, nullptr, &instance) != VK_SUCCESS){ VK_ERROR("failed to create VkInstance"); }
    VK_INFO("created VkInstance");
}

bool Device::check_validation_layer_support(){
    // first get number of avalible validation layers
    uint32_t layer_c = 0;
    vkEnumerateInstanceLayerProperties(&layer_c, nullptr);

    // store all the properties of each layer in a vector
    std::vector<VkLayerProperties> layers(layer_c);
    vkEnumerateInstanceLayerProperties(&layer_c, layers.data());

    // check if properties of each layer is available
    for(auto V : validation_layers){
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

std::vector<const char*> Device::get_required_extensions(){
    /*
    GLFW Extensions

    Extensions for vulkan to interface with GLFW so we use built in 
    glfw function that returns the extension(s)
    */
    uint32_t glfw_extension_c = 0;
    const char** glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_c);

    // vector to hold all extensions
    std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_c);

    if(enable_validation_layers){
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    /*
    vkEnumerateInstanceExtensionProperties param structure:
    1. string to specify which validation layer to get extension layers from
    2. pointer to uint32_t to get or hold # of extensions
    3. pointer to a list of VkExtensionProperties structs
    
    If 3. is nullptr then 2. is simply set to the # of extensions
    */
    uint32_t available_extension_c = 0;
    
    vkEnumerateInstanceExtensionProperties(nullptr, &available_extension_c, nullptr); // get # of extentions
    std::vector<VkExtensionProperties> available_extensions(available_extension_c); // set up vector to hold VkExtensionProperties structs

    vkEnumerateInstanceExtensionProperties(nullptr, &available_extension_c, available_extensions.data());   // get VkExtensionProperties structs

    VK_INFO("number of available extensions: " << available_extension_c);
    
    VK_INFO("available extensions:");
    for(const auto& e : available_extensions){ P('\t' << e.extensionName << '\n'); }

    P("required extensions:\n");
    for(const auto& e : extensions){ P('\t' << e << '\n'); }
    
    return extensions;
}

void Device::pick(){
    uint32_t device_c = 0;
    
    /*
    vkEnumeratePhysicalDevices
    @param our VkInstance
    @param # of physical devices
    @param array of VkPhysicalDevice handles
    @return VK_SUCCESS or some error code
    */
    vkEnumeratePhysicalDevices(instance, &device_c, nullptr);

    if(device_c == 0){ VK_ERROR("failed to find GPUs with Vulkan support"); }
    VK_INFO("physical device count: " << device_c);

    std::vector<VkPhysicalDevice> devices(device_c);
    vkEnumeratePhysicalDevices(instance, &device_c, devices.data());

    // just pick the first device
    for(const auto& d : devices){
        if(is_device_suitable(d)){
            physical_device = d;
            break;
        }
    }

    if(physical_device == VK_NULL_HANDLE){ VK_ERROR("failed to find a suitable GPU"); }
    
    VkPhysicalDeviceProperties device_properties;
    vkGetPhysicalDeviceProperties(physical_device, &device_properties);
    VK_INFO("physical device: " << device_properties.deviceName);
}

bool Device::is_device_suitable(VkPhysicalDevice device){
    QFI indices = find_que_families(device);
    bool extensions_support = check_device_extension_support(device);

    /* Get basic device properties such as
    - api and driver version
    - vender and device id */
    VkPhysicalDeviceProperties device_properties;
    vkGetPhysicalDeviceProperties(device, &device_properties);

    /* Get device features, there are alot of features that
    are returned to device_features list of them are
    https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPhysicalDeviceFeatures.html*/
    VkPhysicalDeviceFeatures device_features;
    vkGetPhysicalDeviceFeatures(device, &device_features);
    
    /* Checking if device supports swapchain */
    bool sc = false;
    if(extensions_support){
        SCSD sc_support = query_swapchain_support(device);
        sc = !sc_support.formats.empty() && !sc_support.present_modes.empty();
    }

    return ~indices && extensions_support && sc && device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && device_features.geometryShader;
}

bool Device::check_device_extension_support(VkPhysicalDevice device){
    uint32_t count;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr);

    std::vector<VkExtensionProperties> extensions(count);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &count, extensions.data());

    std::set<std::string> required_extensions(device_extensions.begin(), device_extensions.end());

    for(const auto& e : extensions){
        required_extensions.erase(e.extensionName);
    }
    return required_extensions.empty();
}

QFI Device::find_que_families(VkPhysicalDevice device){
    QFI indices;
    
    /* Get number of queue families available in device */
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
    
    /* Get queue family properties of device */
    std::vector<VkQueueFamilyProperties> que_families(count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, que_families.data());

    for(int i = 0; i < count; i++){
        /* Check to see if one of the queue families support VK_QUEUE_GRAPHICS_BIT */
        if(que_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT){
            indices.graphics_family = i;
        }

        /* Check to see if one of the queue families support surfaceKHR presentation */ 
        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);
        if(present_support){
            indices.present_family = i;
        }
    }

    return indices;
}

void Device::create_logical_device(){
    QFI indices = find_que_families(physical_device);

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<uint32_t> unique_queue_families = {
        indices.graphics_family.value(),
        indices.present_family.value()
    };

    float queue_priority = 1.f;
    for(uint32_t qf : unique_queue_families){        
        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = qf;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(queue_create_info);
    }

    VkPhysicalDeviceFeatures features{};

    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.pEnabledFeatures = &features;

    /* enable VK_KHR_swapchain */
    create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
    create_info.ppEnabledExtensionNames = device_extensions.data();

    /* This is unnecessary but we have this here to support older vulkan devices */
    if(enable_validation_layers){
        create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        create_info.ppEnabledLayerNames = validation_layers.data();
    } else {
        create_info.enabledLayerCount = 0;
    }

    if(vkCreateDevice(physical_device, &create_info, nullptr, &device) != VK_SUCCESS){
        VK_ERROR("failed to create logical device");
    }
    VK_INFO("created logical device");

    vkGetDeviceQueue(device, indices.graphics_family.value(), 0, &gfx_queue);
    vkGetDeviceQueue(device, indices.present_family.value(), 0, &present_queue);
}

SCSD Device::query_swapchain_support(VkPhysicalDevice device){
    SCSD details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
    
    uint32_t format_c, present_c;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_c, nullptr);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_c, nullptr);

    if(format_c != 0){
        details.formats.resize(format_c);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_c, details.formats.data());
    }

    if(present_c != 0){
        details.present_modes.resize(present_c);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_c, details.present_modes.data());
    }

    return details;
}

}
