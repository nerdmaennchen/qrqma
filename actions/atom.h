#pragma once

#include "../grammar/grammar.h"
#include "context.h"
#include "types.h"

#include <string>
#include <cstdint>
#include <sstream>
#include <utility>

namespace qrqma {
namespace actions {
namespace pegtl = tao::pegtl;

template<typename Rule> 
struct action;

template <> 
struct action<grammar::string_literal> {
    template <typename Input>
    static void apply(const Input &in, Context &context) {
        std::string s{std::string_view{ in.begin() + 1, in.size() - 2}}; // the string without the quotation marks;
        context.pushExpression(
            typeid(std::string),
            [s = std::move(s)]{
                return s;
            }
        );
    }
};

template <> struct action<grammar::integer> {
    template <typename Input>
    static void apply(const Input &in, Context &context) {
        types::Integer num{};
        std::stringstream{in.string()} >> num;
        context.pushExpression(
            typeid(num),
            [num = std::move(num)]{
                return num;
            }
        );
    }
};

template <> struct action<grammar::float_num> {
    template <typename Input>
    static void apply(const Input &in, Context &context) {
        types::Float num{};
        std::stringstream{in.string()} >> num;
        context.pushExpression(
            typeid(num),
            [num = std::move(num)]{
                return num;
            }
        );
    }
};

template <> struct action<grammar::bool_true> {
    template <typename Input>
    static void apply(const Input &, Context &context) {
        context.pushExpression( typeid(true), []{ return true; } );
    }

    static void apply(std::string const &in, Context &context);
};
template <> struct action<grammar::bool_false> {
    template <typename Input>
    static void apply(const Input &, Context &context) {
        context.pushExpression( typeid(false), []{ return false; } );
    }

    static void apply(std::string const &in, Context &context);
};

template <>
struct action<grammar::identifier> {
    template <typename Input>
    static void apply(const Input &in, Context &context) {
        auto const& s = context[in.string()];
        context.pushExpression(s.type(), [&s]{
            return s;
        });
    }
};

template <> struct action<grammar::atom_list> : pegtl::change_states<Context> {
    template< typename Rule, pegtl::apply_mode A, pegtl::rewind_mode M, template< typename... > class Action, template< typename... > class Control, typename Input>
    [[nodiscard]] static bool match( Input& in, Context& context)
    {
        return pegtl::change_states<Context>::match< Rule, A, M, Action, Control >(std::make_index_sequence<1>{}, in, context.childContext(), context );
    }

    template <typename Input> 
    static void success(const Input &, Context& inner_context, Context& outer_context) {
        std::vector<symbol::Symbol> sym_list;
        for (auto const& e : inner_context.popAllExpressions()) {
            sym_list.emplace_back(e.eval_f());
        }
        symbol::List list{std::move(sym_list)};
        outer_context.pushExpression(typeid(list), [l = std::move(list)]{
            return l;
        });
    }
};

} // namespace actions
} // namespace qrqma