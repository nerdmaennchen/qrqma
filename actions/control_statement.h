#pragma once

#include "context.h"
#include "../grammar/grammar.h"

namespace qrqma {
namespace actions {

namespace pegtl = tao::pegtl;

template <> struct action<grammar::if_control_statement> : pegtl::change_states<Context, Context, Context> {
    template< typename Rule, pegtl::apply_mode A, pegtl::rewind_mode M, template< typename... > class Action, template< typename... > class Control, typename Input>
    [[nodiscard]] static bool match( Input& in, Context& oldC)
    {
        return pegtl::change_states<Context, Context, Context>::match< Rule, A, M, Action, Control >(std::make_index_sequence<3>{}, in, oldC, oldC.childContext(), oldC.childContext(), oldC );
    }

    template <typename Input> 
    static void success(const Input &,Context& outerC, Context& ifC, Context& elseC, Context&) {
        auto e = outerC.popExpression();
        auto c = outerC.convert(e.type(), typeid(types::Bool));
        auto test = [e=std::move(e), c=std::move(c)] {
            return std::any_cast<types::Bool>(c(e.eval_f()));
        };
        
        outerC.addToken([test=std::move(test), &ifC, &elseC]() -> std::string {
            if (test()) {
                return ifC();
            }
            return elseC();
        });
    }
};

template <> struct action<grammar::if_statement> : pegtl::change_states<Context> {
    template< typename Rule, pegtl::apply_mode A, pegtl::rewind_mode M, template< typename... > class Action, template< typename... > class Control, typename Input>
    [[nodiscard]] static bool match( Input& in, Context& outerC, Context&, Context&)
    {
        return pegtl::change_states<Context>::match< Rule, A, M, Action, Control >(std::make_index_sequence<1>{}, in, outerC );
    }

    template <typename Input> 
    static void success(const Input &, Context&) { }
};

template <> struct action<grammar::if_content> : pegtl::change_states<Context> {
    template< typename Rule, pegtl::apply_mode A, pegtl::rewind_mode M, template< typename... > class Action, template< typename... > class Control, typename Input>
    [[nodiscard]] static bool match( Input& in, Context&, Context& ifC, Context&)
    {
        return pegtl::change_states<Context>::match< Rule, A, M, Action, Control >(std::make_index_sequence<1>{}, in, ifC );
    }

    template <typename Input> 
    static void success(const Input &, Context&) { }
};

template <> struct action<grammar::else_content> : pegtl::change_states<Context> {
    template< typename Rule, pegtl::apply_mode A, pegtl::rewind_mode M, template< typename... > class Action, template< typename... > class Control, typename Input>
    [[nodiscard]] static bool match( Input& in, Context&, Context&, Context& elseC)
    {
        return pegtl::change_states<Context>::match< Rule, A, M, Action, Control >(std::make_index_sequence<1>{}, in, elseC );
    }

    template <typename Input> 
    static void success(const Input &, Context&) { }
};


}
}