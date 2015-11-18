#pragma once

namespace kr
{
  using GlobalGameLoopCallback = ezDelegate<void()>;

  /// \brief Static class to register game loops with a name.
  ///
  /// \todo Dependency management. Something like \c depend(GameLoop* pLoop, GameLoop* pDependency);. Fail for cyclic deps.
  namespace GlobalGameLoop
  {
    /// \brief Set or add a named loop in / to the global game loop.
    /// \return \c EZ_SUCCESS if the names game loop did not exist yet, \c EZ_FAILURE otherwise.
    KR_ENGINE_API void set(ezStringView loopName, GlobalGameLoopCallback callback, ezLogInterface* pLogInterface = nullptr);

    /// \brief Get a registered game loop instance by name.
    /// \note It is not safe to store a pointer to the callback, as it might be moved in memory the next time set() or remove() is called.
    /// \return \c nullptr if the named game loop does not exist.
    KR_ENGINE_API GlobalGameLoopCallback* get(ezStringView loopName, ezLogInterface* pLogInterface = nullptr);

    /// \brief Remove a registered game loop by name.
    /// \return \c EZ_SUCCESS if a game loop of the given name could be found.
    KR_ENGINE_API ezResult remove(ezStringView loopName, ezLogInterface* pLogInterface = nullptr);

    /// \brief Set the priority for a named game loop.
    ///
    /// A higher priority value means that the loop is executed earlier.
    ///
    /// \pre A game loop with the given name must already exist.
    /// \see set()
    KR_ENGINE_API void setPriority(ezStringView loopName, ezInt32 priority, ezLogInterface* pLogInterface = nullptr);

    /// \brief Get the priority for a named game loop.
    ///
    /// A higher priority value means that the loop is executed earlier.
    ///
    /// \pre A game loop with the given name must already exist.
    /// \see set()
    KR_ENGINE_API ezInt32 getPriority(ezStringView loopName, ezLogInterface* pLogInterface = nullptr);

    /// \brief Ticks all registered game loops in order.
    KR_ENGINE_API void tick(ezLogInterface* pLogInterface = nullptr);

    /// \brief Whether the global game loop should be ticked or not.
    /// \note This value is just a hint, so calls to tick() can still be made.
    /// \see setKeepTicking()
    KR_ENGINE_API bool keepTicking();

    /// \brief Set whether the global game loop should be ticked or not.
    /// \note Calls to tick() can still be made.
    /// \see keepTicking()
    KR_ENGINE_API void setKeepTicking(bool value);

    /// \brief Whether the global game loop is currently ticking or not.
    KR_ENGINE_API bool isTicking();

    /// \brief Prints the order in which the registered callbacks are executed in for each tick, one per line.
    KR_ENGINE_API void printTickOrder(ezLogInterface* pLogInterface = nullptr);

    /// \brief Resets the internal state of the global game loop.
    /// \pre The global game loop must not be ticking when calling this function.
    KR_ENGINE_API void reset();
  };
}
