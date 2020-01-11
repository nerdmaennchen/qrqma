#include "context.h"
#include "types.h"
#include "../demangle.h"

namespace {

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...)->overloaded<Ts...>;

} // namespace

namespace qrqma {
namespace actions {

symbol::Symbol const &Context::operator[](std::string const &name) const {
    Context const *context{this};
    while (context) {
        auto it = context->symbols.find(name);
        if (it != context->symbols.end()) {
            return it->second;
        }
        context = context->parent;
    }
    throw std::runtime_error("no symbol with name \"" + name + "\" provided!");
}

std::string Context::operator()() const {
    std::string s;
    for (auto const &t : tokens) {
        std::visit(overloaded{[&](StaticText const &t) { s += t; },
                              [&](Callable const &c) { s += c(); }},
                   t);
    }
    return s;
}

void Context::addToken(Token t) { tokens.emplace_back(std::move(t)); }


void Context::pushExpression(Expression f) {
    expression_stack.emplace_back(std::move(f));
}

Context::Expression Context::popExpression() {
    auto e = std::move(expression_stack.back());
    expression_stack.pop_back();
    return e;
}

std::vector<Context::Expression> Context::popAllExpressions() {
    std::vector<Context::Expression> newStack;
    std::swap(newStack, expression_stack);
    return newStack;
}

Context::Context(SymbolTable in_symbols)
    : symbols{std::move(in_symbols)} {}

Context::Context(Context const* c_parent)
    : parent{c_parent} {}


Context& Context::childContext() {
    return *childContexts.emplace_back(std::make_unique<Context>(this)).get();
}

Context::ConverterFunc Context::convert(std::type_info const& from, std::type_info const& to) {
    if (from == to) {
        return [](std::any const& a) { return a; };
    }
    if (typeid(types::String) == to) {
        if (typeid(types::Integer) == from) {
            return [](std::any const& a) { return std::to_string(std::any_cast<types::Integer>(a)); };
        } else if (typeid(types::Float) == from) {
            return [](std::any const& a) { return std::to_string(std::any_cast<types::Float>(a)); };
        } else if (typeid(types::Bool) == from) {
            return [](std::any const& a) { 
                if (std::any_cast<types::Bool>(a)) {
                    return std::string{"True"};
                } else {
                    return std::string{"False"};
                }
            };
        }
    } else if (typeid(types::Bool) == to) {
        if (typeid(std::string) == from) {
            return [](std::any const& a) { return not std::any_cast<std::string>(a).empty();};
        } else if (typeid(types::Integer) == from) {
            return [](std::any const& a) { return static_cast<types::Bool>(std::any_cast<types::Integer>(a)); };
        } else if (typeid(types::Float) == from) {
            return [](std::any const& a) { return static_cast<types::Bool>(std::any_cast<types::Float>(a)); };
        }
    }
    throw std::runtime_error("dont know how to create a converter from type " + internal::demangle(from) + " to " + internal::demangle(to));
    return [](std::any const& a) {
        return a;
    };
}

}
}
