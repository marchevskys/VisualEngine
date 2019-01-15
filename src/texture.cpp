#include "texture.h"
#include "logger.h"

#include <GL/glew.h>
#include <SOIL/SOIL.h>
namespace Visual {
Texture::Texture(const char *path) {

    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);                    // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
                                                                  // Set our texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load, create texture and generate mipmaps

    unsigned char *image = SOIL_load_image(path, &m_width, &m_height, &m_channels, SOIL_LOAD_AUTO);
    if (image) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(image);
        glBindTexture(GL_TEXTURE_2D, 0);
    } else {
        DLOG(path, " ", m_channels);
        THROW("No texture found");
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::use() {
    //glTexSubImage2D(GL_TEXTURE_2D,0,0,0,1920,1080,GL_DEPTH_COMPONENT,GL_FLOAT,&tabDepth);
    //void* aaa;
    //glTexSubImage2D(GL_TEXTURE_2D, 3, 0, 0, m_width, m_height, GL_RGB, GL_FLOAT, &aaa);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
}

Texture::~Texture() {
    glDeleteTextures(1, &m_textureID);
}
} // namespace Visual
