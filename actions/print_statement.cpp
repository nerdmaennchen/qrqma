#include "print_statement.h"

namespace qrqma {
namespace actions {

void action<grammar::print_expression>::apply(Context &context) {
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

}
}