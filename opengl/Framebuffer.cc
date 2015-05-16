#include "opengl/Framebuffer.hh"

#include <iostream>
#include <stdexcept>
#include <sstream>

#include "opengl/Framebuffer.hh"

using namespace glm;

namespace opengl {

Framebuffer::Framebuffer(Config config, ivec2 size)
    : config(config),
      size(size),
      name(0),
      depthName(0),
      depthTexture(config & DEPTH_TEXTURE ? new Texture : nullptr) {
    glGenFramebuffers(1, &name);
    glBindFramebuffer(GL_FRAMEBUFFER, name);

    if (config & COLOR_TEXTURE) {
        glBindTexture(GL_TEXTURE_2D, colorTexture->getName());

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 

        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                               GL_TEXTURE_2D, colorTexture->getName(), 0);
    }

    if (config & DEPTH_TEXTURE) {
        glBindTexture(GL_TEXTURE_2D, depthTexture->getName());

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32,
                     size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT,
                     nullptr);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D, depthTexture->getName(), 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::stringstream ss;
        ss << status;
        throw std::runtime_error("Failed to create framebuffer:" + ss.str());
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /*
    if (config & DEPTH) {
        glGenRenderbuffersEXT(1, &depthName);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthName);
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,
                                 GL_DEPTH_COMPONENT,
                                 size.x,
                                 size.y);
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
                                     GL_DEPTH_ATTACHMENT_EXT,
                                     GL_RENDERBUFFER_EXT,
                                     depthName);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
                                     GL_DEPTH_ATTACHMENT_EXT,
                                     GL_RENDERBUFFER_EXT,
                                     depthName);
    }
    */
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffersEXT(1, &name);

    if (depthName)
        glDeleteRenderbuffersEXT(1, &name);
}

Texture const& Framebuffer::getColorTexture() const {
    assert(colorTexture);
    return *colorTexture.get();
}

void Framebuffer::renderIntoImpl(std::function<void()> const& f,
                                 Framebuffer::Clear clear) const {
    glBindFramebuffer(GL_FRAMEBUFFER, name);
    glPushAttrib(GL_VIEWPORT_BIT);
    glViewport(0, 0, size.x, size.y);

    if (clear) {
        GLuint clearFlags = 0;
        //if (config & COLOR) clearFlags |= GL_COLOR_BUFFER_BIT;
        if (config & DEPTH_TEXTURE) clearFlags |= GL_DEPTH_BUFFER_BIT;

        glClear(clearFlags);
    }

    f();

    glPopAttrib();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace game
