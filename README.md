# local_ptr
Shared pointer with relaxed requirements:
- no multithreading;
- no weak_ptr;
- no type-erased deleter;

So pretty much just a reference counted unique_ptr, dumb but lightweight.
Also supports custom deleters (like unique_ptr, as a template parameter) and custom allocators for reference counter.

Usage: just include local_ptr.hpp

lptr::local_ptr<T> pointer1 = new T*;
auto pointer2 = lptr::make_local<T>(args); //works like std::make_shared
