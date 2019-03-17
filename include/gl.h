#ifndef OGL_H
#define OGL_H

class GL {
  public:
    // clang-format off
    enum class Cull : unsigned char      { Front = 0x1, Back, None };
    enum class Blend : unsigned char     { Disabled = 0x1, OneMinusSourceAlpha, Additive };
    enum class DepthTest : unsigned char { Enabled = 0x1, Disabled };
    enum class FillMode : unsigned char  { Fill = 0x1, Line };
    // clang-format on

    static void setCullMode(Cull cull);
    static void setBlendState(Blend blend);
    static void setDepthTest(DepthTest dt);
    static void setFillMode(FillMode mode);

  private:
    static Cull currentCullMode;
    static Blend currentBlendMode;
    static DepthTest currentDepthTestMode;
    static FillMode currentFillMode;
};

#endif // OGL_H
