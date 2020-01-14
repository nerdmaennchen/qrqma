
#include <iostream>
#include "qrqma/template.h"

#include "sargparse/Parameter.h"

namespace {

void demo();
auto demo_cmd = sargp::Command{"demo", "run the demo from the documentation", demo};

void demo() {
    auto templateStr = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <title>My Webpage</title>
</head>
<body>
    <ul id="navigation">
    {% for item in navigation %}
        <li><a href="{{ href(item) }}">{{ caption(item) }}</a></li>
    {% endfor %}
    </ul>

    <h1>My Webpage</h1>
    {{ a_variable }}

    {# a comment #}
</body>
</html>)";
    struct NavItem { std::string href, caption; };
    qrqma::Template rendering{
        templateStr,
        {
            {"navigation", qrqma::symbol::List{{NavItem{"Home", "index.html"}, NavItem{"Blog", "blog.html"}}} },
            {"href", qrqma::symbol::Function{[](NavItem const& ni) {
                return ni.href;
            }}},
            {"caption", qrqma::symbol::Function{[](NavItem const& ni) {
                return ni.caption;
            }}},
            {"a_variable", std::string{"qrqma is awesome!"}}
        }
    };
    std::cout << rendering() << std::endl;
}

}
