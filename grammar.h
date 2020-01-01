#pragma once
#include "tao/pegtl.hpp"

namespace qrqma {

namespace grammar {

struct grammar;

namespace pegtl = tao::pegtl;

template <typename T> struct padded : pegtl::pad<T, pegtl::space> {};

template <char delim>
struct quoted_string
    : pegtl::if_must<pegtl::one<delim>, pegtl::until<pegtl::one<delim>>> {};
struct string_literal : pegtl::sor<quoted_string<39>, quoted_string<34>> {
}; // a string enclosed in '' or ""

struct number_i32
    : pegtl::seq<pegtl::opt<pegtl::one<'-'>>, pegtl::plus<pegtl::digit>,
                 pegtl::opt<pegtl::one<'L'>>> {};
struct number_i64
    : pegtl::seq<pegtl::opt<pegtl::one<'-'>>, pegtl::plus<pegtl::digit>,
                 pegtl::one<'L', 'l'>, pegtl::one<'L', 'l'>> {};
struct number_ui32
    : pegtl::seq<pegtl::opt<pegtl::one<'-'>>, pegtl::plus<pegtl::digit>,
                 pegtl::one<'U', 'u'>, pegtl::opt<pegtl::one<'L', 'l'>>> {};
struct number_ui64 : pegtl::seq<pegtl::opt<pegtl::one<'-'>>,
                                pegtl::plus<pegtl::digit>, pegtl::one<'U', 'u'>,
                                pegtl::one<'L', 'l'>, pegtl::one<'L', 'l'>> {};
struct number : pegtl::sor<number_i32, number_i64, number_ui32, number_ui64> {};

struct identifier : pegtl::identifier {};

struct atom : pegtl::sor<string_literal, number, identifier> {};

struct function_call;
struct expression;

namespace ops {

template <typename op_sep>
struct bin_op : pegtl::seq<expression, op_sep, expression> {};
struct bin_op_star : bin_op<pegtl::one<'*'>> {};
struct bin_op_fslash : bin_op<pegtl::one<'/'>> {};
struct bin_op_percent : bin_op<pegtl::one<'%'>> {};
struct bin_op_plus : bin_op<pegtl::one<'+'>> {};
struct bin_op_minus : bin_op<pegtl::one<'-'>> {};
struct bin_op_lshift : bin_op<pegtl::two<60>> {}; // <<
struct bin_op_rshift : bin_op<pegtl::two<62>> {}; // >>

struct bin_op_cmp_leq : bin_op<pegtl::string<60, 61>> {}; // <=
struct bin_op_cmp_geq : bin_op<pegtl::string<62, 61>> {}; // >=
struct bin_op_cmp_eq : bin_op<pegtl::two<61>> {};         // ==
struct bin_op_cmp_neq : bin_op<pegtl::string<33, 61>> {}; // !=

struct bin_op_amp : bin_op<pegtl::one<38>> {};    // &
struct bin_op_hat : bin_op<pegtl::one<94>> {};    // ^
struct bin_op_pipe : bin_op<pegtl::one<124>> {};  // |
struct bin_op_damp : bin_op<pegtl::two<38>> {};   // &&
struct bin_op_dpipe : bin_op<pegtl::two<124>> {}; // &&

} // namespace ops

struct binary_op_expression
    : pegtl::sor<ops::bin_op_dpipe, ops::bin_op_damp, ops::bin_op_pipe,
                 ops::bin_op_hat, ops::bin_op_amp,

                 ops::bin_op_cmp_neq, ops::bin_op_cmp_eq, ops::bin_op_cmp_geq,
                 ops::bin_op_cmp_leq,

                 ops::bin_op_rshift, ops::bin_op_lshift, ops::bin_op_minus,
                 ops::bin_op_plus, ops::bin_op_percent, ops::bin_op_fslash,
                 ops::bin_op_star> {};

struct expression : pegtl::sor<atom, function_call, binary_op_expression> {};

struct function_call
    : pegtl::seq<identifier,
                 pegtl::if_must<pegtl::one<'('>,
                                pegtl::list<expression, pegtl::one<','>>,
                                pegtl::one<')'>>> {};

struct print_expression_o_brackets : pegtl::string<123, 123> {}; // {{
struct print_expression_c_brackets : pegtl::string<125, 125> {}; // }}

struct control_statement_o_brackets : pegtl::string<123, 37> {}; // {%
struct control_statement_c_brackets : pegtl::string<37, 125> {}; // %}

struct comment_o_brackets : pegtl::string<123, 35> {}; // {#
struct comment_c_brackets : pegtl::string<35, 125> {}; // #}
struct comment
    : pegtl::if_must<comment_o_brackets, pegtl::until<comment_c_brackets>> {};

template <typename Token, typename... R>
struct control_statement_token
    : pegtl::seq<control_statement_o_brackets,
                 pegtl::until<control_statement_c_brackets,
                              pegtl::seq<padded<Token>, padded<R>...>>> {};

struct end_statement : control_statement_token<pegtl::keyword<'e', 'n', 'd'>> {
};
struct if_statement : control_statement_token<pegtl::keyword<'i', 'f'>> {};
struct if_control_statement
    : pegtl::if_must<if_statement, grammar, end_statement> {};

struct control_statement : pegtl::sor<if_control_statement> {};

struct print_expression
    : pegtl::if_must<print_expression_o_brackets,
                     pegtl::until<print_expression_c_brackets, expression>> {};

struct any_opening_brackets
    : pegtl::sor<print_expression_o_brackets, control_statement_o_brackets,
                 comment_o_brackets> {};
struct text
    : pegtl::until<pegtl::at<pegtl::sor<pegtl::eof, any_opening_brackets>>> {};

struct control_text : pegtl::sor<print_expression, control_statement, comment> {
};
struct grammar : pegtl::list<text, control_text> {};

} // namespace grammar
} // namespace qrqma