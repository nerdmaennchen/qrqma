#include "context.h"
#include "types.h"
#include "../demangle.h"

#include <numeric>

namespace {

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...)->overloaded<Ts...>;

} // namespace

namespace qrqma {
namespace actions {

Context::Symbol& Context::operator[](std::string const &name) {
    Context *context{this};
    while (context) {
        auto it = context->symbols.find(name);
        if (it != context->symbols.end()) {
            return it->second;
        }
        context = context->parent;
    };
    return symbol::the_undefined_symbol;
}
symbol::Symbol const* Context::getSymbol(std::string const &name) const {
    Context const *context{this};
    while (context) {
        auto it = context->symbols.find(name);
        if (it != context->symbols.end()) {
            return &it->second;
        }
        context = context->parent;
    }
    return nullptr;
}

Context::SymbolTable const& Context::getSymbolTable() const { return symbols; }

void Context::setSymbol(std::string const& name, Symbol symbol) {
    symbols[name] = std::move(symbol);
}

Context::Block const& Context::getBlock(std::string const &name) const {
    Context const *context{this};
    while (context) {
        auto it = context->blocks.find(name);
        if (it != context->blocks.end()) {
            return it->second;
        }
        context = context->parent;
    }
    throw std::runtime_error("no block with name \"" + name + "\" provided!");
}
void Context::setBlock(std::string const& name, Block block) {
    blocks.emplace(name, std::move(block));
}
Context::BlockTable Context::popBlockTable() { 
    BlockTable table;
    std::swap(table, blocks);
    return table; 
}

Context::RenderOutput Context::operator()() const {
    RenderOutput ro;
    for (auto const &t : tokens) {
        auto ro_sub = std::visit(overloaded{
            [](StaticText const &t) -> RenderOutput { return {t, false}; },
            [](Renderable const &c) -> RenderOutput { return c(); },
            [](auto const&) -> RenderOutput { return {"", true}; }},
        t);
        ro.rendered += ro_sub.rendered;
        if (ro_sub.stop_rendering_flag) {
            return {std::move(ro.rendered), true};
        }
    }
    return ro;
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

Context::Context(SymbolTable in_symbols, BlockTable in_blocks)
    : symbols{std::move(in_symbols)} 
    , blocks{std::move(in_blocks)}
{}

Context::Context(Context* c_parent)
    : parent{c_parent} {}


Context& Context::childContext() {
    return *childContexts.emplace_back(std::make_unique<Context>(this)).get();
}

Context::ConverterFunc Context::convert(std::type_info const& from, std::type_info const& to) {
    if (from == to) {
        return [](std::any const& a) { return a; };
    }

    if (from == typeid(types::LazyEvaluate)) {
        // if the type is not known yet
        return [&to](std::any const& a) {
            if (a.type() == typeid(types::LazyEvaluate)) { // cannot convert anything during runtime from void
                throw std::runtime_error("cannot (runtime) convert from void to " + internal::demangle(to));
            }
            return convert(a.type(), to)(a);
        };
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
        } else if (typeid(symbol::List) == from) {
            return [](std::any const& a) {
                auto const& l = std::any_cast<symbol::List const&>(a);
                std::string ret = "[";
                auto to_str = [](std::any const& a) {
                    return std::any_cast<std::string>(convert(a.type(), typeid(std::string))(a));
                };
                if (not l.empty()) {
                    ret += std::accumulate(std::next(std::begin(l)), std::end(l), to_str(*std::begin(l)), [=](std::string const& l, std::any const& r) {
                        return l + ", " + to_str(r);
                    });
                }
                ret += "]";
                return ret;
            };
        }
    } else if (typeid(types::Bool) == to) {
        if (typeid(std::string) == from) {
            return [](std::any const& a) { return not std::any_cast<std::string const&>(a).empty();};
        } else if (typeid(types::Integer) == from) {
            return [](std::any const& a) { return static_cast<types::Bool>(std::any_cast<types::Integer>(a)); };
        } else if (typeid(types::Float) == from) {
            return [](std::any const& a) { return static_cast<types::Bool>(std::any_cast<types::Float>(a)); };
        } else if (typeid(symbol::Undefined) == from) {
            return [](std::any const&) { return false; };
        }
    }
    throw std::runtime_error("dont know how to create a converter from type " + internal::demangle(from) + " to " + internal::demangle(to));
    return [](std::any const& a) {
        return a;
    };
}

}
}
