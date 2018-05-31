#include "pch.h"
#pragma warning( disable:4251 )

#include "TestCommon.h"
#include "Core/GateBase.h"
#include "Core/CompositeGate.h"
#include "BasicGates\NOTGate.h"
#include "BasicGates\ANDGate.h"
#include "BasicGates\NANDGate.h"
#include "BasicGates\ORGate.h"
#include "BasicGates\NORGate.h"
#include "BasicGates\XORGate.h"
#include "BasicGates\WireGate.h"
#include "BasicGates\BufferGate.h"
#include "BasicGates\SRLatch.h"
#include "BasicGates\DLatch.h"
#include "BasicGates\DFlipFlop.h"
#include "BasicGates\JKFlipFlop.h"
#include "BasicGates\TFlipFlop.h"
#include "Tools\LogicTools.h"

namespace UnitTests
{
	using namespace DigiLib::Core;

	GatePtr Build74163Counter()
	{
		CompositeGatePtr component = CompositeGate::Create("counter");
		component->AddInput("/ld");
		component->AddInput("/clr");
		component->AddInput("clk");
		component->AddInput("enp");
		component->AddInput("ent");
		component->AddInput("data", 4);

		component->AddOutput("q", 4);
		component->AddOutput("rco");
		
		component->AddGate("not1", BasicGates::NOTGate::Create());
		component->AddGate("not2", BasicGates::NOTGate::Create());
		component->AddGate("not4", BasicGates::NOTGate::Create());

		component->AddGate("a1", BasicGates::ANDGate::Create());
		component->AddGate("a2", BasicGates::ANDGate::Create(3));
		component->AddGate("a3", BasicGates::ANDGate::Create());
		component->AddGate("a4", BasicGates::ANDGate::Create());
		component->AddGate("a5", BasicGates::ANDGate::Create());
		component->AddGate("a6", BasicGates::ANDGate::Create());
		component->AddGate("a7", BasicGates::ANDGate::Create());
		component->AddGate("a8", BasicGates::ANDGate::Create());
		component->AddGate("a9", BasicGates::ANDGate::Create(5));
		component->AddGate("a10", BasicGates::ANDGate::Create());
		component->AddGate("a11", BasicGates::ANDGate::Create());
		component->AddGate("a12", BasicGates::ANDGate::Create());
		component->AddGate("a13", BasicGates::ANDGate::Create());
		component->AddGate("a14", BasicGates::ANDGate::Create());
		component->AddGate("a15", BasicGates::ANDGate::Create());
		component->AddGate("a16", BasicGates::ANDGate::Create());
		component->AddGate("a17", BasicGates::ANDGate::Create());

		component->AddGate("o1", BasicGates::ORGate::Create());
		component->AddGate("o2", BasicGates::ORGate::Create());
		component->AddGate("o3", BasicGates::ORGate::Create());
		component->AddGate("o4", BasicGates::ORGate::Create());

		component->AddGate("x1", BasicGates::XORGate::Create());
		component->AddGate("x2", BasicGates::XORGate::Create());
		component->AddGate("x3", BasicGates::XORGate::Create());
		component->AddGate("x4", BasicGates::XORGate::Create());

		component->AddGate("d1", BasicGates::DFlipFlop::Create(false));
		component->AddGate("d2", BasicGates::DFlipFlop::Create(false));
		component->AddGate("d3", BasicGates::DFlipFlop::Create(false));
		component->AddGate("d4", BasicGates::DFlipFlop::Create(false));
		
		// Clock
		component->GetPin("clk")->ConnectTo(component->GetGate("not1")->GetPin("in"));
		component->GetGate("not1")->GetPin("out")->ConnectTo(component->GetGate("d1")->GetPin("clk"));
		component->GetGate("not1")->GetPin("out")->ConnectTo(component->GetGate("d2")->GetPin("clk"));
		component->GetGate("not1")->GetPin("out")->ConnectTo(component->GetGate("d3")->GetPin("clk"));
		component->GetGate("not1")->GetPin("out")->ConnectTo(component->GetGate("d4")->GetPin("clk"));

		// Out
		component->GetGate("d1")->GetPin("q")->ConnectTo(component->GetPin("q", 0));
		component->GetGate("d2")->GetPin("q")->ConnectTo(component->GetPin("q", 1));
		component->GetGate("d3")->GetPin("q")->ConnectTo(component->GetPin("q", 2));
		component->GetGate("d4")->GetPin("q")->ConnectTo(component->GetPin("q", 3));

		// LD
		component->GetPin("/ld")->ConnectTo(component->GetGate("a7")->GetPin("in1"), true);

		// CLR
		component->GetPin("/clr")->ConnectTo(component->GetGate("not2")->GetPin("in"));
		component->GetGate("not2")->GetPin("out")->ConnectTo(component->GetGate("a7")->GetPin("in2"), true);
		component->GetGate("not2")->GetPin("out")->ConnectTo(component->GetGate("a8")->GetPin("in2"), true);
		component->GetGate("a7")->GetPin("out")->ConnectTo(component->GetGate("a8")->GetPin("in1"), true);
		component->GetGate("a7")->GetPin("out")->ConnectTo(component->GetGate("a10")->GetPin("in1"));
		component->GetGate("a7")->GetPin("out")->ConnectTo(component->GetGate("a12")->GetPin("in1"));
		component->GetGate("a7")->GetPin("out")->ConnectTo(component->GetGate("a14")->GetPin("in1"));
		component->GetGate("a7")->GetPin("out")->ConnectTo(component->GetGate("a16")->GetPin("in1"));
		component->GetGate("a8")->GetPin("out")->ConnectTo(component->GetGate("a11")->GetPin("in1"));
		component->GetGate("a8")->GetPin("out")->ConnectTo(component->GetGate("a13")->GetPin("in1"));
		component->GetGate("a8")->GetPin("out")->ConnectTo(component->GetGate("a15")->GetPin("in1"));
		component->GetGate("a8")->GetPin("out")->ConnectTo(component->GetGate("a17")->GetPin("in1"));

		// DATA
		component->GetPin("data", 0)->ConnectTo(component->GetGate("a10")->GetPin("in2"));
		component->GetPin("data", 1)->ConnectTo(component->GetGate("a12")->GetPin("in2"));
		component->GetPin("data", 2)->ConnectTo(component->GetGate("a14")->GetPin("in2"));
		component->GetPin("data", 3)->ConnectTo(component->GetGate("a16")->GetPin("in2"));

		// flip-D
		component->GetGate("a10")->GetPin("out")->ConnectTo(component->GetGate("o1")->GetPin("in1"));
		component->GetGate("a12")->GetPin("out")->ConnectTo(component->GetGate("o2")->GetPin("in1"));
		component->GetGate("a14")->GetPin("out")->ConnectTo(component->GetGate("o3")->GetPin("in1"));
		component->GetGate("a16")->GetPin("out")->ConnectTo(component->GetGate("o4")->GetPin("in1"));

		component->GetGate("a11")->GetPin("out")->ConnectTo(component->GetGate("o1")->GetPin("in2"));
		component->GetGate("a13")->GetPin("out")->ConnectTo(component->GetGate("o2")->GetPin("in2"));
		component->GetGate("a15")->GetPin("out")->ConnectTo(component->GetGate("o3")->GetPin("in2"));
		component->GetGate("a17")->GetPin("out")->ConnectTo(component->GetGate("o4")->GetPin("in2"));

		component->GetGate("o1")->GetPin("out")->ConnectTo(component->GetGate("d1")->GetPin("d"));
		component->GetGate("o2")->GetPin("out")->ConnectTo(component->GetGate("d2")->GetPin("d"));
		component->GetGate("o3")->GetPin("out")->ConnectTo(component->GetGate("d3")->GetPin("d"));
		component->GetGate("o4")->GetPin("out")->ConnectTo(component->GetGate("d4")->GetPin("d"));

		// ENP ENT
		component->GetPin("enp")->ConnectTo(component->GetGate("a3")->GetPin("in1"));
		component->GetPin("ent")->ConnectTo(component->GetGate("a3")->GetPin("in2"));
		component->GetPin("ent")->ConnectTo(component->GetGate("not4")->GetPin("in"));
		component->GetGate("not4")->GetPin("out")->ConnectTo(component->GetGate("a9")->GetPin("in5"), true);
		
		component->GetGate("a3")->GetPin("out")->ConnectTo(component->GetGate("x1")->GetPin("in1"));
		component->GetGate("a3")->GetPin("out")->ConnectTo(component->GetGate("a4")->GetPin("in2"));
		component->GetGate("a3")->GetPin("out")->ConnectTo(component->GetGate("a5")->GetPin("in2"));
		component->GetGate("a3")->GetPin("out")->ConnectTo(component->GetGate("a6")->GetPin("in2"));

		// notq1
		component->GetGate("d1")->GetPin("/q")->ConnectTo(component->GetGate("x1")->GetPin("in2"), true);
		component->GetGate("d1")->GetPin("/q")->ConnectTo(component->GetGate("a4")->GetPin("in1"), true);
		component->GetGate("d1")->GetPin("/q")->ConnectTo(component->GetGate("a1")->GetPin("in2"), true);
		component->GetGate("d1")->GetPin("/q")->ConnectTo(component->GetGate("a2")->GetPin("in3"), true);
		component->GetGate("d1")->GetPin("/q")->ConnectTo(component->GetGate("a9")->GetPin("in4"), true);

		// notq2
		component->GetGate("d2")->GetPin("/q")->ConnectTo(component->GetGate("x2")->GetPin("in2"), true);
		component->GetGate("d2")->GetPin("/q")->ConnectTo(component->GetGate("a1")->GetPin("in1"), true);
		component->GetGate("d2")->GetPin("/q")->ConnectTo(component->GetGate("a2")->GetPin("in2"), true);
		component->GetGate("d2")->GetPin("/q")->ConnectTo(component->GetGate("a9")->GetPin("in3"), true);

		// notq3
		component->GetGate("d3")->GetPin("/q")->ConnectTo(component->GetGate("x3")->GetPin("in2"), true);
		component->GetGate("d3")->GetPin("/q")->ConnectTo(component->GetGate("a2")->GetPin("in1"), true);
		component->GetGate("d3")->GetPin("/q")->ConnectTo(component->GetGate("a9")->GetPin("in2"), true);

		// notq4
		component->GetGate("d4")->GetPin("/q")->ConnectTo(component->GetGate("x4")->GetPin("in2"), true);
		component->GetGate("d4")->GetPin("/q")->ConnectTo(component->GetGate("a9")->GetPin("in1"), true);

		// misc
		component->GetGate("a1")->GetPin("out")->ConnectTo(component->GetGate("a5")->GetPin("in1"));
		component->GetGate("a4")->GetPin("out")->ConnectTo(component->GetGate("x2")->GetPin("in1"));
		component->GetGate("a5")->GetPin("out")->ConnectTo(component->GetGate("x3")->GetPin("in1"));
		component->GetGate("a2")->GetPin("out")->ConnectTo(component->GetGate("a6")->GetPin("in1"));
		component->GetGate("a6")->GetPin("out")->ConnectTo(component->GetGate("x4")->GetPin("in1"));

		// xor
		component->GetGate("x1")->GetPin("out")->ConnectTo(component->GetGate("a11")->GetPin("in2"));
		component->GetGate("x2")->GetPin("out")->ConnectTo(component->GetGate("a13")->GetPin("in2"));
		component->GetGate("x3")->GetPin("out")->ConnectTo(component->GetGate("a15")->GetPin("in2"));
		component->GetGate("x4")->GetPin("out")->ConnectTo(component->GetGate("a17")->GetPin("in2"));

		// RCO
		component->GetGate("a9")->GetPin("out")->ConnectTo(component->GetPin("rco"));

		return component;
	}

