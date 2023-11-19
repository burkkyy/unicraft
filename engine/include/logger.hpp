/**
 * @file logger.hpp
 * @author Caleb Burke
 * @date Nov 18, 2023
 */

#pragma once

#include <iostream>

#ifndef NDEBUG

#define INFO(T, S)      std::cout << "\033[1;32m[ " << T << " ]\033[0m " << S << '\n'
#define UPDATE(T, S)    std::cout << "\033[1;34m[ " << T << " ]\033[0m " << S << '\n'
#define WARNING(T, S)   std::cout << "\033[1;33m[ " << T << " ]\033[0m " << S << '\n'
#define VK_INFO(S)      INFO("VULKAN", S)
#define VK_WARNING(S)   WARNING("VULKAN", S)

#else

#define INFO(T, S)
#define UPDATE(T, S)
#define WARNING(T, S)
#define VK_INFO(S)
#define VK_WARNING(S)

#endif

#define ERROR(T, ...)     std::cout << "\033[1;31m[ " << T << " ]\033[0m " << __VA_ARGS__ << '\n'
#define VK_ERROR(...)     ERROR("VULKAN", __VA_ARGS__)
