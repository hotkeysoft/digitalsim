#include "pch.h"
#pragma warning( disable:4251 )

#include "TestCommon.h"
#include "Core\GateBase.h"
#include "Core\CompositeGate.h"
#include "BasicGates\NOTGate.h"
#include "BasicGates\WireGate.h"
#include "BasicGates\BufferGate.h"
#include "Tools\LogicTools.h"

namespace UnitTests
{
	using namespace Core;

	GatePtr BuildBuffer(size_t width)
	{
		GatePtr buffer = BasicGates::BufferGate::Create(width);

		IOPinPtr inPin = buffer->GetPin("in");
		EXPECT_NE(nullptr, inPin);

		IOPinPtr outPin = buffer->GetPin("out");
		EXPECT_NE(nullptr, outPin);

		buffer->GetPin("en")->Set(IOState::HI);

		return buffer;
	}

	TEST(TestBus, BusCreate)
	{
		GatePtr buffer = BuildBuffer(4);

		ASSERT_EQ(4, buffer->GetPin("in")->GetWidth());
		ASSERT_EQ(4, buffer->GetPin("out")->GetWidth());
	}

	TEST(TestBus, BusGetSetPin)
	{
		GatePtr buffer = BuildBuffer(4);

		// Set all bus pins	
		buffer->GetPin("in")->Set(IOState(IOState::HI, 4));
		IOPinPtr outPin = buffer->GetPin("out");
		ASSERT_EQ(outPin->Get(), IOState({ IOState::HI, IOState::HI, IOState::HI, IOState::HI }));

		// Set all bus pins	
		buffer->GetPin("in")->Set(IOState(IOState::LOW, 4));
		ASSERT_EQ(outPin->Get(), IOState({ IOState::LOW, IOState::LOW, IOState::LOW, IOState::LOW }));

		// Width mismatch
		ASSERT_THROW(buffer->GetPin("in")->Set(IOState(IOState::HI)), std::invalid_argument);

		IOPinPtr pin13 = buffer->GetPin("in", 1, 3);
		ASSERT_THROW(pin13->Set(IOState({ IOState::HI, IOState::LOW })), std::invalid_argument);

		// Bad pin
		ASSERT_EQ(nullptr, buffer->GetPin("bad"));
		ASSERT_EQ(nullptr, buffer->GetPin("bad", 1));
		ASSERT_EQ(nullptr, buffer->GetPin("bad", 1, 2));
	}

	TEST(TestBus, BusToBus)
	{
		GatePtr buffer1 = BuildBuffer(4);
		GatePtr buffer2 = BuildBuffer(4);
		GatePtr buffer3 = BuildBuffer(8);
		
		GatePtr not1 = BasicGates::NOTGate::Create();
		GatePtr not2 = BasicGates::NOTGate::Create();

		// 4 pin to 8-bus
		ASSERT_THROW(buffer1->GetPin("out")->ConnectTo(buffer3->GetPin("in")), std::invalid_argument);

		// 8-bus to 4-bus
		ASSERT_THROW(buffer3->GetPin("out")->ConnectTo(buffer1->GetPin("in")), std::invalid_argument);

		buffer1->GetPin("out")->ConnectTo(buffer2->GetPin("in"));

		buffer1->GetPin("in")->Set(IOState({ IOState::HI, IOState::HI, IOState::HI, IOState::HI }));
		ASSERT_EQ(buffer2->GetPin("out")->Get(), IOState({ IOState::HI, IOState::HI, IOState::HI, IOState::HI }));

		ASSERT_THROW(buffer1->GetPin("in")->Set(IOState({ IOState::HI, IOState::HI })), std::invalid_argument);

		buffer1->GetPin("in")->Set(IOState({ IOState::HI, IOState::HI, IOState::LOW, IOState::HI }));
		ASSERT_EQ(buffer2->GetPin("out")->Get(), IOState({ IOState::HI, IOState::HI, IOState::LOW, IOState::HI }));

		buffer2->GetPin("en")->Set(IOState::LOW);
		ASSERT_EQ(buffer2->GetPin("out")->Get(), IOState({ IOState::HI_Z, IOState::HI_Z, IOState::HI_Z, IOState::HI_Z }));

		buffer2->GetPin("en")->Set(IOState::HI);
		ASSERT_EQ(buffer2->GetPin("out")->Get(), IOState({ IOState::HI, IOState::HI, IOState::LOW, IOState::HI }));
	}

