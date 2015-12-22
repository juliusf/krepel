#pragma once

#include <krEngine/ownership.h>
#include <krEngine/game/mainModule.h>
#include <krEngine/game/gameLoop.h>

#include <System/Window/Window.h>
#include <Foundation/Logging/HTMLWriter.h>
#include <Foundation/Time/Clock.h>


namespace kr
{
  class Window;

  struct DefaultGameLoopPriorities
  {
    enum Enum
    {
      Clock = -20,
      MessagePump = -10,
      Input = -5,
      Module = 0,
      Rendering = 20,
    };

    DefaultGameLoopPriorities() = delete;
  };

  class KR_ENGINE_API DefaultWindow : public ezWindow
  {
  public: // *** Inherited from ezWindow
    virtual void OnClickCloseMessage();

  public: // *** Accessors

    bool userRequestsClose() const { return m_userRequestsClose; }

  protected: // *** Data
    bool m_userRequestsClose;
  };

  class KR_ENGINE_API DefaultMainModule : public MainModule
  {
  public: // *** Construction
    DefaultMainModule();
    virtual ~DefaultMainModule();

  public: // *** Inherited via kr::MainModule
    virtual void OnCoreStartup() override;
    virtual void OnEngineStartup() override;
    virtual void OnEngineShutdown() override;
    virtual void OnCoreShutdown() override;

  public: // *** Runtime

    /// \brief The module tick function.
    ///
    /// Override this to hook into the global game loop after the clock
    /// and input has been processed.
    ///
    /// Alternatively, you can directly use kr::GlobalGameLoop to hook
    /// into there. Use kr::DefaultGameLoopPriorities to set your own priority.
    virtual void tick() {}

  public: // *** Properties
    Borrowed<Window> window() { return m_pWindow; }
    ezClock* clock() { return &m_clock; }

  protected: // *** Data
    ezPlugin m_plugin{ false };
    ezClock m_clock;
    ezWindowCreationDesc m_windowDesc;
    Owned<Window> m_pWindow;
    ezLogWriter::HTML m_htmlLog;
  };
}
