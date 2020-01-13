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

template <> struct action<grammar::ops::index> {
    template <typename Input>
    static void apply(const Input &in, Context &context) {
        // the rightmost expression is the index expression; the previous one is the target
        auto idxExpr = context.popExpression();
        auto targetExpr = context.popExpression();
        
        auto const& t = targetExpr.type();
        if (t == typeid(symbol::List)) {
            auto const& idxT = idxExpr.type();
            context.pushExpression(typeid(void), [c=context.convert(idxT, typeid(types::Integer)), idxE=std::move(idxExpr), tE=std::move(targetExpr)] {
                auto l = tE.eval<symbol::List>();
                return l.at(std::any_cast<types::Integer>(c(idxE.eval_f())));
            });
        } else if (t == typeid(symbol::Map)) {
            auto const& idxT = idxExpr.type();
            context.pushExpression(typeid(void), [c=context.convert(idxT, typeid(types::String)), idxE=std::move(idxExpr), tE=std::move(targetExpr)] {
                auto m = tE.eval<symbol::Map>();
                auto idx = std::any_cast<types::String>(c(idxE.eval_f()));
                return m.at(idx);
            });
        } else {
            throw std::runtime_error("in line: \n" + in.string() + "\ncannot utilize symbol of type \"" + internal::demangle(t) + "\" as list or map");
        }
    }
};

} // namespace actions
} // namespace qrqma