	GatePtr BuildFullAdder()
	{
		CompositeGatePtr component = CompositeGate::Create("FULLADDER");
		component->AddInput("x");
		component->AddInput("y");
		component->AddInput("cin");
		component->AddOutput("s");
		component->AddOutput("cout");

		GatePtr xor1 = BasicGates::XORGate::Create();
		GatePtr xor2 = BasicGates::XORGate::Create();

		GatePtr and1 = BasicGates::ANDGate::Create();
		GatePtr and2 = BasicGates::ANDGate::Create();
		GatePtr and3 = BasicGates::ANDGate::Create();

		GatePtr or1 = BasicGates::ORGate::Create(3);

		component->AddGate("xor1", xor1);
		component->AddGate("xor2", xor2);
		component->AddGate("and1", and1);
		component->AddGate("and2", and2);
		component->AddGate("and3", and3);
		component->AddGate("or1", or1);

		component->GetPin("x")->ConnectTo(xor1->GetPin("in1"));
		component->GetPin("x")->ConnectTo(and1->GetPin("in2"));
		component->GetPin("x")->ConnectTo(and3->GetPin("in2"));

		component->GetPin("y")->ConnectTo(xor1->GetPin("in2"));
		component->GetPin("y")->ConnectTo(and2->GetPin("in2"));
		component->GetPin("y")->ConnectTo(and3->GetPin("in1"));

		component->GetPin("cin")->ConnectTo(xor2->GetPin("in2"));
		component->GetPin("cin")->ConnectTo(and1->GetPin("in1"));
		component->GetPin("cin")->ConnectTo(and2->GetPin("in1"));

		xor1->GetPin("out")->ConnectTo(xor2->GetPin("in1"));
		xor2->GetPin("out")->ConnectTo(component->GetPin("s"));

		and1->GetPin("out")->ConnectTo(or1->GetPin("in1"));
		and2->GetPin("out")->ConnectTo(or1->GetPin("in2"));
		and3->GetPin("out")->ConnectTo(or1->GetPin("in3"));

		or1->GetPin("out")->ConnectTo(component->GetPin("cout"));

		return component;
	}

