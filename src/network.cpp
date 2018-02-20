#include "pch.hpp"
#include <sstream>

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
	Activation_func func() { return mNode->activation_function; }
protected:
	friend std::ostream& operator<<(std::ostream& out, const PyNetworkNode& h);
	NetworkNode* mNode;
};

std::ostream& operator<<(std::ostream& ss, const PyNetworkNode& h)
{
	if (!h.mNode)
		return ss << "[NULL]";
	return ss << "[bias:" << h.mNode->bias << ", func:" << h.mNode->activation_function << "]";
}


class PyNetworkLink
{
public:
	PyNetworkLink(NetworkLink* link)
		:mLink(link) {}

	PyNetworkNode get_input() { return PyNetworkNode(mLink->input); }
	PyNetworkNode get_output() { return PyNetworkNode(mLink->output); }
	Number get_weight() { return mLink->weight; }
protected:
	friend std::ostream& operator<<(std::ostream& out, const PyNetworkLink& h);
	NetworkLink* mLink;
};

std::ostream& operator<<(std::ostream& ss, const PyNetworkLink& h)
{
	if (!h.mLink)
		return ss << "[NULL]";
	return ss << "[input]:";
}

class PyNetwork
{
public:
	typedef PyCList<NetworkNode, PyNetworkNode> PyNetworkNodeList;
	typedef PyCList<NetworkLink, PyNetworkLink> PyNetworkLinkList;
public:
	PyNetwork(Integer node_count, Integer links_count)
	{
		mNetwork = network_create(node_count, links_count);
		mNodes.set(mNetwork->nodes, node_count);
		mLinks.set(mNetwork->links, links_count);
	}
	~PyNetwork()
	{
		network_destroy(mNetwork);
	}
	Integer node_count() const { return mNetwork->node_count; }
	Integer links_count() const { return mNetwork->links_count; }
	PyNetworkNodeList nodes() { return mNodes; }
	PyNetworkLinkList links() { return mLinks; }
protected:
	Network*			mNetwork;
	PyNetworkNodeList	mNodes;
	PyNetworkLinkList	mLinks;
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
		.def_property_readonly("bias", &PyNetworkNode::bias)
		.def("__repr__", [](PyNetworkNode &a) { std::ostringstream ss; ss << a; return ss.str(); });

	py::class_<PyNetworkLink>(m, "NetworkLink")
		.def_property_readonly("input", &PyNetworkLink::get_input)
		.def_property_readonly("output", &PyNetworkLink::get_output)
		.def_property_readonly("weight", &PyNetworkLink::get_weight)
		.def("__repr__", [](PyNetworkLink &a) { std::ostringstream ss; ss << a; return ss.str(); });

	bind_clist<PyNetwork::PyNetworkNodeList>(m, "NetworkNodeList");
	bind_clist<PyNetwork::PyNetworkLinkList>(m, "NetworkLinkList");
	py::class_<PyNetwork>(m, "Network")
		.def(py::init<Integer, Integer>())
		.def_property_readonly("node_count", &PyNetwork::node_count)
		.def_property_readonly("links_count", &PyNetwork::links_count)
		.def_property_readonly("nodes", &PyNetwork::nodes)
		.def_property_readonly("links", &PyNetwork::links);
}
