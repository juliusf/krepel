#include <krEngine/game/defaultMainModule.h>

class ~{friendly_name}_module : public kr::DefaultMainModule
{
public:
  ~{friendly_name}_module()
  {
    m_windowDesc.m_Title = "~{friendly_name} - Powered by krepel";
  }

} s_mainModule; // <-- Static instance is required.
