#include <krEngineTests/pch.h>
#include <catch.hpp>

#include <krEngine/transform2D.h>
#include <krEngine/rendering.h>

#include <CoreUtils/Graphics/Camera.h>

TEST_CASE("Workflow", "[sprite]")
{
  using namespace kr;

  KR_TESTS_RAII_CORE_STARTUP;

  auto pWindow = Window::createAndOpen();

  KR_TESTS_RAII_ENGINE_STARTUP;

  ezCamera cam;
  cam.SetCameraMode(ezCamera::OrthoFixedWidth,                 // Using an orthographic cam.
                    (float)pWindow->getClientAreaSize().width, // Width of the orthographic cam.
                    0.1f,                                      // Near plane.
                    1.0f);                                     // Far plane.
  cam.LookAt(ezVec3(0, 0, 0.5f),  // Camera Position.
             ezVec3(0, 0, 0),     // Target Position.
             ezVec3(0, 0, 1.0f)); // Up Vector.

  auto tex = Texture::load("<texture>kitten.dds");
  auto sampler = Sampler::create();
  auto shader = Sprite::createDefaultShader();

  Sprite sprite;
  REQUIRE_FALSE(canRender(sprite));
  sprite.setLocalBounds(ezRectFloat(0, 0, 128, 128));
  initialize(sprite, tex, sampler, shader);
  sprite.getSampler()->setFiltering(TextureFiltering::Nearest);

  // Handle the close-event of the window.
  bool run = true;
  pWindow->getEvent().AddEventHandler([&run](const WindowEventArgs& e)
  {
    if (e.type == WindowEventArgs::ClickClose)
    {
      run = false;
    }
  });

  Renderer::addExtractionListener([&sprite, &cam](Renderer::Extractor& e)
  {
    auto aspectRatio = 16.0f/9.0f;
    extract(e, cam, aspectRatio);

    REQUIRE(canRender(sprite));

    auto t = Transform2D::zero();
    t.position = ezVec2(-sprite.getLocalBounds().width / 2.0f);
    t.rotation = ezAngle::Degree(30.0f);
    extract(e, sprite, t);
  });

  while(run)
  {
    processWindowMessages(pWindow);
    Renderer::extract();
    Renderer::update(pWindow);
  }
}
