// user defined macros
#include <iostream>
#define P(x)            std::cout << x
#define INFO(T, S)      P("\033[1;32m[" << T << " INFO]\033[0m " << S << '\n')
#define WARNING(T, S)   std::cerr << "\033[1;33m[" << T << " WARNING]\033[0m " << S << '\n'
#define ERROR(T, S)     std::cerr << "\033[1;31m[" << T << " ERROR]\033[0m " << S << '\n'
#define VK_INFO(x)      INFO("VULKAN", x)
#define VK_WARNING(x)   WARNING("VULKAN", x)
#define VK_ERROR(x)     throw std::runtime_error("\033[1;31m[VULKAN ERROR]\033[0m " + (std::string)(x))
