#pragma once
#include <krEngine/referenceCounting.h>
#include <krEngine/rendering/texture.h>
#include <krEngine/rendering/vertexBuffer.h>
#include <krEngine/rendering/shader.h>

namespace kr
{
  namespace Renderer{ class Extractor; };

  class KR_ENGINE_API Sprite
  {
  public: // *** Accessors/Mutators
    bool getNeedsUpdate() const { return m_needsUpdate; }

    void setColor(ezColor c) { m_color = move(c); }
    ezColor getColor() const { return m_color; }

    /// \brief Set the current texture.
    void setTexture(TexturePtr pTex)
    {
      m_needsUpdate = true;
      m_pTexture = pTex;
    }

    /// \brief Gets the handle to the current texture.
    TexturePtr getTexture() const { return m_pTexture; }

    /// \brief Set the bounds (or "view") of this sprite into the texture.
    void setBounds(ezRectU32 newBounds)
    {
      m_needsUpdate = true;
      m_bounds = move(newBounds);
    }

    /// \brief Get the bounds of this sprite.
    ezRectU32 getBounds() const { return m_bounds; }

    ShaderUniform getTextureUniform() const { return m_uTexture; }
    ShaderUniform getColorUniform() const { return m_uColor; }
    ShaderProgramPtr getShader() const { return m_pShader; }
    VertexBufferPtr getVertexBuffer() const { return m_pVertexBuffer; }
    SamplerPtr getSampler() const { return m_pSampler; }

  public: // *** Friends
    friend KR_ENGINE_API void update(Sprite& sprite);

  private: // *** Data
    /// \brief \c true if the state was altered without updating.
    bool m_needsUpdate = false;

    ShaderUniform m_uTexture;
    ShaderUniform m_uColor;

    ShaderProgramPtr m_pShader;

    VertexBufferPtr m_pVertexBuffer;

    SamplerPtr m_pSampler;

    /// \brief Handle to the texture used by this sprite.
    TexturePtr m_pTexture;

    /// \brief The bounds (or "view") of this sprite into the texture.
    ///
    /// By default, the entire texture is used.
    ezRectU32 m_bounds = { 0u, 0u };

    ezColor m_color = ezColor::GetWhite();
  };

  /// \brief Updates the internal state of \a sprite.
  KR_ENGINE_API void update(Sprite& sprite);

  /// \brief Initializes a sprite.
  /// \note This is actually just an update call.
  inline ezResult initialize(Sprite& sprite)
  {
    update(sprite);
    return sprite.getNeedsUpdate() ? EZ_FAILURE : EZ_SUCCESS;
  }
}
