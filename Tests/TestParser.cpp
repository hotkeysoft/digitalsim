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

	CompositeGatePtr BuildTestGate(bool addInputs = true, bool addOutputs = true, bool addGates = true)
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

		CompositeGatePtr gate = BuildTestGate();
		parser.Attach(gate);
		EXPECT_EQ(gate, parser.Get());
	}

	TEST(TestParser, ParseConnections)
	{
		TextParser parser;
		CompositeGatePtr gate = BuildTestGate();
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

		EXPECT_EQ(0, gate->GetConnectedToPin("in1").size());
		EXPECT_EQ(0, gate->GetConnectedToPin("in2").size());
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

	TEST(TestParser, WireStatement)
	{
		TextParser parser;
		CompositeGatePtr gate = BuildTestGate();
		parser.Attach(gate);

		EXPECT_THROW(parser.ParseWireSection(nullptr), std::invalid_argument);
		EXPECT_THROW(parser.ParseWireSection(""), std::invalid_argument);
		EXPECT_THROW(parser.ParseWireSection(" "), std::invalid_argument);
		EXPECT_THROW(parser.ParseWireSection(" in1-> and1.in1, "), std::invalid_argument);
		EXPECT_THROW(parser.ParseWireSection(" , in1-> and1.in1, "), std::invalid_argument);
		EXPECT_THROW(parser.ParseWireSection(" in1-> and1.in1,, in1-> and1.in2"), std::invalid_argument);
		EXPECT_THROW(parser.ParseWireSection(" in1-> and1.in1 in1-> and1.in2,,"), std::invalid_argument);
		EXPECT_THROW(parser.ParseWireSection(" in1->and1.in2 , in1, -> and1.in2"), std::invalid_argument);

		// Rebuild gate some valid connections could be there
		gate = BuildTestGate();
		parser.Attach(gate);

		parser.ParseWireSection("  in1 -> and1.in1 ,\n in1-> and1.in2 ");
		parser.ParseWireSection(" in2-> \n\t and2.in1 , in2 \n -> \n and2.in2 ");
		parser.ParseWireSection(" \n\tbusin[1] -> \n\n\t buffer.in[1] , \n\t\t buffer.out ->busout \n");

		EXPECT_EQ(2, gate->GetConnectedToPin("in1").size());
		EXPECT_EQ(2, gate->GetConnectedToPin("in2").size());
		EXPECT_EQ(1, gate->GetConnectedToPin("busin").size());
		EXPECT_EQ(1, gate->GetConnectedFromPin("busout").size());
	}

	TEST(TestParser, InputsStatement)
	{
		TextParser parser;
		CompositeGatePtr gate = BuildTestGate(false, false, false);
		parser.Attach(gate);

		EXPECT_THROW(parser.ParseInputsSection(nullptr), std::invalid_argument);
		EXPECT_THROW(parser.ParseInputsSection(""), std::invalid_argument);
		EXPECT_THROW(parser.ParseInputsSection(" "), std::invalid_argument);
		EXPECT_THROW(parser.ParseInputsSection(" in1 in2, "), std::invalid_argument);
		EXPECT_THROW(parser.ParseInputsSection(" , in3 "), std::invalid_argument);
		EXPECT_THROW(parser.ParseInputsSection(" in4 ,, /in5"), std::invalid_argument);
		EXPECT_THROW(parser.ParseInputsSection(" in6[4] ,,"), std::invalid_argument);
		EXPECT_THROW(parser.ParseInputsSection(" in7, //in8, in9"), std::invalid_argument);
		EXPECT_THROW(parser.ParseInputsSection(" in10[4:8]"), std::invalid_argument);
		EXPECT_THROW(parser.ParseInputsSection(" in11[17]"), std::out_of_range);

		// Rebuild gate some valid connections could be there
		gate = BuildTestGate(false, false, false);
		parser.Attach(gate);

		parser.ParseInputsSection("in1");
		EXPECT_EQ(1, gate->GetInputCount());
		parser.ParseInputsSection("in2, in3");
		EXPECT_EQ(3, gate->GetInputCount());
		parser.ParseInputsSection("in4[6], /in5[2]");
		EXPECT_EQ(5, gate->GetInputCount());
		parser.ParseInputsSection("in6[15], /input7, /en \n\n\t ");
		EXPECT_EQ(8, gate->GetInputCount());
		parser.ParseInputsSection(" in11[1], in12[16]");
		EXPECT_EQ(10, gate->GetInputCount());

		EXPECT_EQ(1, gate->GetPin("in1")->GetWidth());
		EXPECT_EQ(IOPin::INPUT, gate->GetPin("in1")->GetDirection());
		EXPECT_EQ(1, gate->GetPin("in2")->GetWidth());
		EXPECT_EQ(1, gate->GetPin("in3")->GetWidth());
		EXPECT_EQ(6, gate->GetPin("in4")->GetWidth());
		EXPECT_EQ(2, gate->GetPin("/in5")->GetWidth());
		EXPECT_EQ(15, gate->GetPin("in6")->GetWidth());
		EXPECT_EQ(1, gate->GetPin("/input7")->GetWidth());
		EXPECT_EQ(1, gate->GetPin("/en")->GetWidth());
		EXPECT_EQ(1, gate->GetPin("in11")->GetWidth());
		EXPECT_EQ(16, gate->GetPin("in12")->GetWidth());
	}

	TEST(TestParser, OutputsStatement)
	{
		TextParser parser;
		CompositeGatePtr gate = BuildTestGate(false, false, false);
		parser.Attach(gate);

		EXPECT_THROW(parser.ParseOutputsSection(nullptr), std::invalid_argument);
		EXPECT_THROW(parser.ParseOutputsSection(""), std::invalid_argument);
		EXPECT_THROW(parser.ParseOutputsSection(" "), std::invalid_argument);
		EXPECT_THROW(parser.ParseOutputsSection(" in1 in2, "), std::invalid_argument);
		EXPECT_THROW(parser.ParseOutputsSection(" , in3 "), std::invalid_argument);
		EXPECT_THROW(parser.ParseOutputsSection(" in4 ,, /in5"), std::invalid_argument);
		EXPECT_THROW(parser.ParseOutputsSection(" in6[4] ,,"), std::invalid_argument);
		EXPECT_THROW(parser.ParseOutputsSection(" in7, //in8, in9"), std::invalid_argument);
		EXPECT_THROW(parser.ParseOutputsSection(" in10[4:8]"), std::invalid_argument);
		EXPECT_THROW(parser.ParseOutputsSection(" in11[17]"), std::out_of_range);

		// Rebuild gate some valid connections could be there
		gate = BuildTestGate(false, false, false);
		parser.Attach(gate);

		parser.ParseOutputsSection("in1");
		EXPECT_EQ(1, gate->GetOutputCount());
		parser.ParseOutputsSection("in2, in3");
		EXPECT_EQ(3, gate->GetOutputCount());
		parser.ParseOutputsSection("in4[6], /in5[2]");
		EXPECT_EQ(5, gate->GetOutputCount());
		parser.ParseOutputsSection("in6[15], /input7, /en \n\n\t ");
		EXPECT_EQ(8, gate->GetOutputCount());
		parser.ParseOutputsSection(" in11[1], in12[16]");
		EXPECT_EQ(10, gate->GetOutputCount());

		EXPECT_EQ(1, gate->GetPin("in1")->GetWidth());
		EXPECT_EQ(IOPin::OUTPUT, gate->GetPin("in1")->GetDirection());
		EXPECT_EQ(1, gate->GetPin("in2")->GetWidth());
		EXPECT_EQ(1, gate->GetPin("in3")->GetWidth());
		EXPECT_EQ(6, gate->GetPin("in4")->GetWidth());
		EXPECT_EQ(2, gate->GetPin("/in5")->GetWidth());
		EXPECT_EQ(15, gate->GetPin("in6")->GetWidth());
		EXPECT_EQ(1, gate->GetPin("/input7")->GetWidth());
		EXPECT_EQ(1, gate->GetPin("/en")->GetWidth());
		EXPECT_EQ(1, gate->GetPin("in11")->GetWidth());
		EXPECT_EQ(16, gate->GetPin("in12")->GetWidth());
	}

	TEST(TestParser, Statement)
	{
		TextParser parser;
		CompositeGatePtr gate = BuildTestGate(false, false, false);
		parser.Attach(gate);

		TextParser::Sections sections = parser.GetSections("S1: section1 ; S2: section2 ; S3: section3 ;\n");
		ASSERT_EQ(3, sections.size());
		EXPECT_STREQ("S1", sections[0].Name.c_str());
		EXPECT_STREQ("S2", sections[1].Name.c_str());
		EXPECT_STREQ("S3", sections[2].Name.c_str());

		EXPECT_STREQ("section1", sections[0].Data.c_str());
		EXPECT_STREQ("section2", sections[1].Data.c_str());
		EXPECT_STREQ("section3", sections[2].Data.c_str());

		EXPECT_THROW(parser.GetSections("S1:: section1 ; S2: section2 ; S3: section3\n"), std::invalid_argument);
		EXPECT_THROW(parser.GetSections("S1: section1: ; S2: section2 ; S3: section3\n"), std::invalid_argument);
		EXPECT_THROW(parser.GetSections("S1: section1 :; S2: section2 ; S3: section3\n"), std::invalid_argument);
		EXPECT_THROW(parser.GetSections("S1: section1 ; : section2 ; S3: section3\n"), std::invalid_argument);
		EXPECT_THROW(parser.GetSections("S1: section1 ;   S;3: section3\n"), std::invalid_argument);
		EXPECT_THROW(parser.GetSections(": section1 ; S2: section2 ; S3: section3\n"), std::invalid_argument);
	}

	TEST(TestParser, PrseGate)
	{
		TextParser parser;
		CompositeGatePtr gate = BuildTestGate(false, false, false);
		parser.Attach(gate);

		parser.ParseGate("Inputs: ; Outputs: ; Wires: ;");
	}
}