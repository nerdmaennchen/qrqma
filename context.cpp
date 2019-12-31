#include "context.h"

namespace {

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...)->overloaded<Ts...>;

} // namespace

namespace qrqma {

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

Context::Context(std::map<std::string, symbol::Symbol> in_symbols)
    : symbols{std::move(in_symbols)} {}

} // namespace qrqma
