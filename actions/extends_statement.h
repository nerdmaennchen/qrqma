#pragma once

#include "context.h"
#include "../grammar/grammar.h"

#include <vector>
#include <string_view>

namespace qrqma {
namespace actions {

namespace pegtl = tao::pegtl;

template <typename Rule> 
struct action;

template <> struct action<grammar::extends_control_statement> : pegtl::change_states<Context, Context> {
    template< typename Rule, pegtl::apply_mode A, pegtl::rewind_mode M, template< typename... > class Action, template< typename... > class Control, typename Input>
    [[nodiscard]] static bool match( Input& in, Context& oldC)
    {
        return pegtl::change_states<Context, Context>::match< Rule, A, M, Action, Control >(std::make_index_sequence<2>{}, in, oldC.childContext(), oldC );
    }

    template <typename Input> 
    static void success(const Input &,Context& inner_ctx, Context& outer_ctx) {
        success(inner_ctx, outer_ctx);
    }
    static void success(Context& inner_ctx, Context &outer_ctx);
};

template <> struct action<grammar::extends_body> : pegtl::change_states<Context> {
    template< typename Rule, pegtl::apply_mode A, pegtl::rewind_mode M, template< typename... > class Action, template< typename... > class Control, typename Input>
    [[nodiscard]] static bool match( Input& in, Context& innerC, Context&)
    {
        return pegtl::change_states<Context>::match< Rule, A, M, Action, Control >(std::make_index_sequence<1>{}, in, innerC);
    }

    template <typename Input> 
    static void success(const Input &, Context&) { }
};

template <> 
struct action<grammar::extends_statement> : pegtl::change_states<Context> {
    template< typename Rule, pegtl::apply_mode A, pegtl::rewind_mode M, template< typename... > class Action, template< typename... > class Control, typename Input>
    [[nodiscard]] static bool match( Input& in, Context&, Context& outer_ctx)
    {
        return pegtl::change_states<Context>::match< Rule, A, M, Action, Control >(std::make_index_sequence<1>{}, in, outer_ctx);
    }

    template <typename Input>
    static void success(const Input&, Context&) {}
};


}
}