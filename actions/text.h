#pragma once

#include "context.h"


namespace qrqma {
namespace grammar {
struct text;
}
namespace actions {

template <> struct action<grammar::text> {
    template <typename Input>
    static void apply(const Input &in, Context &context) {
        context.addToken(Context::StaticText{in.string()});
    }
};

} // namespace actions
} // namespace qrqma