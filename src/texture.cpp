#include "texture.h"
#include "logger.h"

#include <GL/glew.h>
#include <SOIL/SOIL.h>
namespace Visual {

void ITexture::useSlot(GLuint slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
}

void ITexture::bind() const {
    glBindTexture(GL_TEXTURE_2D, m_textureId);
}

void ITexture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

ITexture::~ITexture() {
    glDeleteTextures(1, &m_textureId);
}

TextureFormFile::TextureFormFile(const char *path) {

    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_2D, m_textureId);                    // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
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

TextureFrameBuffer::TextureFrameBuffer(const int size) {
    m_width = size;
    m_height = size;
    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
}

} // namespace Visual
