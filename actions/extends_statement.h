#pragma once

#include "context.h"
#include "../grammar/grammar.h"

#include <vector>
#include <string_view>

namespace qrqma {
namespace actions {

namespace pegtl = tao::pegtl;

template <typename Rule> 
struct action;

template <> struct action<grammar::extends_control_statement> {
    template <typename Input> 
    static void apply(const Input &, Context& context) {
        apply(context);
    }
    static void apply(Context& context);
};

}
}