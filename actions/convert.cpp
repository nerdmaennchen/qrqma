#include "convert.h"
#include "types.h"
#include "../demangle.h"

#include <numeric>

namespace qrqma {
namespace actions {


std::any convert(std::any const& in, std::type_info const& to) {
    if (in.type() == to) {
        return in;
    }

    if (typeid(types::String) == to) {
        if (typeid(void) == in.type()) {
            return std::string{};
        } else if (typeid(types::Undefined) == in.type()) {
            return std::string{};
        }else if (typeid(types::Integer) == in.type()) {
            return std::to_string(std::any_cast<types::Integer>(in));
        } else if (typeid(types::Float) == in.type()) {
            return std::to_string(std::any_cast<types::Float>(in));
        } else if (typeid(types::Bool) == in.type()) {
            return std::string{std::any_cast<types::Bool>(in)?"True":"False"};
        } else if (typeid(symbol::List) == in.type()) {
            auto const& l = std::any_cast<symbol::List const&>(in);
            std::string ret = "[";
            auto to_str = [] (std::any const& e) {
                return convert<std::string>(e);
            };
            if (not l.empty()) {
                ret += std::accumulate(std::next(std::begin(l)), std::end(l), to_str(*std::begin(l)), [=](std::string const& l, std::any const& r) {
                    return l + ", " + to_str(r);
                });
            }
            ret += "]";
            return ret;
        } else if (typeid(symbol::Map) == in.type()) {
            auto to_str = [] (std::pair<std::string, std::any> const& p) {
                return "\"" + p.first + "\": " + convert<std::string>(p.second);
            };
            auto const& m = std::any_cast<symbol::Map const&>(in);
            std::string ret = "{";
            if (not m.empty()) {
                ret += std::accumulate(std::next(std::begin(m)), std::end(m), to_str(*std::begin(m)), [=](std::string const& l, auto const& p) {
                    return l + ", " + "\"" + p.first + "\": " + convert<std::string>(p.second);
                });
            }
            ret += "}";
            return ret;
        }
    } else if (typeid(types::Bool) == to) {
        if (typeid(std::string) == in.type()) {
            return not std::any_cast<std::string const&>(in).empty();
        } else if (typeid(types::Integer) == in.type()) {
            return static_cast<types::Bool>(std::any_cast<types::Integer>(in));
        } else if (typeid(types::Float) == in.type()) {
            return static_cast<types::Bool>(std::any_cast<types::Float>(in));
        } else if (typeid(types::Undefined) == in.type()) {
            return false;
        } else if (typeid(void) == in.type()) {
            return false;
        } else if (typeid(types::List) == in.type()) {
            return not std::any_cast<types::List const&>(in).empty();
        } else if (typeid(types::Map) == in.type()) {
            return not std::any_cast<types::Map const&>(in).empty();
        }
    }
    throw std::runtime_error("dont know how to create a converter from type " + internal::demangle(in.type()) + " to " + internal::demangle(to));
}

}
}