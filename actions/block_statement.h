#pragma once

#include "context.h"
#include "overloaded.h"
#include "../grammar/grammar.h"

namespace qrqma {
namespace actions {

namespace pegtl = tao::pegtl;

template <> struct action<grammar::block_control_statement> : pegtl::change_states<std::string, Context> {
    template< typename Rule, pegtl::apply_mode A, pegtl::rewind_mode M, template< typename... > class Action, template< typename... > class Control, typename Input>
    [[nodiscard]] static bool match( Input& in, Context& oldC) {
        return pegtl::change_states<std::string, Context>::match< Rule, A, M, Action, Control >(std::make_index_sequence<2>{}, in, std::string{}, oldC.childContext(), oldC );
    }

    template <typename Input> 
    static void success(const Input &, std::string& block_name, Context& inner_ctx, Context& outer_ctx) {
        auto renderFunc = [&inner_ctx](){
            return inner_ctx();
        };

        outer_ctx.setBlock(block_name, renderFunc);
        outer_ctx.addToken([&outer_ctx, block_name=std::move(block_name)]{
            return std::visit(detail::overloaded{
                    [](Context::StaticText const &t) -> std::string { return t; },
                    [](Context::Callable const &c) -> std::string { return c(); }},
                outer_ctx.getBlock(block_name));
        });
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