	GatePtr Build4BitAdder()
	{
		CompositeGatePtr adder4 = CompositeGate::Create("adder4");
		adder4->AddInput("x", 4);
		adder4->AddInput("y", 4);
		adder4->AddInput("cin");
		adder4->AddOutput("s", 4);
		adder4->AddOutput("cout");

		adder4->AddGate("a0", BuildFullAdder());
		adder4->AddGate("a1", BuildFullAdder());
		adder4->AddGate("a2", BuildFullAdder());
		adder4->AddGate("a3", BuildFullAdder());

		adder4->GetPin("x", 0)->ConnectTo(adder4->GetGate("a0")->GetPin("x"));
		adder4->GetPin("x", 1)->ConnectTo(adder4->GetGate("a1")->GetPin("x"));
		adder4->GetPin("x", 2)->ConnectTo(adder4->GetGate("a2")->GetPin("x"));
		adder4->GetPin("x", 3)->ConnectTo(adder4->GetGate("a3")->GetPin("x"));

		adder4->GetPin("y", 0)->ConnectTo(adder4->GetGate("a0")->GetPin("y"));
		adder4->GetPin("y", 1)->ConnectTo(adder4->GetGate("a1")->GetPin("y"));
		adder4->GetPin("y", 2)->ConnectTo(adder4->GetGate("a2")->GetPin("y"));
		adder4->GetPin("y", 3)->ConnectTo(adder4->GetGate("a3")->GetPin("y"));

		adder4->GetGate("a0")->GetPin("s")->ConnectTo(adder4->GetPin("s", 0));
		adder4->GetGate("a1")->GetPin("s")->ConnectTo(adder4->GetPin("s", 1));
		adder4->GetGate("a2")->GetPin("s")->ConnectTo(adder4->GetPin("s", 2));
		adder4->GetGate("a3")->GetPin("s")->ConnectTo(adder4->GetPin("s", 3));

		adder4->GetPin("cin")->ConnectTo(adder4->GetGate("a0")->GetPin("cin"));
		adder4->GetGate("a0")->GetPin("cout")->ConnectTo(adder4->GetGate("a1")->GetPin("cin"));
		adder4->GetGate("a1")->GetPin("cout")->ConnectTo(adder4->GetGate("a2")->GetPin("cin"));
		adder4->GetGate("a2")->GetPin("cout")->ConnectTo(adder4->GetGate("a3")->GetPin("cin"));
		adder4->GetGate("a3")->GetPin("cout")->ConnectTo(adder4->GetPin("cout"));

		return adder4;
	}

