#pragma once

#include <map>
#include <variant>
#include <vector>
#include <typeinfo>
#include <optional>

#include "../loader.h"
#include "../symbol.h"
#include "../unique_function.h"

namespace qrqma {
namespace actions {

struct Context {
    using Symbol = symbol::Symbol;
    using StaticText = symbol::StaticText;
    using Callable = symbol::Callable;
    using Block = symbol::Block;

    using Token = std::variant<StaticText, Callable>;

    using SymbolTable = symbol::SymbolTable;
    using BlockTable  = symbol::BlockTable;

    // a function that eats an any and converts the inner type to something else
    using ConverterFunc = std::function<std::any(std::any const&)>;

    struct Expression {
        using FuncType = qrqma::unique_func<Symbol()>;

        Expression(std::type_info const& t, FuncType func) 
          : mType{t}, mEval{std::move(func)} {}

        Expression(Expression&& other) noexcept : mType{std::move(other.mType)}, mEval{std::move(other.mEval)} {}

        template<typename T>
        T eval() const { return std::any_cast<T>(eval_f()); }

        auto eval_f() const { return mEval(); }
        std::type_info const& type() { return mType; }

    private:
        std::type_info const& mType;
        FuncType mEval;
    };

    Context() = default;
    Context(SymbolTable symbols, BlockTable blocks);
    Context(Context const* parent);

    void setTemplateLoader(TemplateLoader loader) {
        templateLoader = std::move(loader);
    }
    TemplateLoader getTemplateLoader() const {
        return *templateLoader;
    }

    void addToken(Token t);

    void pushExpression(Expression f);
    void pushExpression(std::type_info const& t, Expression::FuncType f) {
        pushExpression(Expression{t, std::move(f)});
    }
    
    Expression popExpression();
    std::vector<Expression> popAllExpressions();
    
    auto expressionStackSize() const {
        return expression_stack.size();
    }

    Symbol const& operator[](std::string const &name) const;
    Symbol const* getSymbol(std::string const &name) const;
    SymbolTable const& getSymbolTable() const;
    void setSymbol(std::string const& name, Symbol symbol);

    Block const& getBlock(std::string const &name) const;
    void setBlock(std::string const& name, Block block);
    BlockTable popBlockTable();


    // render this context as string
    std::string operator()() const;

    static ConverterFunc convert(std::type_info const& from, std::type_info const& to);

    Context& childContext();

    Context const* parentContext() const {
        return parent;
    }

private:

    std::vector<std::unique_ptr<Context>> childContexts;
    std::vector<Token> tokens;

    SymbolTable symbols;
    BlockTable blocks;

    std::vector<Expression> expression_stack;

    std::optional<TemplateLoader> templateLoader {nullptr};

    Context const* parent{nullptr};
};

}
}
