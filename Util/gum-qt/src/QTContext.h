#pragma once
#include <functional>

struct QTContextData
{
  std::function<void()> bindFunc = nullptr;
  std::function<void()> unbindFunc = nullptr;
};