#pragma once

#include "context.h"
#include "../grammar/expression.h"
#include "../symbol.h"
#include "../demangle.h"

#include <algorithm>

namespace qrqma {
namespace actions {

template <> struct action<grammar::ops::call> {
    template <typename Input>
    static void apply(const Input &, Context &context) {
        // pop everything from the stack that is not a function
        std::vector<Context::Expression> args_r;
        while (true) {
            auto exp = context.popExpression();
            auto const& t = exp.type;
            if (t == typeid(symbol::Function)) {
                auto ftor = exp.eval<symbol::Function>();
                auto const& inArgTypes = ftor.argTypes;

                std::vector<Context::Expression> args;
                std::size_t i{0};
                for (auto it = std::rbegin(args_r); it != std::rend(args_r); ++it) {
                    auto const& type = it->type;
                    args.emplace_back(*inArgTypes[i], [c=context.convert(type, *inArgTypes[i]), f=std::move(it->eval_f)]{
                        return c(f());
                    });
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
};

} // namespace actions
} // namespace qrqma
