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

	CompositeGate* NestGate(const char* name, GateBase* toNest)
	{
		CompositeGate* comp = new CompositeGate(name);
		IOPinPtr pin = comp->AddInput("in");
		IOPinPtr pout = comp->AddOutput("out");
		BasicGates::NOTGate* notGate = new BasicGates::NOTGate();
		comp->AddGate("not", notGate);
		comp->AddGate("inner", toNest);

		comp->GetPin("in")->ConnectTo(notGate->GetPin("in"));
		toNest->GetPin("out")->ConnectTo(comp->GetPin("out"));
		notGate->GetPin("out")->ConnectTo(toNest->GetPin("in"));

		return comp;
	}

	TEST(TestCore, TestAddInput)
	{
		CompositeGate* gate = new CompositeGate("test");

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

		delete gate;
	}

	TEST(TestCore, TestAddOutput)
	{
		CompositeGate* gate = new CompositeGate("test");

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

		delete gate;
	}

	TEST(TestCore, TestGetPin)
	{
		BasicGates::ANDGate* gate = new BasicGates::ANDGate();

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
		CompositeGate* gate = new CompositeGate("test");
		gate->AddInput("in");
		gate->AddOutput("out");

		CompositeGate* gate2 = new CompositeGate("test2");
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

		delete gate;
		delete gate2;
	}

	TEST(TestCore, DISABLED_TestFeedback)
	{
		GateBase* gate = new BasicGates::NOTGate();
		GateBase* gate2 = new BasicGates::NOTGate();
			
		//Logger::WriteMessage("TestFeedback: gate->gate2->gate");
		gate->GetPin("out")->ConnectTo(gate2->GetPin("in"));
		gate2->GetPin("out")->ConnectTo(gate->GetPin("in"));

//			gate->GetPin("in")->Set(IOState::LOW);
		FAIL();

		delete gate;
		delete gate2;
	}

	TEST(TestCore, TestSetName)
	{
		GateBase * notGate = new BasicGates::NOTGate();
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
		CompositeGate * gate = new CompositeGate("test");
		GateBase * dummyGate = new BasicGates::NOTGate();
		GateBase * notGate = new BasicGates::NOTGate();
		GateBase * notGate2 = new BasicGates::NOTGate();

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
		CompositeGate * gate = new CompositeGate("test");
		GateBase * notGate = new BasicGates::NOTGate();
		ASSERT_EQ(nullptr, notGate->GetParent());

		gate->AddGate("not", notGate);
		ASSERT_EQ(1, (int)gate->GetGateCount());
		ASSERT_EQ(1, gate->GetInternalGates().size());
		
		EXPECT_THROW(gate->GetGate(nullptr), std::invalid_argument);
		ASSERT_EQ(nullptr, gate->GetGate(""));
		ASSERT_EQ(nullptr, gate->GetGate(" "));
		ASSERT_EQ(nullptr, gate->GetGate(" ba"));
		ASSERT_EQ(nullptr, gate->GetGate(" not"));
		GateBase* child = gate->GetGate("not");
		ASSERT_EQ(notGate, child);
		ASSERT_STREQ("not", child->GetName().c_str());
		ASSERT_EQ(gate, child->GetParent());
	}

	TEST(TestCore, TestComponentToTopLevel)
	{
		//Logger::WriteMessage("TestComponentToTopLevel: Outside->Outside");
		CompositeGate * gate = new CompositeGate("test");
		gate->AddOutput("out");
		CompositeGate * gate2 = new CompositeGate("test2");
		gate2->AddInput("in");

		gate->GetPin("out")->ConnectTo(gate2->GetPin("in"));
	}

	TEST(TestCore, TestInnerToComponent)
	{
		CompositeGate * gate = new CompositeGate("test");
		gate->AddInput("in");
		gate->AddOutput("out");
		BasicGates::NOTGate * not1 = new BasicGates::NOTGate();
		BasicGates::NOTGate * not2 = new BasicGates::NOTGate();
		BasicGates::NOTGate * not3 = new BasicGates::NOTGate();
		BasicGates::NOTGate * not4 = new BasicGates::NOTGate();
		BasicGates::NOTGate * not5 = new BasicGates::NOTGate();
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
		GateBase * outsideNot = new BasicGates::NOTGate();

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
		
		BasicGates::NOTGate * not6 = new BasicGates::NOTGate();
		//Logger::WriteMessage("TestInnerComponentToComponent: Outside gate->Outside pin (ok)");
		not6->GetPin("out")->ConnectTo(gate->GetPin("in"));
		//Logger::WriteMessage("TestInnerComponentToComponent: Validate Outside gate->Outside pin");
		not6->GetPin("in")->Set(IOState::HI);
		ASSERT_EQ(IOState::HI, gate->GetPin("out")->Get());
		not6->GetPin("in")->Set(IOState::LOW);
		ASSERT_EQ(IOState::LOW, gate->GetPin("out")->Get());

		BasicGates::NOTGate * not7 = new BasicGates::NOTGate();
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
		BasicGates::NOTGate* notGate = new BasicGates::NOTGate();

		ASSERT_STREQ("not", notGate->GetName().c_str());
		ASSERT_STREQ("not", notGate->GetFullName().c_str());
		ASSERT_STREQ("in", notGate->GetPin("in")->GetName().c_str());
		ASSERT_STREQ("not.in", notGate->GetPin("in")->GetFullName().c_str());

		CompositeGate* nested1 = NestGate("nest1", notGate);
		ASSERT_NE(nullptr, nested1);
		ASSERT_STREQ("inner", notGate->GetName().c_str());
		ASSERT_STREQ("nest1.inner", notGate->GetFullName().c_str());
		ASSERT_STREQ("in", notGate->GetPin("in")->GetName().c_str());
		ASSERT_STREQ("nest1.inner.in", notGate->GetPin("in")->GetFullName().c_str());

		CompositeGate* nested2 = NestGate("nest2", nested1);
		ASSERT_NE(nullptr, nested1);
		ASSERT_STREQ("inner", notGate->GetName().c_str());
		ASSERT_STREQ("nest2.inner.inner", notGate->GetFullName().c_str());
		ASSERT_STREQ("in", notGate->GetPin("in")->GetName().c_str());
		ASSERT_STREQ("nest2.inner.inner.in", notGate->GetPin("in")->GetFullName().c_str());
		
		CompositeGate* nested3 = NestGate("nest3", nested2);
		ASSERT_NE(nullptr, nested1);
		ASSERT_STREQ("inner", notGate->GetName().c_str());
		ASSERT_STREQ("nest3.inner.inner.inner", notGate->GetFullName().c_str());
		ASSERT_STREQ("in", notGate->GetPin("in")->GetName().c_str());
		ASSERT_STREQ("nest3.inner.inner.inner.in", notGate->GetPin("in")->GetFullName().c_str());
	}

	void AssertEqualOutputs(GateBase* g1, GateBase* g2)
	{
		ASSERT_EQ(g1->GetPin("Y0")->Get(), g2->GetPin("Y0")->Get());
		ASSERT_EQ(g1->GetPin("Y1")->Get(), g2->GetPin("Y1")->Get());
		ASSERT_EQ(g1->GetPin("Y2")->Get(), g2->GetPin("Y2")->Get());
		ASSERT_EQ(g1->GetPin("Y3")->Get(), g2->GetPin("Y3")->Get());
	}

	TEST(TestCore, TestClone)
	{
		CompositeGate* base = BuildDecoder();
		GateBase* clonedBase = base->Clone("clone");
		CompositeGate* cloned = dynamic_cast<CompositeGate*>(clonedBase);

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
		GateBase* b1 = new BasicGates::BufferGate();
		GateBase* b2 = new BasicGates::BufferGate();
		GateBase* wire = new BasicGates::WireGate();

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
		BasicGates::NOTGate* notGate = new BasicGates::NOTGate();

		CompositeGate* nested1 = NestGate("nest1", notGate);
		ASSERT_NE(nullptr, nested1);
		ASSERT_EQ(2, nested1->GetGateCount());
		nested1->GetPin("in")->Set(IOState::HI);
		ASSERT_EQ(IOState::HI, nested1->GetPin("out")->Get());
		nested1->GetPin("in")->Set(IOState::LOW);
		ASSERT_EQ(IOState::LOW, nested1->GetPin("out")->Get());

		CompositeGate* nested2 = NestGate("nest2", nested1);
		ASSERT_NE(nullptr, nested2);
		ASSERT_EQ(2, nested2->GetGateCount());
		nested1->GetPin("in")->Set(IOState::HI);
		ASSERT_EQ(IOState::HI, nested1->GetPin("out")->Get());
		nested1->GetPin("in")->Set(IOState::LOW);
		ASSERT_EQ(IOState::LOW, nested1->GetPin("out")->Get());

		CompositeGate* nested3 = NestGate("nest3", nested2);
		ASSERT_NE(nullptr, nested3);
		ASSERT_EQ(2, nested3->GetGateCount());
		nested1->GetPin("in")->Set(IOState::HI);
		ASSERT_EQ(IOState::HI, nested1->GetPin("out")->Get());
		nested1->GetPin("in")->Set(IOState::LOW);
		ASSERT_EQ(IOState::LOW, nested1->GetPin("out")->Get());

		// Now Clone it
		GateBase* clone = nested3->Clone("clone");

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
		BasicGates::NOTGate* not1 = new BasicGates::NOTGate();
		BasicGates::NOTGate* not2 = new BasicGates::NOTGate();
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
		BasicGates::NOTGate* not1 = new BasicGates::NOTGate();
		BasicGates::NOTGate* not2 = new BasicGates::NOTGate();
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
		BasicGates::NOTGate* not1 = new BasicGates::NOTGate();
		BasicGates::NOTGate* not2 = new BasicGates::NOTGate();
		not1->GetPin("out")->ConnectTo(not2->GetPin("in"));

		// Size is always number of pins
		EXPECT_EQ(2, not2->GetConnectedFromPins().size());
		EXPECT_EQ(2, not2->GetConnectedToPins().size());
		EXPECT_EQ(2, not1->GetConnectedFromPins().size());
		EXPECT_EQ(2, not1->GetConnectedToPins().size());
	}

}