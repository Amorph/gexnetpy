#include "pch.hpp"

namespace py = pybind11;

void bind_network(py::module &m);

PYBIND11_MODULE(gexnetpy, m) {
	bind_network(m);

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
