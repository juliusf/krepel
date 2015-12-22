#include <krEngine/game/gameLoop.h>

#include <Foundation/Configuration/SubSystem.h>


EZ_BEGIN_SUBSYSTEM_DECLARATION(krEngine, GlobalGameLoop)
  ON_CORE_SHUTDOWN
  {
    kr::GlobalGameLoop::reset();
  }
EZ_END_SUBSYSTEM_DECLARATION


namespace
{
  struct GameLoop
  {
    bool isGarbage{ false };
    ezString name;
    ezInt32 priority{ 0 };
    kr::GlobalGameLoopCallback callback;

    GameLoop(ezStringView name, kr::GlobalGameLoopCallback cb) : name{ kr::move(name) }, callback{ kr::move(cb) }
    {
    }
  };
}

static ezDynamicArray<GameLoop, ezStaticAllocatorWrapper>& globalGameLoops()
{
  static ezDynamicArray<GameLoop, ezStaticAllocatorWrapper> value;
  return value;
}

static bool g_gameLoopsNeedSorting{ false };

#include <algorithm>

static void sortByPriority()
{
  auto& loops{ globalGameLoops() };

  std::stable_sort(begin(loops), end(loops), [](const GameLoop& a, const GameLoop& b){ return a.priority > b.priority; });

  g_gameLoopsNeedSorting = false;
}

GameLoop* internalGet(ezStringView loopName, ezLogInterface* pLogInterface)
{
  auto& all = globalGameLoops();
  for(auto& loop : all)
  {
    if(loop.name == loopName)
    {
      ezLog::VerboseDebugMessage(pLogInterface, "Found global game loop with the given name.");
      return &loop;
    }
  }

  ezLog::Warning(pLogInterface, "Unable to find global game loop with the given name.");
  return nullptr;
}

void kr::GlobalGameLoop::set(ezStringView loopName, GlobalGameLoopCallback callback, ezLogInterface* pLogInterface)
{
  EZ_LOG_BLOCK(pLogInterface, "Add Global Game Loop", ezStringBuilder(loopName).GetData());

  if (!callback.IsValid())
  {
    ezLog::Warning(pLogInterface, "The given game loop callback is not valid.");
  }

  auto& all = globalGameLoops();
  for(auto& loop : all)
  {
    if(loop.name == loopName)
    {
      ezLog::Info(pLogInterface, "Overwriting existing game loop.");
      loop.callback = move(callback);
      return;
    }
  }

  all.PushBack(move(GameLoop{ move(ezString{ loopName }), move(callback) }));
  ezLog::Success(pLogInterface, "Game loop added successfully.");

  g_gameLoopsNeedSorting = true;
}

kr::GlobalGameLoopCallback* kr::GlobalGameLoop::get(ezStringView loopName, ezLogInterface* pLogInterface)
{
  EZ_LOG_BLOCK(pLogInterface, "Get Global Game Loop", ezStringBuilder{ loopName });

  auto ptr{ internalGet(loopName, pLogInterface) };
  return ptr == nullptr ? nullptr : &ptr->callback;
}

ezResult kr::GlobalGameLoop::remove(ezStringView loopName, ezLogInterface* pLogInterface /*= nullptr*/)
{
  EZ_LOG_BLOCK(pLogInterface, "Removing Global Game Loop", ezStringBuilder(loopName));

  auto& all{ globalGameLoops() };
  for(ezUInt32 i = 0; i < all.GetCount(); ++i)
  {
    if(all[i].name == loopName)
    {
      if (GlobalGameLoop::isTicking())
      {
        // While ticking, we cannot simply remove the loop. We mark it as garbage and remove them once we're done ticking.
        all[i].isGarbage = true;
      }
      else
      {
        // Since we are not ticking the global game loop, we can immediately remove it.
        all.RemoveAt(i);
      }

      ezLog::Success(pLogInterface, "Removed game loop.");
      return EZ_SUCCESS;
    }
  }

  ezLog::Warning(pLogInterface, "Given game loop instance is not registered. Unable to remove.");
  return EZ_FAILURE;
}

void kr::GlobalGameLoop::setPriority(ezStringView loopName, ezInt32 priority, ezLogInterface* pLogInterface /*= nullptr*/)
{
  EZ_LOG_BLOCK(pLogInterface, "Set Priority For Global Game Loop", ezStringBuilder(loopName));

  auto pLoop = internalGet(loopName, pLogInterface);
  if(pLoop == nullptr)
  {
    ezLog::Warning(pLogInterface, "Unable to set priority on loop ");
    return;
  }

  pLoop->priority = priority;
  g_gameLoopsNeedSorting = true;
}

ezInt32 kr::GlobalGameLoop::getPriority(ezStringView loopName, ezLogInterface* pLogInterface /*= nullptr*/)
{
  EZ_LOG_BLOCK(pLogInterface, "Get Priority of Global Game Loop", ezStringBuilder(loopName));

  auto pLoop = internalGet(loopName, pLogInterface);
  if(pLoop == nullptr)
  {
    ezLog::Warning(pLogInterface, "Unable to set priority on loop ");
    return 0;
  }

  return pLoop->priority;
}

static bool g_globalGameLoopIsTicking{ false };

void kr::GlobalGameLoop::tick(ezLogInterface* pLogInterface)
{
  g_globalGameLoopIsTicking = true;
  KR_ON_SCOPE_EXIT{ g_globalGameLoopIsTicking = false; };

  if (g_gameLoopsNeedSorting)
  {
    ezLog::Info(pLogInterface, "Sorting game loops by priority.");
    sortByPriority();
  }

  // Tick all loops.
  auto& all = globalGameLoops();
  for(auto& loop : all)
  {
    EZ_LOG_BLOCK(pLogInterface, "Ticking Global Game Loop", loop.name);
    if (!loop.isGarbage && loop.callback.IsValid())
    {
      loop.callback();
    }
  }

  // Collect Garbage
  // ===============
  ezUInt32 leftToCheck{ all.GetCount() };
  while(leftToCheck > 0)
  {
    auto index = leftToCheck - 1;
    if (all[index].isGarbage)
    {
      all.RemoveAt(index);
    }
    --leftToCheck;
  }
}

static bool g_keepGlobalGameLoopTicking{ true };

bool kr::GlobalGameLoop::keepTicking()
{
  return g_keepGlobalGameLoopTicking;
}

void kr::GlobalGameLoop::setKeepTicking(bool value)
{
  g_keepGlobalGameLoopTicking = value;
}

bool kr::GlobalGameLoop::isTicking()
{
  return g_globalGameLoopIsTicking;
}

void kr::GlobalGameLoop::printTickOrder(ezLogInterface* pLogInterface /*= nullptr*/)
{
  if (g_gameLoopsNeedSorting)
  {
    sortByPriority();
  }

  auto& loops{ globalGameLoops() };
  for (auto& loop : loops)
  {
    ezLog::Info(pLogInterface, "%s", loop.name.GetData());
  }
}

void kr::GlobalGameLoop::reset()
{
  globalGameLoops().Clear();
  g_gameLoopsNeedSorting = false;
  g_globalGameLoopIsTicking = false;
  g_keepGlobalGameLoopTicking = true;
}
