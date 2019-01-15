#ifndef TEXTURE_H
#define TEXTURE_H
namespace Visual {
class Texture {
    unsigned int m_textureID;
    int m_width, m_height, m_channels;

  public:
    Texture(const char *path);
    ~Texture();
    void use();
};
} // namespace Visual
#endif // TEXTURE_H
