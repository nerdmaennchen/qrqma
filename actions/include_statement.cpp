#include "include_statement.h"

#include "types.h"
#include "../grammar/grammar.h"
#include "actions.h"
#include "../demangle.h"
#include "../overloaded.h"

namespace qrqma {
namespace actions {

namespace pegtl = tao::pegtl;

void action<grammar::include_control_statement>::apply(ContextP& context, bool ignore_source_not_found) {
	auto e = context->popExpression();

	auto loaderCtx = context.get();
	while (loaderCtx and not loaderCtx->getTemplateLoader()) {
		loaderCtx = loaderCtx->getParentContext();
	}
	
	auto loader = loaderCtx->getTemplateLoader();
	if (not loader) {
		throw std::runtime_error{"cannot include a template without specifying a template loader!"};
	}

    std::visit(detail::overloaded{
        [&] (types::ConstantExpression const& ce) {
			auto name = std::any_cast<std::string>(ce.eval());
			try {
				auto content = loader(name);
				pegtl::parse<pegtl::if_must<grammar::grammar, pegtl::eof>, actions::action>(
					pegtl::memory_input{content, ""}, 
					context
				);
			} catch (...) {
				if (ignore_source_not_found) {
					return;
				}
				throw;
			}
        },
        [&] (types::NonconstantExpression& nce) {
			context->addRenderToken([c=context.get(), nce=std::move(nce), loader, ignore_source_not_found] -> Context::RenderOutput {
				auto sub_c = std::make_unique<Context>(c);
				auto name = std::any_cast<std::string>(nce.eval());
				try {
					auto content = loader(name);
					pegtl::parse<pegtl::if_must<grammar::grammar, pegtl::eof>, actions::action>(
						pegtl::memory_input{content, ""}, 
						sub_c
					);
					return (*sub_c)(false);
				} catch (...) {
					if (ignore_source_not_found) {
						return {"", false};
					}
					throw;
				}
			});
		}
    }, e);
    
}

}
}
