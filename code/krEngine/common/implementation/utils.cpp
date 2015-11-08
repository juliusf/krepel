#include <krEngine/common/utils.h>

#if EZ_ENABLED(EZ_PLATFORM_WINDOWS)
  #include "utils_Windows.inl"
#else
  #error common/utils not implemented for this platform.
#endif

static ezStringBuilder defaultRootHelper()
{
  ezStringBuilder root{ ezOSFile::GetApplicationDirectory() };
  root.PathParentDirectory();
  return root;
}

const ezStringBuilder& kr::defaultRoot()
{
  static auto root{ defaultRootHelper() };
  return root;
}