	GatePtr BuildDecoder()
	{
		CompositeGatePtr component = CompositeGate::Create("DECODER");
		component->AddInput("en");
		component->AddInput("i0");
		component->AddInput("i1");
		component->AddOutput("y0");
		component->AddOutput("y1");
		component->AddOutput("y2");
		component->AddOutput("y3");

		component->AddGate("noti0", BasicGates::NOTGate::Create());
		component->AddGate("noti1", BasicGates::NOTGate::Create());
		component->AddGate("and0", BasicGates::ANDGate::Create(3));
		component->AddGate("and1", BasicGates::ANDGate::Create(3));
		component->AddGate("and2", BasicGates::ANDGate::Create(3));
		component->AddGate("and3", BasicGates::ANDGate::Create(3));

		component->GetGate("and0")->GetPin("out")->ConnectTo(component->GetPin("y0"));
		component->GetGate("and1")->GetPin("out")->ConnectTo(component->GetPin("y1"));
		component->GetGate("and2")->GetPin("out")->ConnectTo(component->GetPin("y2"));
		component->GetGate("and3")->GetPin("out")->ConnectTo(component->GetPin("y3"));

		component->GetPin("en")->ConnectTo(component->GetGate("and0")->GetPin("in3"));
		component->GetPin("en")->ConnectTo(component->GetGate("and1")->GetPin("in3"));
		component->GetPin("en")->ConnectTo(component->GetGate("and2")->GetPin("in3"));
		component->GetPin("en")->ConnectTo(component->GetGate("and3")->GetPin("in3"));

		component->GetPin("i0")->ConnectTo(component->GetGate("noti0")->GetPin("in"));
		component->GetPin("i0")->ConnectTo(component->GetGate("and1")->GetPin("in1"));
		component->GetPin("i0")->ConnectTo(component->GetGate("and3")->GetPin("in1"));

		component->GetPin("i1")->ConnectTo(component->GetGate("noti1")->GetPin("in"));
		component->GetPin("i1")->ConnectTo(component->GetGate("and2")->GetPin("in2"));
		component->GetPin("i1")->ConnectTo(component->GetGate("and3")->GetPin("in2"));

		component->GetGate("noti0")->GetPin("out")->ConnectTo(component->GetGate("and0")->GetPin("in1"));
		component->GetGate("noti0")->GetPin("out")->ConnectTo(component->GetGate("and2")->GetPin("in1"));

		component->GetGate("noti1")->GetPin("out")->ConnectTo(component->GetGate("and0")->GetPin("in2"));
		component->GetGate("noti1")->GetPin("out")->ConnectTo(component->GetGate("and1")->GetPin("in2"));

		return component;
	}

