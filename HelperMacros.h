#pragma once

#include <memory>

template <typename T>
using UPtr = std::unique_ptr<T>;

template <typename T>
using SPtr = std::shared_ptr<T>;

template <typename T>
using WPtr = std::weak_ptr<T>;

using std::make_shared;
using std::make_unique;

#define SAFE_RELEASE(p) { if(p){p->Release(); p = nullptr;} }