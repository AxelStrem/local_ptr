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

	template <class T> using DefaultAllocator = std::allocator<T>;

	template <class T, class Alloc> class DefaultDeleter
	{
	public:
		constexpr DefaultDeleter() noexcept = default;

		void operator()(T* ptr) const noexcept
		{
			static_assert(0 < sizeof(T),
				"can't delete an incomplete type");
			Alloc a;
			a.destroy(ptr);
			a.deallocate(ptr, 1);
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

	template <class T,
		template <typename> class Alloc = DefaultAllocator,
		class D = typename DefaultDeleter<T, Alloc<T>>,
		template <typename> class CBAlloc = Alloc>
		class local_ptr
	{
	private:
		T* ptr;
		ControlBlock* pCB;

		void Release()
		{
			if (pCB->Decrement())
				return;
			D d;
			d(ptr);
			CBAlloc<ControlBlock> cbAlloc;
			cbAlloc.destroy(pCB);
			cbAlloc.deallocate(pCB, 1);
		}

		local_ptr(nullptr_t) {}

		template<class... ArgT> void ConstructFromArgs(ArgT&&... Args)
		{
			Alloc<T> alloc;
			ptr = alloc.allocate(1);
			alloc.construct(ptr, Args);

			CBAlloc<ControlBlock> cbAlloc;
			pCB = cbAlloc.allocate(1);
			cbAlloc.construct(pCB);
		}

	public:
		local_ptr(T* pObj) : ptr(pObj)
		{
			CBAlloc<ControlBlock> cbAlloc;
			pCB = cbAlloc.allocate(1);
			cbAlloc.construct(pCB);
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

_LPTR_NAMESPACE_END

#endif