	TEST(TestGates, NOTGate)
	{
		GatePtr gate = BasicGates::NOTGate::Create();
		Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::ResultListType compare({ IOState::HI, IOState::LOW });

		EXPECT_EQ(compare, out);

		gate->GetPin("in")->Set(IOState::UNDEF);
		EXPECT_EQ(IOState::UNDEF, gate->GetPin("out")->Get().Get());
	}

	TEST(TestGates, WireGate)
	{
		GatePtr gate = BasicGates::WireGate::Create();
		Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::ResultListType compare({ IOState::LOW, IOState::HI });

		EXPECT_EQ(compare, out);

		gate->GetPin("in")->Set(IOState::UNDEF);
		EXPECT_EQ(IOState::UNDEF, gate->GetPin("out")->Get().Get());
	}

	TEST(TestGates, ANDGate)
	{
		GatePtr gate = BasicGates::ANDGate::Create();
		Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::ResultListType compare({ IOState::LOW, IOState::LOW, IOState::LOW, IOState::HI });
		EXPECT_EQ(compare, out);
	}

	TEST(TestGates, ANDGate4)
	{
		GatePtr gate = BasicGates::ANDGate::Create(4);
		Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::ResultListType compare({
			IOState::LOW, IOState::LOW, IOState::LOW, IOState::LOW,
			IOState::LOW, IOState::LOW, IOState::LOW, IOState::LOW,
			IOState::LOW, IOState::LOW, IOState::LOW, IOState::LOW,
			IOState::LOW, IOState::LOW, IOState::LOW, IOState::HI,
		});
		EXPECT_EQ(compare, out);
	}

	TEST(TestGates, ORGate)
	{
		GatePtr gate = BasicGates::ORGate::Create();
		Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::ResultListType compare({ IOState::LOW, IOState::HI, IOState::HI, IOState::HI });
		EXPECT_EQ(compare, out);
	}

	TEST(TestGates, ORGate4)
	{
		GatePtr gate = BasicGates::ORGate::Create(4);
		Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::ResultListType compare = {
			IOState::LOW, IOState::HI, IOState::HI, IOState::HI,
			IOState::HI, IOState::HI, IOState::HI, IOState::HI,
			IOState::HI, IOState::HI, IOState::HI, IOState::HI,
			IOState::HI, IOState::HI, IOState::HI, IOState::HI,
		};
		EXPECT_EQ(compare, out);
	}

