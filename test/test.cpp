#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <iostream>
#include "local_ptr.hpp"

using namespace lptr;

TEST_CASE("basic tests", "[file]")
{
	SECTION("make_local")
	{
		auto p = make_local<int>(10);
		std::cout<<"p = "<<(*p)<<std::endl;

		REQUIRE(1 == 1);
	}
}