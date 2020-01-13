#pragma once

#include "qrqma/tao/pegtl.hpp"

namespace qrqma {
namespace actions {

// the default action
namespace pegtl = tao::pegtl;
template <typename Rule> struct action : pegtl::nothing<Rule> {};

} // namespace actions
} // namespace qrqma


#include "atom.h"
#include "function_call.h"
#include "index_operator.h"
#include "block_statement.h"
#include "extends_statement.h"
#include "if_statement.h"
#include "for_statement.h"
#include "set_statement.h"
#include "print_statement.h"
#include "expression.h"
#include "text.h"