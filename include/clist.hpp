#ifndef __GEX_NET_PY_CLIST_HPP__
#define __GEX_NET_PY_CLIST_HPP__

template<typename T, typename WrapType>
class PyCListIterator
{
public:
	PyCListIterator(T* list, size_t size)
		:mList(list)
		,mIndex(0)
		,mSize(size)
	{}
	PyCListIterator<T, WrapType> iter() { return PyCListIterator<T, WrapType>(mList, mSize); }
	WrapType next(){
		if (mIndex >= mSize)
			throw pybind11::stop_iteration();
		return WrapType(mList + mIndex++);
	}
protected:
	T*	   mList;
	size_t mIndex;
	size_t mSize;

};

template<typename T, typename WrapType>
class PyCList
{
public:
	typedef PyCListIterator<T, WrapType> Iterator;
	typedef PyCList<T, WrapType> ThisClass;
public:
	PyCList()
		: mList(nullptr)
		, mSize(0)
	{}

	PyCList(T* list, size_t size)
		:mList(list)
		,mSize(size)
	{}

	void set(T* list, size_t size)
	{
		mList = list;
		mSize = size;
	}

	size_t size() { return mSize; }
	WrapType get(size_t index) {
		return WrapType(mList + index);
	}
	Iterator iter() { return Iterator(mList, mSize); }

	std::string to_string()
	{
		std::ostringstream ss;
		ss << "PyList [";
		for (size_t i = 0; i < mSize; i++)
		{
			ss << get(i);
			if (i + 1 < mSize)
			{
				ss << ", ";
			}
		}
		ss << "]";
		return ss.str();
	}

protected:
	T*		mList;
	size_t	mSize;
};

template<typename T>
void bind_clist(pybind11::module &m, const std::string& pyName)
{
	pybind11::class_<T::Iterator>(m, (pyName + "Iterator").c_str())
		.def("__iter__", &T::Iterator::iter)
		.def("__next__", &T::Iterator::next);
	pybind11::class_<T>(m, pyName.c_str())
		.def("__len__", &T::size)
		.def("__getitem__", &T::get)
		.def("__iter__", &T::iter)
		.def("__repr__", &T::to_string);
}


#endif
