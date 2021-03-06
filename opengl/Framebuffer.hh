#pragma once

#include <functional>
#include <memory>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "opengl/Texture.hh"

namespace opengl {

struct Framebuffer {
    enum Config {
        COLOR_TEXTURE = 1,
        //DEPTH = 2
        DEPTH_TEXTURE = 4
    };

    enum Clear : bool {
        NO_CLEAR = false,
        CLEAR = true
    };

    Framebuffer(Config config, glm::ivec2 size);
    ~Framebuffer();

    Texture const& getColorTexture() const;

    template<typename F>
    void renderInto(F f, Clear clear = NO_CLEAR) const {
        renderIntoImpl(f, clear); 
    }

private:
    Config config;
    glm::ivec2 size;

    GLuint name;
    GLuint depthName;

    std::unique_ptr<Texture> colorTexture;
    std::unique_ptr<Texture> depthTexture;

    void renderIntoImpl(std::function<void()> const&, Clear) const;
};

} // namespace game
