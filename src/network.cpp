#include "pch.hpp"
#include "clist.hpp"
#include <pybind11/stl.h>

extern "C"
{
	#include "network.h"
}

class PyNetworkNode
{
public:
	PyNetworkNode(NetworkNode* node) 
		:mNode(node) {}
	Number bias() { return mNode->bias; }

protected:
	NetworkNode* mNode;
};

class PyNetwork
{
public:
	typedef PyCList<NetworkNode, PyNetworkNode> PyNetworkNodeList;
public:
	PyNetwork(Integer node_count, Integer links_count)
	{
		mNetwork = network_create(node_count, links_count);
		mNodes.set(mNetwork->nodes, node_count);
	}
	~PyNetwork()
	{
		network_destroy(mNetwork);
	}
	Integer node_count() const { return mNetwork->node_count; }
	Integer links_count() const { return mNetwork->links_count; }
	PyNetworkNodeList nodes() { return mNodes; }
protected:
	Network*			mNetwork;
	PyNetworkNodeList	mNodes;
};

PyNetwork* py_network_create(Integer node_count, Integer links_count)
{
	return new PyNetwork(node_count, links_count);
}



namespace py = pybind11;

void bind_network(py::module &m)
{
	//py::make_iterator
	m.def("network_create", &py_network_create);
	py::class_<PyNetworkNode>(m, "NetworkNode")
		.def_property_readonly("bias", &PyNetworkNode::bias);
	py::class_<PyNetwork::PyNetworkNodeList::Iterator>(m, "NetworkNodeListIterator")
		.def("__iter__", &PyNetwork::PyNetworkNodeList::Iterator::iter)
		.def("__next__", &PyNetwork::PyNetworkNodeList::Iterator::next);
	py::class_<PyNetwork::PyNetworkNodeList>(m, "NetworkNodeList")
		.def("__len__", &PyNetwork::PyNetworkNodeList::size)
		.def("__getitem__", &PyNetwork::PyNetworkNodeList::get)
		.def("__iter__", &PyNetwork::PyNetworkNodeList::iter);
	py::class_<PyNetwork>(m, "Network")
		.def(py::init<Integer, Integer>())
		.def_property_readonly("node_count", &PyNetwork::node_count)
		.def_property_readonly("links_count", &PyNetwork::links_count)
		.def_property_readonly("nodes", &PyNetwork::nodes)
		.def_property_readonly("test_list", [](PyNetwork& net) {return std::vector<PyNetworkNode>{net.nodes().get(0), net.nodes().get(1)}; });
}
