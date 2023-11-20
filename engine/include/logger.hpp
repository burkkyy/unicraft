/**
 * @file logger.hpp
 * @author Caleb Burke
 * @date Nov 18, 2023
 */

#pragma once

#include <iostream>

#ifndef NDEBUG

#define INFO(T, ...)      std::cout << "\033[1;32m[ " << T << " ]\033[0m " << __VA_ARGS__ << '\n'
#define UPDATE(T, ...)    std::cout << "\033[1;34m[ " << T << " ]\033[0m " << __VA_ARGS__ << '\n'
#define WARNING(T, ...)   std::cout << "\033[1;33m[ " << T << " ]\033[0m " << __VA_ARGS__ << '\n'
#define VK_INFO(...)      INFO("VULKAN", __VA_ARGS__)
#define VK_WARNING(...)   WARNING("VULKAN", __VA_ARGS__)

#else

#define INFO(T, ...)
#define UPDATE(T, ...)
#define WARNING(T, ...)
#define VK_INFO(...)
#define VK_WARNING(...)

#endif

#define ERROR(T, ...)     std::cout << "\033[1;31m[ " << T << " ]\033[0m " << __VA_ARGS__ << '\n'
#define VK_ERROR(...)     ERROR("VULKAN", __VA_ARGS__)
