// proto.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Core/CompositeGate.h"
#include "BasicGates/ANDGate.h"
#include "BasicGates/NANDGate.h"
#include "BasicGates/ORGate.h"
#include "BasicGates/XORGate.h"
#include "BasicGates/NOTGate.h"
#include "BasicGates/WireGate.h"
#include "BasicGates/BufferGate.h"
#include "BasicGates/SRLatch.h"
#include "BasicGates/DFlipFlop.h"
#include "BasicGates/TFlipFlop.h"
#include "BasicGates/JKFlipFlop.h"
#include "Core/Simulator.h"
#include "Parser/PartsBin.h"
#include "Parser/TextParser.h"
#include "Tools/LogicTools.h"

#include <vector>
#include <memory>

using namespace DigiLib;
using namespace DigiLib::Core;
using namespace DigiLib::Parser;


void TestTFlipFlopOscillation()
{
	for (int i = 0; i<10; ++i)
	{
		SimulatorPtr sim = Simulator::Create("sim");
		CompositeGatePtr tflip = std::dynamic_pointer_cast<CompositeGate>(BasicGates::TFlipFlop::Create());
		sim->AddGate("tflip", tflip);
	
		sim->PostEvent({ IOState::LOW, tflip->GetGate("dflip")->GetPin("clk") });

		sim->Pulse(tflip->GetGate("dflip")->GetPin("clk"), 40, 60);
		sim->Pulse(tflip->GetGate("dflip")->GetPin("clk"), 80, 100);
		sim->Pulse(tflip->GetGate("dflip")->GetPin("clk"), 120, 140);
		
		//sim->PostEvent({ IOState::LOW, sim->GetGate("tflip")->GetPin("t") });

		//sim->Pulse(sim->GetGate("tflip")->GetPin("t"), 40, 60);
		//sim->Pulse(sim->GetGate("tflip")->GetPin("t"), 80, 120);

		while (1)
		{
			std::cout << "Tick = " << sim->GetTick() << std::endl;
			for (auto & item : sim->GetEventQueue())
			{
				std::cout << "T(" << item.first << ") " << item.second << std::endl;
			}

			std::cout << Tools::LogicTools::PrintInternalStates(sim->GetGate("tflip"), 4);
			sim->Tick();
			getchar();
		}
		
		std::cout << Tools::LogicTools::LogicAnalyser(sim, {
			sim->GetGate("tflip")->GetPin("t"),
			sim->GetGate("tflip")->GetPin("q")
			}, 320);
	}
}

void TestJKFlipFlopInitialState()
{
	for (int i = 0; i < 10; ++i)
	{
		SimulatorPtr sim = Simulator::Create("sim");
		CompositeGatePtr jk = std::dynamic_pointer_cast<CompositeGate>(BasicGates::JKFlipFlop::Create());
		sim->AddGate("jk", jk);

		sim->PostEvent({ IOState::LOW, sim->FindPin("jk.j") });
		sim->PostEvent({ IOState::LOW, sim->FindPin("jk.k") });
		sim->PostEvent({ IOState::LOW, sim->FindPin("jk.clk") });

		sim->Pulse(sim->FindPin("jk.j"), 50, 200);
		sim->Pulse(sim->FindPin("jk.k"), 50, 200);
		
		sim->Pulse(sim->FindPin("jk.clk"), 70, 100);
		sim->Pulse(sim->FindPin("jk.clk"), 120, 140);


		while (1)
		{
			std::cout << "Tick = " << sim->GetTick() << std::endl;
			for (auto & item : sim->GetEventQueue())
			{
				std::cout << "T(" << item.first << ") " << item.second << std::endl;
			}

			std::cout << Tools::LogicTools::PrintInternalStates(sim->GetGate("jk"), 2);
			sim->Tick();
			getchar();
		}


		std::cout << Tools::LogicTools::LogicAnalyser(sim, {
			sim->FindPin("jk.j"),
			sim->FindPin("jk.k"),
			sim->FindPin("jk.clk"),
			nullptr,
			sim->FindPin("jk.q") }, 60) << "+++++++++++" << std::endl;
	}
}

