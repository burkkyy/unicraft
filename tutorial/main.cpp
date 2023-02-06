// std libraries
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cassert>
#include <string>
#include <cstring>
#include <vector>
#include <memory>
#include <optional>

// vulkan libraries
#include <vulkan/vulkan.h>

// glfw libraries
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// glm libraries
// #include <glm/glm.hpp>

// user defined macros
#define P(x)            std::cout << x
#define INFO(T, S)      P("\033[1;32m[" << T << " INFO]\033[0m " << S << '\n')
#define WARNING(T, S)   std::cerr << "\033[1;33m[" << T << " WARNING]\033[0m " << S << '\n'
#define ERROR(T, S)     std::cerr << "\033[1;31m[" << T << " ERROR]\033[0m " << S << '\n'
#define VK_INFO(x)      INFO("VULKAN", x)
#define VK_WARNING(x)   WARNING("VULKAN", x)
#define VK_ERROR(x)     throw std::runtime_error("\033[1;31m[VULKAN ERROR]\033[0m " + (std::string)(x))

struct QueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
};

class App {
    public:
        void run(){
            __init__();
        }

    private:
        const uint32_t WIDTH = 800;
        const uint32_t HEIGHT = 600;
        const char* NAME = "unicraft";

        const std::vector<const char*> validation_layers = {
            "VK_LAYER_KHRONOS_validation"
        };

        #ifdef NDEBUG
            const bool enable_validation_layers = false;
        #else
            const bool enable_validation_layers = true;
        #endif

        GLFWwindow* window;
        VkInstance instance;
        VkDebugUtilsMessengerEXT debug_messenger;
        
        /*
        This is the vulkan handler for a physical GPU.
        Implicitly destroyed when VkInstance is destroyed.

        It is set to a null handle, so we can later check
        if it is still a null handle for error checking
        */
        VkPhysicalDevice physical_device = VK_NULL_HANDLE;

        void __init__(){
            __init__glfw();
            __init__vulkan();
            loop();
            cleanup();
        }

        void __init__glfw(){
            glfwInit();
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
            window = glfwCreateWindow(WIDTH, HEIGHT, NAME, nullptr, nullptr);
        }

        void __init__vulkan(){
            create_instance();
            setup_debug_messenger();
            pick_physical_device();
        }

        void pick_physical_device(){
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
        }

        bool is_device_suitable(VkPhysicalDevice device){
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
            
            return device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && device_features.geometryShader;
        }

        QueFamilyIndices find_que_families(VkPhysicalDevice device){
            QueFamilyIndices indices;
            return indices;
        }

        void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info) {
            create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            create_info.pfnUserCallback = debug_callback;
        }

        void setup_debug_messenger(){
            if(!enable_validation_layers){ return; }

            VkDebugUtilsMessengerCreateInfoEXT create_info;
            populate_debug_messenger_create_info(create_info);

            if(create_debug_utils_messenger_EXT(instance, &create_info, nullptr, &debug_messenger) != VK_SUCCESS){
                VK_ERROR("failed to set up debug messenger");
            }
            VK_INFO("set up debug messenger");
        }

        VkResult create_debug_utils_messenger_EXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
            auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
            if(func != nullptr) {
                return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
            } else {
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }

        void destroy_debug_utils_messenger_EXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
            if(func != nullptr){ func(instance, debugMessenger, pAllocator); }
        }

        void create_instance(){
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

        void loop(){
            while(!glfwWindowShouldClose(window)){
                glfwPollEvents();
            }
        }

        void cleanup(){
            if(enable_validation_layers){
                destroy_debug_utils_messenger_EXT(instance, debug_messenger, nullptr);
            }

            /*
            Note that all other vulkan resources should be destroyed before VkInstance
            */
            vkDestroyInstance(instance, nullptr);

            /*
            Destroys all glfw resources
            */
            glfwDestroyWindow(window);
            glfwTerminate();
        }

        bool check_validation_layer_support(){
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

        std::vector<const char*> get_required_extensions(){
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
};

int main(int, char const**){
    App app;

    try{
        app.run();
    } catch(const std::exception& e){
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
