#include "pch.h"
#pragma warning( disable:4251 )

#include "TestCommon.h"
#include "Core\IOState.h"
#include "Core\GateBase.h"
#include "Core\Simulator.h"
#include "BasicGates\ANDGate.h"
#include "BasicGates\BufferGate.h"
#include "Parser\TextParser.h"

namespace UnitTests
{
	using namespace Core;
	using namespace Parser;

	GatePtr BuildTestGate(bool addInputs = true, bool addOutputs = true, bool addGates = true)
	{
		CompositeGatePtr gate = CompositeGate::Create("gate");

		if (addInputs)
		{
			gate->AddInput("in1");
			gate->AddInput("in2");
			gate->AddInput("busin", 8);
		}
		
		if (addOutputs)
		{
			gate->AddOutput("out1");
			gate->AddOutput("out2");
			gate->AddOutput("busout", 8);
		}

		if (addGates)
		{
			gate->AddGate("and1", BasicGates::ANDGate::Create());
			gate->AddGate("and2", BasicGates::ANDGate::Create());
			gate->AddGate("buffer", BasicGates::BufferGate::Create(8));
		}

		return gate;
	}

	TEST(TestParser, Attach)
	{
		TextParser parser;
		EXPECT_EQ(nullptr, parser.Get());

		parser.Attach(nullptr);
		EXPECT_EQ(nullptr, parser.Get());

		GatePtr gate = BuildTestGate();
		parser.Attach(gate);
		EXPECT_EQ(gate, parser.Get());
	}

	TEST(TestParser, ParseConnections)
	{
		TextParser parser;
		GatePtr gate = BuildTestGate();
		parser.Attach(gate);

		EXPECT_THROW(parser.ParseConnection(nullptr), std::invalid_argument);
		EXPECT_THROW(parser.ParseConnection(""), std::invalid_argument);
		EXPECT_THROW(parser.ParseConnection(" "), std::invalid_argument);
		EXPECT_THROW(parser.ParseConnection("in2"), std::invalid_argument);
		EXPECT_THROW(parser.ParseConnection(" in2-"), std::invalid_argument);
		EXPECT_THROW(parser.ParseConnection(" in2-> "), std::invalid_argument);
		EXPECT_THROW(parser.ParseConnection(" ->in2 "), std::invalid_argument);
		EXPECT_THROW(parser.ParseConnection(" ->in2-> "), std::invalid_argument);
		EXPECT_THROW(parser.ParseConnection(" in1->i2- > "), std::invalid_argument);

		EXPECT_EQ(0, gate->GetConnectedFromPin("in2").size());
		EXPECT_EQ(0, gate->GetConnectedToPin("in2").size());

		//TODO how to indicate 'negative' connections?
		parser.ParseConnection("in1 -> and1.in1");
		parser.ParseConnection(" in1-> and1.in2 ");
		parser.ParseConnection(" in2-> \n\t and2.in1 ");
		parser.ParseConnection(" in2 \n -> \n and2.in2 ");
		parser.ParseConnection(" busin[1] -> \n\n\tbuffer.in[1] ");
		parser.ParseConnection("buffer.out ->busout ");

		EXPECT_EQ(2, gate->GetConnectedToPin("in1").size());
		EXPECT_EQ(2, gate->GetConnectedToPin("in2").size());
		EXPECT_EQ(1, gate->GetConnectedToPin("busin").size());
		EXPECT_EQ(1, gate->GetConnectedFromPin("busout").size());
	}
}