	TEST(TestBus, PinsToBus)
	{
		GatePtr buffer = BuildBuffer(4);

		GatePtr not1 = BasicGates::NOTGate::Create();
		GatePtr not2 = BasicGates::NOTGate::Create();

		// 1 pin to 4-bus
		ASSERT_THROW(not1->GetPin("out")->ConnectTo(buffer->GetPin("in")), std::invalid_argument);

		// 4-bus to 1 pin
		ASSERT_THROW(buffer->GetPin("out")->ConnectTo(not1->GetPin("in")), std::invalid_argument);

		// Pin to bus pin
		not1->GetPin("out")->ConnectTo(buffer->GetPin("in", 0));
		not1->GetPin("out")->ConnectTo(buffer->GetPin("in", 1));

		not1->GetPin("out")->ConnectTo(not2->GetPin("in"));
		not2->GetPin("out")->ConnectTo(buffer->GetPin("in", 2));

		buffer->GetPin("en")->Set(IOState::HI);

		not1->GetPin("in")->Set(IOState::HI);
		ASSERT_EQ(buffer->GetPin("out", 0)->Get(), IOState::LOW);
		ASSERT_EQ(buffer->GetPin("out", 1)->Get(), IOState::LOW);
		ASSERT_EQ(buffer->GetPin("out", 2)->Get(), IOState::HI);

		not1->GetPin("in")->Set(IOState::LOW);
		ASSERT_EQ(buffer->GetPin("out", 0)->Get(), IOState::HI);
		ASSERT_EQ(buffer->GetPin("out", 1)->Get(), IOState::HI);
		ASSERT_EQ(buffer->GetPin("out", 2)->Get(), IOState::LOW);
	}

	GatePtr BuildGateInternalBus()
	{
		CompositeGatePtr gate = CompositeGate::Create("bus");
		gate->AddInput("in", 4);
		gate->AddOutput("out", 4);

		GatePtr w0 = BasicGates::WireGate::Create();
		GatePtr w1 = BasicGates::WireGate::Create();
		GatePtr n2 = BasicGates::NOTGate::Create();
		GatePtr n3 = BasicGates::NOTGate::Create();

		gate->AddGate("w0", w0);
		gate->AddGate("w1", w1);
		gate->AddGate("n2", n2);
		gate->AddGate("n3", n3);

		gate->GetPin("in", 0)->ConnectTo(w0->GetPin("in"));
		gate->GetPin("in", 1)->ConnectTo(w1->GetPin("in"));
		gate->GetPin("in", 2)->ConnectTo(n2->GetPin("in"));
		gate->GetPin("in", 3)->ConnectTo(n3->GetPin("in"));

		w0->GetPin("out")->ConnectTo(gate->GetPin("out", 0));
		w1->GetPin("out")->ConnectTo(gate->GetPin("out", 1));
		n2->GetPin("out")->ConnectTo(gate->GetPin("out", 2));
		n3->GetPin("out")->ConnectTo(gate->GetPin("out", 3));
		return gate;
	}

	TEST(TestBus, ComponentBus)
	{
		TEST_COUT << "bus->pins->bus";

		GatePtr gate = BuildGateInternalBus();

		gate->GetPin("in")->Set(IOState({ IOState::LOW, IOState::LOW, IOState::LOW, IOState::LOW}));
		EXPECT_EQ(gate->GetPin("out")->Get(), IOState({ IOState::LOW, IOState::LOW, IOState::HI, IOState::HI }));

		gate->GetPin("in")->Set(IOState({ IOState::HI, IOState::HI, IOState::HI, IOState::HI }));
		EXPECT_EQ(gate->GetPin("out")->Get(), IOState({ IOState::HI, IOState::HI, IOState::LOW, IOState::LOW }));
	}

