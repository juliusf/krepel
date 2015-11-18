#pragma once

#include <Foundation/Types/Types.h>
#include <Foundation/Utilities/EnumerableClass.h>
#include <Foundation/Configuration/Plugin.h>

namespace kr
{
  class KR_ENGINE_API MainModule : public ezSubSystem
  {
  public: // *** Static
    static void setGlobalInstance(MainModule* pModule);
    static MainModule* globalInstance();

  public: // *** Construction
    MainModule();
    virtual ~MainModule();

  public: // *** Inherited from ezSubSystem
    virtual const char* GetSubSystemName() const override;
    virtual const char* GetGroupName() const override;
    virtual const char* GetDependency(ezInt32 depIndex) override;
  };
}
