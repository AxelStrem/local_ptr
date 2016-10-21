#ifndef _SHARED_LOCAL_PTR_
#define _SHARED_LOCAL_PTR_

#ifndef SHARED_LOCAL_PTR_GLOBAL
#define _LPTR_NAMESPACE_BEGIN namespace lptr{
#define _LPTR_NAMESPACE_END };
#else
#define _LPTR_NAMESPACE_BEGIN
#define _LPTR_NAMESPACE_END
#endif


#include <memory>

_LPTR_NAMESPACE_BEGIN

	template <class T> class DefaultDeleter
	{
	public:
		constexpr DefaultDeleter() noexcept = default;

		void operator()(T* ptr) const noexcept
		{
			static_assert(0 < sizeof(T),
				"can't delete an incomplete type");
			delete ptr;
		}
	};

	class ControlBlock
	{
		int refcount;
	public:
		ControlBlock() : refcount(1) {}
		void Increment()
		{
			refcount++;
		}

		int Decrement()
		{
			return --refcount;
		}

		int GetRefcount()
		{
			return refcount;
		}
	};


	template <class T> class DefaultCBAlloc
	{
	public:
		ControlBlock* Allocate(T* pObject)
		{
			return new ControlBlock();
		}
		void Deallocate(ControlBlock* pCB, T* pObject)
		{
			delete pCB;
		}
	};

	template <class T,
		class D = typename DefaultDeleter<T>,
	    class CBAlloc = typename DefaultCBAlloc<T>>
		class local_ptr
	{
	private:
		T* ptr;
		ControlBlock* pCB;

		void Release()
		{
			if (pCB->Decrement())
				return;
			
			CBAlloc cba;
			cba.Deallocate(pCB, ptr);

			D d;
			d(ptr);
		}

		local_ptr(nullptr_t) {}

	public:
		local_ptr(T* pObj) : ptr(pObj)
		{
			CBAlloc cbAlloc;
			pCB = cbAlloc.Allocate(ptr);
		}

		local_ptr(const local_ptr& src) : ptr(src.ptr), pCB(src.pCB)
		{
			pCB->Increment();
		}

		local_ptr& operator=(const local_ptr& src)
		{
			Release();

			ptr = src.ptr;
			pCB = src.pCB;
		}

		local_ptr(local_ptr&& src) : ptr(src.ptr), pCB(src.pCB)
		{}

		local_ptr& operator=(local_ptr&& src)
		{
			ptr = src.ptr;
			pCB = src.pCB;
		}

		T* get()
		{
			return ptr;
		}

		int refcount()
		{
			return pCB->GetRefcount();
		}

		~local_ptr()
		{
			Release();
		}
	};

template<class T, class... ArgT> local_ptr<T> make_local(ArgT&&... Args)
{
	T* ptr = new T(Args...);

	return local_ptr<T>(ptr);
}

_LPTR_NAMESPACE_END

#endif