	GatePtr BuildGateBus()
	{
		CompositeGatePtr gate = CompositeGate::Create("bus");
		gate->AddInput("in0");
		gate->AddInput("in1");
		gate->AddInput("in2");
		gate->AddInput("in3");

		gate->AddOutput("out0");
		gate->AddOutput("out1");
		gate->AddOutput("out2");
		gate->AddOutput("out3");

		GatePtr buf = BuildBuffer(4);
		buf->GetPin("en")->Set(IOState::HI);
		gate->AddGate("buf", buf);

		gate->GetPin("in0")->ConnectTo(buf->GetPin("in", 0));
		gate->GetPin("in1")->ConnectTo(buf->GetPin("in", 1));
		gate->GetPin("in2")->ConnectTo(buf->GetPin("in", 2));
		gate->GetPin("in3")->ConnectTo(buf->GetPin("in", 3));

		buf->GetPin("out", 0)->ConnectTo(gate->GetPin("out0"));
		buf->GetPin("out", 1)->ConnectTo(gate->GetPin("out1"));
		buf->GetPin("out", 2)->ConnectTo(gate->GetPin("out2"));
		buf->GetPin("out", 3)->ConnectTo(gate->GetPin("out3"));

		gate->GetPin("in0")->Set(IOState::LOW);
		gate->GetPin("in1")->Set(IOState::LOW);
		gate->GetPin("in2")->Set(IOState::LOW);
		gate->GetPin("in3")->Set(IOState::LOW);

		return gate;
	}

	TEST(TestBus, ComponentBus2)
	{
		TEST_COUT << "pins->bus->pins";
		GatePtr gate = BuildGateBus();

		EXPECT_EQ(gate->GetPin("out0")->Get(), IOState::LOW);
		EXPECT_EQ(gate->GetPin("out1")->Get(), IOState::LOW);
		EXPECT_EQ(gate->GetPin("out2")->Get(), IOState::LOW);
		EXPECT_EQ(gate->GetPin("out3")->Get(), IOState::LOW);

		gate->GetPin("in2")->Set(IOState::HI_Z);

		EXPECT_EQ(gate->GetPin("out0")->Get(), IOState::LOW);
		EXPECT_EQ(gate->GetPin("out1")->Get(), IOState::LOW);
		EXPECT_EQ(gate->GetPin("out2")->Get(), IOState::HI_Z);
		EXPECT_EQ(gate->GetPin("out3")->Get(), IOState::LOW);

		gate->GetPin("in0")->Set(IOState::HI);
		gate->GetPin("in1")->Set(IOState::HI);
		gate->GetPin("in2")->Set(IOState::HI);
		gate->GetPin("in3")->Set(IOState::HI);

		EXPECT_EQ(gate->GetPin("out0")->Get(), IOState::HI);
		EXPECT_EQ(gate->GetPin("out1")->Get(), IOState::HI);
		EXPECT_EQ(gate->GetPin("out2")->Get(), IOState::HI);
		EXPECT_EQ(gate->GetPin("out3")->Get(), IOState::HI);

		TEST_COUT << "pins->bus->pins (clone)";
	}

	TEST(TestBus, ComponentClone1)
	{
		TEST_COUT << "bus->pins->bus (Clone)";
		GatePtr gate = BuildGateInternalBus();
		GatePtr clone = gate->Clone("clone");
		std::cout << Tools::LogicTools::PrintInternalConnections(gate);
		std::cout << Tools::LogicTools::PrintInternalConnections(clone);

		clone->GetPin("in")->Set(IOState({ IOState::LOW, IOState::LOW, IOState::LOW, IOState::LOW }));
		EXPECT_EQ(clone->GetPin("out")->Get(), IOState({ IOState::LOW, IOState::LOW, IOState::HI, IOState::HI }));

		clone->GetPin("in")->Set(IOState({ IOState::HI, IOState::HI, IOState::HI, IOState::HI }));
		EXPECT_EQ(clone->GetPin("out")->Get(), IOState({ IOState::HI, IOState::HI, IOState::LOW, IOState::LOW }));
	}

