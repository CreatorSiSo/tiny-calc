#pragma once

#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

template <typename T>
using Box = std::unique_ptr<T>;

template <typename T>
using Vector = std::vector<T>;

using String = std::string;
using StringView = std::string_view;
using StringStream = std::stringstream;
using Ostream = std::ostream;
using Istream = std::istream;
