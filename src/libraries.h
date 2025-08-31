#pragma once
#include <string>
#include <format>
#include <vector>
#include <variant>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <iomanip>

namespace lib {
    using Literal = std::variant<std::monostate, std::string, double, bool>;
    using Byte = uint8_t;
}