	TEST(TestBus, ComponentClone2)
	{
		TEST_COUT << "pins->bus->pins (Clone)";
		GatePtr gate = BuildGateBus();
		GatePtr clone = gate->Clone("clone");
		std::cout << Tools::LogicTools::PrintInternalConnections(gate);
		std::cout << Tools::LogicTools::PrintInternalConnections(clone);

		clone->GetPin("in0")->Set(IOState::LOW);
		clone->GetPin("in1")->Set(IOState::LOW);
		clone->GetPin("in2")->Set(IOState::LOW);
		clone->GetPin("in3")->Set(IOState::LOW);

		EXPECT_EQ(clone->GetPin("out0")->Get(), IOState::LOW);
		EXPECT_EQ(clone->GetPin("out1")->Get(), IOState::LOW);
		EXPECT_EQ(clone->GetPin("out2")->Get(), IOState::LOW);
		EXPECT_EQ(clone->GetPin("out3")->Get(), IOState::LOW);

		clone->GetPin("in2")->Set(IOState::HI_Z);

		EXPECT_EQ(clone->GetPin("out0")->Get(), IOState::LOW);
		EXPECT_EQ(clone->GetPin("out1")->Get(), IOState::LOW);
		EXPECT_EQ(clone->GetPin("out2")->Get(), IOState::HI_Z);
		EXPECT_EQ(clone->GetPin("out3")->Get(), IOState::LOW);

		clone->GetPin("in0")->Set(IOState::HI);
		clone->GetPin("in1")->Set(IOState::HI);
		clone->GetPin("in2")->Set(IOState::HI);
		clone->GetPin("in3")->Set(IOState::HI);

		EXPECT_EQ(clone->GetPin("out0")->Get(), IOState::HI);
		EXPECT_EQ(clone->GetPin("out1")->Get(), IOState::HI);
		EXPECT_EQ(clone->GetPin("out2")->Get(), IOState::HI);
		EXPECT_EQ(clone->GetPin("out3")->Get(), IOState::HI);
	}
	
	TEST(TestBus, PinRangeOnePin)
	{
		GatePtr buffer = BuildBuffer(4);

		EXPECT_THROW(buffer->GetPin("in", -1), std::out_of_range);
		EXPECT_THROW(buffer->GetPin("in", 5), std::out_of_range);
		EXPECT_NE(nullptr, buffer->GetPin("in", 0));
		EXPECT_NE(nullptr, buffer->GetPin("in", 3));
		
		IOPinPtr inPin = buffer->GetPin("in");
		IOPinPtr singlePin2 = buffer->GetPin("in", 2);
		IOPinPtr singlePin0 = buffer->GetPin("in", 0);
		EXPECT_EQ(1, singlePin2->GetWidth());
		EXPECT_STREQ("in[2]", singlePin2->GetName().c_str());
		EXPECT_STREQ("in", singlePin2->GetRawName().c_str());

		TEST_COUT << "Change bus, check single pin";

		inPin->Set(IOState({IOState::LOW, IOState::LOW, IOState::LOW, IOState::LOW }));
		EXPECT_EQ(inPin->Get(), IOState({ IOState::LOW, IOState::LOW, IOState::LOW, IOState::LOW }));
		EXPECT_EQ(IOState::LOW, inPin->Get().Get(2));
		EXPECT_EQ(singlePin2->Get(), inPin->Get().Get(2));
		EXPECT_EQ(singlePin0->Get(), inPin->Get().Get(0));

		inPin->Set(IOState({ IOState::LOW, IOState::LOW, IOState::HI, IOState::LOW }));
		EXPECT_EQ(inPin->Get(), IOState({ IOState::LOW, IOState::LOW, IOState::HI, IOState::LOW }));
		EXPECT_EQ(IOState::HI, inPin->Get().Get(2));
		EXPECT_EQ(singlePin2->Get(), inPin->Get().Get(2));
		EXPECT_EQ(singlePin2->Get(), inPin->Get().Get(2));
		EXPECT_EQ(singlePin0->Get(), inPin->Get().Get(0));
		
		TEST_COUT << "Change pin, check bus";
		singlePin2->Set(IOState::HI_Z);
		EXPECT_EQ(singlePin2->Get(), inPin->Get().Get(2));
		EXPECT_EQ(inPin->Get(), IOState({ IOState::LOW, IOState::LOW, IOState::HI_Z, IOState::LOW }));

		singlePin2->Set(IOState::UNDEF);
		singlePin0->Set(IOState::HI_Z);
		EXPECT_EQ(singlePin2->Get(), inPin->Get().Get(2));
		EXPECT_EQ(singlePin0->Get(), inPin->Get().Get(0));
		EXPECT_EQ(inPin->Get(), IOState({ IOState::HI_Z, IOState::LOW, IOState::UNDEF, IOState::LOW }));

		GatePtr not1 = BasicGates::NOTGate::Create();
		EXPECT_THROW(not1->GetPin("in", 0), std::invalid_argument);
	}

