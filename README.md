# local_ptr [![Build Status](https://travis-ci.org/AxelStrem/local_ptr.svg?branch=master)](https://travis-ci.org/AxelStrem/local_ptr)
Shared pointer with relaxed requirements:
- no thread-safe reference counting (by default);
- no weak_ptr;
- no type-erased deleter;
- no type-erased allocator;

So pretty much just a reference counted unique_ptr, dumb but lightweight.  
Mimics the behavior of **shared_ptr**.
Also supports custom allocators/deleters (like unique_ptr, as a template parameter) and custom allocators for reference counter.

Usage: just include local_ptr.hpp

    lptr::local_ptr<T> pointer1 = new T*;  
    auto pointer2 = lptr::make_local<T>(args); //works like std::make_shared,
                                               //but not intrusive; use make_intrusive instead

# intrusive_ptr
`local_ptr` specialization for intrusive reference counter: object and ref-counter are adjacent in memory and allocated with one `new` call.

    auto pointer3 = lptr::make_intrusive<T>(args); //works like std::make_shared
