#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>

#include "RouterUtil/Router.hpp"

class Router_Test_Class {};

/*
TEST_CASE_METHOD(Router_Test_Class, "Router::convertUrlToRegexForm behavior")
{

	SECTION("URL with no parameters")
	{
		std::string url = "/fixed/path";
		REQUIRE(Router::convertUrlToRegexForm(url) == "/fixed/path");
	}

	SECTION("URL with 1 parameter")
	{
		std::string url = "/:id/path";
		REQUIRE(Router::convertUrlToRegexForm(url) == "/([^/]+)/path");
	}

	SECTION("URL with parameter at the end")
	{
		std::string url = "/test/:id";
		REQUIRE(Router::convertUrlToRegexForm(url) == "/test/([^/]+)");
	}

	SECTION("URL with only parameter")
	{
		std::string url = "/:id";
		REQUIRE(Router::convertUrlToRegexForm(url) == "/([^/]+)");
	}

}*/
