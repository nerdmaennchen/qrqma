#include "function_call.h"
#include "../symbol.h"
#include "../demangle.h"

#include <stdexcept>

namespace qrqma {
namespace actions {

void action<grammar::ops::call>::apply(const std::string &in, Context &context) {
    // pop everything from the stack that is not a function
    std::vector<Context::Expression> args_r;
    while (true) {
        auto exp = context.popExpression();
        auto const& t = exp.type();
        if (t == typeid(symbol::Function)) {
            auto ftor = exp.eval<symbol::Function>();
            auto const& inArgTypes = ftor.argTypes;
            if (args_r.size() != inArgTypes.size()) {
                throw std::invalid_argument("cannot perform the call: \"" + in + "\": invalid numer of arguments\nexpected " + std::to_string(inArgTypes.size()) + " got " + std::to_string(args_r.size()));
            }
            std::vector<Context::Expression> args;
            std::size_t i{0};
            for (auto it = std::rbegin(args_r); it != std::rend(args_r); ++it) {
                auto const& type = it->type();
                try {
                    args.emplace_back(*inArgTypes[i], [c=context.convert(type, *inArgTypes[i]), e=std::move(*it)]{
                        return c(e.eval_f());
                    });
                } catch (std::exception const& err) {
                    std::throw_with_nested(
                        std::runtime_error("convert arg no " + std::to_string(i) 
                        + " from type \"" + internal::demangle(type) 
                        + "\" to type: \"" + internal::demangle(*inArgTypes[i])
                    ));
                }
            }
            context.pushExpression(ftor.targetType, [exp=std::move(exp), args=std::move(args)]{
                auto ftor = exp.eval<symbol::Function>();
                std::vector<std::any> argV;
                for (auto const& arg : args) {
                    argV.emplace_back(arg.eval_f());
                }
                return ftor(argV);
            });
            return;
        }
        args_r.emplace_back(std::move(exp));
    }
}

} // namespace actions
} // namespace qrqma
