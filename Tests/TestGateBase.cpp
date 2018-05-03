#include "pch.h"
#pragma warning( disable:4251 )

#include "TestCommon.h"
#include "Core\GateBase.h"
#include "Core\CompositeGate.h"
#include "BasicGates\NOTGate.h"
#include "BasicGates\ANDGate.h"
#include "BasicGates\NANDGate.h"
#include "BasicGates\ORGate.h"
#include "BasicGates\XORGate.h"
#include "BasicGates\WireGate.h"
#include "BasicGates\BufferGate.h"
#include "Tools\LogicTools.h"

namespace UnitTests
{		
	using namespace Core;

	CompositeGatePtr NestGate(const char* name, GatePtr toNest)
	{
		CompositeGatePtr comp = CompositeGate::Create(name);
		IOPinPtr pin = comp->AddInput("in");
		IOPinPtr pout = comp->AddOutput("out");
		GatePtr notGate = BasicGates::NOTGate::Create();
		comp->AddGate("not", notGate);
		comp->AddGate("inner", toNest);

		IOPinPtr in2 = comp->GetPin("in");
		IOPinPtr notin2 = notGate->GetPin("in");

		comp->GetPin("in")->ConnectTo(notGate->GetPin("in"));
		toNest->GetPin("out")->ConnectTo(comp->GetPin("out"));
		notGate->GetPin("out")->ConnectTo(toNest->GetPin("in"));

		return comp;
	}

	TEST(TestCore, TestAddInput)
	{
		CompositeGatePtr gate = CompositeGate::Create("test");

		//Logger::WriteMessage("TestAddInput: Bad input");
		EXPECT_THROW(gate->AddInput(NULL), std::invalid_argument);
		EXPECT_THROW(gate->AddInput(""), std::invalid_argument);
		EXPECT_THROW(gate->AddInput(" "), std::invalid_argument);
		EXPECT_THROW(gate->AddInput("3"), std::invalid_argument);
		EXPECT_THROW(gate->AddInput("3d"), std::invalid_argument);
		EXPECT_THROW(gate->AddInput("a b"), std::invalid_argument);
		EXPECT_THROW(gate->AddInput("a "), std::invalid_argument);
		EXPECT_THROW(gate->AddInput(" a"), std::invalid_argument);
		EXPECT_THROW(gate->AddInput("a23456789012345678901234567890123"), std::invalid_argument);
		gate->AddInput("a2345678901234567890123456789012");
		ASSERT_EQ(1, (int)gate->GetInputCount());

		//Logger::WriteMessage("TestAddInput: Good Input");
		gate->AddInput("a");
		ASSERT_EQ(2, (int)gate->GetInputCount());

		gate->AddInput("a12345");
		ASSERT_EQ(3, (int)gate->GetInputCount());
		ASSERT_EQ(0, (int)gate->GetOutputCount());

		ASSERT_NE(nullptr, gate->GetPin("a"));
		ASSERT_NE(nullptr, gate->GetPin("a12345"));
			
		//Logger::WriteMessage("TestAddInput: Duplicate");
		EXPECT_THROW(gate->AddInput("a"), std::invalid_argument);
		EXPECT_THROW(gate->AddOutput("a"), std::invalid_argument);

		ASSERT_EQ(3, (int)gate->GetInputCount());
		ASSERT_EQ(0, (int)gate->GetOutputCount());

		EXPECT_THROW(gate->AddInput("width", 0), std::out_of_range);
		EXPECT_THROW(gate->AddInput("width", -1), std::out_of_range);
		EXPECT_THROW(gate->AddInput("width", 17), std::out_of_range);

		ASSERT_NE(nullptr, gate->AddInput("width", 16));
	}

