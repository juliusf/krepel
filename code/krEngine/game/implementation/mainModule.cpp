#include <krEngine/game/mainModule.h>

static kr::MainModule* g_pMainModule{ nullptr };

void kr::MainModule::setGlobalInstance(MainModule* pModule)
{
  g_pMainModule = pModule;
}

kr::MainModule* kr::MainModule::globalInstance()
{
  return g_pMainModule;
}

kr::MainModule::MainModule()
{
}

kr::MainModule::~MainModule()
{
}

const char* kr::MainModule::GetSubSystemName() const
{
  return "main";
}

const char* kr::MainModule::GetGroupName() const
{
  return "Game";
}

const char* kr::MainModule::GetDependency(ezInt32 depIndex)
{
  // Note: ez expects a nullptr when all dependencies are given.
  //       If we never return nullptr, we get an infinite loop when
  //       starting up the ezEngine.
  const char* deps[] = { "krEngine", nullptr };
  return deps[depIndex];
}
