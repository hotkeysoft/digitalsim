#include "pch.h"
#pragma warning( disable:4251 )

#include "TestCommon.h"
#include "Core/IOState.h"
#include "Core/GateBase.h"
#include "Core/Simulator.h"
#include "BasicGates\DFlipFlop.h"
#include "BasicGates\NANDGate.h"
#include "BasicGates\ANDGate.h"
#include "BasicGates\ORGate.h"
#include "BasicGates\BufferGate.h"
#include "Parser\TextParser.h"
#include "Parser\PartsBin.h"

namespace UnitTests
{
	using namespace Core;
	using namespace Parser;

	PartsBinPtr BuildPartsBin(bool populate = true, bool nandOnly = false)
	{
		PartsBinPtr parts = PartsBin::Create();
		
		if (populate)
		{
			parts->AddPart("NAND", BasicGates::NANDGate::Create());

			if (!nandOnly)
			{
				parts->AddPart("AND", BasicGates::ANDGate::Create());
				parts->AddPart("OR", BasicGates::ORGate::Create());
			}
		}

		return parts;
	}

	CompositeGatePtr BuildDFF()
	{
		CompositeGatePtr gate = CompositeGate::Create("DFF");
		gate->AddInput("in");
		gate->AddInput("clk");
		gate->AddOutput("out");
		gate->AddGate("dflip", BasicGates::DFlipFlop::Create());

		gate->GetPin("in")->ConnectTo(gate->FindPin("dflip.d"));
		gate->GetPin("clk")->ConnectTo(gate->FindPin("dflip.clk"));
		gate->FindPin("dflip.q")->ConnectTo(gate->GetPin("out"));
		return gate;
	}

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

		parser.ParseConnection("");
		parser.ParseConnection(" ");

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

	TEST(TestParser, ParseConnections2)
	{
		TextParser parser;
		CompositeGatePtr gate = BuildTestGate();
		parser.Attach(gate);

		parser.ParseConnection(" vcc -> and1.in1");
		parser.ParseConnection(" gnd -> and1.in2");
		parser.ParseConnection(" vcc -> busin[5]");

		// Can only connect vcc/gnd to single pin for now
		parser.ParseConnection(" vcc -> buffer.in[0]");
		parser.ParseConnection(" gnd -> buffer.in[4]");

		EXPECT_THROW(parser.ParseConnection(" vcc -> buffer.in "), std::invalid_argument);
		EXPECT_THROW(parser.ParseConnection(" gnd -> buffer.in[2-3] "), std::invalid_argument);
	}

