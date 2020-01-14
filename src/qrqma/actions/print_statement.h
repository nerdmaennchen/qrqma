#pragma once

#include "../grammar/grammar.h"
#include "context.h"

namespace qrqma {
namespace actions {

template <>
struct action<grammar::print_expression> {
    template <typename Input>
    static void apply(const Input&, Context &context) {
        auto stack = context.popAllExpressions();
        if (stack.empty()) {
            return;
        }
        auto& expression = stack.back();
        auto const& from_type = expression.type();
        context.addToken([
            expression=std::move(expression), 
            converter = context.convert(from_type, typeid(std::string))]() -> Context::RenderOutput  {
            return {std::any_cast<std::string>(converter(expression.eval_f())), false};
        });
    }
};

}
}