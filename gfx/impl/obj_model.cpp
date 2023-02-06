#include "obj_model.hpp"

namespace gfx {

Object_Model::Object_Model(){

}

Object_Model::~Object_Model(){

}

std::vector<VkVertexInputBindingDescription> Object_Model::Vertex::get_binding_descriptions(){
    std::vector<VkVertexInputBindingDescription> binding_descriptions(1);
    binding_descriptions[0].binding = 0;
    binding_descriptions[0].stride = sizeof(Vertex);
    binding_descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return binding_descriptions;
}

std::vector<VkVertexInputAttributeDescription> Object_Model::Vertex::get_attribute_descriptions(){
    std::vector<VkVertexInputAttributeDescription> attribute_descriptions{};
    attribute_descriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
    attribute_descriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
    attribute_descriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
    attribute_descriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});
    return attribute_descriptions;
}

}