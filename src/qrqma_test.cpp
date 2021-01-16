#include "qrqma_test.h"
#include "qrqma/template.h"

#include "qrqma/tao/pegtl.hpp"
#include "qrqma/actions/actions.h"

namespace pegtl = tao::pegtl;

namespace qrqma_test
{

namespace {

void test(std::string const& content) 
{
    try {
        qrqma::Template rendering{
            content,
            {
                // to play with qrqma add symbols here
                {"test", std::string{"Hello from compiling"}},
                {"callable", qrqma::symbol::Function{[]() -> std::string {
                    return "output from a callable function";
                }}},
            }
        };

        std::cout << rendering(
            // to play with qrqma add symbols here (for runtime defined symbols)
            {
                {"runtime_value", std::string{"Hello from rendering"}},
            }
        ) << std::endl;
    } catch (pegtl::parse_error const& error) {
        std::cerr << error.what() << std::endl;
    }
}

}

void qrqma_test() 
{
    if (template_text) {
        test(*template_text);
    }
    if (template_file) {
        auto content = qrqma::defaultLoader()(*template_file);
        test(content);
    }
}

sargp::Task qrqma_test_task{qrqma_test};

}