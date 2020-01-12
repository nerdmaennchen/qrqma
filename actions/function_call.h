#pragma once

#include "context.h"
#include "../grammar/expression.h"
#include "../symbol.h"
#include "../demangle.h"

#include <stdexcept>

namespace qrqma {
namespace actions {

template<typename Rule> 
struct action;

template <> struct action<grammar::ops::call> {
    template <typename Input>
    static void apply(const Input &in, Context &context) {
        apply(in.string(), context);
    }

    static void apply(std::string const &in, Context &context);
};

} // namespace actions
} // namespace qrqma
