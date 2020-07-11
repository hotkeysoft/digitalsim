#pragma once
#include "pch.h"
#include "Core/GateBase.h"
#include "Core/CompositeGate.h"
#include "Tools\LogicTools.h"

using namespace DigiLib;

namespace UnitTests
{
	using namespace DigiLib::Core;
	
	GatePtr BuildDecoder();
	GatePtr BuildFullAdder();
	GatePtr Build4BitAdder();
	GatePtr Build74163Counter();
}

static constexpr const char* COLOR_RESET = "\033[m";
static constexpr const char* COLOR_RED = "\033[0;31m";
static constexpr const char* COLOR_GREEN = "\033[0;32m";
static constexpr const char* COLOR_YELLOW = "\033[0;33m";

#define PRINTF(...) do { printf("%s[          ] %s", COLOR_GREEN, COLOR_YELLOW); printf(__VA_ARGS__); printf(COLOR_RESET); } while (0)

// C++ stream interface
class TestCout : public std::stringstream
{
public:
	~TestCout()
	{
		PRINTF("%s\n", str().c_str());
	}
};

#define TEST_COUT  TestCout()
