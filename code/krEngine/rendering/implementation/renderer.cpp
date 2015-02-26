#include <krEngine/rendering/renderer.h>
#include <krEngine/rendering/extraction.h>
#include <krEngine/rendering/window.h>

#include <krEngine/rendering/implementation/windowImpl.h>
#include <krEngine/rendering/implementation/opelGlCheck.h>
#include <krEngine/rendering/implementation/extractionBuffer.h>
#include <krEngine/rendering/implementation/extractionData.h>

namespace
{
  using ExtractionAllocator = ezAllocator<ezMemoryPolicies::ezAlignedHeapAllocation,
                                          ezMemoryTrackingFlags::All>;

}

// Globals
// =======
static bool g_initialized = false;

static kr::ExtractionBuffer* g_pReadBuffer;
static kr::ExtractionBuffer* g_pWriteBuffer;
static kr::Renderer::ExtractionEvent g_ExtractionEvent;

void GLAPIENTRY debugCallbackOpenGL(GLenum source,
                                    GLenum type,
                                    GLuint id,
                                    GLenum severity,
                                    GLsizei length,
                                    const GLchar* message,
                                    const void* userParam)
{
  /// \todo Implement me.
}

// clang-format off
namespace kr
{
  EZ_BEGIN_SUBSYSTEM_DECLARATION(krEngine, Renderer)
    BEGIN_SUBSYSTEM_DEPENDENCIES
    "Foundation",
    "Core"
    END_SUBSYSTEM_DEPENDENCIES

    krEngineRendererSubSystem() :
      m_extractionAllocator("Extraction")
    {
    }

    ExtractionAllocator m_extractionAllocator;

    ezByte m_mem_readBuffer[sizeof(ExtractionBuffer)];
    ezByte m_mem_writeBuffer[sizeof(ExtractionBuffer)];

    ON_CORE_STARTUP
    {
      g_pReadBuffer = new (m_mem_readBuffer) ExtractionBuffer(&m_extractionAllocator);
      g_pReadBuffer->setMode(ExtractionBuffer::Mode::ReadOnly);

      g_pWriteBuffer = new (m_mem_writeBuffer) ExtractionBuffer(&m_extractionAllocator);
      g_pReadBuffer->setMode(ExtractionBuffer::Mode::WriteOnly);

      g_initialized = true;
    }

    ON_ENGINE_STARTUP
    {
      //glDebugMessageCallback(debugCallbackOpenGL, nullptr);
    }

    ON_ENGINE_SHUTDOWN
    {

      //glDebugMessageCallback(nullptr, nullptr);
    }

    ON_CORE_SHUTDOWN
    {
      g_pWriteBuffer->~ExtractionBuffer();
      g_pWriteBuffer = nullptr;

      g_pReadBuffer->~ExtractionBuffer();
      g_pReadBuffer = nullptr;

      g_initialized = false;
    }
  EZ_END_SUBSYSTEM_DECLARATION
}
// clang-format on

namespace kr
{
  namespace Renderer
  {
    class ExtractorImpl : public Extractor
    {
    public:
      ExtractorImpl() = default;
    };
  }
}

static void renderExtractionData(ezUByte* current, ezUByte* max)
{
  using namespace kr;

  while(current < max)
  {
    auto data = reinterpret_cast<ExtractionData*>(current);

    switch(data->type)
    {
    case ExtractionDataType::Sprite:
    {
      auto& sprite = *static_cast<SpriteData*>(data);
      draw(sprite);
      sprite.~SpriteData();
    }
      break;
    default:
      EZ_REPORT_FAILURE("Unknown extraction data type.");
      break;
    }

    EZ_ASSERT(current + data->byteCount <= max, "Must never exceed max!");
    current += data->byteCount;
  }
}

void kr::Renderer::extract()
{
  g_pWriteBuffer->reset();
  ExtractorImpl e;
  g_ExtractionEvent.Broadcast(e);
  swap(g_pReadBuffer, g_pWriteBuffer);
}

static ezResult presentFrame(const kr::WindowImpl& window)
{
  return ::SwapBuffers(window.m_hDC) ? EZ_SUCCESS
                                     : EZ_FAILURE;
}

void kr::Renderer::update(ezTime dt, RefCountedPtr<Window> pTarget)
{
  if(isNull(pTarget))
  {
    ezLog::Warning("Invalid target window.");
    return;
  }

  auto& window = getImpl(pTarget);
  /// \todo This is Window specific.
  glCheck(wglMakeCurrent(window.m_hDC, window.m_hRC));

  // Clear the Screen
  // ================
  {
    auto& clearColor = window.m_clearColor;
    glCheck(glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a));
  }

  glCheck(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

  // Render the Data
  // ===============
  {
    ezUByte* current = begin(*g_pReadBuffer);
    ezUByte* max = end(*g_pReadBuffer);
    renderExtractionData(current, max);
  }

  // Swap Buffers
  // ============

  if (presentFrame(window).Failed())
  {
    ezLog::Warning("Failed to present frame.");
  }
}

void kr::Renderer::addExtractionListener(ExtractionEventListener listener)
{
  g_ExtractionEvent.AddEventHandler(listener);
}

void kr::Renderer::removeExtractionListener(ExtractionEventListener listener)
{
  g_ExtractionEvent.RemoveEventHandler(listener);
}

void kr::extract(Renderer::Extractor& e,
                 const Sprite& sprite,
                 ezTransform transform)
{
  auto pData = g_pWriteBuffer->allocate<SpriteData>();
  pData->pTexture = sprite.getTexture();
  pData->pVertexBuffer = sprite.getVertexBuffer();
  pData->pShader = sprite.getShader();
  pData->pSampler = sprite.getSampler();

  pData->uColor = sprite.getColorUniform();
  pData->uTexture = sprite.getTextureUniform();

  pData->transform = move(transform);
  pData->color = sprite.getColor();
}