	TEST(TestBus, PinRangeMultiplePins)
	{
		GatePtr buffer = BuildBuffer(4);

		EXPECT_THROW(buffer->GetPin("in", -1, -1), std::out_of_range);
		EXPECT_THROW(buffer->GetPin("in", 0, -1), std::out_of_range);
		EXPECT_THROW(buffer->GetPin("in", 1, 0), std::out_of_range);
		EXPECT_THROW(buffer->GetPin("in", 3, 4), std::out_of_range);
		EXPECT_THROW(buffer->GetPin("in", 5, 5), std::out_of_range);
		
		EXPECT_NE(nullptr, buffer->GetPin("in", 2, 2));
		EXPECT_NE(nullptr, buffer->GetPin("in", 0, 2));
		EXPECT_NE(nullptr, buffer->GetPin("in", 2, 3));

		IOPinPtr inPin = buffer->GetPin("in");
		IOPinPtr singlePin22 = buffer->GetPin("in", 2, 2);
		IOPinPtr singlePin02 = buffer->GetPin("in", 0, 2);
		IOPinPtr singlePin13 = buffer->GetPin("in", 1, 3);

		EXPECT_EQ(1, singlePin22->GetWidth());
		EXPECT_EQ(3, singlePin02->GetWidth());
		EXPECT_EQ(3, singlePin13->GetWidth());
		EXPECT_STREQ("in[0:2]", singlePin02->GetName().c_str());
		EXPECT_STREQ("in", singlePin02->GetRawName().c_str());
		EXPECT_STREQ("in[2]", singlePin22->GetName().c_str());

		TEST_COUT << "Change bus, check pin range";

		inPin->Set(IOState({ IOState::LOW, IOState::LOW, IOState::LOW, IOState::LOW }));
		EXPECT_EQ(singlePin02->Get(), inPin->Get().Get(0, 2));
		EXPECT_EQ(singlePin13->Get(), inPin->Get().Get(1, 3));

		inPin->Set(IOState({ IOState::LOW, IOState::LOW, IOState::HI, IOState::LOW }));
		EXPECT_EQ(singlePin02->Get(), inPin->Get().Get(0, 2));
		EXPECT_EQ(singlePin13->Get(), inPin->Get().Get(1, 3));

		TEST_COUT << "Change single pin, check pin range";
		singlePin22->Set(IOState::HI_Z);
		EXPECT_EQ(singlePin02->Get(), IOState({ IOState::LOW, IOState::LOW, IOState::HI_Z }));
		EXPECT_EQ(singlePin13->Get(), IOState({ IOState::LOW, IOState::HI_Z, IOState::LOW }));
		
		singlePin22->Set(IOState::UNDEF);
		EXPECT_EQ(singlePin02->Get(), IOState({ IOState::LOW, IOState::LOW, IOState::UNDEF }));
		EXPECT_EQ(singlePin13->Get(), IOState({ IOState::LOW, IOState::UNDEF, IOState::LOW }));

		TEST_COUT << "Change pin range, check bus";
		singlePin02->Set(IOState({ IOState::HI, IOState::HI, IOState::HI }));
		EXPECT_EQ(singlePin13->Get(), IOState({ IOState::HI, IOState::HI, IOState::LOW }));
		EXPECT_EQ(singlePin22->Get(), IOState::HI);
	}

