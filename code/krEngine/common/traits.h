#pragma once

namespace kr
{
  template<typename T>
  using NonConst = typename std::remove_const<T>::type;

  template<typename T>
  using NonRef = typename std::remove_reference<T>::type;
}
