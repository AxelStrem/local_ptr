#ifndef _SHARED_LOCAL_PTR_
#define _SHARED_LOCAL_PTR_

#ifndef SHARED_LOCAL_PTR_GLOBAL
#define _LPTR_NAMESPACE_BEGIN namespace lptr{
#define _LPTR_NAMESPACE_END };
#else
#define _LPTR_NAMESPACE_BEGIN
#define _LPTR_NAMESPACE_END
#endif

#include<utility>

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


	template <class T> class DefaultRefCounter
	{
		ControlBlock* pCB;
	public:
		DefaultRefCounter() {}
		DefaultRefCounter(const DefaultRefCounter& src) : pCB(src.pCB) {}

		void Allocate(T* pObject)
		{
			pCB = new ControlBlock();
		}
		void Deallocate(T* pObject)
		{
			delete pCB;
		}

		void Increment(T* pObject)
		{
			return pCB->Increment();
		}

		int Decrement(T* pObject)
		{
			return pCB->Decrement();
		}

		int GetRefcount(T* pObject)
		{
			return pCB->GetRefcount();
		}

		T* ForwardObject(T* pObject)
		{
			return pObject;
		}

		T* RevertObject(T* pObject)
		{
			return pObject;
		}
	};

	template <class T,
		class D = typename DefaultDeleter<T>,
	    class RefCounter = typename DefaultRefCounter<T>>
		class local_ptr
	{
	private:
		T* ptr;
		RefCounter mRC;

		void Release()
		{
			if (mRC.Decrement(ptr))
				return;
			
			mRC.Deallocate(ptr);

			D d;
			d(ptr);
		}

		local_ptr(nullptr_t) {}

	public:
		typedef decltype(mRC.ForwardObject(ptr)) element_pointer;
		typedef typename std::remove_pointer<element_pointer>::type element_type;

		local_ptr(T* pObj) : ptr(pObj)
		{
			mRC.Allocate(ptr);
		}


		local_ptr(const local_ptr& src) : ptr(src.ptr), mRC(src.mRC)
		{
			mRC.Increment(ptr);
		}

		local_ptr& operator=(const local_ptr& src)
		{
			Release();

			ptr = src.ptr;
			mRC = src.pCB;

			return *this;
		}

		local_ptr(local_ptr&& src) : ptr(src.ptr), pCB(src.mRC)
		{}

		void swap(local_ptr& src)
		{
			std::swap(ptr, src.ptr);
			std::swap(mRC, src.mRC);
		}

		template<class P> void reset(P* n_ptr)
		{
			T* t_ptr = mRC.RevertObject(n_ptr);
			local_ptr(n_ptr).swap(*this);
		}

		void reset()
		{
			reset(nullptr);
		}

		local_ptr& operator=(local_ptr&& src)
		{
			swap(src);

			return *this;
		}

		element_type* get()
		{
			return mRC.ForwardObject(ptr);
		}

		element_type& operator*()
		{
			return (*get());
		}

		element_type* operator->()
		{
			return get();
		}

		int use_count()
		{
			return mRC.GetRefcount(ptr);
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

template<class T> class intrusiveCB
{
public:
	ControlBlock mCB;
	T mObject;
	template<class... ArgT> intrusiveCB(ArgT&&... Args) : mObject(std::forward<ArgT...>(Args...)) {}
};

template<class T> class IntrusiveRefCounter
{
public:
	void Allocate(intrusiveCB<T>* pObject)
	{}
	void Deallocate(intrusiveCB<T>* pObject)
	{}

	void Increment(intrusiveCB<T>* pObject)
	{
		return pObject->mCB.Increment();
	}

	int Decrement(intrusiveCB<T>* pObject)
	{
		return pObject->mCB.Decrement();
	}

	int GetRefcount(intrusiveCB<T>* pObject)
	{
		return pObject->mCB.GetRefcount();
	}

	T* ForwardObject(intrusiveCB<T>* pObject)
	{
		return &(pObject->mObject);
	}
};

template<class T> using intrusive_ptr = local_ptr<intrusiveCB<T>, DefaultDeleter<intrusiveCB<T>>, IntrusiveRefCounter<T>>;

template<class T, class... ArgT> intrusive_ptr<T> make_intrusive(ArgT&&... Args)
{
	intrusiveCB<T>* ptr = new intrusiveCB<T>(Args...);
	return intrusive_ptr<T>(ptr);
}

_LPTR_NAMESPACE_END

#endif