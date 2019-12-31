#pragma once

#include <map>
#include <variant>
#include <vector>

#include "symbol.h"
#include "unique_function.h"

namespace qrqma {

struct Context {
    struct StaticText : std::string {};
    using Callable = unique_func<std::string()>;
    using Token = std::variant<StaticText, Callable>;

    Context *parent{nullptr};

    void addToken(Token t);

    symbol::Symbol const &operator[](std::string const &name) const;
    std::string operator()() const;

    Context(std::map<std::string, symbol::Symbol> symbols);

  private:
    std::vector<Token> tokens;
    std::map<std::string, symbol::Symbol> symbols;
};

} // namespace qrqma
