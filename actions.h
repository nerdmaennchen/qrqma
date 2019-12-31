#pragma once

#include "context.h"
#include "demangle.h"
#include "grammar.h"

#include "tao/pegtl.hpp"
#include <charconv>
#include <cstdint>
#include <string>

namespace qrqma {
namespace actions {

namespace detail {

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...)->overloaded<Ts...>;

} // namespace detail

struct Literal : std::string {};
struct Identifier : std::string {};

template <typename Num> struct Number { Num i; };
using NumberV = std::variant<Number<std::int32_t>, Number<std::int64_t>,
                             Number<std::uint32_t>, Number<std::uint64_t>>;

using Word = std::variant<Literal, NumberV, Identifier>;
struct WordCollection : std::vector<Word> {};

struct Expression {

}

namespace pegtl = tao::pegtl;

using namespace grammar;

template <typename Rule> struct action : pegtl::nothing<Rule> {};

template <> struct action<text> {
    template <typename Input>
    static void apply(const Input &in, Context &parseState) {
        parseState.addToken(Context::StaticText{in.string()});
    }
};

template <> struct action<string_literal> {
    template <typename Input>
    static void apply(const Input &in, WordCollection &wordCol) {
        auto sv = in.string_view();
        wordCol.emplace_back(Literal{std::string{std::string_view{
            sv.begin() + 1,
            sv.size() - 2}}}); // the string without the quotation marks
    }
};

template <typename Num> struct number_action_base {
    template <typename Input>
    static void apply(const Input &in, WordCollection &wordCol) {
        Number<Num> i;
        std::from_chars(in.begin(), in.end(), i.i, 0);
        wordCol.emplace_back(NumberV{Number<Num>{i}});
    }
};

template <> struct action<number_i32> : number_action_base<std::int32_t> {};
template <> struct action<number_ui32> : number_action_base<std::uint32_t> {};
template <> struct action<number_i64> : number_action_base<std::int64_t> {};
template <> struct action<number_ui64> : number_action_base<std::uint64_t> {};

template <> struct action<identifier> {
    template <typename Input>
    static void apply(const Input &in, WordCollection &wordCol) {
        wordCol.emplace_back(Identifier{in.string()});
    }
};

template <>
struct action<print_expression_content>
    : tao::pegtl::change_states<WordCollection> {

    template <typename Input>
    static void success(const Input &, WordCollection &words, Context &scope) {
        auto handleIdentifier = [&](Identifier const &i) {
            symbol::Symbol const &first = scope[i];
            if (first.type() == typeid(std::string)) {
                scope.addToken(
                    Context::StaticText{std::any_cast<std::string>(first)});
            }
            if (first.type() == typeid(symbol::Function)) {
                std::vector<symbol::Symbol const *> items;
                std::vector<symbol::Symbol> addSymbolStorage;

                std::for_each(
                    next(begin(words)), end(words), [&](auto const &w) {
                        std::visit(
                            detail::overloaded{
                                [&](Identifier const &i) {
                                    items.emplace_back(&scope[i]);
                                },
                                [&](NumberV const &number) {
                                    std::visit(
                                        detail::overloaded{[&](auto const &n) {
                                            items.emplace_back(
                                                &addSymbolStorage.emplace_back(
                                                    n.i));
                                        }},
                                        number);
                                },
                                [&](Literal const &l) {
                                    items.emplace_back(
                                        &addSymbolStorage.emplace_back(
                                            static_cast<std::string>(l)));
                                },
                            },
                            w);
                    });
                scope.addToken(Context::Callable{
                    [items = std::move(items),
                     localSyms = std::move(addSymbolStorage), first] {
                        return std::any_cast<symbol::Function const &>(first)(
                            items);
                    }});
            }
        };
        // auto handleDefault = [&](auto) {
        //     for (auto const& w : words) {
        //         std::visit(detail::overloaded{
        //             [&](Literal const &l) {
        //             scope.addToken(Context::StaticText{l}); },
        //             [&](Integer const &i) {
        //             scope.addToken(Context::StaticText{std::to_string(i.i)});
        //             },
        //             [&](Identifier const& i) {
        //                 symbol::Symbol const& s = scope[i];
        //                 if (s.type() == typeid(std::string)) {
        //                     scope.addToken(Context::StaticText{std::any_cast<std::string>(s)});
        //                 } else {
        //                     throw std::runtime_error("cannot handle token \""
        //                     + i
        //                     + "\"of type: " + internal::demangle(s.type()));
        //                 }
        //             },
        //         }, w);
        //     }
        // };
        std::visit(
            detail::overloaded{
                handleIdentifier,
                // handleDefault,
                [](auto) {},
            },
            words[0]);
    }
};

template <> struct action<if_control_statement> {
    template <typename Input> static void apply(const Input &in, Context &) {
        std::cout << "if_control_statement: \"" << in.string() << "\""
                  << std::endl;
    }
};

} // namespace actions
} // namespace qrqma