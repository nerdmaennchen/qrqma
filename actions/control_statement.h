#pragma once

#include "context.h"
#include "../grammar/grammar.h"

namespace qrqma {
namespace actions {

namespace pegtl = tao::pegtl;

template <> struct action<grammar::if_content> : pegtl::change_states<Context> {
    template< typename Rule,
            pegtl::apply_mode A,
            pegtl::rewind_mode M,
            template< typename... >
            class Action,
            template< typename... >
            class Control,
            typename Input>
    [[nodiscard]] static bool match( Input& in, Context& oldC)
    {
        return pegtl::change_states<Context>::match< Rule, A, M, Action, Control >(std::make_index_sequence<1>{}, in, oldC.childContext(), oldC );
    }

    template <typename Input> 
    static void success(const Input &, Context& innerC, Context& outerC) {
        auto e = outerC.popExpression();
        auto c = outerC.convert(e.type, typeid(types::Bool));
        auto test = [e=std::move(e), c=std::move(c)]{
            return std::any_cast<types::Bool>(c(e.eval_f()));
        };
        
        outerC.addToken([test=std::move(test), &innerC]() -> std::string {
            if (test()) {
                return innerC();
            }
            return "";
        });
    }
};


}
}