	TEST(TestCore, TestAddOutput)
	{
		CompositeGatePtr gate = CompositeGate::Create("test");

		//Logger::WriteMessage("TestAddOutput: Bad Input");
		EXPECT_THROW(gate->AddOutput(NULL), std::invalid_argument);
		EXPECT_THROW(gate->AddOutput(""), std::invalid_argument);
		EXPECT_THROW(gate->AddOutput(" "), std::invalid_argument);
		EXPECT_THROW(gate->AddOutput("3"), std::invalid_argument);
		EXPECT_THROW(gate->AddOutput("3d"), std::invalid_argument);
		EXPECT_THROW(gate->AddOutput("a b"), std::invalid_argument);
		EXPECT_THROW(gate->AddOutput("a "), std::invalid_argument);
		EXPECT_THROW(gate->AddOutput(" a"), std::invalid_argument);
		EXPECT_THROW(gate->AddOutput("a23456789012345678901234567890123"), std::invalid_argument);
		gate->AddOutput("a2345678901234567890123456789012");
		ASSERT_EQ(1, (int)gate->GetOutputCount());
		
		//Logger::WriteMessage("TestAddOutput: Good Input");
		gate->AddOutput("a");
		ASSERT_EQ(2, (int)gate->GetOutputCount());

		gate->AddOutput("a12345");
		ASSERT_EQ(3, (int)gate->GetOutputCount());
		ASSERT_EQ(0, (int)gate->GetInputCount());

		ASSERT_NE(nullptr, gate->GetPin("a"));
		ASSERT_NE(nullptr, gate->GetPin("a12345"));

		//Logger::WriteMessage("TestAddOutput: Duplicate");
		EXPECT_THROW(gate->AddOutput("a"), std::invalid_argument);
		EXPECT_THROW(gate->AddInput("a"), std::invalid_argument);

		ASSERT_EQ(3, (int)gate->GetOutputCount());
		ASSERT_EQ(0, (int)gate->GetInputCount());

		EXPECT_THROW(gate->AddOutput("width", 0), std::out_of_range);
		EXPECT_THROW(gate->AddOutput("width", -1), std::out_of_range);
		EXPECT_THROW(gate->AddOutput("width", 17), std::out_of_range);

		EXPECT_THROW(gate->AddOutput("width", 1, IOPin::IO_DIRECTION::INPUT), std::invalid_argument);

		ASSERT_NE(nullptr, gate->AddOutput("width", 16));
	}

	TEST(TestCore, TestGetPin)
	{
		GatePtr gate = BasicGates::ANDGate::Create();

		EXPECT_THROW(gate->GetPin(nullptr), std::invalid_argument);
		ASSERT_EQ(nullptr, gate->GetPin(""));
		ASSERT_EQ(nullptr, gate->GetPin(" "));
		ASSERT_EQ(nullptr, gate->GetPin(" in1"));
		ASSERT_EQ(nullptr, gate->GetPin("in1 "));
		ASSERT_EQ(nullptr, gate->GetPin("IN1"));

		ASSERT_NE(nullptr, gate->GetPin("in1"));

		ASSERT_NE(gate->GetPin("in1"), gate->GetPin("in2"));
		ASSERT_NE(gate->GetPin("in1"), gate->GetPin("out"));
		ASSERT_EQ(gate->GetPin("in1"), gate->GetPin("in1"));
		ASSERT_EQ(gate->GetPin("in2"), gate->GetPin("in2"));
		ASSERT_EQ(gate->GetPin("out"), gate->GetPin("out"));
	}

	TEST(TestCore, TestConnectTo)
	{
		CompositeGatePtr gate = CompositeGate::Create("test");
		gate->AddInput("in");
		gate->AddOutput("out");

		CompositeGatePtr gate2 = CompositeGate::Create("test2");
		gate2->AddInput("in");
		gate2->AddOutput("out");

		//Logger::WriteMessage("TestConnectTo: Connect to nothing");
		EXPECT_THROW(gate->GetPin("out")->ConnectTo(nullptr), std::invalid_argument);

		//Logger::WriteMessage("TestConnectTo: Connect to self (out to in)");
		EXPECT_THROW(gate->GetPin("out")->ConnectTo(gate->GetPin("in")), std::invalid_argument);

		//Logger::WriteMessage("TestConnectTo: Connect to self (in to in)");
		EXPECT_THROW(gate->GetPin("in")->ConnectTo(gate->GetPin("in")), std::invalid_argument);

		//Logger::WriteMessage("TestConnectTo: Connect to self (out to out)");
		EXPECT_THROW(gate->GetPin("out")->ConnectTo(gate->GetPin("out")), std::invalid_argument);

		//Logger::WriteMessage("TestConnectTo: Connect to self (in to out)");
		EXPECT_THROW(gate->GetPin("in")->ConnectTo(gate->GetPin("out")), std::invalid_argument);

		//Logger::WriteMessage("TestConnectTo: Duplicate");
		gate->GetPin("out")->ConnectTo(gate2->GetPin("in"));
		EXPECT_THROW(gate->GetPin("out")->ConnectTo(gate2->GetPin("in")), std::invalid_argument);
	}

