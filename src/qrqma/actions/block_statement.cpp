#include "block_statement.h"

#include "../overloaded.h"

namespace qrqma {
namespace actions {

void action<grammar::block_control_statement>::success(std::string& block_name, Context& inner_ctx, Context& outer_ctx) {
    auto renderFunc = [&inner_ctx](){
        return inner_ctx();
    };

    outer_ctx.setBlock(block_name, renderFunc);
    outer_ctx.addToken([&outer_ctx, block_name=std::move(block_name)]{
        return std::visit(detail::overloaded{
                [](Context::StaticText const &t) -> Context::RenderOutput { return {t, false}; },
                [](Context::Renderable const &c) -> Context::RenderOutput { return c(); },
                [](auto const&) -> Context::RenderOutput { return {"", true}; }},
            outer_ctx.getBlock(block_name)
        );
    });
}

}
}