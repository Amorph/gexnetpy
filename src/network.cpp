#include "pch.hpp"
#include <sstream>
#include <iomanip>

#include "clist.hpp"
#include <pybind11/stl.h>


extern "C"
{
	#include "network.h"
	#include "gexnet.h"
	#include "stream.h"
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

class PyGNStream
{
public:
	PyGNStream(struct GNStream* stream)
		:stream_(stream)
	{

	}
	~PyGNStream()
	{
		stream_->system->stream->destroy(stream_);
	}
	void load(const pybind11::list& init_data)
	{
		struct GNSystem* G = stream_->system;
		GNStreamLockData* lock = G->stream->lock(stream_, 0, init_data.size(), 0);
		uint8_t* data = (uint8_t*)lock->data;
		for (size_t i = 0; i < lock->count; i++)
		{
			if (lock->type == GN_TYPE_NUMBER)
			{
				GNNumber num = init_data[i].cast<GNNumber>();
				*((GNNumber*)data) = num;
			}else if (lock->type == GN_TYPE_INDEX)
			{
				GNIndex num = init_data[i].cast<GNIndex>();
				*((GNIndex*)data) = num;
			}
			data += lock->element_size;
		}
		G->stream->unlock(lock);
	}
	void clear() { stream_->system->stream->clear(stream_);	}
protected:
	struct GNStream* stream_;
};

class PyGNSystem
{
public:

public:
	PyGNSystem()
	{
		G = gexnet_native_init(NULL);
	}
	PyGNStream* create_stream(GNType type, const pybind11::list& init_data)
	{
		struct GNStream* stream = G->create_stream(G, type, init_data.size(), NULL);
		PyGNStream* pyStream = new PyGNStream(stream);

		pyStream->load(init_data);

		return pyStream;
	}
protected:
	struct GNSystem* G;
};

namespace py = pybind11;

void bind_network(py::module &m)
{
	m.attr("GN_TYPE_UNKNOWN") = GN_TYPE_UNKNOWN;
	m.attr("GN_TYPE_FLOAT") = GN_TYPE_FLOAT;
	m.attr("GN_TYPE_INTEGER") = GN_TYPE_INTEGER;
	m.attr("GN_TYPE_INDEX") = GN_TYPE_INDEX;
	m.attr("GN_TYPE_INDEX_INDEX") = GN_TYPE_INDEX_INDEX;
	m.attr("GN_TYPE_LINK") = GN_TYPE_LINK;
	m.attr("GN_TYPE_NUMBER") = GN_TYPE_NUMBER;

	py::class_<PyGNStream>(m, "GNStream")
		.def("clear", &PyGNStream::clear)
		.def("load", &PyGNStream::load);

	py::class_<PyGNSystem>(m, "GNSystem")
		.def(py::init<>())
		.def("create_stream", &PyGNSystem::create_stream);
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
