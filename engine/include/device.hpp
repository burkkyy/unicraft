/**
 * @file device.hpp
 * @author Caleb Burke
 * @date Nov 18, 2023
 */

#pragma once

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
    Device&(const Device&) = delete;
    Device& operator=(const Device&) = delete;
    Device(Device&&) = delete;
    Device& operator=(Device&&) = delete;

private:
    void init();

    VkInstance instance;
};

}   // namespace eng
}   // namespace uni
