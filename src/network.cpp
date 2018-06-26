#include "pch.hpp"
#include <sstream>
#include <iomanip>

#include "clist.hpp"
#include <pybind11/stl.h>


extern "C"
{
	#include "network.h"
}

/*

class PyNetworkNode
{
	friend class PyNetworkLink;
public:
	PyNetworkNode(NetworkNode* node) 
		:mNode(node) {}

	bool			valid() { return mNode != nullptr; }

	Number			get_bias() { if (!mNode) return -INFINITY; return mNode->bias; }
	void			set_bias(Number number) { mNode->bias = number; }
	Activation_func	get_func() { if (!mNode) return std::numeric_limits<Activation_func>::min(); return mNode->activation_function; }
	void			set_func(Activation_func func) { mNode->activation_function = func; }
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

	size_t			get_input() { return mLink->input; }
	void			set_input(size_t node) { mLink->input = node; }

	size_t			get_output() { return mLink->output; }
	void			set_output(size_t node) { mLink->output = node; }
	Number			get_weight() { if (!mLink) throw pybind11::stop_iteration(); return mLink->weight; }
	void			set_weight(Number weight) { mLink->weight = weight; }
protected:
	friend std::ostream& operator<<(std::ostream& out, const PyNetworkLink& h);
	NetworkLink* mLink;
};

std::ostream& operator<<(std::ostream& ss, const PyNetworkLink& h)
{
	if (!h.mLink)
		return ss << "[NULL]";
	return ss << "[0x" << std::hex << h.mLink->input 
		<< "-" << h.mLink->weight << "-0x" << h.mLink->output << "]";
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


*/

namespace py = pybind11;

void bind_network(py::module &m)
{
	//m.def("network_create", &py_network_create);
	/*
	//py::make_iterator
	m.def("network_create", &py_network_create);
	m.attr("NULL_LINK") = NULL_LINK;
	py::class_<PyNetworkNode>(m, "NetworkNode")
		.def_property("bias", &PyNetworkNode::get_bias, &PyNetworkNode::set_bias)
		.def_property("func", &PyNetworkNode::get_func, &PyNetworkNode::set_func)
		.def("__repr__", [](PyNetworkNode &a) { std::ostringstream ss; ss << a; return ss.str(); })
		.def("__bool__", [](PyNetworkNode &a) { return a.valid(); });

	py::class_<PyNetworkLink>(m, "NetworkLink")
		.def_property("input", &PyNetworkLink::get_input, &PyNetworkLink::set_input, py::arg().none(true))
		.def_property("output", &PyNetworkLink::get_output, &PyNetworkLink::set_output)
		.def_property("weight", &PyNetworkLink::get_weight, &PyNetworkLink::set_weight)
		.def("__repr__", [](PyNetworkLink &a) { std::ostringstream ss; ss << a; return ss.str(); });

	bind_clist<PyNetwork::PyNetworkNodeList>(m, "NetworkNodeList");
	bind_clist<PyNetwork::PyNetworkLinkList>(m, "NetworkLinkList");
	py::class_<PyNetwork>(m, "Network")
		.def(py::init<Integer, Integer>())
		.def_property_readonly("node_count", &PyNetwork::node_count)
		.def_property_readonly("links_count", &PyNetwork::links_count)
		.def_property_readonly("nodes", &PyNetwork::nodes)
		.def_property_readonly("links", &PyNetwork::links);
		*/
}
