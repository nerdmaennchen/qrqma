#pragma once

#include "unique_function.h"
#include <functional>
#include <type_traits>
#include <any>
#include <map>
#include <vector>
#include <variant>

namespace qrqma {
namespace symbol {

struct StaticText : std::string {};

// the RenderOutput mechanism is used to render only parts of a template (e.g., for use as child template)
struct RenderOutput { 
    std::string rendered;
    bool stop_rendering_flag {false};
};
using Renderable = unique_func<RenderOutput()>;
using Block = std::variant<StaticText, Renderable>;
using BlockTable = std::map<std::string, Block>;

struct Undefined {};
using Symbol = std::any;
inline Symbol the_undefined_symbol = Symbol{Undefined{}};
using SymbolTable = std::map<std::string, Symbol>;

struct Function {
    using AnyVec = std::vector<std::any>;

    std::function<std::any(AnyVec const &)> f_int;
    std::type_info const& targetType;
    std::vector<std::type_info const*> argTypes;

    std::any operator()(AnyVec const &anyVec) const { return f_int(anyVec); }

    template <typename ArgType> static auto pullArg(int idx) {
        return [=](AnyVec const &anyVec) {
            return std::any_cast<ArgType>(anyVec.at(idx));
        };
    }

    template <typename R, typename... Args>
    Function(std::function<R(Args...)> &&func) : targetType{typeid(R)} {
        using indexes = std::index_sequence_for<Args...>;
        f_int = function_construct_helper<std::tuple<Args...>>(std::move(func), indexes{});
        argTypes = {
            &typeid(std::remove_reference_t<std::remove_cv_t<Args>>)...
        };
    }

    template <typename _Functor,
              typename _Signature = typename detail::__function_guide_helper<
                  decltype(&_Functor::operator())>::type>
    Function(_Functor &&func)
        : Function{std::function<_Signature>(std::forward<_Functor>(func))} {}

private:
    template <typename Tuple, typename F, std::size_t... I>
    decltype(f_int) function_construct_helper(F &&f, std::index_sequence<I...>) {
        return [=](AnyVec const &anyvec) {
            return std::invoke(f, pullArg<std::decay_t<decltype(std::get<I>(
                                      std::declval<Tuple>()))>>(I)(anyvec)...);
        };
    }
};

using List = std::vector<Symbol>;
using Map = std::map<std::string, Symbol>;

} // namespace symbol
} // namespace qrqma