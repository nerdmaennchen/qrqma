#pragma once
#include "../tao/pegtl.hpp"
#include "spaces.h"

namespace qrqma {
namespace grammar {
namespace pegtl = tao::pegtl;

struct expression;
struct atom;

template <char delim>
struct quoted_string : pegtl::if_must<pegtl::one<delim>, pegtl::until<pegtl::one<delim>>> {};
struct string_literal : pegtl::sor<quoted_string<39>, quoted_string<34>> {}; // a string enclosed in '' or ""

struct digits : pegtl::plus<pegtl::digit> {};
struct integer : digits {};

struct bool_true  : pegtl::sor<pegtl::keyword<'T', 'r', 'u', 'e'>, pegtl::keyword<'t', 'r', 'u', 'e'>> {};
struct bool_false : pegtl::sor<pegtl::keyword<'F', 'a', 'l', 's', 'e'>, pegtl::keyword<'f', 'a', 'l', 's', 'e'>> {};
struct boolean : pegtl::sor<bool_true, bool_false> {};

struct float_exponent : pegtl::seq<pegtl::one<'e', 'E'>, pegtl::opt<pegtl::one<'-'>>, digits> {};
struct float_num : pegtl::seq<
                    pegtl::star<pegtl::digit>,
                    pegtl::sor<
                        pegtl::seq<pegtl::one<'.'>, pegtl::star<pegtl::digit>, pegtl::opt<float_exponent>>,
                        float_exponent
                    >> {};
struct number : pegtl::sor<float_num, integer> {};

// similar to pegtl::identifier_other but with the '.' character allowed
using identifier_other = pegtl::internal::ranges< pegtl::internal::peek_char, 'a', 'z', 'A', 'Z', '0', '9', '_', '_', '.' >;
struct identifier : pegtl::seq<pegtl::not_at<boolean>, pegtl::seq< pegtl::identifier_first, pegtl::star< identifier_other > >> {};

struct atom_list : pegtl::if_must<pegtl::one<'['>, pegtl::opt<pegtl::list<padded<atom>, pegtl::one<','>>>, spaces, pegtl::one<']'>> {};
struct atom : pegtl::sor<string_literal, boolean, number, identifier, atom_list> {};

namespace ops {

template<int precedence>
struct lhs_term : lhs_term<precedence-1> {};
struct lhs_expression : padded<ops::lhs_term<3>> {};

template<int precedence>
struct infix_term : infix_term<precedence-1> {};

template <typename op_sep, int precedence>
struct infix_op : pegtl::seq<op_sep, lhs_expression, pegtl::opt<infix_term<precedence>>> {};

template <typename op_sep, int precedence>
struct prefix_unary_op : pegtl::seq<op_sep, spaces, lhs_term<precedence>> {};

template<typename rule>
struct try_match : pegtl::if_must<pegtl::at<rule>, rule> {};

struct arg_list : pegtl::opt<pegtl::list<expression, pegtl::one<','>>> {};
struct call  : try_match<pegtl::seq<identifier, spaces, pegtl::one<'('>, arg_list, pegtl::one<')'> > > {};
struct index : try_match<pegtl::seq<identifier, spaces, pegtl::one<'['>, expression, pegtl::one<']'> > > {};

struct unary_plus  : prefix_unary_op<pegtl::one<'+'>, 3> {};
struct unary_minus : prefix_unary_op<pegtl::one<'-'>, 3> {};
struct unary_not   : prefix_unary_op<pegtl::sor<pegtl::one<'!'>, pegtl::keyword<'n', 'o', 't'>>, 3> {};

struct star    : infix_op<pegtl::one<'*'>, 5> {};
struct fslash  : infix_op<pegtl::one<'/'>, 5> {};
struct percent : infix_op<pegtl::one<'%'>, 5> {};

struct plus  : infix_op<pegtl::one<'+'>, 6> {};
struct minus : infix_op<pegtl::one<'-'>, 6> {};

struct lshift : infix_op<pegtl::two<60>, 7> {}; // <<
struct rshift : infix_op<pegtl::two<62>, 7> {}; // >>

struct cmp_lt  : infix_op<pegtl::one<60>, 8> {};        // <
struct cmp_leq : infix_op<pegtl::string<60, 61>, 8> {}; // <=
struct cmp_gt  : infix_op<pegtl::one<62>, 8> {};        // >
struct cmp_geq : infix_op<pegtl::string<62, 61>, 8> {}; // >=

struct cmp_eq  : infix_op<pegtl::two<61>, 9> {};        // ==
struct cmp_neq : infix_op<pegtl::string<33, 61>, 9> {}; // !=

struct amp   : infix_op<pegtl::one<38>, 10>  {}; // &
struct hat   : infix_op<pegtl::one<94>, 11>  {}; // ^
struct pipe  : infix_op<pegtl::one<124>, 12> {}; // |
struct damp  : infix_op<pegtl::sor<pegtl::two<38>, pegtl::keyword<'a', 'n', 'd'>>, 13>  {}; // &&, and
struct dpipe : infix_op<pegtl::sor<pegtl::two<124>, pegtl::keyword<'o', 'r'>>, 14> {}; // ||, or

struct comma_seq : infix_op<pegtl::one<44>, 17> {}; // ,

struct braced_expression : pegtl::if_must< pegtl::one<'('>, expression, pegtl::one<')'> > {};

template<> struct lhs_term< 0> : pegtl::sor<atom, braced_expression> {};
template<> struct lhs_term< 2> : pegtl::sor<ops::call, ops::index, lhs_term<1>> {};
template<> struct lhs_term< 3> : pegtl::sor<ops::unary_plus, ops::unary_minus, ops::unary_not, lhs_term<2>> {};

template<> struct infix_term< 4> : pegtl::failure {};
template<> struct infix_term< 5> : pegtl::sor<infix_term<4>, ops::star, ops::fslash, ops::percent> {};
template<> struct infix_term< 6> : pegtl::sor<infix_term<5>, ops::plus, ops::minus> {};
template<> struct infix_term< 7> : pegtl::sor<infix_term<6>, ops::lshift, ops::rshift> {};
template<> struct infix_term< 8> : pegtl::sor<infix_term<7>, ops::cmp_lt, ops::cmp_leq, ops::cmp_gt, ops::cmp_geq> {};
template<> struct infix_term< 9> : pegtl::sor<infix_term<8>, ops::cmp_eq, ops::cmp_neq> {};
template<> struct infix_term<10> : pegtl::sor<infix_term<9>, ops::amp> {};
template<> struct infix_term<11> : pegtl::sor<infix_term<10>, ops::hat> {};
template<> struct infix_term<12> : pegtl::sor<infix_term<11>, ops::pipe> {};
template<> struct infix_term<13> : pegtl::sor<infix_term<12>, ops::damp> {};
template<> struct infix_term<14> : pegtl::sor<infix_term<13>, ops::dpipe> {};
template<> struct infix_term<17> : pegtl::sor<ops::comma_seq, infix_term<16>> {};

struct infix_expression : infix_term<17> {};
}

struct expression : pegtl::seq<ops::lhs_expression, pegtl::star<ops::infix_expression>> {};

}
}