	TEST(TestCore, DISABLED_TestFeedback)
	{
		GatePtr gate = BasicGates::NOTGate::Create();
		GatePtr gate2 = BasicGates::NOTGate::Create();
			
		//Logger::WriteMessage("TestFeedback: gate->gate2->gate");
		gate->GetPin("out")->ConnectTo(gate2->GetPin("in"));
		gate2->GetPin("out")->ConnectTo(gate->GetPin("in"));

//			gate->GetPin("in")->Set(IOState::LOW);
		FAIL();
	}

	TEST(TestCore, TestSetName)
	{
		GatePtr notGate = BasicGates::NOTGate::Create();
		ASSERT_STREQ("not", notGate->GetName().c_str());

		EXPECT_THROW(notGate->SetName(NULL), std::invalid_argument);
		EXPECT_THROW(notGate->SetName(""), std::invalid_argument);
		EXPECT_THROW(notGate->SetName(" "), std::invalid_argument);
		EXPECT_THROW(notGate->SetName("3"), std::invalid_argument);
		EXPECT_THROW(notGate->SetName("3d"), std::invalid_argument);
		EXPECT_THROW(notGate->SetName("a b"), std::invalid_argument);
		EXPECT_THROW(notGate->SetName("a "), std::invalid_argument);
		EXPECT_THROW(notGate->SetName(" a"), std::invalid_argument);
		EXPECT_THROW(notGate->SetName("a23456789012345678901234567890123"), std::invalid_argument);
		notGate->SetName("a2345678901234567890123456789012");

		ASSERT_STREQ("a2345678901234567890123456789012", notGate->GetName().c_str());
	}

	TEST(TestCore, TestAddGate)
	{
		CompositeGatePtr gate = CompositeGate::Create("test");
		GatePtr dummyGate = BasicGates::NOTGate::Create();
		GatePtr notGate = BasicGates::NOTGate::Create();
		GatePtr notGate2 = BasicGates::NOTGate::Create();

		ASSERT_EQ(0, (int)gate->GetGateCount());
		ASSERT_EQ(0, gate->GetInternalGates().size());

		//Logger::WriteMessage("TestAddGate: Bad Input");
		EXPECT_THROW(gate->AddGate(NULL, dummyGate), std::invalid_argument);
		EXPECT_THROW(gate->AddGate("", dummyGate), std::invalid_argument);
		EXPECT_THROW(gate->AddGate(" ", dummyGate), std::invalid_argument);
		EXPECT_THROW(gate->AddGate("3", dummyGate), std::invalid_argument);
		EXPECT_THROW(gate->AddGate("3d", dummyGate), std::invalid_argument);
		EXPECT_THROW(gate->AddGate("a b", dummyGate), std::invalid_argument);
		EXPECT_THROW(gate->AddGate("a ", dummyGate), std::invalid_argument);
		EXPECT_THROW(gate->AddGate(" a", dummyGate), std::invalid_argument);
		EXPECT_THROW(gate->AddGate("a23456789012345678901234567890123", dummyGate), std::invalid_argument);
		gate->AddGate("a2345678901234567890123456789012", dummyGate);
		ASSERT_EQ(1, (int)gate->GetGateCount());
				
		gate->AddGate("not", notGate);
		ASSERT_EQ(2, (int)gate->GetGateCount());
		ASSERT_EQ(2, gate->GetInternalGates().size());

		//Logger::WriteMessage("TestAddGate: null gate");
		EXPECT_THROW(gate->AddGate("null", nullptr), std::invalid_argument);

		//Logger::WriteMessage("TestAddGate: Duplicate name");
		EXPECT_THROW(gate->AddGate("not", notGate2), std::invalid_argument);

		ASSERT_EQ(2, (int)gate->GetGateCount());

		//Logger::WriteMessage("TestAddGate: Duplicate gate (TODO: Clone gate so this is possible)");
		EXPECT_THROW(gate->AddGate("not2", notGate), std::invalid_argument);

		ASSERT_EQ(2, (int)gate->GetGateCount());

		//Logger::WriteMessage("TestAddGate: Self");
		EXPECT_THROW(gate->AddGate("not2", gate), std::invalid_argument);
	}
	
