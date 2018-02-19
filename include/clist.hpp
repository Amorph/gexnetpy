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
protected:
	T*		mList;
	size_t	mSize;
};

#endif
