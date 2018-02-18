#include "pch.hpp"

extern "C"
{
	#include "network.h"
}

class PyNetwork
{
public:
	PyNetwork(Integer node_count, Integer links_count)
	{
		mNetwork = network_create(node_count, links_count);
	}
	~PyNetwork()
	{
		network_destroy(mNetwork);
	}
	Integer node_count() const { return mNetwork->node_count; }
	Integer links_count() const { return mNetwork->links_count; }
protected:
	Network* mNetwork;
};

PyNetwork* py_network_create(Integer node_count, Integer links_count)
{
	return new PyNetwork(node_count, links_count);
}

namespace py = pybind11;

void bind_network(py::module &m)
{
	// py::make_iterator
	m.def("network_create", &py_network_create);
	py::class_<PyNetwork>(m, "Network")
		.def(py::init<Integer, Integer>())
		.def_property_readonly("node_count", &PyNetwork::node_count)
		.def_property_readonly("links_count", &PyNetwork::links_count);
}
