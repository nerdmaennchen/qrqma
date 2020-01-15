#include "atom.h"

#include <string>
#include <cstdint>
#include <sstream>
#include <utility>

namespace qrqma {
namespace actions {

void action<grammar::string_literal>::apply(const std::string &in, Context &context) {
    std::string s{in.data() + 1, in.size() - 2}; // the string without the quotation marks;
    context.pushExpression(
        typeid(std::string),
        [s = std::move(s)]{
            return s;
        }
    );
}

void action<grammar::integer>::apply(const std::string &in, Context &context) {
    types::Integer num{};
    std::stringstream{in} >> num;
    context.pushExpression(
        typeid(num),
        [num = std::move(num)]{
            return num;
        }
    );
}

void action<grammar::float_num>::apply(const std::string &in, Context &context) {
    types::Float num{};
    std::stringstream{in} >> num;
    context.pushExpression(
        typeid(num),
        [num = std::move(num)]{
            return num;
        }
    );
}

void action<grammar::bool_true>::apply(const std::string&, Context &context) {
    context.pushExpression( typeid(true), []{ return true; } );
}
void action<grammar::bool_false>::apply(const std::string&, Context &context) {
    context.pushExpression( typeid(false), []{ return false; } );
}

void action<grammar::identifier>::apply(const std::string &in, Context &context) {
    auto const& s = context[in];
    context.pushExpression(s.type(), [&s] {
        return s;
    });
}

void action<grammar::atom_list>::success(Context& inner_context, Context& outer_context) {
    symbol::List l;
    for (auto const& e : inner_context.popAllExpressions()) {
        l.emplace_back(e.eval_f());
    }
    outer_context.pushExpression(typeid(l), [l = std::move(l)]{
        return l;
    });
}

void action<grammar::atom_map>::success(Context& inner_context, Context& outer_context) {
    symbol::Map m;
    auto expressions = inner_context.popAllExpressions();
    for (auto k_it=expressions.begin(); k_it != expressions.end(); k_it+=2) {
        auto v_it = std::next(k_it);
        auto c = outer_context.convert(k_it->type(), typeid(types::String));
        m.emplace(std::any_cast<types::String>(c(k_it->eval_f())), v_it->eval_f());
    }
    outer_context.pushExpression(typeid(m), [m = std::move(m)]{
        return m;
    });
}

} // namespace actions
} // namespace qrqma