void TestDFlipFlopInitialState()
{
	for (int i = 0; i < 10; ++i)
	{
		SimulatorPtr sim = Simulator::Create("sim");
		CompositeGatePtr dflip = std::dynamic_pointer_cast<CompositeGate>(BasicGates::DFlipFlop::Create());
		sim->AddGate("dflip", dflip);
		dflip->GetPin("/q")->ConnectTo(dflip->GetPin("d"));

		sim->PostEvent({ IOState::LOW, sim->GetGate("dflip")->GetPin("clk") });

		sim->Pulse(sim->GetGate("dflip")->GetPin("clk"), 40, 60);
		sim->Pulse(sim->GetGate("dflip")->GetPin("clk"), 80, 100);
		sim->Pulse(sim->GetGate("dflip")->GetPin("clk"), 120, 140);

		while (1)
		{
			std::cout << "Tick = " << sim->GetTick() << std::endl;
			for (auto & item : sim->GetEventQueue())
			{
				std::cout << "T(" << item.first << ") " << item.second << std::endl;
			}

			std::cout << Tools::LogicTools::PrintInternalStates(sim->GetGate("dflip"), 3);
			sim->Tick();
			getchar();
		}


		std::cout << Tools::LogicTools::LogicAnalyser(sim, {
			sim->GetGate("dflip")->GetPin("d"),
			sim->GetGate("dflip")->GetPin("clk"),
			nullptr,
			sim->GetGate("dflip")->GetPin("q") }, 60) << "+++++++++++" << std::endl;

	}
}

void TestDFlipFlopOscillation()
{
	for (int i = 0; i < 10; ++i)
	{
		SimulatorPtr sim = Simulator::Create("sim");
		CompositeGatePtr dflip = std::dynamic_pointer_cast<CompositeGate>(BasicGates::DFlipFlop::Create());
		sim->AddGate("dflip", dflip);

		sim->PostEvent({ IOState::LOW, sim->GetGate("dflip")->GetPin("d") });
		sim->PostEvent({ IOState::LOW, sim->GetGate("dflip")->GetPin("clk") });

		sim->Pulse(sim->GetGate("dflip")->GetPin("d"), 30, 40);

		while(1)
		{
			std::cout << "Tick = " << sim->GetTick() << std::endl;
			for (auto & item : sim->GetEventQueue())
			{
				std::cout << "T(" << item.first << ") " << item.second << std::endl;
			}
		
			std::cout << Tools::LogicTools::PrintInternalStates(sim->GetGate("dflip"));
			sim->Tick();
			getchar();
		}


		std::cout << Tools::LogicTools::LogicAnalyser(sim, {
			sim->GetGate("dflip")->GetPin("d"),
			sim->GetGate("dflip")->GetPin("clk"),
			nullptr,
			sim->GetGate("dflip")->GetPin("q") }, 60) << "+++++++++++" << std::endl;

	}
}

