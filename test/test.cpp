#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <iostream>
#include "local_ptr.hpp"

using namespace lptr;

class TestIntAllocator
{
	static int value;
public:
	static char* allocate(size_t size)
	{
		return (char*)(&value);
	}
	static void free(char* ptr)
	{

	}
};

int TestIntAllocator::value = 0;

static int TestIntAllocator::value = 0;

TEST_CASE("basic tests", "[file]")
{
	SECTION("make_local")
	{
		auto p = make_local<int>(10);
		int a = (*p);
		int* ip = p.get();
		auto p1 = p;
		auto p2 = p;

		REQUIRE(p.use_count() == 3);
	}
	
	SECTION("allocate_local")
	{
		auto p = allocate_local<int, TestIntAllocator>(5);
		int a = (*p);
		int* ip = p.get();
		auto p1 = p;

		REQUIRE(p.unique() != true);
	}
	
	SECTION("make_intrusive")
	{
		auto p = make_intrusive<int>(10);
		int a = (*p);
		int* ip = p.get();
		
		{
			auto p2 = p;
		}

		REQUIRE(p.unique() == true);
	}
}