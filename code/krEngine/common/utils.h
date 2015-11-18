#pragma once

namespace kr
{
  /// \brief Used to provide function or constructor overloads to initialize
  ///        Something to zero.
  /// \todo Remove this.
  enum ZeroInit { Zero };

  /// \brief Used to provide function or constructor overloads to initialize
  ///        Something to the identity form.
  /// \todo Remove this.
  enum IdentityInit { Identity };
}

namespace krInternal
{
  struct OnScopeExit
  {
    ezDelegate<void()> m_func;

    template<typename F>
    OnScopeExit(F func) : m_func(kr::move(func)) {}
    ~OnScopeExit() { m_func(); }
  };
}

/// \note Don't forget the trailing semicolon ';'!
/// \example KR_ON_SCOPE_EXIT{ printf("Leaving scope...\n"); };
#define KR_ON_SCOPE_EXIT ::krInternal::OnScopeExit EZ_CONCAT(_onScopeExit_, EZ_SOURCE_LINE) = [&]

namespace kr
{
  /// \brief Return the current working directory.
  /// \return The path as a string view. Guaranteed to be null-terminated.
  KR_ENGINE_API ezStringView cwd();

  /// \brief The absolute native path to the default data directory.
  KR_ENGINE_API const ezStringBuilder& defaultRoot();

  inline ezStringBuilder makePath(const char* szPath1, const char* szPath2 = nullptr, const char* szPath3 = nullptr, const char* szPath4 = nullptr)
  {
    ezStringBuilder path;
    path.AppendPath(szPath1, szPath2, szPath3, szPath4);
    path.MakeCleanPath();
    return path;
  }

  /// \return  One of "-debug", "-minsize", "-reldeb", or "" (i.e. the empty string).
  KR_ENGINE_API const char* configPostfix();
}