	TEST(TestCore, TestGetGate)
	{
		CompositeGatePtr gate = CompositeGate::Create("test");
		GatePtr notGate = BasicGates::NOTGate::Create();
		ASSERT_EQ(nullptr, notGate->GetParent());

		gate->AddGate("not", notGate);
		ASSERT_EQ(1, (int)gate->GetGateCount());
		ASSERT_EQ(1, gate->GetInternalGates().size());
		
		EXPECT_THROW(gate->GetGate(nullptr), std::invalid_argument);
		ASSERT_EQ(nullptr, gate->GetGate(""));
		ASSERT_EQ(nullptr, gate->GetGate(" "));
		ASSERT_EQ(nullptr, gate->GetGate(" ba"));
		ASSERT_EQ(nullptr, gate->GetGate(" not"));
		GatePtr child = gate->GetGate("not");
		ASSERT_EQ(notGate, child);
		ASSERT_STREQ("not", child->GetName().c_str());
		ASSERT_EQ(gate, child->GetParent());
	}

	TEST(TestCore, TestComponentToTopLevel)
	{
		//Logger::WriteMessage("TestComponentToTopLevel: Outside->Outside");
		CompositeGatePtr gate = CompositeGate::Create("test");;
		gate->AddOutput("out");
		CompositeGatePtr gate2 = CompositeGate::Create("test2");
		gate2->AddInput("in");

		gate->GetPin("out")->ConnectTo(gate2->GetPin("in"));
	}

	TEST(TestCore, TestInnerToComponent)
	{
		CompositeGatePtr gate = CompositeGate::Create("test");
		gate->AddInput("in");
		gate->AddOutput("out");
		GatePtr not1 = BasicGates::NOTGate::Create();
		GatePtr not2 = BasicGates::NOTGate::Create();
		GatePtr not3 = BasicGates::NOTGate::Create();
		GatePtr not4 = BasicGates::NOTGate::Create();
		GatePtr not5 = BasicGates::NOTGate::Create();
		gate->AddGate("not1", not1);
		gate->AddGate("not2", not2);
		gate->AddGate("not3", not3);
		gate->AddGate("not4", not4);
		gate->AddGate("not5", not5);
		ASSERT_EQ(5, (int)gate->GetGateCount());

		//Logger::WriteMessage("TestInnerToComponent: Inside->Inside");
		not1->GetPin("out")->ConnectTo(not2->GetPin("in"));
		ASSERT_EQ(IOState::UNDEF, not2->GetPin("out")->Get());
		not1->GetPin("in")->Set(IOState::HI);
		ASSERT_EQ(IOState::HI, not2->GetPin("out")->Get());
		not1->GetPin("in")->Set(IOState::LOW);
		ASSERT_EQ(IOState::LOW, not2->GetPin("out")->Get());

		// Outside gate
		GatePtr outsideNot = BasicGates::NOTGate::Create();

		//Logger::WriteMessage("TestInnerToComponent: Inside->Input pin (not allowed)");			
		EXPECT_THROW(not3->GetPin("out")->ConnectTo(gate->GetPin("in")), std::invalid_argument);

		//Logger::WriteMessage("TestInnerToComponent: Inside->Output pin (ok)");
		gate->GetPin("in")->ConnectTo(not4->GetPin("in"));

		//Logger::WriteMessage("TestInnerToComponent: Input pin->Inside (ok)");
		not4->GetPin("out")->ConnectTo(gate->GetPin("out"));

		//Logger::WriteMessage("TestInnerToComponent: Validate inner gate working");
		gate->GetPin("in")->Set(IOState::HI);
		ASSERT_EQ(IOState::LOW, gate->GetPin("out")->Get());
		gate->GetPin("in")->Set(IOState::LOW);
		ASSERT_EQ(IOState::HI, gate->GetPin("out")->Get());

		//Logger::WriteMessage("TestInnerToComponent: Output pin->inside (not allowed)");
		EXPECT_THROW(gate->GetPin("out")->ConnectTo(not5->GetPin("in")), std::invalid_argument);

		//Logger::WriteMessage("TestInnerToComponent: Inside->Outside gate (not allowed)");
		EXPECT_THROW(not5->GetPin("out")->ConnectTo(outsideNot->GetPin("in")), std::invalid_argument);

		//Logger::WriteMessage("TestInnerToComponent: Outside gate->Inside (not allowed)");
		EXPECT_THROW(outsideNot->GetPin("out")->ConnectTo(not5->GetPin("in")), std::invalid_argument);
		
		GatePtr not6 = BasicGates::NOTGate::Create();
		//Logger::WriteMessage("TestInnerComponentToComponent: Outside gate->Outside pin (ok)");
		not6->GetPin("out")->ConnectTo(gate->GetPin("in"));
		//Logger::WriteMessage("TestInnerComponentToComponent: Validate Outside gate->Outside pin");
		not6->GetPin("in")->Set(IOState::HI);
		ASSERT_EQ(IOState::HI, gate->GetPin("out")->Get());
		not6->GetPin("in")->Set(IOState::LOW);
		ASSERT_EQ(IOState::LOW, gate->GetPin("out")->Get());

		GatePtr not7 = BasicGates::NOTGate::Create();
		//Logger::WriteMessage("TestInnerComponentToComponent: Outside pin->Outside gate (ok)");
		gate->GetPin("out")->ConnectTo(not7->GetPin("in"));
		//Logger::WriteMessage("TestInnerComponentToComponent: Validate Outside pin->Outside gate");
		gate->GetPin("in")->Set(IOState::HI);
		ASSERT_EQ(IOState::HI, not7->GetPin("out")->Get());
		gate->GetPin("in")->Set(IOState::LOW);
		ASSERT_EQ(IOState::LOW, not7->GetPin("out")->Get());
	}

