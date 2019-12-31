#include "StringRenderer.h"
#include "actions.h"
#include "context.h"
#include "grammar.h"
#include "tao/pegtl.hpp"

#include <functional>
#include <vector>

namespace qrqma {

struct StringRenderer::Pimpl {
    Context context;

    Pimpl(std::map<std::string, symbol::Symbol> symbols)
        : context{std::move(symbols)} {}
};

namespace pegtl = tao::pegtl;
StringRenderer::StringRenderer(std::string_view input,
                               std::map<std::string, symbol::Symbol> symbols)
    : pimpl{std::make_unique<Pimpl>(std::move(symbols))} {
    pegtl::parse<pegtl::seq<grammar::grammar, pegtl::eof>, actions::action>(
        pegtl::memory_input{input, ""}, pimpl->context);
}

StringRenderer::~StringRenderer() {}

std::string StringRenderer::operator()() const { return pimpl->context(); }

} // namespace qrqma