	TEST(TestParser, WireStatement)
	{
		TextParser parser;
		CompositeGatePtr gate = BuildTestGate();
		parser.Attach(gate);

		EXPECT_THROW(parser.ParseWireSection(nullptr), std::invalid_argument);
		EXPECT_THROW(parser.ParseWireSection(" in1-> and1.in1, "), std::invalid_argument);
		EXPECT_THROW(parser.ParseWireSection(" , in1-> and1.in1, "), std::invalid_argument);
		EXPECT_THROW(parser.ParseWireSection(" in1-> and1.in1,, in1-> and1.in2"), std::invalid_argument);
		EXPECT_THROW(parser.ParseWireSection(" in1-> and1.in1 in1-> and1.in2,,"), std::invalid_argument);
		EXPECT_THROW(parser.ParseWireSection(" in1->and1.in2 , in1, -> and1.in2"), std::invalid_argument);

		// Rebuild gate some valid connections could be there
		gate = BuildTestGate();
		parser.Attach(gate);

		parser.ParseWireSection("");
		parser.ParseWireSection(" ");
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

		parser.ParseInputsSection("");
		parser.ParseInputsSection(" ");
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
		EXPECT_THROW(parser.ParseOutputsSection(" in1 in2, "), std::invalid_argument);
		EXPECT_THROW(parser.ParseOutputsSection(" , in3 "), std::invalid_argument);
		EXPECT_THROW(parser.ParseOutputsSection(" in4 ,, /in5"), std::invalid_argument);
		EXPECT_THROW(parser.ParseOutputsSection(" in6[4] ,,"), std::invalid_argument);
		EXPECT_THROW(parser.ParseOutputsSection(" in7, //in8, in9"), std::invalid_argument);
		EXPECT_THROW(parser.ParseOutputsSection(" in10[4:8]"), std::invalid_argument);
		EXPECT_THROW(parser.ParseOutputsSection(" in11[17]"), std::out_of_range);
		EXPECT_THROW(parser.ParseOutputsSection(" i/n12 "), std::invalid_argument);
		EXPECT_THROW(parser.ParseOutputsSection(" 13in "), std::invalid_argument);

		// Rebuild gate some valid connections could be there
		gate = BuildTestGate(false, false, false);
		parser.Attach(gate);

		parser.ParseOutputsSection("");
		parser.ParseOutputsSection(" ");
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

	TEST(TestParser, PartsStatement)
	{
		TextParser parser;
		CompositeGatePtr gate = BuildTestGate(false, false, false);
		PartsBinPtr parts = BuildPartsBin(true);

		parser.Attach(gate, parts);
		
		EXPECT_EQ(0, gate->GetGateCount());
		parser.ParsePartsSection("AND and, \n\tAND and2, OR or ");
		EXPECT_EQ(3, gate->GetGateCount());
		parser.ParsePartsSection("  OR anotherOr  ");
		EXPECT_EQ(4, gate->GetGateCount());
		
		// Already defined part name
		EXPECT_THROW(parser.ParsePartsSection("AND and2"), std::invalid_argument);
		EXPECT_EQ(4, gate->GetGateCount());

		// name is part type
		EXPECT_THROW(parser.ParsePartsSection("AND OR"), std::invalid_argument);
		EXPECT_EQ(4, gate->GetGateCount());

		// part not found
		EXPECT_THROW(parser.ParsePartsSection("NXAND nxand"), std::invalid_argument);
		EXPECT_EQ(4, gate->GetGateCount());
		
		gate->Reset();

		// no-op
		parser.ParsePartsSection("");
		parser.ParsePartsSection("  ");
		EXPECT_EQ(0, gate->GetGateCount());

		EXPECT_THROW(parser.ParsePartsSection(nullptr), std::invalid_argument);
		EXPECT_THROW(parser.ParsePartsSection(" , "), std::invalid_argument);
		EXPECT_THROW(parser.ParsePartsSection(" ,,"), std::invalid_argument);
		EXPECT_THROW(parser.ParsePartsSection("AND 3and  "), std::invalid_argument);
		EXPECT_THROW(parser.ParsePartsSection("A/ND and  "), std::invalid_argument);
		EXPECT_THROW(parser.ParsePartsSection("AND"), std::invalid_argument);
		EXPECT_THROW(parser.ParsePartsSection("AND|and"), std::invalid_argument);
		EXPECT_THROW(parser.ParsePartsSection("AND and&"), std::invalid_argument);
		EXPECT_THROW(parser.ParsePartsSection("or"), std::invalid_argument);
		EXPECT_EQ(0, gate->GetGateCount());
	}
	
	TEST(TestParser, Statement)
	{
		TextParser parser;
		CompositeGatePtr gate = BuildTestGate(false, false, false);
		parser.Attach(gate);

		TextParser::Sections sections = parser.GetSections("S1: section1 ; S2: section2 ; S3: section3 ;\n");
		ASSERT_EQ(3, sections.size());
		EXPECT_STREQ("S1", sections[0].name.c_str());
		EXPECT_STREQ("S2", sections[1].name.c_str());
		EXPECT_STREQ("S3", sections[2].name.c_str());

		EXPECT_STREQ("section1", sections[0].data.c_str());
		EXPECT_STREQ("section2", sections[1].data.c_str());
		EXPECT_STREQ("section3", sections[2].data.c_str());

		EXPECT_THROW(parser.GetSections("S1:: section1 ; S2: section2 ; S3: section3\n"), std::invalid_argument);
		EXPECT_THROW(parser.GetSections("S1: section1: ; S2: section2 ; S3: section3\n"), std::invalid_argument);
		EXPECT_THROW(parser.GetSections("S1: section1 :; S2: section2 ; S3: section3\n"), std::invalid_argument);
		EXPECT_THROW(parser.GetSections("S1: section1 ; : section2 ; S3: section3\n"), std::invalid_argument);
		EXPECT_THROW(parser.GetSections("S1: section1 ;   S;3: section3\n"), std::invalid_argument);
		EXPECT_THROW(parser.GetSections(": section1 ; S2: section2 ; S3: section3\n"), std::invalid_argument);
	}

	TEST(TestParser, ParseGate)
	{
		TextParser parser;
		CompositeGatePtr gate = BuildTestGate(false, false, false);
		PartsBinPtr parts = BuildPartsBin();
		parser.Attach(gate, parts);
	
		// Missing sections
		EXPECT_THROW(parser.ParseGate("Outputs: ; Parts: ; Wires: ;"), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Inputs: ; Parts: ; Wires: ;"), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Inputs: ; Outputs: ;  Wires: ;"), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Inputs: ; Outputs: ; Parts: ; "), std::invalid_argument);

		// Repeated sections
		EXPECT_THROW(parser.ParseGate("Inputs: ; Inputs: ; Outputs: ; Parts: ; Wires: ;"), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Inputs: ; Outputs: ; Inputs: ;  Parts: ; Wires: ;"), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Inputs: ; Outputs: ; Parts: ; Inputs: ;  Wires: ;"), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Inputs: ; Outputs: ; Parts: ; Wires: ; Inputs: ; "), std::invalid_argument);

		EXPECT_THROW(parser.ParseGate("Outputs: ; Inputs: ; Outputs: ; Parts: ; Wires: ;"), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Inputs: ; Outputs: ; Outputs: ;  Parts: ; Wires: ;"), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Inputs: ; Outputs: ; Parts: ; Outputs: ;  Wires: ;"), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Inputs: ; Outputs: ; Parts: ; Wires: ; Outputs: ; "), std::invalid_argument);

		EXPECT_THROW(parser.ParseGate("Parts: ; Inputs: ; Outputs: ; Parts: ; Wires: ;"), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Inputs: ; Parts: ; Outputs: ;  Parts: ; Wires: ;"), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Inputs: ; Outputs: ; Parts: ; Parts: ;  Wires: ;"), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Inputs: ; Outputs: ; Parts: ; Wires: ; Parts: ; "), std::invalid_argument);

		EXPECT_THROW(parser.ParseGate("Wires: ; Inputs: ; Outputs: ; Parts: ; Wires: ;"), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Inputs: ; Wires: ; Outputs: ;  Parts: ; Wires: ;"), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Inputs: ; Outputs: ; Wires: ; Parts: ;  Wires: ;"), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Inputs: ; Outputs: ; Parts: ; Wires: ; Wires: ; "), std::invalid_argument);

		// Invalid order
		EXPECT_THROW(parser.ParseGate("Inputs: ; Wires: ; Outputs: ; Parts: ; "), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Wires: ; Inputs: ; Outputs: ; Parts: ; "), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Outputs: ; Inputs: ; Parts: ; Wires: ;"), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Outputs: ; Inputs: ; Wires: ; Parts: ; "), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Inputs: ; Wires: ; Outputs: ; Parts: ; "), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Inputs: ; Parts: ; Outputs: ; Wires: ; "), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Inputs: ; Parts: ; Outputs: ; Wires: ; "), std::invalid_argument);

		// Too many
		EXPECT_THROW(parser.ParseGate("Toomany: ; Inputs: ; Outputs: ; Parts: ; Wires: ; "), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Inputs: ; Toomany: ; Outputs: ; Parts: ; Wires: ; "), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Inputs: ; Outputs: ; Toomany: ; Parts: ; Wires: ; "), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Inputs: ; Outputs: ; Parts: ; Toomany: ; Wires: ; "), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Inputs: ; Outputs: ; Parts: ; Wires: ; Toomany: ;"), std::invalid_argument);

		// Mandatory only
		parser.ParseGate("Inputs: ; Outputs: ; Parts: ; Wires: ;");

		// Optional
		parser.ParseGate("Description: ; Inputs: ; Outputs: ; Parts: ; Wires: ;");		

		// Optional out of place
		EXPECT_THROW(parser.ParseGate("Inputs: ; Description: ; Outputs: ; Parts: ; Wires: ; "), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Inputs: ; Outputs: ; Description: ; Parts: ; Wires: ; "), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Inputs: ; Outputs: ; Parts: ; Description: ; Wires: ; "), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Inputs: ; Outputs: ; Parts: ; Wires: ; Description: ;"), std::invalid_argument);

		// Optional + missing mandatory
		EXPECT_THROW(parser.ParseGate("Description: ; Outputs: ; Parts: ; Wires: ;"), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Description: ; Inputs: ; Parts: ; Wires: ;"), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Description: ; Inputs: ; Outputs: ; Wires: ;"), std::invalid_argument);
		EXPECT_THROW(parser.ParseGate("Description: ; Inputs: ; Outputs: ; Parts: ;"), std::invalid_argument);
	}

	TEST(TestParser, Comments)
	{
		TextParser parser;
		CompositeGatePtr gate = BuildTestGate(false, false, false);
		parser.Attach(gate);

		TextParser::Sections sections = parser.GetSections("S1: section1 #; S2: section2 ; S3: section3 \n;");
		ASSERT_EQ(1, sections.size());
		EXPECT_STREQ("S1", sections[0].name.c_str());
		EXPECT_STREQ("section1", sections[0].data.c_str());

		sections = parser.GetSections("S1: section1 ; # S2: section2 ;\n S3: section3 ;\n");
		ASSERT_EQ(2, sections.size());
		EXPECT_STREQ("S1", sections[0].name.c_str());
		EXPECT_STREQ("section1", sections[0].data.c_str());
		EXPECT_STREQ("S3", sections[1].name.c_str());
		EXPECT_STREQ("section3", sections[1].data.c_str());

		sections = parser.GetSections("S1: section1 ; # this is a comment \nS2: section2 ;\n S3: section3 ;\n");
		ASSERT_EQ(3, sections.size());
		EXPECT_STREQ("S1", sections[0].name.c_str());
		EXPECT_STREQ("section1", sections[0].data.c_str());
		EXPECT_STREQ("S2", sections[1].name.c_str());
		EXPECT_STREQ("section2", sections[1].data.c_str());
		EXPECT_STREQ("S3", sections[2].name.c_str());
		EXPECT_STREQ("section3", sections[2].data.c_str());

		sections = parser.GetSections("# begin ;S1: comment ; \n S1: section1 ;# this is a comment \nS2: section2 ;\n S3: section3 ;\n");
		ASSERT_EQ(3, sections.size());
		EXPECT_STREQ("S1", sections[0].name.c_str());
		EXPECT_STREQ("section1", sections[0].data.c_str());
		EXPECT_STREQ("S2", sections[1].name.c_str());
		EXPECT_STREQ("section2", sections[1].data.c_str());
		EXPECT_STREQ("S3", sections[2].name.c_str());
		EXPECT_STREQ("section3", sections[2].data.c_str());
	}

	bool isDir(std::string path)
	{
		struct stat info;

		if (stat(path.c_str(), &info) != 0)
		{
			return false;
		}
		else if (info.st_mode & S_IFDIR)
		{
			return true;
		}

		return false;
	}

	std::string GetTestFile(std::string fileName)
	{
		// By default, we should be running from the project's directory. 
		// However, if running from VC test harness, we're in the build directory for some reason...
		// Fake it by looking in two places in a not very subtle way

		std::string basePath = "./TestFiles";
		if (isDir(basePath))
		{
			return basePath + fileName;
		}

		char data[1024];
		_getcwd(data, 1024);
		basePath = data;
		basePath += "/../../Tests/TestFiles";
		if (isDir(basePath))
		{
			return basePath + fileName;
		}
		throw std::invalid_argument("Can't find test file base directory");
	}

	TEST(TestParser, LoadFromFile)
	{
		PartsBinPtr parts = BuildPartsBin(true, true);
		CompositeGatePtr gate;
		TextParser parser;

		gate = BuildTestGate(false, false, false);
		parser.Attach(gate, parts);
		parser.LoadFromFile(GetTestFile("/Good/XOR.txt").c_str());
		parts->AddPart("XOR", gate);
		EXPECT_EQ(2, parts->GetPartCount());

		{
			Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
			Tools::LogicTools::ResultListType compare({ IOState::LOW, IOState::HI, IOState::HI, IOState::LOW });
			EXPECT_EQ(compare, out);
		}

		gate = BuildTestGate(false, false, false);
		parser.Attach(gate, parts);
		parser.LoadFromFile(GetTestFile("/Good/NOT.txt").c_str());
		parts->AddPart("NOT", gate);
		EXPECT_EQ(3, parts->GetPartCount());

		{
			Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
			Tools::LogicTools::ResultListType compare({ IOState::HI, IOState::LOW });
			EXPECT_EQ(compare, out);
		}

		gate = BuildTestGate(false, false, false);
		parser.Attach(gate, parts);
		parser.LoadFromFile(GetTestFile("/Good/NOT4B.txt").c_str());
		parts->AddPart("NOT4B", gate);
		EXPECT_EQ(4, parts->GetPartCount());


		gate = BuildTestGate(false, false, false);
		parser.Attach(gate, parts);
		parser.LoadFromFile(GetTestFile("/Good/OR.txt").c_str());
		parts->AddPart("OR", gate);
		EXPECT_EQ(5, parts->GetPartCount());

		{
			Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
			Tools::LogicTools::ResultListType compare({ IOState::LOW, IOState::HI, IOState::HI, IOState::HI });
			EXPECT_EQ(compare, out);
		}

		gate = BuildTestGate(false, false, false);
		parser.Attach(gate, parts);
		parser.LoadFromFile(GetTestFile("/Good/OR4W.txt").c_str());
		parts->AddPart("OR4W", gate);
		EXPECT_EQ(6, parts->GetPartCount());

		//TODO: GetTruthTable doesn't work with bus
		//{
		//	Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
		//	Tools::LogicTools::ResultListType compare({ IOState::LOW, IOState::HI, IOState::HI, IOState::HI });
		//	EXPECT_EQ(compare, out);
		//}

		gate = BuildTestGate(false, false, false);
		parser.Attach(gate, parts);
		parser.LoadFromFile(GetTestFile("/Good/AND.txt").c_str());
		parts->AddPart("AND", gate);
		EXPECT_EQ(7, parts->GetPartCount());

		{
			Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
			Tools::LogicTools::ResultListType compare({ IOState::LOW, IOState::LOW, IOState::LOW, IOState::HI });
			EXPECT_EQ(compare, out);
		}

		gate = BuildTestGate(false, false, false);
		parser.Attach(gate, parts);
		parser.LoadFromFile(GetTestFile("/Good/HALFADDER.txt").c_str());
		parts->AddPart("HALFADDER", gate);
		EXPECT_EQ(8, parts->GetPartCount());

		{
			Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
			Tools::LogicTools::ResultListType compare({ IOState::LOW, IOState::LOW, IOState::LOW, IOState::HI,
				IOState::LOW, IOState::HI, IOState::HI, IOState::LOW });
			EXPECT_EQ(compare, out);
		}

		gate = BuildTestGate(false, false, false);
		parser.Attach(gate, parts);
		parser.LoadFromFile(GetTestFile("/Good/FULLADDER.txt").c_str());
		parts->AddPart("FULLADDER", gate);
		EXPECT_EQ(9, parts->GetPartCount());

		{
			Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
			Tools::LogicTools::ResultListType compare({ IOState::LOW, IOState::LOW, IOState::LOW, IOState::HI,
				IOState::LOW, IOState::HI, IOState::HI, IOState::LOW,
				IOState::LOW, IOState::HI, IOState::HI, IOState::LOW,
				IOState::HI, IOState::LOW, IOState::HI, IOState::HI, });
			EXPECT_EQ(compare, out);
		}

		gate = BuildTestGate(false, false, false);
		parser.Attach(gate, parts);
		parser.LoadFromFile(GetTestFile("/Good/ADDER4B.txt").c_str());
		parts->AddPart("ADDER4B", gate);
		EXPECT_EQ(10, parts->GetPartCount());

		gate = BuildTestGate(false, false, false);
		parser.Attach(gate, parts);
		parser.LoadFromFile(GetTestFile("/Good/MUX.txt").c_str());
		parts->AddPart("MUX", gate);
		EXPECT_EQ(11, parts->GetPartCount());

		{
			Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
			Tools::LogicTools::ResultListType compare({ IOState::LOW, IOState::LOW, IOState::LOW, IOState::HI,
				IOState::HI, IOState::LOW, IOState::HI, IOState::HI });
			EXPECT_EQ(compare, out);
		}

		gate = BuildTestGate(false, false, false);
		parser.Attach(gate, parts);
		parser.LoadFromFile(GetTestFile("/Good/MUX4B.txt").c_str());
		parts->AddPart("MUX4B", gate);
		EXPECT_EQ(12, parts->GetPartCount());

		gate = BuildTestGate(false, false, false);
		parser.Attach(gate, parts);
		parser.LoadFromFile(GetTestFile("/Good/DEMUX.txt").c_str());
		parts->AddPart("DEMUX", gate);
		EXPECT_EQ(13, parts->GetPartCount());

		{
			Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
			Tools::LogicTools::ResultListType compare({ IOState::LOW, IOState::LOW, IOState::LOW, IOState::LOW,
				IOState::HI, IOState::LOW, IOState::LOW, IOState::HI });
			EXPECT_EQ(compare, out);
		}

		gate = BuildTestGate(false, false, false);
		parser.Attach(gate, parts);
		parser.LoadFromFile(GetTestFile("/Good/DEMUX4W.txt").c_str());
		parts->AddPart("DEMUX4W", gate);
		EXPECT_EQ(14, parts->GetPartCount());

		gate = BuildTestGate(false, false, false);
		parser.Attach(gate, parts);
		parser.LoadFromFile(GetTestFile("/Good/NAND4B.txt").c_str());
		parts->AddPart("NAND4B", gate);
		EXPECT_EQ(15, parts->GetPartCount());

		gate = BuildTestGate(false, false, false);
		parser.Attach(gate, parts);
		parser.LoadFromFile(GetTestFile("/Good/ALU4B.txt").c_str());
		parts->AddPart("ALU4B", gate);
		EXPECT_EQ(16, parts->GetPartCount());

		gate = BuildTestGate(false, false, false);
		parser.Attach(gate, parts);
		parser.LoadFromFile(GetTestFile("/Good/DECODER.txt").c_str());
		parts->AddPart("DECODER", gate);
		EXPECT_EQ(17, parts->GetPartCount());

		// Need DFF for last components
		parts->AddPart("DFF", BuildDFF());
		EXPECT_EQ(18, parts->GetPartCount());

		gate = BuildTestGate(false, false, false);
		parser.Attach(gate, parts);
		parser.LoadFromFile(GetTestFile("/Good/COUNTER4B.txt").c_str());
		parts->AddPart("COUNTER4B", gate);
		EXPECT_EQ(19, parts->GetPartCount());

		gate = BuildTestGate(false, false, false);
		parser.Attach(gate, parts);
		parser.LoadFromFile(GetTestFile("/Good/REGISTER.txt").c_str());
		parts->AddPart("REGISTER", gate);
		EXPECT_EQ(20, parts->GetPartCount());

		gate = BuildTestGate(false, false, false);
		parser.Attach(gate, parts);
		parser.LoadFromFile(GetTestFile("/Good/REGISTER4B.txt").c_str());
		parts->AddPart("REGISTER4B", gate);
		EXPECT_EQ(21, parts->GetPartCount());
	}

	GatePtr FindPart(PartsBinPtr parts, const char * name)
	{
		std::cout << "Looking for part: " << name << std::endl;

		TextParser parser;
		CompositeGatePtr gate = BuildTestGate(false, false, false);
		parser.Attach(gate, parts);
		std::ostringstream os;
		os << "/Good/" << name << ".txt";

		try
		{
			parser.LoadFromFile(GetTestFile(os.str()).c_str());
			return gate;
		}
		catch (std::exception e)
		{
			std::cerr << "Error while loading part " << os.str() << std::endl;
		}

		return nullptr;
	}

	TEST(TestParser, PartFinder)
	{
		PartsBinPtr parts = BuildPartsBin(true, true);
		parts->SetPartFinder(FindPart);

		parts->AddPart("DFF", BuildDFF());

		CompositeGatePtr gate;
		TextParser parser;

		gate = BuildTestGate(false, false, false);
		parser.Attach(gate, parts);
		parser.LoadFromFile(GetTestFile("/Good/REGISTER4B.txt").c_str());
		parts->AddPart("REGISTER4B", gate);

		gate = BuildTestGate(false, false, false);
		parser.Attach(gate, parts);
		parser.LoadFromFile(GetTestFile("/Good/DECODER.txt").c_str());
		parts->AddPart("DECODER", gate);
	}
}