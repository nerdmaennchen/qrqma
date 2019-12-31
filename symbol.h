#pragma once

#include "unique_function.h"
#include <any>
#include <functional>

namespace qrqma {
namespace symbol {

using Symbol = std::any;

struct Function {
    using AnyVec = std::vector<std::any const *>;

    std::function<std::string(AnyVec const &)> f_int;

    std::string operator()(AnyVec const &anyVec) const { return f_int(anyVec); }

    template <typename ArgType> static auto pullArg(int idx) {
        return [=](AnyVec const &anyVec) {
            return std::any_cast<ArgType>(*anyVec.at(idx));
        };
    }

    template <typename Tuple, typename F, std::size_t... I>
    decltype(f_int) construct_helper(F &&f, std::index_sequence<I...>) {
        return [=](AnyVec const &anyvec) {
            return std::invoke(f, pullArg<std::decay_t<decltype(std::get<I>(
                                      std::declval<Tuple>()))>>(I)(anyvec)...);
        };
    }

    template <typename R, typename... Args>
    Function(std::function<R(Args...)> &&func) {
        using indexes = std::index_sequence_for<Args...>;
        f_int =
            construct_helper<std::tuple<Args...>>(std::move(func), indexes{});
    }

    template <typename _Functor,
              typename _Signature = typename detail::__function_guide_helper<
                  decltype(&_Functor::operator())>::type>
    Function(_Functor &&func)
        : Function{std::function<_Signature>(std::forward<_Functor>(func))} {}
};

} // namespace symbol
} // namespace qrqma