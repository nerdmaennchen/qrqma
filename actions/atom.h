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

template <> 
struct action<grammar::string_literal> {
    template <typename Input> 
    static void apply(const Input &in, Context &context) {
        auto sv = in.string_view();
        std::string s{std::string_view{ sv.begin() + 1, sv.size() - 2}}; // the string without the quotation marks;
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
    static void apply(const Input&, Context &context) {
        context.pushExpression( typeid(true), []{ return true; } );
    }
};
template <> struct action<grammar::bool_false> {
    template <typename Input>
    static void apply(const Input&, Context &context) {
        context.pushExpression( typeid(false), []{ return false; } );
    }
};

template <>
struct action<grammar::identifier> {
    template <typename Input>
    static void apply(const Input &in, Context &context) {
        auto symbol = context[in.string()];
        auto const&t = symbol.type();
        context.pushExpression(t, [s = std::move(symbol)]{
            return s;
        });
    }
};

} // namespace actions
} // namespace qrqma