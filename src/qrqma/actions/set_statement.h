#pragma once

#include "context.h"
#include "../grammar/grammar.h"

namespace qrqma {
namespace actions {

namespace pegtl = tao::pegtl;

template <> struct action<grammar::set_control_statement> : pegtl::change_states<std::string, Context> {
    template< typename Rule, pegtl::apply_mode A, pegtl::rewind_mode M, template< typename... > class Action, template< typename... > class Control, typename Input>
    [[nodiscard]] static bool match( Input& in, Context& context)
    {
        return pegtl::change_states<std::string, Context>::match< Rule, A, M, Action, Control >(std::make_index_sequence<2>{}, in, std::string{}, context );
    }

    template <typename Input> 
    static void success(const Input &, std::string &symbol_name, Context& context) {
        auto e = context.popExpression();
        context.setSymbol(symbol_name, e.eval_f());
    }
};

template <> struct action<grammar::set_identifier> {
    template <typename Input> 
    static void apply(const Input &in, std::string &symbol_name, Context&) {
        symbol_name = in.string();
    }
};

template <> struct action<grammar::set_expression> : pegtl::change_states<Context> {
    template< typename Rule, pegtl::apply_mode A, pegtl::rewind_mode M, template< typename... > class Action, template< typename... > class Control, typename Input>
    [[nodiscard]] static bool match( Input& in, std::string&, Context& context)
    {
        return pegtl::change_states<Context>::match< Rule, A, M, Action, Control >(std::make_index_sequence<1>{}, in, context );
    }

    template <typename Input> 
    static void success(const Input &, Context&) { }
};

}
}