	TEST(TestCore, TestFullName)
	{
		GatePtr notGate = BasicGates::NOTGate::Create();

		ASSERT_STREQ("not", notGate->GetName().c_str());
		ASSERT_STREQ("not", notGate->GetFullName().c_str());
		ASSERT_STREQ("in", notGate->GetPin("in")->GetName().c_str());
		ASSERT_STREQ("not.in", notGate->GetPin("in")->GetFullName().c_str());

		CompositeGatePtr nested1 = NestGate("nest1", notGate);
		ASSERT_NE(nullptr, nested1);
		ASSERT_STREQ("inner", notGate->GetName().c_str());
		ASSERT_STREQ("nest1.inner", notGate->GetFullName().c_str());
		ASSERT_STREQ("in", notGate->GetPin("in")->GetName().c_str());
		ASSERT_STREQ("nest1.inner.in", notGate->GetPin("in")->GetFullName().c_str());

		CompositeGatePtr nested2 = NestGate("nest2", nested1);
		ASSERT_NE(nullptr, nested1);
		ASSERT_STREQ("inner", notGate->GetName().c_str());
		ASSERT_STREQ("nest2.inner.inner", notGate->GetFullName().c_str());
		ASSERT_STREQ("in", notGate->GetPin("in")->GetName().c_str());
		ASSERT_STREQ("nest2.inner.inner.in", notGate->GetPin("in")->GetFullName().c_str());
		
		CompositeGatePtr nested3 = NestGate("nest3", nested2);
		ASSERT_NE(nullptr, nested1);
		ASSERT_STREQ("inner", notGate->GetName().c_str());
		ASSERT_STREQ("nest3.inner.inner.inner", notGate->GetFullName().c_str());
		ASSERT_STREQ("in", notGate->GetPin("in")->GetName().c_str());
		ASSERT_STREQ("nest3.inner.inner.inner.in", notGate->GetPin("in")->GetFullName().c_str());
	}

	void AssertEqualOutputs(GatePtr g1, GatePtr g2)
	{
		ASSERT_EQ(g1->GetPin("Y0")->Get(), g2->GetPin("Y0")->Get());
		ASSERT_EQ(g1->GetPin("Y1")->Get(), g2->GetPin("Y1")->Get());
		ASSERT_EQ(g1->GetPin("Y2")->Get(), g2->GetPin("Y2")->Get());
		ASSERT_EQ(g1->GetPin("Y3")->Get(), g2->GetPin("Y3")->Get());
	}