	TEST(TestGates, XORGate)
	{
		GatePtr gate = BasicGates::XORGate::Create();
		Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::ResultListType compare({ IOState::LOW, IOState::HI, IOState::HI, IOState::LOW });
		EXPECT_EQ(compare, out);
	}

	TEST(TestGates, NANDGate)
	{
		GatePtr gate = BasicGates::NANDGate::Create();
		Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::ResultListType compare({ IOState::HI, IOState::HI, IOState::HI, IOState::LOW });
		EXPECT_EQ(compare, out);
	}

	TEST(TestGates, NANDGate4)
	{
		GatePtr gate = BasicGates::NANDGate::Create(4);
		Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::ResultListType compare({
			IOState::HI, IOState::HI, IOState::HI, IOState::HI,
			IOState::HI, IOState::HI, IOState::HI, IOState::HI,
			IOState::HI, IOState::HI, IOState::HI, IOState::HI,
			IOState::HI, IOState::HI, IOState::HI, IOState::LOW,
		});
		EXPECT_EQ(compare, out);
	}

	TEST(TestGates, CustomDecoder)
	{
		GatePtr component = BuildDecoder();
		
		ASSERT_NE(nullptr, component);

		Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(component);
		Tools::LogicTools::ResultListType compare = {
			IOState::LOW, IOState::LOW, IOState::LOW, IOState::LOW,
			IOState::LOW, IOState::LOW, IOState::LOW, IOState::LOW,
			IOState::LOW, IOState::LOW, IOState::LOW, IOState::LOW,
			IOState::LOW, IOState::LOW, IOState::LOW, IOState::LOW,
			IOState::HI, IOState::LOW, IOState::LOW, IOState::LOW,
			IOState::LOW, IOState::LOW, IOState::HI, IOState::LOW,
			IOState::LOW, IOState::HI, IOState::LOW, IOState::LOW,
			IOState::LOW, IOState::LOW, IOState::LOW, IOState::HI,
		};
		EXPECT_EQ(compare, out);
	}

	TEST(TestGates, CustomComplexComponent)
	{
		GatePtr decoder1 = BuildDecoder();
		ASSERT_NE(nullptr, decoder1);

		GatePtr decoder2 = BuildDecoder();
		ASSERT_NE(nullptr, decoder2);

		GatePtr notI1 = BasicGates::NOTGate::Create();

		CompositeGatePtr decoder3to8 = CompositeGate::Create("Decoder3to8");
		decoder3to8->AddInput("i0");
		decoder3to8->AddInput("i1");
		decoder3to8->AddInput("i2");
		decoder3to8->AddOutput("y0");
		decoder3to8->AddOutput("y1");
		decoder3to8->AddOutput("y2");
		decoder3to8->AddOutput("y3");
		decoder3to8->AddOutput("y4");
		decoder3to8->AddOutput("y5");
		decoder3to8->AddOutput("y6");
		decoder3to8->AddOutput("y7");

		decoder3to8->AddGate("dec1", decoder1);
		decoder3to8->AddGate("dec2", decoder2);
		decoder3to8->AddGate("notI1", notI1);

		decoder1->GetPin("y0")->ConnectTo(decoder3to8->GetPin("y0"));
		decoder1->GetPin("y1")->ConnectTo(decoder3to8->GetPin("y1"));
		decoder1->GetPin("y2")->ConnectTo(decoder3to8->GetPin("y2"));
		decoder1->GetPin("y3")->ConnectTo(decoder3to8->GetPin("y3"));
		decoder2->GetPin("y0")->ConnectTo(decoder3to8->GetPin("y4"));
		decoder2->GetPin("y1")->ConnectTo(decoder3to8->GetPin("y5"));
		decoder2->GetPin("y2")->ConnectTo(decoder3to8->GetPin("y6"));
		decoder2->GetPin("y3")->ConnectTo(decoder3to8->GetPin("y7"));

		decoder3to8->GetPin("i0")->ConnectTo(decoder1->GetPin("i0"));
		decoder3to8->GetPin("i0")->ConnectTo(decoder2->GetPin("i0"));
		decoder3to8->GetPin("i1")->ConnectTo(decoder1->GetPin("i1"));
		decoder3to8->GetPin("i1")->ConnectTo(decoder2->GetPin("i1"));
		decoder3to8->GetPin("i2")->ConnectTo(decoder2->GetPin("en"));
		decoder3to8->GetPin("i2")->ConnectTo(notI1->GetPin("in"));
		notI1->GetPin("out")->ConnectTo(decoder1->GetPin("en"));

		Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(decoder3to8);

		Tools::LogicTools::ResultListType compare = {
			IOState::HI, IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,
			IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::HI ,IOState::LOW,IOState::LOW,IOState::LOW,
			IOState::LOW,IOState::LOW,IOState::HI ,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,
			IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::HI ,IOState::LOW,
			IOState::LOW,IOState::HI ,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,
			IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::HI ,IOState::LOW,IOState::LOW,
			IOState::LOW,IOState::LOW,IOState::LOW,IOState::HI ,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,
			IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::HI 
		};

		EXPECT_EQ(compare, out);
	}

