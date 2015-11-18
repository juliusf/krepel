#pragma once

#include <krEngine/ownership/borrowed.h>

#include <CoreUtils/Graphics/Camera.h>


namespace kr
{
  class Window;
  namespace Renderer { class Extractor; }

  struct KR_ENGINE_API CameraDefault2D
  {
    CameraDefault2D(kr::Borrowed<kr::Window> window);

    ~CameraDefault2D();

    void extract(kr::Renderer::Extractor& e);

    float aspect;
    ezCamera cam;
  };
}
