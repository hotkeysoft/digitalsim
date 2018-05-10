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
#include "BasicGates\DFlipFLop.h"
#include "BasicGates\TFlipFLop.h"
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

	TEST(TestCore, AddInput)
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

		gate->AddInput("/a");
		ASSERT_EQ(4, (int)gate->GetInputCount());
		ASSERT_EQ(0, (int)gate->GetOutputCount());

		ASSERT_NE(nullptr, gate->GetPin("a"));
		ASSERT_NE(nullptr, gate->GetPin("a12345"));
			
		//Logger::WriteMessage("TestAddInput: Duplicate");
		EXPECT_THROW(gate->AddInput("a"), std::invalid_argument);
		EXPECT_THROW(gate->AddOutput("a"), std::invalid_argument);
		EXPECT_THROW(gate->AddOutput("/a"), std::invalid_argument);
		EXPECT_THROW(gate->AddInput("/a"), std::invalid_argument);

		ASSERT_EQ(4, (int)gate->GetInputCount());
		ASSERT_EQ(0, (int)gate->GetOutputCount());

		EXPECT_THROW(gate->AddInput("width", 0), std::out_of_range);
		EXPECT_THROW(gate->AddInput("width", -1), std::out_of_range);
		EXPECT_THROW(gate->AddInput("width", 17), std::out_of_range);

		ASSERT_NE(nullptr, gate->AddInput("width", 16));
	}

	TEST(TestCore, AddOutput)
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

		gate->AddOutput("/a");
		ASSERT_EQ(4, (int)gate->GetOutputCount());
		ASSERT_EQ(0, (int)gate->GetInputCount());

		ASSERT_NE(nullptr, gate->GetPin("a"));
		ASSERT_NE(nullptr, gate->GetPin("a12345"));

		//Logger::WriteMessage("TestAddOutput: Duplicate");
		EXPECT_THROW(gate->AddOutput("a"), std::invalid_argument);
		EXPECT_THROW(gate->AddInput("a"), std::invalid_argument);
		EXPECT_THROW(gate->AddOutput("/a"), std::invalid_argument);
		EXPECT_THROW(gate->AddInput("/a"), std::invalid_argument);

		ASSERT_EQ(4, (int)gate->GetOutputCount());
		ASSERT_EQ(0, (int)gate->GetInputCount());

		EXPECT_THROW(gate->AddOutput("width", 0), std::out_of_range);
		EXPECT_THROW(gate->AddOutput("width", -1), std::out_of_range);
		EXPECT_THROW(gate->AddOutput("width", 17), std::out_of_range);

		EXPECT_THROW(gate->AddOutput("width", 1, IOPin::IO_DIRECTION::INPUT), std::invalid_argument);

		ASSERT_NE(nullptr, gate->AddOutput("width", 16));
	}

	TEST(TestCore, GetPin)
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

	TEST(TestCore, ConnectTo)
	{
		CompositeGatePtr gate = CompositeGate::Create("test");
		gate->AddInput("in");
		gate->AddOutput("out");

		CompositeGatePtr gate2 = CompositeGate::Create("test2");
		gate2->AddInput("in");
		gate2->AddOutput("out");
		
		//Logger::WriteMessage("TestConnectTo: Connect to nothing");
		EXPECT_THROW(gate->GetPin("out")->ConnectTo(nullptr), std::invalid_argument);

		//Logger::WriteMessage("TestConnectTo: Connect to self (in to in)");
		EXPECT_THROW(gate->GetPin("in")->ConnectTo(gate->GetPin("in")), std::invalid_argument);

		//Logger::WriteMessage("TestConnectTo: Connect to self (out to out)");
		EXPECT_THROW(gate->GetPin("out")->ConnectTo(gate->GetPin("out")), std::invalid_argument);

		//Logger::WriteMessage("TestConnectTo: Connect to self (in to out)");
		EXPECT_THROW(gate->GetPin("in")->ConnectTo(gate->GetPin("out")), std::invalid_argument);

		EXPECT_EQ(0, gate->GetConnectedToPin("out").size());
		EXPECT_EQ(0, gate->GetConnectedFromPin("out").size());
		EXPECT_EQ(0, gate2->GetConnectedToPin("in").size());
		EXPECT_EQ(0, gate2->GetConnectedFromPin("in").size());

		gate->GetPin("out")->ConnectTo(gate2->GetPin("in"));
		EXPECT_EQ(1, gate->GetConnectedToPin("out").size());
		EXPECT_EQ(0, gate->GetConnectedFromPin("out").size());
		EXPECT_EQ(0, gate2->GetConnectedToPin("in").size());
		EXPECT_EQ(1, gate2->GetConnectedFromPin("in").size());

		// Same link
		EXPECT_THROW(gate->GetPin("out")->ConnectTo(gate2->GetPin("in")), std::invalid_argument);
		EXPECT_EQ(1, gate->GetConnectedToPin("out").size());
		EXPECT_EQ(0, gate->GetConnectedFromPin("out").size());
		EXPECT_EQ(0, gate2->GetConnectedToPin("in").size());
		EXPECT_EQ(1, gate2->GetConnectedFromPin("in").size());
	}

	TEST(TestIOConnection, ConnectToEdgeCases)
	{
		CompositeGatePtr gate = CompositeGate::Create("gate");

		gate->AddInput("in");
		gate->AddInput("in2");
		gate->AddInput("inBus", 8);
		gate->AddInput("inBus2", 8);
		gate->AddOutput("out");
		gate->AddOutput("outBus", 8);
		gate->AddInput("miniBus", 4);
		gate->AddInput("miniBus2", 4);

		gate->AddGate("buf", BasicGates::BufferGate::Create(8));

		gate->FindPin("in")->ConnectTo(gate->FindPin("buf.in[0]"));
		gate->FindPin("in")->ConnectTo(gate->FindPin("buf.in[1]"));
		// Now connected: [11xxxxxx]"

		// buf.in[0] is already connected
		EXPECT_THROW(gate->FindPin("in2")->ConnectTo(gate->FindPin("buf.in[0]")), std::invalid_argument);
		gate->FindPin("in2")->ConnectTo(gate->FindPin("buf.in[2]"));
		// Now connected: [111xxxxx]"

		// buf.in[0-2] already connected
		EXPECT_THROW(gate->FindPin("inBus")->ConnectTo(gate->FindPin("buf.in")), std::invalid_argument);
		gate->FindPin("miniBus")->ConnectTo(gate->FindPin("buf.in[4-7]"));

		// Now connected [111x1111];
		EXPECT_THROW(gate->FindPin("miniBus2")->ConnectTo(gate->FindPin("buf.in[4-7]")), std::invalid_argument);
		gate->FindPin("miniBus2[0]")->ConnectTo(gate->FindPin("buf.in[3]"));

		// Now connected [11111111];
		EXPECT_THROW(gate->FindPin("inBus2")->ConnectTo(gate->FindPin("buf.in")), std::invalid_argument);
	}

	TEST(TestCore, SetName)
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

	TEST(TestCore, AddGate)
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
	
	TEST(TestCore, GetGate)
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
		ASSERT_EQ(gate.get(), child->GetParent());
	}

	TEST(TestCore, ComponentToTopLevel)
	{
		//Logger::WriteMessage("TestComponentToTopLevel: Outside->Outside");
		CompositeGatePtr gate = CompositeGate::Create("test");;
		gate->AddOutput("out");
		CompositeGatePtr gate2 = CompositeGate::Create("test2");
		gate2->AddInput("in");

		gate->GetPin("out")->ConnectTo(gate2->GetPin("in"));
	}

	TEST(TestCore, InnerToComponent)
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

	//TODO: There could be some cases where ConnectTo validations won't work well,
	//      Especially with pin subsets.  More tests should be added

	TEST(TestCore, FullName)
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
		ASSERT_EQ(g1->GetPin("y0")->Get(), g2->GetPin("y0")->Get());
		ASSERT_EQ(g1->GetPin("y1")->Get(), g2->GetPin("y1")->Get());
		ASSERT_EQ(g1->GetPin("y2")->Get(), g2->GetPin("y2")->Get());
		ASSERT_EQ(g1->GetPin("y3")->Get(), g2->GetPin("y3")->Get());
	}

	TEST(TestCore, Clone)
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
		base->GetPin("en")->Set(IOState::LOW);
		base->GetPin("i0")->Set(IOState::LOW);
		base->GetPin("i1")->Set(IOState::LOW);

		cloned->GetPin("en")->Set(IOState::LOW);
		cloned->GetPin("i0")->Set(IOState::LOW);
		cloned->GetPin("i1")->Set(IOState::LOW);

		AssertEqualOutputs(base, cloned);

		base->GetPin("en")->Set(IOState::HI);
		cloned->GetPin("en")->Set(IOState::HI);

		AssertEqualOutputs(base, cloned);

		base->GetPin("i0")->Set(IOState::HI);
		cloned->GetPin("i0")->Set(IOState::HI);

		AssertEqualOutputs(base, cloned);
	}	

	TEST(TestCore, Clone2)
	{
		CompositeGatePtr comp = CompositeGate::Create("comp");
		EXPECT_NE(nullptr, std::dynamic_pointer_cast<CompositeGate>(comp));
		EXPECT_NE(nullptr, std::dynamic_pointer_cast<GateBase>(comp));

		comp->AddGate("and", BasicGates::ANDGate::Create());
		comp->AddGate("tflip", BasicGates::TFlipFlop::Create());

		EXPECT_NE(nullptr, std::dynamic_pointer_cast<BasicGates::ANDGate>(comp->GetGate("and")));
		CompositeGatePtr flip = std::dynamic_pointer_cast<CompositeGate>(comp->GetGate("tflip"));
		EXPECT_NE(nullptr, flip);
		EXPECT_NE(nullptr, std::dynamic_pointer_cast<BasicGates::TFlipFlop>(flip));
		EXPECT_NE(nullptr, std::dynamic_pointer_cast<BasicGates::DFlipFlop>(flip->GetGate("dflip")));
		
		CompositeGatePtr clone = std::dynamic_pointer_cast<CompositeGate>(comp->Clone("clone"));
		EXPECT_NE(nullptr, clone);
		EXPECT_NE(nullptr, std::dynamic_pointer_cast<GateBase>(clone));

		EXPECT_NE(nullptr, std::dynamic_pointer_cast<BasicGates::ANDGate>(clone->GetGate("and")));
		CompositeGatePtr flipClone = std::dynamic_pointer_cast<CompositeGate>(clone->GetGate("tflip"));
		EXPECT_NE(nullptr, flipClone);
		EXPECT_NE(nullptr, std::dynamic_pointer_cast<BasicGates::TFlipFlop>(flipClone));
		EXPECT_NE(nullptr, std::dynamic_pointer_cast<BasicGates::DFlipFlop>(flipClone->GetGate("dflip")));
	}

	TEST(TestCore, HiZOutput)
	{
		GatePtr b1 = BasicGates::BufferGate::Create();
		GatePtr b2 = BasicGates::BufferGate::Create();
		GatePtr wire = BasicGates::WireGate::Create();

		b1->GetPin("out")->ConnectTo(wire->GetPin("in"));
		b2->GetPin("out")->ConnectTo(wire->GetPin("in"));

		ASSERT_EQ(2, wire->GetConnectedFromPin("in").size());

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

	TEST(TestCore, Nested)
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

	TEST(TestCore, GetConnectedToPin)
	{
		GatePtr not1 = BasicGates::NOTGate::Create();
		GatePtr not2 = BasicGates::NOTGate::Create();
		not1->GetPin("out")->ConnectTo(not2->GetPin("in"));

		EXPECT_THROW(not1->GetConnectedToPin((const char*)nullptr), std::invalid_argument);
		EXPECT_THROW(not1->GetConnectedToPin(nullptr), std::invalid_argument);
		EXPECT_THROW(not1->GetConnectedToPin(IOPinPtr()), std::invalid_argument);
		EXPECT_THROW(not1->GetConnectedToPin(""), std::invalid_argument);
		EXPECT_THROW(not1->GetConnectedToPin("bad"), std::invalid_argument);
		EXPECT_THROW(not1->GetConnectedToPin(not2->GetPin("in")), std::invalid_argument);
		
		EXPECT_EQ(0, not1->GetConnectedToPin("in").size());
		EXPECT_EQ(0, not1->GetConnectedToPin(not1->GetPin("in")).size());

		EXPECT_EQ(1, not1->GetConnectedToPin("out").size());
		EXPECT_EQ(1, not1->GetConnectedToPin(not1->GetPin("out")).size());
	}
	
	TEST(TestCore, GetConnectedFromPin)
	{
		GatePtr not1 = BasicGates::NOTGate::Create();
		GatePtr not2 = BasicGates::NOTGate::Create();
		not1->GetPin("out")->ConnectTo(not2->GetPin("in"));

		EXPECT_THROW(not2->GetConnectedFromPin((const char*)nullptr), std::invalid_argument);
		EXPECT_THROW(not2->GetConnectedFromPin((IOPinPtr)nullptr), std::invalid_argument);
		EXPECT_THROW(not2->GetConnectedFromPin(""), std::invalid_argument);
		EXPECT_THROW(not2->GetConnectedFromPin("bad"), std::invalid_argument);
		EXPECT_THROW(not2->GetConnectedFromPin(not1->GetPin("in")), std::invalid_argument);

		EXPECT_EQ(1, not2->GetConnectedFromPin("in").size());
		EXPECT_EQ(1, not2->GetConnectedFromPin(not2->GetPin("in")).size());

		EXPECT_EQ(0, not2->GetConnectedFromPin("out").size());
		EXPECT_EQ(0, not2->GetConnectedFromPin(not2->GetPin("out")).size());
	}

	TEST(TestCore, GetConnectedFromToPins)
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

	TEST(TestCore, GateCount)
	{
		GatePtr gate = Build4BitAdder();
		EXPECT_EQ(4, gate->GetGateCount(false));
		EXPECT_EQ(52, gate->GetGateCount(true));

		EXPECT_EQ(1, BasicGates::NOTGate::Create()->GetGateCount(false));
		EXPECT_EQ(1, BasicGates::NOTGate::Create()->GetGateCount(true));
	}

	TEST(TestCore, FindPin)
	{
		GatePtr gate = BasicGates::ANDGate::Create(3);

		//TEST_COUT << "Badly formed names";
		EXPECT_THROW(gate->FindPin(nullptr), std::invalid_argument);
		EXPECT_EQ(nullptr, gate->FindPin(""));
		EXPECT_EQ(nullptr, gate->FindPin("a"));
		EXPECT_EQ(nullptr, gate->FindPin(".a"));
		EXPECT_EQ(nullptr, gate->FindPin(".in1"));
		EXPECT_EQ(nullptr, gate->FindPin("in1."));
		EXPECT_EQ(nullptr, gate->FindPin("in1 "));
		EXPECT_EQ(nullptr, gate->FindPin(" in1 "));
		EXPECT_EQ(nullptr, gate->FindPin("in1 "));
		EXPECT_EQ(nullptr, gate->FindPin("a.in1."));
		EXPECT_EQ(nullptr, gate->FindPin("in1.a"));
		EXPECT_EQ(nullptr, gate->FindPin("/in1"));
		EXPECT_EQ(nullptr, gate->FindPin("in1/"));
		EXPECT_EQ(nullptr, gate->FindPin("and"));
		EXPECT_EQ(nullptr, gate->FindPin("AND."));
		EXPECT_EQ(nullptr, gate->FindPin("AND.in1"));
		EXPECT_EQ(nullptr, gate->FindPin("and.IN1"));
		EXPECT_EQ(nullptr, gate->FindPin("and.in"));
		EXPECT_EQ(nullptr, gate->FindPin("and.iN1"));
		EXPECT_EQ(nullptr, gate->FindPin("in["));
		EXPECT_EQ(nullptr, gate->FindPin("in[1"));
		EXPECT_EQ(nullptr, gate->FindPin("in[1]"));
		EXPECT_EQ(nullptr, gate->FindPin("in[1-]"));
		EXPECT_EQ(nullptr, gate->FindPin("in[-1]"));
		EXPECT_EQ(nullptr, gate->FindPin("in[]"));
		EXPECT_EQ(nullptr, gate->FindPin("in[2-1]"));
		
		IOPinPtr pin = gate->GetPin("in1");
		EXPECT_NE(nullptr, pin);

		//TEST_COUT << "Single pin in basic gate";
		EXPECT_EQ(pin, gate->FindPin("in1"));
		EXPECT_EQ(nullptr, gate->FindPin("and.in1"));
		EXPECT_NE(nullptr, gate->FindPin("in2"));
		EXPECT_NE(pin, gate->FindPin("in2"));
	}

	TEST(TestCore, FindPin2)
	{
		//TEST_COUT << "Bus, pin index";
		GatePtr gate = BasicGates::BufferGate::Create(8);

		//TEST_COUT << "Badly formed indices";
		EXPECT_EQ(nullptr, gate->FindPin("in["));
		EXPECT_EQ(nullptr, gate->FindPin("in[1"));
		EXPECT_EQ(nullptr, gate->FindPin("in[1-"));
		EXPECT_EQ(nullptr, gate->FindPin("in[1-2"));
		EXPECT_EQ(nullptr, gate->FindPin("in[1-2."));
		EXPECT_EQ(nullptr, gate->FindPin("in[1-]"));
		EXPECT_EQ(nullptr, gate->FindPin("in[-1]"));
		EXPECT_EQ(nullptr, gate->FindPin("in[]"));
		EXPECT_EQ(nullptr, gate->FindPin("in[1-2-]"));
		EXPECT_EQ(nullptr, gate->FindPin("in[2-3-]"));
		EXPECT_EQ(nullptr, gate->FindPin("in[2-.3]"));
		EXPECT_EQ(nullptr, gate->FindPin("in[2.3]"));
		EXPECT_EQ(nullptr, gate->FindPin("in[2-3.]"));
		EXPECT_EQ(nullptr, gate->FindPin("in[2-3]."));
		EXPECT_EQ(nullptr, gate->FindPin("in[2-3a]"));
		EXPECT_EQ(nullptr, gate->FindPin("in[a2-3]"));
		EXPECT_EQ(nullptr, gate->FindPin("in[2a-3]"));
		EXPECT_THROW(gate->FindPin("in[2-1]"), std::out_of_range);
		EXPECT_THROW(gate->FindPin("in[8]"), std::out_of_range);

		//TEST_COUT << "Good indices";
		EXPECT_EQ(8, gate->FindPin("in")->GetWidth());
		EXPECT_EQ(1, gate->FindPin("in[0]")->GetWidth());
		EXPECT_EQ(1, gate->FindPin("in[1]")->GetWidth());
		EXPECT_EQ(1, gate->FindPin("in[7]")->GetWidth());
		EXPECT_EQ(2, gate->FindPin("in[0-1]")->GetWidth());
		EXPECT_EQ(2, gate->FindPin("in[2-3]")->GetWidth());
		EXPECT_EQ(7, gate->FindPin("in[1-7]")->GetWidth());

		EXPECT_NE(nullptr, gate->FindPin("en"));
		EXPECT_NE(nullptr, gate->FindPin("en[0]"));
		EXPECT_THROW(gate->FindPin("en[1]"), std::out_of_range);
	}

	TEST(TestCore, FindPin3)
	{
		// For compositegate objects

		//TEST_COUT << "Multi-level";
		CompositeGatePtr gate = std::dynamic_pointer_cast<CompositeGate>(Build74163Counter());
		ASSERT_NE(nullptr, gate);
		CompositeGatePtr d1 = std::dynamic_pointer_cast<CompositeGate>(gate->GetGate("d1"));
		ASSERT_NE(nullptr, d1);
		CompositeGatePtr d1d1 = std::dynamic_pointer_cast<CompositeGate>(d1->GetGate("d1"));
		ASSERT_NE(nullptr, d1d1);
		CompositeGatePtr d1d2 = std::dynamic_pointer_cast<CompositeGate>(d1->GetGate("d2"));
		ASSERT_NE(nullptr, d1d2);

		//TEST_COUT << "Level 0";
		EXPECT_EQ(gate->FindPin("/ld").get(), gate->GetPin("/ld").get());
		EXPECT_EQ(gate->FindPin("counter./ld"), nullptr);
		EXPECT_EQ(gate->FindPin("counter.data[1]"), nullptr);

		// Cannot compare pin subsets directly they are generated on demand
		EXPECT_NE(nullptr, gate->FindPin("data[1]").get());

		//TEST_COUT << "Level 1";
		EXPECT_EQ(gate->FindPin("counter.not1.in"), nullptr);
		EXPECT_EQ(gate->FindPin("not1.in").get(), gate->GetGate("not1")->GetPin("in").get());
		EXPECT_EQ(gate->FindPin("counter.d1.q"), nullptr);
		EXPECT_EQ(gate->FindPin("d1.q").get(), d1->GetPin("q").get());
		EXPECT_EQ(gate->FindPin("counter.d1./q"), nullptr);
		EXPECT_EQ(gate->FindPin("d1./q").get(), d1->GetPin("/q").get());

		//TEST_COUT << "Level 2";
		EXPECT_EQ(gate->FindPin("counter.d1.not2.in"), nullptr);
		EXPECT_EQ(gate->FindPin("d1.not2.in").get(), d1->GetGate("not2")->GetPin("in").get());
		EXPECT_EQ(gate->FindPin("counter.d1.d1.q"), nullptr);
		EXPECT_EQ(gate->FindPin("d1.d1.q").get(), d1d1->GetPin("q").get());
		EXPECT_EQ(gate->FindPin("counter.d1.d2./q"), nullptr);
		EXPECT_EQ(gate->FindPin("d1.d2./q").get(), d1d2->GetPin("/q").get());

		//TEST_COUT << "Level 3";
		EXPECT_EQ(gate->FindPin("counter.d1.d2.not.in"), nullptr);
		EXPECT_EQ(gate->FindPin("d1.d2.not.in").get(), d1d2->GetGate("not")->GetPin("in").get());

		//Bad cases
		EXPECT_THROW(gate->FindPin(nullptr), std::invalid_argument);
		EXPECT_EQ(nullptr, gate->FindPin(""));
		EXPECT_EQ(nullptr, gate->FindPin("a"));
		EXPECT_EQ(nullptr, gate->FindPin(".a"));
		EXPECT_EQ(nullptr, gate->FindPin(".in1"));
		EXPECT_EQ(nullptr, gate->FindPin("in1."));
		EXPECT_EQ(nullptr, gate->FindPin("in1 "));
		EXPECT_EQ(nullptr, gate->FindPin(" in1 "));
		EXPECT_EQ(nullptr, gate->FindPin("in1 "));
		EXPECT_EQ(nullptr, gate->FindPin("a.in1."));
		EXPECT_EQ(nullptr, gate->FindPin("in1.a"));
		EXPECT_EQ(nullptr, gate->FindPin("/in1"));
		EXPECT_EQ(nullptr, gate->FindPin("in1/"));
		EXPECT_EQ(nullptr, gate->FindPin("and"));
		EXPECT_EQ(nullptr, gate->FindPin("AND."));
		EXPECT_EQ(nullptr, gate->FindPin("d1."));
		EXPECT_EQ(nullptr, gate->FindPin("d1.d2"));
		EXPECT_EQ(nullptr, gate->FindPin(".d1.d2"));
		EXPECT_EQ(nullptr, gate->FindPin("d1.[1]"));
	}
}