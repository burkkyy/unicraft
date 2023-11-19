/**
 * @file device.cpp
 * @author Caleb Burke
 * @date Nov 18, 2023
 */

#include "device.hpp"

namespace uni {
namespace eng {

/**
 * @brief Constructor
 * @note Only constructor
 * @param[in] window
 */
Device::Device(Window& window) : window{window} {
    init();
}

Device::~Device(){

}

/**
 * @brief Creates device
 * @note Called by constructor
 * @return void
 */
void Device::init(){

}

}   // namespace eng
}   // namespace uni
