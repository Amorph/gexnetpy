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

class PyGNStreamLockData
{
public:
	PyGNStreamLockData(struct GNStreamLockData* lock)
	{
		lock_ = lock;
		pydata = pybind11::list(lock->count);
		switch (lock->type)
		{
		case GN_TYPE_FLOAT:
		{
			GNNumber* data = (GNNumber*)lock->data;
			for (size_t i = 0; i < lock->count; i++)
				pydata[i] = pybind11::cast(data[i]);
		}break;
		case GN_TYPE_INTEGER:
		{
			GNInteger* data = (GNInteger*)lock->data;
			for (size_t i = 0; i < lock->count; i++)
				pydata[i] = pybind11::cast(data[i]);
		}break;
		case GN_TYPE_INDEX:
		{
			GNIndex* data = (GNIndex*)lock->data;
			for (size_t i = 0; i < lock->count; i++)
				pydata[i] = pybind11::cast(data[i]);
		}break;
		case GN_TYPE_INDEX_INDEX:
		{
			GNLink* data = (GNLink*)lock->data;
			for (size_t i = 0; i < lock->count; i++)
			{
				pybind11::tuple link(2);
				link[0] = data[i].input;
				link[1] = data[i].output;
				pydata[i] = link;
			}
		}break;
		}
	}
	~PyGNStreamLockData()
	{
		if(lock_)
			lock_->stream->system->stream->unlock(lock_);
	}

	void unlock()
	{
		switch (lock_->type)
		{
		case GN_TYPE_FLOAT:
		{
			GNNumber* data = (GNNumber*)lock_->data;
			for (size_t i = 0; i < lock_->count; i++)
				data[i] = pydata[i].cast<GNNumber>();
		}break;
		case GN_TYPE_INTEGER:
		{
			GNInteger* data = (GNInteger*)lock_->data;
			for (size_t i = 0; i < lock_->count; i++)
				data[i] = pydata[i].cast<GNInteger>();
		}break;
		case GN_TYPE_INDEX:
		{
			GNIndex* data = (GNIndex*)lock_->data;
			for (size_t i = 0; i < lock_->count; i++)
				data[i] = pydata[i].cast<GNIndex>();
		}break;
		case GN_TYPE_INDEX_INDEX:
		{
			GNLink* data = (GNLink*)lock_->data;
			
			for (size_t i = 0; i < lock_->count; i++)
			{
				pybind11::tuple& link = pydata[i].cast<pybind11::tuple>();
				data[i].input = link[0].cast<GNIndex>();
				data[i].input = link[1].cast<GNIndex>();
			}
		}break;
		}
		lock_->stream->system->stream->unlock(lock_);
		lock_ = NULL;
		pydata = pybind11::list();
	}
	GNType type() const { return lock_? lock_->type : GN_TYPE_UNKNOWN; }
	GNIndex count() const { return lock_ ? lock_->count : 0; }
	size_t element_size() const { return lock_ ? lock_->element_size : 0; }

protected:
	struct GNStreamLockData* lock_;
public:
	pybind11::list pydata;
};

class PyGNStream
{
	friend class PyGNSystem;
public:
	PyGNStream(struct GNStream* stream)
		:stream_(stream)
	{

	}
	~PyGNStream()
	{
		stream_->system->stream->destroy(stream_);
	}

	GNType type() const { return stream_ ? stream_->type : GN_TYPE_UNKNOWN; }
	GNIndex count() const { return stream_ ? stream_->count : 0; }
	pybind11::list get_stream_data()
	{
		PyGNStreamLockData* lock = this->lock(0, 0, 0);

		pybind11::list res = lock->pydata;
		delete lock;

		return res;
	}

	void set_stream_data(const pybind11::list& init_data)
	{
		struct GNSystem* G = stream_->system;
		GNStreamLockData* lock = G->stream->lock(stream_, 0, init_data.size(), 0);
		uint8_t* data = (uint8_t*)lock->data;
		for (size_t i = 0; i < lock->count; i++)
		{
			switch (lock->type)
			{
			case GN_TYPE_NUMBER:
			{
				GNNumber num = init_data[i].cast<GNNumber>();
				*((GNNumber*)data) = num;
			}break;
			case GN_TYPE_INTEGER:
			{
				GNInteger num = init_data[i].cast<GNInteger>();
				*((GNInteger*)data) = num;
			}break;
			case GN_TYPE_INDEX:
			{
				GNIndex num = init_data[i].cast<GNIndex>();
				*((GNIndex*)data) = num;
			}break;
			case GN_TYPE_INDEX_INDEX:
			{
				typedef pybind11::tuple tuple;
				tuple& link_raw = init_data[i].cast<tuple>();

				if (link_raw.size() < 2)
					continue;

				GNLink link;
				link.input = link_raw[0].cast<GNIndex>();
				link.output = link_raw[1].cast<GNIndex>();
				*((GNLink*)data) = link;
			}break;
			}
			data += lock->element_size;
		}
		G->stream->unlock(lock);
	}