	TEST(TestGates, Clone)
	{
		CompositeGatePtr comp = CompositeGate::Create("comp");

		GatePtr andGate = BasicGates::ANDGate::Create();

		// Share one input to avoir truth table explosion
		IOPinPtr input = comp->AddInput("masterIn");

		comp->AddGate("and", andGate);
		input->ConnectTo(andGate->GetPin("in1"));
		comp->AddInput("and2")->ConnectTo(andGate->GetPin("in2"));
		andGate->GetPin("out")->ConnectTo(comp->AddOutput("andout"));

		GatePtr orGate = BasicGates::ORGate::Create();
		comp->AddGate("or", orGate);
		input->ConnectTo(orGate->GetPin("in1"));
		comp->AddInput("or2")->ConnectTo(orGate->GetPin("in2"));
		orGate->GetPin("out")->ConnectTo(comp->AddOutput("orout"));

		GatePtr xorGate = BasicGates::XORGate::Create();
		comp->AddGate("xor", xorGate);
		input->ConnectTo(xorGate->GetPin("in1"));
		comp->AddInput("xor2")->ConnectTo(xorGate->GetPin("in2"));
		xorGate->GetPin("out")->ConnectTo(comp->AddOutput("xorout"));

		GatePtr nandGate = BasicGates::NANDGate::Create();
		comp->AddGate("nand", nandGate);
		input->ConnectTo(nandGate->GetPin("in1"));
		comp->AddInput("nand2")->ConnectTo(nandGate->GetPin("in2"));
		nandGate->GetPin("out")->ConnectTo(comp->AddOutput("nandout"));

		GatePtr notGate = BasicGates::NOTGate::Create();
		comp->AddGate("not", notGate);
		input->ConnectTo(notGate->GetPin("in"));
		notGate->GetPin("out")->ConnectTo(comp->AddOutput("notout"));

		GatePtr wireGate = BasicGates::WireGate::Create();
		comp->AddGate("wire", wireGate);
		input->ConnectTo(wireGate->GetPin("in"));
		wireGate->GetPin("out")->ConnectTo(comp->AddOutput("wireout"));

		GatePtr bufferGate = BasicGates::BufferGate::Create();
		comp->AddGate("buffer", bufferGate);
		input->ConnectTo(bufferGate->GetPin("in"));
		comp->AddInput("bufferEN")->ConnectTo(bufferGate->GetPin("en"));
		bufferGate->GetPin("out")->ConnectTo(comp->AddOutput("bufferout"));

		GatePtr norGate = BasicGates::NORGate::Create();
		comp->AddGate("nor", norGate);
		input->ConnectTo(norGate->GetPin("in1"));
		comp->AddInput("nor2")->ConnectTo(norGate->GetPin("in2"));
		norGate->GetPin("out")->ConnectTo(comp->AddOutput("norout"));

		EXPECT_EQ(8, comp->GetGateCount());

		Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(comp);
		EXPECT_EQ(1024, out.size());

		GatePtr clone = comp->Clone("clone");
		ASSERT_NE(nullptr, clone);
		EXPECT_NE(comp, clone);

		Tools::LogicTools::ResultListType outClone = Tools::LogicTools::GetTruthTable(clone);
		EXPECT_EQ(1024, outClone.size());

		EXPECT_EQ(out, outClone);
	}

