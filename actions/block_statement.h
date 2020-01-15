#pragma once

#include "context.h"
#include "../tao/pegtl.hpp"

namespace qrqma {

namespace grammar {
struct block_control_statement;
struct block_identifier;
struct block_content;   
}

namespace actions {

namespace pegtl = tao::pegtl;

template<typename Rule> 
struct action;

template <> struct action<grammar::block_control_statement> : pegtl::change_states<std::string, Context> {
    template< typename Rule, pegtl::apply_mode A, pegtl::rewind_mode M, template< typename... > class Action, template< typename... > class Control, typename Input>
    [[nodiscard]] static bool match( Input& in, Context& oldC) {
        return pegtl::change_states<std::string, Context>::match< Rule, A, M, Action, Control >(std::make_index_sequence<2>{}, in, std::string{}, oldC.childContext(), oldC );
    }

    static void success(std::string& block_name, Context& inner_ctx, Context& outer_ctx);
    template <typename Input>
    static void success(const Input &, std::string& block_name, Context& inner_ctx, Context& outer_ctx) {
        success(block_name, inner_ctx, outer_ctx);
    }
};

template <> struct action<grammar::block_identifier> {
    template <typename Input> 
    static void apply(const Input &in, std::string &block_name, Context&) {
        block_name = in.string();
    }
};

template <> struct action<grammar::block_content> : pegtl::change_states<Context> {
    template< typename Rule, pegtl::apply_mode A, pegtl::rewind_mode M, template< typename... > class Action, template< typename... > class Control, typename Input>
    [[nodiscard]] static bool match( Input& in, std::string&, Context& ctx) {
        return pegtl::change_states<Context>::match< Rule, A, M, Action, Control >(std::make_index_sequence<1>{}, in, ctx );
    }

    template <typename Input> 
    static void success(const Input &, Context&) { }
};

}
}