	void clear() { stream_->system->stream->clear(stream_);	}
	void copy(PyGNStream* input) { stream_->system->stream->copy(stream_, input->stream_); }
	void add(PyGNStream* input) { stream_->system->stream->add(stream_, input->stream_); }
	void set_stream_data_indexed(PyGNStream* indexes, PyGNStream* data) { stream_->system->stream->set_stream_data_indexed(stream_, indexes->stream_, data->stream_); }
	void get_stream_data_indexed(PyGNStream* indexes, PyGNStream* output) { stream_->system->stream->get_stream_data_indexed(stream_, indexes->stream_, output->stream_); }
	void multiply_add_links(PyGNStream* links, PyGNStream* x, PyGNStream* weights) { stream_->system->stream->multiply_add_links(stream_, links->stream_, x->stream_, weights->stream_); }
	void process_stream(PyGNStream* input, size_t function) { stream_->system->stream->process_stream(stream_, input->stream_, function); }

	PyGNStreamLockData* lock(GNIndex start, GNIndex count, size_t flags)
	{
		struct GNStreamLockData* lock_data = stream_->system->stream->lock(stream_, start, count, flags);
		return new PyGNStreamLockData(lock_data);
	}
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
	PyGNStream* create_stream(GNType type, GNIndex size)
	{
		struct GNStream* stream = G->create_stream(G, type, size, NULL);
		return new PyGNStream(stream);
	}
	PyGNStream* create_stream_data(GNType type, const pybind11::list& init_data)
	{
		struct GNStream* stream = G->create_stream(G, type, init_data.size(), NULL);
		PyGNStream* pyStream = new PyGNStream(stream);

		pyStream->set_stream_data(init_data);

		return pyStream;
	}
	GNIndex compute_node_count(PyGNStream* stream)
	{
		return G->compute->node_count(stream->stream_);
	}
	pybind11::tuple compute_in_out(PyGNStream* stream, GNIndex node_count)
	{
		GNStream *in, *out;
		if (!G->compute->in_out(stream->stream_, node_count, &in, &out))
			return pybind11::none();
		pybind11::tuple result(2);
		result[0] = pybind11::cast(new PyGNStream(in));
		result[1] = pybind11::cast(new PyGNStream(out));
		return result;
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
	m.attr("GN_FUNCTION_TANH") = GN_FUNCTION_TANH;

	py::class_<PyGNStreamLockData>(m, "GNStreamLockData")
		.def("unlock", &PyGNStreamLockData::unlock)
		.def_readwrite("data", &PyGNStreamLockData::pydata)
		.def_property_readonly("type", &PyGNStreamLockData::type)
		.def_property_readonly("count", &PyGNStreamLockData::count)
		.def_property_readonly("element_size", &PyGNStreamLockData::element_size);

	py::class_<PyGNStream>(m, "GNStream")
		.def_property_readonly("type", &PyGNStream::type)
		.def_property_readonly("count", &PyGNStream::count)
		.def_property_readonly("data", &PyGNStream::get_stream_data)

		.def("clear", &PyGNStream::clear)
		.def("set_stream_data", &PyGNStream::set_stream_data)
		.def("get_stream_data", &PyGNStream::get_stream_data)
		.def("copy", &PyGNStream::copy)
		.def("set_stream_data_indexed", &PyGNStream::set_stream_data_indexed)
		.def("get_stream_data_indexed", &PyGNStream::get_stream_data_indexed)
		.def("multiply_add_links", &PyGNStream::multiply_add_links)
		.def("add", &PyGNStream::add)
		.def("process_stream", &PyGNStream::process_stream)
		.def("lock", &PyGNStream::lock);

	py::class_<PyGNSystem>(m, "GNSystem")
		.def(py::init<>())
		.def("create_stream", &PyGNSystem::create_stream)
		.def("create_stream_data", &PyGNSystem::create_stream_data)
		.def("compute_node_count", &PyGNSystem::compute_node_count)
		.def("compute_in_out", &PyGNSystem::compute_in_out);
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