	TEST(TestGates, FullAdder)
	{
		GatePtr gate = BuildFullAdder();		
		Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::ResultListType compare({ IOState::LOW, IOState::LOW, IOState::LOW, IOState::HI, 
			IOState::LOW, IOState::HI, IOState::HI, IOState::LOW,
			IOState::LOW, IOState::HI, IOState::HI, IOState::LOW,
			IOState::HI, IOState::LOW, IOState::HI, IOState::HI });
		EXPECT_EQ(compare, out);

		TEST_COUT << "Building 4 bit adder";
		GatePtr adder4 = Build4BitAdder();

		adder4->GetPin("cin")->Set(IOState::LOW);
		adder4->GetPin("x")->Set(IOState::FromInt(2, 4));
		adder4->GetPin("y")->Set(IOState::FromInt(2, 4));
		EXPECT_EQ(IOState::FromInt(4, 4), adder4->GetPin("s")->Get());
		EXPECT_EQ(IOState(IOState::LOW), adder4->GetPin("cout")->Get());

		adder4->GetPin("x")->Set(IOState::FromInt(10, 4));
		adder4->GetPin("y")->Set(IOState::FromInt(5, 4));
		EXPECT_EQ(IOState::FromInt(15, 4), adder4->GetPin("s")->Get());
		EXPECT_EQ(IOState(IOState::LOW), adder4->GetPin("cout")->Get());

		adder4->GetPin("x")->Set(IOState::FromInt(15, 4));
		adder4->GetPin("y")->Set(IOState::FromInt(1, 4));
		EXPECT_EQ(IOState::FromInt(0, 4), adder4->GetPin("s")->Get());
		EXPECT_EQ(IOState(IOState::HI), adder4->GetPin("cout")->Get());

		adder4->GetPin("x")->Set(IOState::FromInt(15, 4));
		adder4->GetPin("y")->Set(IOState::FromInt(15, 4));
		EXPECT_EQ(IOState::FromInt(14, 4), adder4->GetPin("s")->Get());
		EXPECT_EQ(IOState(IOState::HI), adder4->GetPin("cout")->Get());

		adder4->GetPin("cin")->Set(IOState::HI);
		adder4->GetPin("x")->Set(IOState::FromInt(15, 4));
		adder4->GetPin("y")->Set(IOState::FromInt(15, 4));
		EXPECT_EQ(IOState::FromInt(15, 4), adder4->GetPin("s")->Get());
		EXPECT_EQ(IOState(IOState::HI), adder4->GetPin("cout")->Get());
	}

	TEST(TestGates, SRLatch)
	{
		GatePtr gate = BasicGates::SRLatch::Create();
		gate->GetPin("s")->Set(IOState::LOW);
		gate->GetPin("r")->Set(IOState::HI);

		EXPECT_EQ(IOState(IOState::LOW), gate->GetPin("q")->Get());
		EXPECT_EQ(IOState(IOState::HI), gate->GetPin("/q")->Get());

		gate->GetPin("r")->Set(IOState::LOW);

		EXPECT_EQ(IOState(IOState::LOW), gate->GetPin("q")->Get());
		EXPECT_EQ(IOState(IOState::HI), gate->GetPin("/q")->Get());

		gate->GetPin("s")->Set(IOState::HI);

		EXPECT_EQ(IOState(IOState::HI), gate->GetPin("q")->Get());
		EXPECT_EQ(IOState(IOState::LOW), gate->GetPin("/q")->Get());

		gate->GetPin("s")->Set(IOState::LOW);

		EXPECT_EQ(IOState(IOState::HI), gate->GetPin("q")->Get());
		EXPECT_EQ(IOState(IOState::LOW), gate->GetPin("/q")->Get());

		gate->GetPin("r")->Set(IOState::HI);

		EXPECT_EQ(IOState(IOState::LOW), gate->GetPin("q")->Get());
		EXPECT_EQ(IOState(IOState::HI), gate->GetPin("/q")->Get());

		gate->GetPin("r")->Set(IOState::LOW);

		EXPECT_EQ(IOState(IOState::LOW), gate->GetPin("q")->Get());
		EXPECT_EQ(IOState(IOState::HI), gate->GetPin("/q")->Get());

		// Bistable?
	//	gate->GetPin("r")->Set(IOState::HI);
	//	gate->GetPin("s")->Set(IOState::HI);
		
	}
}