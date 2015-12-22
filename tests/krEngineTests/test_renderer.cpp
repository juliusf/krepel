#include <krEngineTests/pch.h>
#include <catch.hpp>

#include <krEngine/rendering.h>

namespace
{
  struct Vertex
  {
    ezVec2 pos;
    ezColor color = ezColor::White;
    ezVec2 texCoords = ezVec2::ZeroVector();
  };
}

EZ_DECLARE_REFLECTABLE_TYPE(EZ_NO_LINKAGE, Vertex);

EZ_BEGIN_STATIC_REFLECTED_TYPE(Vertex, ezNoBase, 1, ezRTTINoAllocator);
  EZ_BEGIN_PROPERTIES
    // We use the names used in the shader here.
    EZ_MEMBER_PROPERTY("vs_position",  pos),
    EZ_MEMBER_PROPERTY("vs_color",     color),
    EZ_MEMBER_PROPERTY("vs_texCoords", texCoords)
  EZ_END_PROPERTIES
EZ_END_STATIC_REFLECTED_TYPE();

TEST_CASE("Experiments", "[renderer][experiments]")
{
  using namespace kr;

  KR_TESTS_RAII_CORE_STARTUP;

  ezWindowCreationDesc desc;
  desc.m_ClientAreaSize.width = 600;
  desc.m_ClientAreaSize.height = 480;
  desc.m_Title = "Sprite Test";
  auto pWindow = Window::createAndOpen(desc);
  REQUIRE(pWindow != nullptr);
  //pWindow->setClearColor(ezColor::CornflowerBlue);

  KR_TESTS_RAII_ENGINE_STARTUP;

  {
    // Shaders and Shader Program
    // ==========================
    auto shader = ShaderProgram::loadAndLink("<shader>texturedQuad.vs", "<shader>texturedQuad.fs");
    REQUIRE(shader != nullptr);

    KR_RAII_BIND_SHADER(shader);

    // Vertices
    // ========
    Vertex vertices[4];

    const auto tw = 512;
    const auto th = 512;

    const auto x = 32;
    const auto y = 32;
    const auto w = x + 256;
    const auto h = y + 256;

    auto left   = float(x) / float(tw);
    auto right  = float(w) / float(tw);
    auto top    = float(h) / float(th);
    auto bottom = float(y) / float(th);

    //vertices[0].pos.Set(-1.0f, -1.0f); // Lower Left
    //vertices[1].pos.Set(-1.0f,  1.0f); // Upper Left
    //vertices[2].pos.Set( 1.0f, -1.0f); // Lower Right
    //vertices[3].pos.Set( 1.0f,  1.0f); // Upper Right

    vertices[0].pos.Set(left  * 2 - 1,bottom * 2 - 1); // Lower Left
    vertices[1].pos.Set(left  * 2 - 1,top    * 2 - 1); // Upper Left
    vertices[2].pos.Set(right * 2 - 1,bottom * 2 - 1); // Lower Right
    vertices[3].pos.Set(right * 2 - 1,top    * 2 - 1); // Upper Right

    vertices[0].color = ezColor::Red;
    vertices[1].color = ezColor::Green;
    vertices[2].color = ezColor::Blue;
    vertices[3].color = ezColor::Yellow;

    //vertices[0].texCoords.Set(0.0f, 1.0f); // Upper Left
    //vertices[1].texCoords.Set(0.0f, 0.0f); // Lower Left
    //vertices[2].texCoords.Set(1.0f, 1.0f); // Upper Right
    //vertices[3].texCoords.Set(1.0f, 0.0f); // Lower Right

    vertices[0].texCoords.Set(left,  top);
    vertices[1].texCoords.Set(left,  bottom);
    vertices[2].texCoords.Set(right, top);
    vertices[3].texCoords.Set(right, bottom);

    // Vertex Buffer
    // =============
    auto vb = VertexBuffer::create(BufferUsage::StaticDraw, PrimitiveType::Triangles);
    //REQUIRE(setupLayout(vb, prg, "Vertex").Succeeded());
    setupLayout(vb, shader, "Vertex");
    REQUIRE(uploadData(vb, ezMakeArrayPtr(vertices)).Succeeded());

    // Texture
    // =======
    auto tex = Texture::load("<texture>kitten.dds");
    REQUIRE(tex != nullptr);

    auto pSampler = Sampler::create();
    KR_RAII_BIND_SAMPLER(pSampler, TextureSlot(0));

    uploadData(shaderUniformOf(shader, "u_texture"), TextureSlot(0));

    glBindTexture(GL_TEXTURE_2D, tex->getGlHandle());

    KR_RAII_BIND_VERTEX_BUFFER(vb, shader);

    bool run = true;
    pWindow->getEvent().AddEventHandler([&run](const WindowEventArgs& e)
    {
      if (e.type == WindowEventArgs::ClickClose)
        run = false;
    });

    auto now = ezTime::Now();
    //auto targetTime = now + ezTime::Seconds(2);
    //while(now < targetTime)
    while(run)
    {
      processWindowMessages(pWindow);
      Renderer::extract();
      Renderer::update(pWindow);
    }
  }
}