	TEST(TestBus, SmallBusToWideBus)
	{
		TEST_COUT << "2x4bus -> 8bus";

		GatePtr buf4a = BuildBuffer(4);
		GatePtr buf4b = BuildBuffer(4);
		GatePtr buf8 = BuildBuffer(8);

		buf4a->GetPin("out")->ConnectTo(buf8->GetPin("in", 0, 3));
		buf4b->GetPin("out")->ConnectTo(buf8->GetPin("in", 4, 7));

		buf4a->GetPin("en")->Set(IOState::HI);
		buf4b->GetPin("en")->Set(IOState::HI);
		buf8->GetPin("en")->Set(IOState::HI);

		buf4a->GetPin("in")->Set(IOState({ IOState::LOW, IOState::LOW, IOState::LOW, IOState::LOW }));
		buf4b->GetPin("in")->Set(IOState({ IOState::HI, IOState::HI, IOState::HI, IOState::HI}));

		EXPECT_EQ(buf8->GetPin("out")->Get(), IOState({ IOState::LOW, IOState::LOW, IOState::LOW, IOState::LOW,
			IOState::HI, IOState::HI, IOState::HI, IOState::HI }));

		buf4a->GetPin("in")->Set(IOState({ IOState::HI, IOState::LOW, IOState::HI, IOState::LOW }));
		EXPECT_EQ(buf8->GetPin("out")->Get(), IOState({ IOState::HI, IOState::LOW, IOState::HI, IOState::LOW,
			IOState::HI, IOState::HI, IOState::HI, IOState::HI }));
	}

	TEST(TestBus, WideBusToSmallBus)
	{
		TEST_COUT << "8bus -> 2x4bus";

		GatePtr buf4a = BuildBuffer(4);
		GatePtr buf4b = BuildBuffer(4);
		GatePtr buf8 = BuildBuffer(8);

		buf8->GetPin("out", 0, 3)->ConnectTo(buf4a->GetPin("in"));
		buf8->GetPin("out", 4, 7)->ConnectTo(buf4b->GetPin("in"));

		buf4a->GetPin("en")->Set(IOState::HI);
		buf4b->GetPin("en")->Set(IOState::HI);
		buf8->GetPin("en")->Set(IOState::HI);

		buf8->GetPin("in")->Set(IOState({ IOState::LOW, IOState::LOW, IOState::LOW, IOState::LOW, 
			IOState::HI, IOState::HI, IOState::HI, IOState::HI }));

		EXPECT_EQ(buf4a->GetPin("out")->Get(), IOState({ IOState::LOW, IOState::LOW, IOState::LOW, IOState::LOW }));
		EXPECT_EQ(buf4b->GetPin("out")->Get(), IOState({ IOState::HI, IOState::HI, IOState::HI, IOState::HI }));

		buf8->GetPin("in")->Set(IOState({ IOState::HI, IOState::LOW, IOState::HI, IOState::LOW,
			IOState::LOW, IOState::HI, IOState::LOW, IOState::HI }));

		EXPECT_EQ(buf4a->GetPin("out")->Get(), IOState({ IOState::HI, IOState::LOW, IOState::HI, IOState::LOW }));
		EXPECT_EQ(buf4b->GetPin("out")->Get(), IOState({ IOState::LOW, IOState::HI, IOState::LOW, IOState::HI }));
	}

}

