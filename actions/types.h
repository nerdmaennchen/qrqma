#pragma once

#include "../symbol.h"
#include <cstdint>
#include <string>

namespace qrqma {
namespace actions {
namespace types {

using Integer = std::int64_t;
using Float = double;
using Bool = bool;
using String = std::string;

using Undefined = symbol::Undefined;
using List = symbol::List;
using Map = symbol::Map;

using LazyEvaluate = void;

}
}
}