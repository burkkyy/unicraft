/*
Class to handle objects loaded from disk that will be rendered onto screen
*/

#include "device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace gfx {

class Object_Model {
public:
    struct Vertex {
        glm::vec2 position{}; // change to vec3 later, currently using 2d
        glm::vec3 color{};
        glm::vec3 normal{};
        glm::vec2 uv{};
        
        static std::vector<VkVertexInputBindingDescription> get_binding_descriptions();
        static std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions();

        bool operator==(const Vertex& v) const {
            return position == v.position && color == v.color && normal == v.color && uv == v.uv;
        }
    };

    Object_Model();
    ~Object_Model();

private:

};

}
