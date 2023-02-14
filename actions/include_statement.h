#pragma once

#include "context.h"
#include <vector>
#include <string_view>

namespace qrqma {
namespace grammar {
struct include_control_statement;
struct try_include_control_statement;
}

namespace actions {

template <typename Rule> 
struct action;

template <> struct action<grammar::include_control_statement> {
    template <typename Input> 
    static void apply(const Input &, ContextP& context) {
        apply(context, false);
    }
    static void apply(ContextP& context, bool ignore_source_not_found);
};


template <> struct action<grammar::try_include_control_statement> {
    template <typename Input> 
    static void apply(const Input &, ContextP& context) {
        action<grammar::include_control_statement>::apply(context, true);
    }
};

}
}