	TEST(TestCore, TestClone)
	{
		CompositeGatePtr base = std::dynamic_pointer_cast<CompositeGate>(BuildDecoder());
		GatePtr clonedBase = base->Clone("clone");
		CompositeGatePtr cloned = std::dynamic_pointer_cast<CompositeGate>(clonedBase);

		ASSERT_NE(nullptr, cloned);
		ASSERT_NE(base, cloned);

		ASSERT_EQ(std::string("clone"), cloned->GetName());
		ASSERT_EQ(base->GetInputCount(), cloned->GetInputCount());
		ASSERT_EQ(base->GetOutputCount(), cloned->GetOutputCount());
		ASSERT_EQ(base->GetGateCount(), cloned->GetGateCount());

		// Compare behavior
		base->GetPin("EN")->Set(IOState::LOW);
		base->GetPin("I0")->Set(IOState::LOW);
		base->GetPin("I1")->Set(IOState::LOW);

		cloned->GetPin("EN")->Set(IOState::LOW);
		cloned->GetPin("I0")->Set(IOState::LOW);
		cloned->GetPin("I1")->Set(IOState::LOW);

		AssertEqualOutputs(base, cloned);

		base->GetPin("EN")->Set(IOState::HI);
		cloned->GetPin("EN")->Set(IOState::HI);

		AssertEqualOutputs(base, cloned);

		base->GetPin("I0")->Set(IOState::HI);
		cloned->GetPin("I0")->Set(IOState::HI);

		AssertEqualOutputs(base, cloned);
	}	

	TEST(TestCore, TestHiZOutput)
	{
		GatePtr b1 = BasicGates::BufferGate::Create();
		GatePtr b2 = BasicGates::BufferGate::Create();
		GatePtr wire = BasicGates::WireGate::Create();

		b1->GetPin("out")->ConnectTo(wire->GetPin("in"));
		b2->GetPin("out")->ConnectTo(wire->GetPin("in"));

		ASSERT_EQ(2, wire->GetConnectedFromPins("in").size());

		b1->GetPin("en")->Set(IOState::LOW);
		b2->GetPin("en")->Set(IOState::LOW);
		ASSERT_EQ(IOState::HI_Z, wire->GetPin("out")->Get());

		b1->GetPin("in")->Set(IOState::HI);
		b1->GetPin("en")->Set(IOState::HI);
		ASSERT_EQ(IOState::HI, wire->GetPin("out")->Get());
		b1->GetPin("in")->Set(IOState::LOW);
		ASSERT_EQ(IOState::LOW, wire->GetPin("out")->Get());
		b2->GetPin("in")->Set(IOState::HI);
		ASSERT_EQ(IOState::LOW, wire->GetPin("out")->Get());
		b1->GetPin("en")->Set(IOState::LOW);
		ASSERT_EQ(IOState::HI_Z, wire->GetPin("out")->Get());
		b2->GetPin("en")->Set(IOState::HI);
		ASSERT_EQ(IOState::HI, wire->GetPin("out")->Get());

		// Both output enabled
		b1->GetPin("en")->Set(IOState::HI);
		ASSERT_EQ(IOState::UNDEF, wire->GetPin("out")->Get());
	}

