#include "pch.h"
#pragma warning( disable:4251 )

#include "Core/IOState.h"
#include "Core/GateBase.h"
#include "Core/IOConnection.h"
#include "BasicGates/NOTGate.h"
#include "BasicGates/BufferGate.h"

namespace UnitTests
{
	using namespace Core;

	TEST(TestIOConnection, Create)
	{
		GatePtr gate1 = BasicGates::NOTGate::Create();
		GatePtr gate2 = BasicGates::NOTGate::Create();
		GatePtr buffer4 = BasicGates::BufferGate::Create(4);
		GatePtr buffer16 = BasicGates::BufferGate::Create(16);
		IOState status = IOState::HI;
		IOState statusWide = IOState(IOState::HI, 16);

		EXPECT_THROW(IOConnection(nullptr, nullptr), std::invalid_argument);
		EXPECT_THROW(IOConnection(gate1->GetPin("in"), nullptr), std::invalid_argument);
		EXPECT_THROW(IOConnection(status, nullptr), std::invalid_argument);
		EXPECT_THROW(IOConnection(statusWide, gate1->GetPin("in")), std::invalid_argument);
		EXPECT_THROW(IOConnection(statusWide, buffer4->GetPin("in")), std::invalid_argument);
		EXPECT_THROW(IOConnection(gate1->GetPin("in"), buffer16->GetPin("in")), std::invalid_argument);

		IOConnection c1(gate1->GetPin("in"), gate1->GetPin("out"));
		EXPECT_EQ(c1.GetSource(), gate1->GetPin("in"));
		EXPECT_EQ(c1.GetTarget(), gate1->GetPin("out"));
		EXPECT_EQ(IOState::UNDEF, c1.GetStatus());

		IOConnection c2(buffer16->GetPin("in"), buffer16->GetPin("out"));

		IOConnection c3(status, gate1->GetPin("in"));
		IOConnection c3b(status, gate1->GetPin("out"));
		EXPECT_EQ(IOState::HI, c3.GetStatus());
		IOConnection c4(statusWide, buffer16->GetPin("in"));

		EXPECT_FALSE(c3 < c1);
		EXPECT_TRUE(c1 < c3);
		EXPECT_TRUE((c3 < c3b) != (c3b < c3));
	}

	TEST(TestIOConnection, ToString)
	{
		GatePtr gate1 = BasicGates::NOTGate::Create();
		GatePtr gate2 = BasicGates::NOTGate::Create();
		GatePtr buffer4 = BasicGates::BufferGate::Create(4);
		GatePtr buffer16 = BasicGates::BufferGate::Create(16);
		IOState status = IOState::HI;
		IOState statusWide = IOState(IOState::HI, 16);

		IOConnection c1(gate1->GetPin("in"), gate1->GetPin("out"));
		IOConnection c1n(gate1->GetPin("in"), gate1->GetPin("out"), true);
		IOConnection c2(buffer16->GetPin("in"), buffer16->GetPin("out"));
		IOConnection c3(status, gate1->GetPin("in"));
		IOConnection c4(statusWide, buffer16->GetPin("in"));

		EXPECT_STREQ("not.in -> not.out", c1.ToString().c_str());
		EXPECT_STREQ("not.in -> /not.out", c1n.ToString().c_str());
		EXPECT_STREQ("buffer.in -> buffer.out", c2.ToString().c_str());
		EXPECT_STREQ("1 -> not.in", c3.ToString().c_str());
		EXPECT_STREQ("{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1} -> buffer.in", c4.ToString().c_str());
	}
}