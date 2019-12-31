#pragma once

#include <map>
#include <memory>
#include <string_view>

#include "symbol.h"

namespace qrqma {

struct StringRenderer {
    StringRenderer(std::string_view input,
                   std::map<std::string, symbol::Symbol> symbols);
    ~StringRenderer();

    std::string operator()() const;

  private:
    struct Pimpl;
    std::unique_ptr<Pimpl> pimpl;
};

} // namespace qrqma