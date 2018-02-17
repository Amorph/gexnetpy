#include "pch.hpp"

extern "C"
{
	#include "network.h"
}

namespace py = pybind11;

void bind_network(py::module &m)
{
	m.def("create_network", &create_network, py::return_value_policy::reference);
}