void TestSRLatch()
{
	SimulatorPtr sim = Simulator::Create("sim");

	sim->AddGate("srlatch", BasicGates::SRLatch::Create());

		sim->PostEvent({ IOState::LOW, sim->GetGate("srlatch")->GetPin("s") });
		sim->PostEvent({ IOState::LOW, sim->GetGate("srlatch")->GetPin("r") });

		sim->Pulse(sim->GetGate("srlatch")->GetPin("r"), 5, 10);

		sim->Pulse(sim->GetGate("srlatch")->GetPin("s"), 20, 25);

		//while(1)
		//{
		//	for (auto & item : sim->GetEventQueue())
		//	{
		//		std::cout << "T(" << item.first << ") " << item.second << std::endl;
		//	}
		//
		//	std::cout << Tools::LogicTools::PrintInternalStates(sim->GetGate("srlatch"));
		//	sim->Tick();
		//	getchar();
		//}

		std::cout << Tools::LogicTools::LogicAnalyser(sim, {
			sim->GetGate("srlatch")->GetPin("r"),
			sim->GetGate("srlatch")->GetPin("s"),
			nullptr,
			sim->GetGate("srlatch")->GetPin("q"),
			sim->GetGate("srlatch")->GetPin("/q")
			}, 100);
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

GatePtr Build3to8Decoder()
{
	GatePtr decoder1 = BuildDecoder();
	GatePtr decoder2 = BuildDecoder();

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

	return decoder3to8;
}

void TestLeak1()
{
	std::vector<GatePtr> adders;
	adders.reserve(255);

	std::cout << "Before load - Pause" << std::endl;
	getchar();

	for (int i = 0; i < 100; ++i)
	{
		adders.push_back(Build4BitAdder());
	}
	std::cout << "Before clean - Pause" << std::endl;
	getchar();
	adders.clear();
	std::cout << "After clean - Pause" << std::endl;
	getchar();
}

void TestLeak2()
{
	std::cout << "Before load - Pause" << std::endl;
	getchar();
		for (int i = 0; i < 1000; ++i)
	{
		GatePtr ptr = Build4BitAdder();
		GatePtr clone = ptr->Clone("clone");
	}
	std::cout << "After clean - Pause" << std::endl;
	getchar();
}

void TestAdder4()
{
	std::cout << "---------------------" << std::endl;
	std::cout << "Building 4 bit adder" << std::endl;
	GatePtr adder4 = Build4BitAdder();
	adder4->GetPin("x")->Set(IOState::FromInt(10, 4));
	adder4->GetPin("y")->Set(IOState::FromInt(4, 4));
	adder4->GetPin("cin")->Set(IOState::LOW);

	std::cout << "10 + 4= " << adder4->GetPin("s")->Get().ToInt16() << std::endl;
}

void TestAdder16()
{
	std::cout << "---------------------" << std::endl;
	std::cout << "Building 16 bit adder" << std::endl;
	CompositeGatePtr adder16 = CompositeGate::Create("adder16");
	adder16->AddInput("cin");
	adder16->AddInput("x", 16);
	adder16->AddInput("y", 16);
	adder16->AddOutput("s", 16);

	adder16->AddGate("a1", Build4BitAdder());
	adder16->AddGate("a2", Build4BitAdder());
	adder16->AddGate("a3", Build4BitAdder());
	adder16->AddGate("a4", Build4BitAdder());

	adder16->GetPin("x", 0, 3)->ConnectTo(adder16->GetGate("a1")->GetPin("x"));
	adder16->GetPin("x", 4, 7)->ConnectTo(adder16->GetGate("a2")->GetPin("x"));
	adder16->GetPin("x", 8, 11)->ConnectTo(adder16->GetGate("a3")->GetPin("x"));
	adder16->GetPin("x", 12, 15)->ConnectTo(adder16->GetGate("a4")->GetPin("x"));

	adder16->GetPin("y", 0, 3)->ConnectTo(adder16->GetGate("a1")->GetPin("y"));
	adder16->GetPin("y", 4, 7)->ConnectTo(adder16->GetGate("a2")->GetPin("y"));
	adder16->GetPin("y", 8, 11)->ConnectTo(adder16->GetGate("a3")->GetPin("y"));
	adder16->GetPin("y", 12, 15)->ConnectTo(adder16->GetGate("a4")->GetPin("y"));

	adder16->GetGate("a1")->GetPin("s")->ConnectTo(adder16->GetPin("s", 0, 3));
	adder16->GetGate("a2")->GetPin("s")->ConnectTo(adder16->GetPin("s", 4, 7));
	adder16->GetGate("a3")->GetPin("s")->ConnectTo(adder16->GetPin("s", 8, 11));
	adder16->GetGate("a4")->GetPin("s")->ConnectTo(adder16->GetPin("s", 12, 15));

	adder16->GetPin("cin")->ConnectTo(adder16->GetGate("a1")->GetPin("cin"));
	adder16->GetGate("a1")->GetPin("cout")->ConnectTo(adder16->GetGate("a2")->GetPin("cin"));
	adder16->GetGate("a2")->GetPin("cout")->ConnectTo(adder16->GetGate("a3")->GetPin("cin"));
	adder16->GetGate("a3")->GetPin("cout")->ConnectTo(adder16->GetGate("a4")->GetPin("cin"));

	adder16->GetPin("cin")->Set(IOState::LOW);
	adder16->GetPin("x")->Set(IOState::FromInt(12345, 16));
	adder16->GetPin("y")->Set(IOState::FromInt(34567, 16));

	std::cout << "12345 + 34567 = " << adder16->GetPin("s")->Get().ToInt16() << std::endl;
}

void TestReverseDecoder(DigiLib::Core::GatePtr &cloned, DigiLib::Core::GatePtr &base)
{
	CompositeGatePtr reverseDecoder = CompositeGate::Create("reverseDecoder");
	reverseDecoder->AddInput("en");
	reverseDecoder->AddInput("i0");
	reverseDecoder->AddInput("i1");
	reverseDecoder->AddOutput("n0");
	reverseDecoder->AddOutput("n1");
	reverseDecoder->AddOutput("n2");
	reverseDecoder->AddOutput("n3");

	reverseDecoder->AddGate("not0", BasicGates::NOTGate::Create());
	reverseDecoder->AddGate("not1", BasicGates::NOTGate::Create());
	reverseDecoder->AddGate("not2", BasicGates::NOTGate::Create());
	reverseDecoder->AddGate("not3", BasicGates::NOTGate::Create());
	reverseDecoder->AddGate("dec", cloned);

	reverseDecoder->GetPin("en")->ConnectTo(cloned->GetPin("en"));
	reverseDecoder->GetPin("i0")->ConnectTo(cloned->GetPin("i0"));
	reverseDecoder->GetPin("i1")->ConnectTo(cloned->GetPin("i1"));
	cloned->GetPin("y0")->ConnectTo(reverseDecoder->GetGate("not0")->GetPin("in"));
	cloned->GetPin("y1")->ConnectTo(reverseDecoder->GetGate("not1")->GetPin("in"));
	cloned->GetPin("y2")->ConnectTo(reverseDecoder->GetGate("not2")->GetPin("in"));
	cloned->GetPin("y3")->ConnectTo(reverseDecoder->GetGate("not3")->GetPin("in"));

	reverseDecoder->GetGate("not0")->GetPin("out")->ConnectTo(reverseDecoder->GetPin("n0"));
	reverseDecoder->GetGate("not1")->GetPin("out")->ConnectTo(reverseDecoder->GetPin("n1"));
	reverseDecoder->GetGate("not2")->GetPin("out")->ConnectTo(reverseDecoder->GetPin("n2"));
	reverseDecoder->GetGate("not3")->GetPin("out")->ConnectTo(reverseDecoder->GetPin("n3"));

	std::cout << Tools::LogicTools::PrintTruthTable(base);
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

GatePtr FindPart(Parser::PartsBinPtr parts, const char * name)
{
	TextParser parser;
	CompositeGatePtr gate = CompositeGate::Create(name);
	parser.Attach(gate, parts);
	std::ostringstream os;
	os << "../Tests/TestFiles/Good/" << name << ".txt";

	std::cout << "Loading: " << os.str() << std::endl;

	try
	{
		parser.LoadFromFile(os.str().c_str());		
		return gate;
	}
	catch (std::exception e)
	{
		std::cerr << "Error loading part: " << os.str() << std::endl;
	}

	return nullptr;
}

void TestCounter4B()
{
	CompositeGatePtr counter = CompositeGate::Create("COUNT4B");
	PartsBinPtr parts = Parser::PartsBin::Create();
	parts->AddPart("NAND", BasicGates::NANDGate::Create());
	parts->AddPart("DFF", BuildDFF());
	parts->SetPartFinder(FindPart);

	TextParser parser;
	parser.Attach(counter, parts);
	std::cout << "Loading part: COUNT4B" << std::endl;
	parser.LoadFromFile("../Tests/TestFiles/Good/COUNTER4B.txt");
	std::cout << "Total gate count: " << counter->GetGateCount(true) << std::endl;
		
	SimulatorPtr sim = Simulator::Create("sim");
	sim->AddGate("COUNT4B", counter);

	sim->PostEvent({ IOState::LOW, sim->FindPin("COUNT4B.load") });
	sim->PostEvent({ IOState::LOW, sim->FindPin("COUNT4B.reset") });
	sim->Pulse(sim->FindPin("COUNT4B.reset"), 1, 200);

	sim->Pulse(sim->FindPin("COUNT4B.load"), 1300, 1500);
	sim->Pulse(sim->FindPin("COUNT4B.in"), IOState::FromInt(3, 4), 1300, 1500);

	sim->Pulse(sim->FindPin("COUNT4B.load"), 2300, 2500);
	sim->Pulse(sim->FindPin("COUNT4B.in"), IOState::FromInt(9, 4), 2300, 2500);


	sim->PostEvent({ IOState(IOState::LOW, 4), sim->FindPin("COUNT4B.in") });
	sim->Clock(sim->FindPin("COUNT4B.clk"), 60, 60, 1, 3500);

	//while (1)
	//{
	//	std::cout << "Tick = " << sim->GetTick() << std::endl;
	//	for (auto & item : sim->GetEventQueue())
	//	{
	//		std::cout << "T(" << item.first << ") " << item.second << std::endl;
	//	}

	//	std::cout << Tools::LogicTools::PrintInternalStates(sim->GetGate("COUNTER4B"), 2);
	//	sim->Tick();
	//	getchar();
	//}


	std::cout << Tools::LogicTools::LogicAnalyser(sim, {
		sim->FindPin("COUNT4B.clk"),
		sim->FindPin("COUNT4B.in"),
		sim->FindPin("COUNT4B.reset"),
		sim->FindPin("COUNT4B.load"),
		nullptr,
		sim->FindPin("COUNT4B.out"),
		}, 3300, 60, true);	
}

void TestRegister4B()
{
	CompositeGatePtr reg = CompositeGate::Create("REG4B");
	PartsBinPtr parts = Parser::PartsBin::Create();
	parts->AddPart("NAND", BasicGates::NANDGate::Create());
	parts->AddPart("DFF", BuildDFF());
	parts->SetPartFinder(FindPart);

	TextParser parser;
	parser.Attach(reg, parts);
	std::cout << "Loading part: REGISTER4B" << std::endl;
	parser.LoadFromFile("../Tests/TestFiles/Good/REGISTER4B.txt");
	
	std::cout << "mux gate count: " << reg->GetGate("mux")->GetGateCount(true) << std::endl;
	std::cout << "flip0 gate count: " << reg->GetGate("flip0")->GetGateCount(true) << std::endl;
	std::cout << "Total gate count: " << reg->GetGateCount(true) << std::endl;	

	SimulatorPtr sim = Simulator::Create("sim");
	sim->AddGate("REG4B", reg);

	sim->PostEvent({ IOState::LOW, sim->FindPin("REG4B.load") });
	sim->PostEvent({ IOState(IOState::LOW, 4), sim->FindPin("REG4B.in") });

	sim->Pulse(sim->FindPin("REG4B.load"), 100, 200);

	sim->Pulse(sim->FindPin("REG4B.in"), IOState::FromInt(3, 4), 400, 600);
	sim->Pulse(sim->FindPin("REG4B.load"), 400, 600);

	sim->Pulse(sim->FindPin("REG4B.in"), IOState::FromInt(12, 4), 800, 1000);
	sim->Pulse(sim->FindPin("REG4B.load"), 800, 1000);

	sim->Pulse(sim->FindPin("REG4B.in"), IOState::FromInt(0, 4), 1200, 1400);
	sim->Pulse(sim->FindPin("REG4B.load"), 1200, 1400);

	sim->Pulse(sim->FindPin("REG4B.in"), IOState::FromInt(1, 4), 1600, 1800);
	sim->Pulse(sim->FindPin("REG4B.load"), 1600, 1800);

	sim->Pulse(sim->FindPin("REG4B.in"), IOState::FromInt(2, 4), 2000, 2200);
	sim->Pulse(sim->FindPin("REG4B.load"), 2000, 2200);

	sim->Pulse(sim->FindPin("REG4B.in"), IOState::FromInt(4, 4), 2400, 2600);
	sim->Pulse(sim->FindPin("REG4B.load"), 2400, 2600);

	sim->Pulse(sim->FindPin("REG4B.in"), IOState::FromInt(8, 4), 2800, 3000);
	sim->Pulse(sim->FindPin("REG4B.load"), 2800, 3000);

	sim->Pulse(sim->FindPin("REG4B.in"), IOState::FromInt(15, 4), 3200, 3400);
	sim->Pulse(sim->FindPin("REG4B.load"), 3200, 3400);
	

	sim->Clock(sim->FindPin("REG4B.clk"), 60, 60, 1, 3500);

	//while (1)
	//{
	//	std::cout << "Tick = " << sim->GetTick() << std::endl;
	//	for (auto & item : sim->GetEventQueue())
	//	{
	//		std::cout << "T(" << item.first << ") " << item.second << std::endl;
	//	}

	//	std::cout << Tools::LogicTools::PrintInternalStates(sim->GetGate("COUNTER4B"), 2);
	//	sim->Tick();
	//	getchar();
	//}
	
	std::cout << Tools::LogicTools::LogicAnalyser(sim, {
		sim->FindPin("REG4B.clk"),
		sim->FindPin("REG4B.in"),
		sim->FindPin("REG4B.load"),
		nullptr,
		sim->FindPin("REG4B.out"),
		}, 3500, 60, true);
}

int main()
{
	try
	{
		TestAdder4();

		TestAdder16();

		//TestLeak1();
		//TestLeak2();

		//TestSRLatch();
		//TestDFlipFlopOscillation();
		//TestTFlipFlopOscillation();
		//TestDFlipFlopInitialState();
		//TestJKFlipFlopInitialState();
		std::cout	
			<< "Test 4 bit counter with load and reset" << std::endl
			<< "--------------------------------------" << std::endl;
		TestCounter4B();
		
		std::cout << std::endl 
			<< "Test 4 bit register" << std::endl
			<< "-------------------" << std::endl;
		TestRegister4B();


	}
	catch (std::exception & e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}

    return 0;
}
