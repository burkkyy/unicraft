#include "pipeline.hpp"

namespace gfx {

Pipeline::Pipeline(Device& d, const std::string& vert_filepath, const std::string& frag_filepath) : device{d}{
    create(vert_filepath, frag_filepath);
}

Pipeline::~Pipeline(){
    vkDestroyShaderModule(device.get_device(), vert, nullptr);
    VK_INFO("destroyed vertex shader module");
    
    vkDestroyShaderModule(device.get_device(), frag, nullptr);
    VK_INFO("destroyed fragment shader module");

    vkDestroyPipeline(device.get_device(), pipeline, nullptr);
    VK_INFO("destroyed graphics pipeline");
}

std::vector<char> Pipeline::load(const std::string& fp){
    std::ifstream f(fp, std::ios::ate | std::ios::binary);
    if(!f.is_open()){ VK_ERROR("failed to open file: " + fp); }

    size_t size = (size_t)f.tellg();
    std::vector<char> buffer(size);

    f.seekg(0);
    f.read(buffer.data(), size);
    f.close();

    return buffer;
}

void Pipeline::create(const std::string& vert_filepath, const std::string& frag_filepath){
    auto vert_code = load(vert_filepath);
    auto frag_code = load(frag_filepath);

    create_shader_mod(vert_code, &vert);
    create_shader_mod(frag_code, &frag);

    VkPipelineShaderStageCreateInfo vert_create_info{
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        NULL, 0, VK_SHADER_STAGE_VERTEX_BIT,
        vert, "main", NULL
    };

    VkPipelineShaderStageCreateInfo frag_create_info{
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        NULL, 0, VK_SHADER_STAGE_FRAGMENT_BIT,
        frag, "main", NULL
    };

    VkPipelineShaderStageCreateInfo stages[] = {vert_create_info, frag_create_info};

    auto binding_descriptions = Object_Model::Vertex::get_binding_descriptions();
    auto attribute_descriptions = Object_Model::Vertex::get_attribute_descriptions();
}

void Pipeline::create_shader_mod(const std::vector<char>& code, VkShaderModule* mod){
    VkShaderModuleCreateInfo create_info{
        VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        nullptr, 0, code.size(),
        reinterpret_cast<const uint32_t*>(code.data()),
    };

    if(vkCreateShaderModule(device.get_device(), &create_info, nullptr, mod) != VK_SUCCESS){
        VK_ERROR("failed to create shader module");
    }
}

}
