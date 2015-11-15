#pragma once

namespace kr
{
  using GameLoopCallback = ezDelegate<void()>;

  /// \brief Static class to register game loops with a name.
  ///
  /// \todo Dependency management. Something like \c depend(GameLoop* pLoop, GameLoop* pDependency);. Fail for cyclic deps.
  class KR_ENGINE_API GlobalGameLoopRegistry
  {
    GlobalGameLoopRegistry() = delete;
  public:

    /// \brief Set or add a named game loop to the global game loop registry.
    /// \return \c EZ_SUCCESS if the names game loop did not exist yet, \c EZ_FAILURE otherwise.
    static void set(ezStringView name, GameLoopCallback callback, ezLogInterface* pLogInterface = nullptr);

    /// \brief Get a registered game loop instance by name.
    /// \note It is not safe to store a pointer to the callback, as it might be moved in memory the next time set() or remove() is called.
    /// \return \c nullptr if the named game loop does not exist.
    static GameLoopCallback* get(ezStringView name, ezLogInterface* pLogInterface = nullptr);

    /// \brief Remove a registered game loop by name.
    /// \return \c EZ_SUCCESS if a game loop of the given name could be found.
    static ezResult remove(ezStringView name, ezLogInterface* pLogInterface = nullptr);

    /// \brief Ticks all registered game loops in order.
    static void tick(ezLogInterface* pLogInterface = nullptr);

    /// \brief Whether the global game loop should be ticked or not.
    /// \note This value is just a hint, so calls to tick() can still be made.
    /// \see setKeepTicking()
    static bool keepTicking();

    /// \brief Set whether the global game loop should be ticked or not.
    /// \note Calls to tick() can still be made.
    /// \see keepTicking()
    static void setKeepTicking(bool value);

    /// \brief Whether the global game loop is currently ticking or not.
    static bool isTicking();

    /// \brief Prints the order in which the registered callbacks are executed in for each tick, one per line.
    static void printTickOrder(ezLogInterface* pLogInterface = nullptr);

    /// \brief Resets the internal state of the global game loop registry.
    /// \pre The global game loop must not be ticking when calling this function.
    static void reset();
  };
}