	TEST(TestCore, TestNested)
	{
		GatePtr notGate = BasicGates::NOTGate::Create();

		CompositeGatePtr nested1 = NestGate("nest1", notGate);
		ASSERT_NE(nullptr, nested1);
		ASSERT_EQ(2, nested1->GetGateCount());
		nested1->GetPin("in")->Set(IOState::HI);
		ASSERT_EQ(IOState::HI, nested1->GetPin("out")->Get());
		nested1->GetPin("in")->Set(IOState::LOW);
		ASSERT_EQ(IOState::LOW, nested1->GetPin("out")->Get());

		CompositeGatePtr nested2 = NestGate("nest2", nested1);
		ASSERT_NE(nullptr, nested2);
		ASSERT_EQ(2, nested2->GetGateCount());
		nested1->GetPin("in")->Set(IOState::HI);
		ASSERT_EQ(IOState::HI, nested1->GetPin("out")->Get());
		nested1->GetPin("in")->Set(IOState::LOW);
		ASSERT_EQ(IOState::LOW, nested1->GetPin("out")->Get());

		CompositeGatePtr nested3 = NestGate("nest3", nested2);
		ASSERT_NE(nullptr, nested3);
		ASSERT_EQ(2, nested3->GetGateCount());
		nested1->GetPin("in")->Set(IOState::HI);
		ASSERT_EQ(IOState::HI, nested1->GetPin("out")->Get());
		nested1->GetPin("in")->Set(IOState::LOW);
		ASSERT_EQ(IOState::LOW, nested1->GetPin("out")->Get());

		// Now Clone it
		GatePtr clone = nested3->Clone("clone");

		// Check independance
		nested1->GetPin("in")->Set(IOState::HI);
		ASSERT_EQ(IOState::HI, nested1->GetPin("out")->Get());

		clone->GetPin("in")->Set(IOState::HI);
		ASSERT_EQ(IOState::HI, clone->GetPin("out")->Get());
		nested1->GetPin("in")->Set(IOState::LOW);
		ASSERT_EQ(IOState::HI, clone->GetPin("out")->Get());
		ASSERT_EQ(IOState::LOW, nested1->GetPin("out")->Get());
	}

	TEST(TestCore, GetConnectedToPins)
	{
		GatePtr not1 = BasicGates::NOTGate::Create();
		GatePtr not2 = BasicGates::NOTGate::Create();
		not1->GetPin("out")->ConnectTo(not2->GetPin("in"));

		EXPECT_THROW(not1->GetConnectedToPins((const char*)nullptr), std::invalid_argument);
		EXPECT_THROW(not1->GetConnectedToPins(nullptr), std::invalid_argument);
		EXPECT_THROW(not1->GetConnectedToPins(""), std::invalid_argument);
		EXPECT_THROW(not1->GetConnectedToPins("bad"), std::invalid_argument);
		EXPECT_THROW(not1->GetConnectedToPins(not2->GetPin("in")), std::invalid_argument);

		EXPECT_EQ(0, not1->GetConnectedToPins("in").size());
		EXPECT_EQ(0, not1->GetConnectedToPins(not1->GetPin("in")).size());

		EXPECT_EQ(1, not1->GetConnectedToPins("out").size());
		EXPECT_EQ(1, not1->GetConnectedToPins(not1->GetPin("out")).size());
	}
	
	TEST(TestLogicTools, GetConnectedFromPins)
	{
		GatePtr not1 = BasicGates::NOTGate::Create();
		GatePtr not2 = BasicGates::NOTGate::Create();
		not1->GetPin("out")->ConnectTo(not2->GetPin("in"));

		EXPECT_THROW(not2->GetConnectedFromPins((const char*)nullptr), std::invalid_argument);
		EXPECT_THROW(not2->GetConnectedFromPins((IOPinPtr)nullptr), std::invalid_argument);
		EXPECT_THROW(not2->GetConnectedFromPins(""), std::invalid_argument);
		EXPECT_THROW(not2->GetConnectedFromPins("bad"), std::invalid_argument);
		EXPECT_THROW(not2->GetConnectedFromPins(not1->GetPin("in")), std::invalid_argument);

		EXPECT_EQ(1, not2->GetConnectedFromPins("in").size());
		EXPECT_EQ(1, not2->GetConnectedFromPins(not2->GetPin("in")).size());

		EXPECT_EQ(0, not2->GetConnectedFromPins("out").size());
		EXPECT_EQ(0, not2->GetConnectedFromPins(not2->GetPin("out")).size());
	}

	TEST(TestLogicTools, GetConnectedFromToPins)
	{
		GatePtr not1 = BasicGates::NOTGate::Create();
		GatePtr not2 = BasicGates::NOTGate::Create();
		not1->GetPin("out")->ConnectTo(not2->GetPin("in"));

		// Size is always number of pins
		EXPECT_EQ(2, not2->GetConnectedFromPins().size());
		EXPECT_EQ(2, not2->GetConnectedToPins().size());
		EXPECT_EQ(2, not1->GetConnectedFromPins().size());
		EXPECT_EQ(2, not1->GetConnectedToPins().size());
	}

}