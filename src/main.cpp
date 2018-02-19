#include "pch.hpp"

#define __DEBUG_START__ 1

#if __DEBUG_START__
#include "windows.h"
#endif

namespace py = pybind11;

void bind_network(py::module &m);

PYBIND11_MODULE(gexnetpy, m) {
#if __DEBUG_START__
	MessageBoxA(0, "GexSIM", "GexSIM", MB_OK);
#endif
	bind_network(m);

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
