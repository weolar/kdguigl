
#ifndef ThreadSpecific_h
#define ThreadSpecific_h

#define DEFINE_THREAD_LOCAL(type, name, arguments)  \
	WTF::mainThreadMutex().lock();                  \
	static WTF::ThreadSpecific<type> type_##name;   \
	WTF::mainThreadMutex().unlock();                \
	if (!type_##name.hasInit())                     \
		{ type_##name = new type arguments; }       \
	type& name = *(type_##name);  

namespace WTF {

void* fastMalloc(size_t);
void fastFree(void*);

long& ThreadSpecTlsKeyCount();
void ThreadSpecificInitTls();
void ThreadSpecificThreadExit();

BOOL WinTlshasInit( __in DWORD dwTlsIndex );
LPVOID WinTlsTryGetValue( __in DWORD dwTlsIndex );

// TLS_OUT_OF_INDEXES is not defined on WinCE.
#ifndef TLS_OUT_OF_INDEXES
#define TLS_OUT_OF_INDEXES 0xffffffff
#endif

// The maximum number of TLS keys that can be created. For simplification, we assume that:
// 1) Once the instance of ThreadSpecific<> is created, it will not be destructed until the program dies.
// 2) We do not need to hold many instances of ThreadSpecific<> data. This fixed number should be far enough.
const int kMaxTlsKeySize = 0x1000;

LPVOID WinTlsGetValue(__in DWORD dwTlsIndex);
BOOL WinTlsSetValue(__in DWORD dwTlsIndex, __in_opt LPVOID lpTlsValue, BOOL bIsPtr);
BOOL WinTlsSetValueNone(__in DWORD dwTlsIndex, BOOL bHasInit, BOOL bIsPtr);
DWORD WinTlsAlloc();

template<typename T> class ThreadSpecific {
	//WTF_MAKE_NONCOPYABLE(ThreadSpecific);
public:
	ThreadSpecific();
	T* operator->();
	operator T*();
	T& operator*();
	void operator=(T* pData);

	// Not implemented. It's technically possible to destroy a thread specific key, but one would need
	// to make sure that all values have been destroyed already (usually, that all threads that used it
	// have exited). It's unlikely that any user of this call will be in that situation - and having
	// a destructor defined can be confusing, given that it has such strong pre-requisites to work correctly.
	~ThreadSpecific() {
	}

	struct Data {
		//WTF_MAKE_NONCOPYABLE(Data);
	public:
		Data(T* value, ThreadSpecific<T>* owner) : value(value), owner(owner) {}

		T* value;
		ThreadSpecific<T>* owner;

		void (*destructor)(void*);
	};

	T* get() {
		Data* data = static_cast<Data*>(WinTlsGetValue(m_index));
		return data ? data->value : 0;
	}

	void set(T* ptr) {
		//ASSERT(!get());
#ifdef _DEBUG
		if (WinTlshasInit(m_index))
			*(int*)0 = 0;
#endif
		Data* data = new Data(ptr, this);
		data->destructor = &ThreadSpecific<T>::destroy;
		WinTlsSetValue(m_index, data, TRUE);
	}

	void forceDestroy(bool bHasInit, bool bNeedDelete) {
		WinTlsSetValueNone(m_index, bNeedDelete, TRUE);
		// 由于本类都是静态全局变量，所以一般不需要delete
		if (bNeedDelete)
			delete this;
		
	}

	static void destroy(void* ptr) {
		ThreadSpecific<T>::Data* data = (ThreadSpecific<T>::Data*)ptr;
		delete data->value;
	}

	bool hasInit() { return !!WinTlshasInit(m_index); }
	int m_index;
};

template<typename T>
inline ThreadSpecific<T>::ThreadSpecific()
	: m_index(-1)
{
	m_index = WinTlsAlloc();
	if (m_index == TLS_OUT_OF_INDEXES || m_index >= kMaxTlsKeySize) {
		*(int*)0 = 0;}
}

template<typename T>
class ThreadSpecificVal {
	//WTF_MAKE_NONCOPYABLE(ThreadSpecificVal);
public:
	ThreadSpecificVal()
		: m_index(-1)
	{
		m_index = WinTlsAlloc();
		if (m_index == TLS_OUT_OF_INDEXES || m_index >= kMaxTlsKeySize) {
			*(int*)0 = 0;}
		setNoDelete(0);
	}

	inline T operator ! ()
	{ return !((T)get()); }

	inline void operator = (T data)
	{ set(data); }

	struct Data {
		//WTF_MAKE_NONCOPYABLE(Data);
	public:
		Data(T value, ThreadSpecificVal<T>* const owner) : value(value), owner(owner) {}
		~Data() { /*owner->destroy(this);*/ }

		T value;
		ThreadSpecificVal<T>* owner;

		void (*destructor)(void*);
	};

	static void destroy(void* ptr)
	{
		T* self = (T*)ptr;
		if (!WinTlshasInit(self->m_index))
			DebugBreak();
		WinTlsSetValue(self->m_index, 0, TRUE);
	}

	T get()
	{
		if (!WinTlshasInit(m_index))
			return 0;
		Data* data = static_cast<Data*>(WinTlsGetValue(m_index));
		//return data ? data->value : false;
		//ASSERT(data);
		return data->value;
	}

	void set(T value)
	{
		Data* data = (Data*)WinTlsTryGetValue(m_index);
		if (data) {
			data->Data::~Data();
			fastFree(data);
		}
		data = (Data*)fastMalloc(sizeof(Data));
		//data->Data::Data(value, this); // GCC不允许直接调用构造
		new (data) Data(value, this);
		//data->destructor = &ThreadSpecificVal<T>::destroy;
		WinTlsSetValue(m_index, data, FALSE);
	}

	void setNoDelete(T value)
	{
		//Data* data = new Data(value, this);
		Data* data = (Data*)fastMalloc(sizeof(Data));
		data->Data::Data(value, this);
		//data->destructor = &ThreadSpecificVal<T>::destroy;
		WinTlsSetValue(m_index, data, FALSE);
	}

private:
	DWORD m_index;
};

template<typename T>
inline bool operator == (ThreadSpecificVal<T>& a, T b)
{ return a.get() == b; }

template<typename T>
inline bool operator != (T b, ThreadSpecificVal<T>& a)
{ return a.get() != b; }

inline void operator ++ (ThreadSpecificVal<int>& a)
{ a.set(a.get() + 1); }

inline void operator -- (ThreadSpecificVal<int>& a)
{ a.set(a.get() - 1); }

template<typename T>
inline ThreadSpecific<T>::operator T*()
{
	T* ptr = static_cast<T*>(get());
	//     if (!ptr) {
	//         // Set up thread-specific value's memory pointer before invoking constructor, in case any function it calls
	//         // needs to access the value, to avoid recursion.
	//         ptr = static_cast<T*>(fastZeroedMalloc(sizeof(T)));
	//         set(ptr);
	//         new (ptr) T;
	//     }
	return ptr;
}

template<typename T>
inline T* ThreadSpecific<T>::operator->()
{
	return operator T*();
}

template<typename T>
inline T& ThreadSpecific<T>::operator*()
{
	return *operator T*();
}

template<typename T>
inline void ThreadSpecific<T>::operator=(T* ptr)
{ 
	set(ptr); 
}

} // WTF

#endif // ThreadSpecific_h