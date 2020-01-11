#pragma once

#include "../grammar/grammar.h"
#include "context.h"

namespace qrqma {
namespace actions {

template <>
struct action<grammar::print_expression> {
    template <typename Input>
    static void apply(const Input&, Context &context) {
        auto expression = context.popExpression();
        auto const& from_type = expression.type;
        context.addToken([
            expression=std::move(expression), 
            converter = context.convert(from_type, typeid(std::string))] {
            return std::any_cast<std::string>(converter(expression.eval_f()));
        });
    }
};

}
}