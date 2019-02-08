#ifndef TEXTURE_H
#define TEXTURE_H
namespace Visual {
typedef unsigned int GLuint;
class ITexture {
  protected:
    GLuint m_textureId = 0;
    int m_width, m_height, m_channels;

  public:
    ITexture() = default;
    ITexture(const ITexture &other) = delete;
    ITexture(ITexture &&other) = delete;
    ITexture &operator=(const ITexture &other) = delete;
    virtual ~ITexture() = 0;

    unsigned int getId() const { return m_textureId; }
    void useSlot(const GLuint slot);
    void bind(const GLuint location) const;
    void unbind(const GLuint location) const;
};

class TextureFormFile : public ITexture {
  public:
    TextureFormFile() = delete;
    TextureFormFile(const char *path);
};

class TextureFrameBuffer : public ITexture {
  public:
    TextureFrameBuffer(const int size);
};

} // namespace Visual
#endif // TEXTURE_H
