#pragma once

#include "context.h"
#include "../grammar/grammar.h"


namespace qrqma {
namespace actions {

template <> struct action<grammar::text> {
    template <typename Input>
    static void apply(const Input &in, Context &context) {
        context.addToken(Context::StaticText{in.string()});
    }
};

} // namespace actions
} // namespace qrqma