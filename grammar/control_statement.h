#pragma once
#include "../tao/pegtl.hpp"
#include "expression.h"
#include "spaces.h"

namespace qrqma {
namespace grammar {
namespace pegtl = tao::pegtl;

struct grammar;

struct control_statement_o_brackets : pegtl::string<123, 37> {}; // {%
struct control_statement_c_brackets : pegtl::string<37, 125> {}; // %}

template <typename Token, typename... R>
struct control_statement_token
    : pegtl::seq<control_statement_o_brackets,
                 pegtl::until<control_statement_c_brackets,
                              pegtl::seq<padded<Token>, padded<R>...>>> {};

struct end_statement : control_statement_token<pegtl::keyword<'e', 'n', 'd'>> {};
struct if_statement         : control_statement_token<pegtl::keyword<'i', 'f'> , expression> {};
struct if_content           : pegtl::seq<pegtl::success, grammar> {};

struct else_statement       : control_statement_token<pegtl::keyword<'e', 'l', 's', 'e'>> {};
struct else_content         : pegtl::seq<pegtl::success, grammar> {};

struct if_control_statement : pegtl::if_must<if_statement, if_content, pegtl::opt<else_statement, else_content>, end_statement> {};

struct set_identifier : identifier {};
struct set_expression : expression {};
struct set_statement : control_statement_token<pegtl::keyword<'s', 'e', 't'> , set_identifier, pegtl::one<'='>, set_expression> {};

struct for_identifier : identifier {};
struct for_container_identifier : pegtl::seq<pegtl::success, expression> {};
struct for_content : pegtl::seq<pegtl::success, grammar> {};
struct for_control_statement : pegtl::seq<
    control_statement_token<pegtl::keyword<'f', 'o', 'r'>, pegtl::list<for_identifier, padded<pegtl::one<','>>>, pegtl::keyword<'i', 'n'>, for_container_identifier>,
    for_content,
    end_statement
    > {};

struct control_statement : pegtl::sor<if_control_statement, set_statement, for_control_statement> {};

}
}