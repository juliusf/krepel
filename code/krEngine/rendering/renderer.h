#pragma once
#include <krEngine/rendering/window.h>

class ezClock;

namespace kr
{
  namespace Renderer
  {
    class KR_ENGINE_API Extractor
    {
    protected: // *** Construction
      Extractor() = default;
    };

    using ExtractionEvent = ezEvent<Extractor&>;
    using ExtractionEventListener = ExtractionEvent::Handler;

    KR_ENGINE_API void addExtractionListener(ExtractionEventListener listener);
    KR_ENGINE_API void removeExtractionListener(ExtractionEventListener listener);

    KR_ENGINE_API void extract();
    KR_ENGINE_API void update(Borrowed<Window> pTarget);

    KR_ENGINE_API ezClock* clock();
  };
}
