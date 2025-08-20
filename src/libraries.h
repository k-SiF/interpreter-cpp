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

namespace lib {
    using Literal = std::variant<std::monostate, std::string, double, bool>;
}