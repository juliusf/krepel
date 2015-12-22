#include <krEngine/game/camera.h>
#include <krEngine/rendering/window.h>
#include <krEngine/rendering/renderer.h>
#include <krEngine/rendering/extraction.h>

kr::CameraDefault2D::CameraDefault2D(kr::Borrowed<kr::Window> window)
{
  auto size = window->getClientAreaSize();
  this->aspect = static_cast<float>(size.width) / size.height;

  this->cam.SetCameraMode(ezCamera::OrthoFixedWidth,       // Using an orthographic cam.
                          static_cast<float>(size.width),  // Width of the orthographic cam.
                          0.1f,                            // Near plane.
                          1.0f);                           // Far plane.
  this->cam.LookAt(ezVec3(0, 0, -0.5f), // Camera Position.
                   ezVec3(0, 0, 0),    // Target Position.
                   ezVec3(0, 1, 0));   // Up Vector.

  kr::Renderer::addExtractionListener({ &CameraDefault2D::extract, this });
}

kr::CameraDefault2D::~CameraDefault2D()
{
  kr::Renderer::removeExtractionListener({ &CameraDefault2D::extract, this });
}

void kr::CameraDefault2D::extract(kr::Renderer::Extractor& e)
{
  kr::extract(e, this->cam, this->aspect);
}

