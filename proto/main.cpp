// proto.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <vector>
#include "Core\CompositeGate.h"
#include "BasicGates\ANDGate.h"
#include "BasicGates\NANDGate.h"
#include "BasicGates\ORGate.h"
#include "BasicGates\XORGate.h"
#include "BasicGates\NOTGate.h"
#include "BasicGates\WireGate.h"
#include "Tools\LogicTools.h"

using namespace DigiLib;

Core::GateBase* BuildDecoder()
{
	Core::CompositeGate * component = new Core::CompositeGate("DECODER");
	component->AddInput("EN");
	component->AddInput("I0");
	component->AddInput("I1");
	component->AddOutput("Y0");
	component->AddOutput("Y1");
	component->AddOutput("Y2");
	component->AddOutput("Y3");

	component->AddGate("NOTI0", new BasicGates::NOTGate());
	component->AddGate("NOTI1", new BasicGates::NOTGate());
	component->AddGate("AND0", new BasicGates::ANDGate(3));
	component->AddGate("AND1", new BasicGates::ANDGate(3));
	component->AddGate("AND2", new BasicGates::ANDGate(3));
	component->AddGate("AND3", new BasicGates::ANDGate(3));

	component->GetGate("AND0")->GetPin("out")->ConnectTo(component->GetPin("Y0"));
	component->GetGate("AND1")->GetPin("out")->ConnectTo(component->GetPin("Y1"));
	component->GetGate("AND2")->GetPin("out")->ConnectTo(component->GetPin("Y2"));
	component->GetGate("AND3")->GetPin("out")->ConnectTo(component->GetPin("Y3"));

	component->GetPin("EN")->ConnectTo(component->GetGate("AND0")->GetPin("in3"));
	component->GetPin("EN")->ConnectTo(component->GetGate("AND1")->GetPin("in3"));
	component->GetPin("EN")->ConnectTo(component->GetGate("AND2")->GetPin("in3"));
	component->GetPin("EN")->ConnectTo(component->GetGate("AND3")->GetPin("in3"));

	component->GetPin("I0")->ConnectTo(component->GetGate("NOTI0")->GetPin("in"));
	component->GetPin("I0")->ConnectTo(component->GetGate("AND1")->GetPin("in1"));
	component->GetPin("I0")->ConnectTo(component->GetGate("AND3")->GetPin("in1"));

	component->GetPin("I1")->ConnectTo(component->GetGate("NOTI1")->GetPin("in"));
	component->GetPin("I1")->ConnectTo(component->GetGate("AND2")->GetPin("in2"));
	component->GetPin("I1")->ConnectTo(component->GetGate("AND3")->GetPin("in2"));

	component->GetGate("NOTI0")->GetPin("out")->ConnectTo(component->GetGate("AND0")->GetPin("in1"));
	component->GetGate("NOTI0")->GetPin("out")->ConnectTo(component->GetGate("AND2")->GetPin("in1"));

	component->GetGate("NOTI1")->GetPin("out")->ConnectTo(component->GetGate("AND0")->GetPin("in2"));
	component->GetGate("NOTI1")->GetPin("out")->ConnectTo(component->GetGate("AND1")->GetPin("in2"));

	return component;
}

Core::GateBase* Build3to8Decoder()
{
	Core::GateBase * decoder1 = BuildDecoder();
	Core::GateBase * decoder2 = BuildDecoder();

	BasicGates::NOTGate *notI1 = new BasicGates::NOTGate();

	Core::CompositeGate * decoder3to8 = new Core::CompositeGate("Decoder3to8");
	decoder3to8->AddInput("I0");
	decoder3to8->AddInput("I1");
	decoder3to8->AddInput("I2");
	decoder3to8->AddOutput("Y0");
	decoder3to8->AddOutput("Y1");
	decoder3to8->AddOutput("Y2");
	decoder3to8->AddOutput("Y3");
	decoder3to8->AddOutput("Y4");
	decoder3to8->AddOutput("Y5");
	decoder3to8->AddOutput("Y6");
	decoder3to8->AddOutput("Y7");

	decoder3to8->AddGate("dec1", decoder1);
	decoder3to8->AddGate("dec2", decoder2);
	decoder3to8->AddGate("notI1", notI1);

	decoder1->GetPin("Y0")->ConnectTo(decoder3to8->GetPin("Y0"));
	decoder1->GetPin("Y1")->ConnectTo(decoder3to8->GetPin("Y1"));
	decoder1->GetPin("Y2")->ConnectTo(decoder3to8->GetPin("Y2"));
	decoder1->GetPin("Y3")->ConnectTo(decoder3to8->GetPin("Y3"));
	decoder2->GetPin("Y0")->ConnectTo(decoder3to8->GetPin("Y4"));
	decoder2->GetPin("Y1")->ConnectTo(decoder3to8->GetPin("Y5"));
	decoder2->GetPin("Y2")->ConnectTo(decoder3to8->GetPin("Y6"));
	decoder2->GetPin("Y3")->ConnectTo(decoder3to8->GetPin("Y7"));

	decoder3to8->GetPin("I0")->ConnectTo(decoder1->GetPin("I0"));
	decoder3to8->GetPin("I0")->ConnectTo(decoder2->GetPin("I0"));
	decoder3to8->GetPin("I1")->ConnectTo(decoder1->GetPin("I1"));
	decoder3to8->GetPin("I1")->ConnectTo(decoder2->GetPin("I1"));
	decoder3to8->GetPin("I2")->ConnectTo(decoder2->GetPin("EN"));
	decoder3to8->GetPin("I2")->ConnectTo(notI1->GetPin("in"));
	notI1->GetPin("out")->ConnectTo(decoder1->GetPin("EN"));

	return decoder3to8;
}

int main()
{
	try
	{
		std::cout << "AND Gate" << std::endl;
		BasicGates::ANDGate andGate;
		Tools::LogicTools::PrintTruthTable({ andGate.GetPin("in1"), andGate.GetPin("in2") }, { andGate.GetPin("out") });

		std::cout << "NAND Gate" << std::endl;
		BasicGates::NANDGate nandGate;
		Tools::LogicTools::PrintTruthTable({ nandGate.GetPin("in1"), nandGate.GetPin("in2") }, { nandGate.GetPin("out") });

		std::cout << "OR Gate" << std::endl;
		BasicGates::ORGate orGate;
		Tools::LogicTools::PrintTruthTable({ orGate.GetPin("in1"), orGate.GetPin("in2") }, { orGate.GetPin("out") });

		std::cout << "Quad input OR Gate" << std::endl;
		BasicGates::ORGate or1;
		BasicGates::ORGate or2;
		BasicGates::ORGate or3;

		or1.GetPin("out")->ConnectTo(or3.GetPin("in1"));
		or2.GetPin("out")->ConnectTo(or3.GetPin("in2"));
		
		Tools::LogicTools::PrintTruthTable({ or1.GetPin("in1"), or1.GetPin("in2"), or2.GetPin("in1"), or2.GetPin("in2") }, { or3.GetPin("out") });

		std::cout << "2-to-4 decoder" << std::endl;
		BasicGates::WireGate I0, I1, EN;
		BasicGates::NOTGate NOTI0, NOTI1;
		BasicGates::ANDGate Y0(3), Y1(3), Y2(3), Y3(3);

		EN.GetPin("out")->ConnectTo(Y0.GetPin("in3"));
		EN.GetPin("out")->ConnectTo(Y1.GetPin("in3"));
		EN.GetPin("out")->ConnectTo(Y2.GetPin("in3"));
		EN.GetPin("out")->ConnectTo(Y3.GetPin("in3"));

		I0.GetPin("out")->ConnectTo(NOTI0.GetPin("in"));
		I0.GetPin("out")->ConnectTo(Y1.GetPin("in1"));
		I0.GetPin("out")->ConnectTo(Y3.GetPin("in1"));

		I1.GetPin("out")->ConnectTo(NOTI1.GetPin("in"));
		I1.GetPin("out")->ConnectTo(Y2.GetPin("in2"));
		I1.GetPin("out")->ConnectTo(Y3.GetPin("in2"));

		NOTI0.GetPin("out")->ConnectTo(Y0.GetPin("in1"));
		NOTI0.GetPin("out")->ConnectTo(Y2.GetPin("in1"));

		NOTI1.GetPin("out")->ConnectTo(Y0.GetPin("in2"));
		NOTI1.GetPin("out")->ConnectTo(Y1.GetPin("in2"));

		Tools::LogicTools::PrintTruthTable({ EN.GetPin("in"), I1.GetPin("in"), I0.GetPin("in") }, { Y3.GetPin("out"), Y2.GetPin("out"), Y1.GetPin("out"), Y0.GetPin("out") });

		// Create component
		Core::CompositeGate component("DECODER");
		component.AddInput("EN");
		component.AddInput("I0");
		component.AddInput("I1");
		component.AddOutput("Y0");
		component.AddOutput("Y1");
		component.AddOutput("Y2");
		component.AddOutput("Y3");

		component.AddGate("NOTI0", new BasicGates::NOTGate());
		component.AddGate("NOTI1", new BasicGates::NOTGate());
		component.AddGate("AND0", new BasicGates::ANDGate(3));
		component.AddGate("AND1", new BasicGates::ANDGate(3));
		component.AddGate("AND2", new BasicGates::ANDGate(3));
		component.AddGate("AND3", new BasicGates::ANDGate(3));

		component.GetGate("AND0")->GetPin("out")->ConnectTo(component.GetPin("Y0"));
		component.GetGate("AND1")->GetPin("out")->ConnectTo(component.GetPin("Y1"));
		component.GetGate("AND2")->GetPin("out")->ConnectTo(component.GetPin("Y2"));
		component.GetGate("AND3")->GetPin("out")->ConnectTo(component.GetPin("Y3"));

		component.GetPin("EN")->ConnectTo(component.GetGate("AND0")->GetPin("in3"));
		component.GetPin("EN")->ConnectTo(component.GetGate("AND1")->GetPin("in3"));
		component.GetPin("EN")->ConnectTo(component.GetGate("AND2")->GetPin("in3"));
		component.GetPin("EN")->ConnectTo(component.GetGate("AND3")->GetPin("in3"));

		component.GetPin("I0")->ConnectTo(component.GetGate("NOTI0")->GetPin("in"));
		component.GetPin("I0")->ConnectTo(component.GetGate("AND1")->GetPin("in1"));
		component.GetPin("I0")->ConnectTo(component.GetGate("AND3")->GetPin("in1"));

		component.GetPin("I1")->ConnectTo(component.GetGate("NOTI1")->GetPin("in"));
		component.GetPin("I1")->ConnectTo(component.GetGate("AND2")->GetPin("in2"));
		component.GetPin("I1")->ConnectTo(component.GetGate("AND3")->GetPin("in2"));

		component.GetGate("NOTI0")->GetPin("out")->ConnectTo(component.GetGate("AND0")->GetPin("in1"));
		component.GetGate("NOTI0")->GetPin("out")->ConnectTo(component.GetGate("AND2")->GetPin("in1"));

		component.GetGate("NOTI1")->GetPin("out")->ConnectTo(component.GetGate("AND0")->GetPin("in2"));
		component.GetGate("NOTI1")->GetPin("out")->ConnectTo(component.GetGate("AND1")->GetPin("in2"));

		Tools::LogicTools::PrintTruthTable(	
			{ component.GetPin("EN"), component.GetPin("I1"), component.GetPin("I0") },
			{ component.GetPin("Y3"), component.GetPin("Y2"), component.GetPin("Y1"), component.GetPin("Y0") });

		Tools::LogicTools::PrintTruthTable(&component);

		Core::GateBase* decoder = Build3to8Decoder();
		Tools::LogicTools::PrintInternalConnections(decoder);
		std::cout << std::endl;
		Core::GateBase* clone = decoder->Clone("clone");
		Tools::LogicTools::PrintInternalConnections(clone);

		Core::GateBase* base = BuildDecoder();
		Core::GateBase* cloned = base->Clone("clone");

		base->GetPin("EN")->Set(Core::IOPin::LOW);
		base->GetPin("I0")->Set(Core::IOPin::LOW);
		base->GetPin("I1")->Set(Core::IOPin::LOW);

		cloned->GetPin("EN")->Set(Core::IOPin::LOW);
		cloned->GetPin("I0")->Set(Core::IOPin::LOW);
		cloned->GetPin("I1")->Set(Core::IOPin::LOW);
		
		Core::CompositeGate* reverseDecoder = new Core::CompositeGate("reverseDecoder");
		reverseDecoder->AddInput("EN");
		reverseDecoder->AddInput("I0");
		reverseDecoder->AddInput("I1");
		reverseDecoder->AddOutput("N0");
		reverseDecoder->AddOutput("N1");
		reverseDecoder->AddOutput("N2");
		reverseDecoder->AddOutput("N3");

		reverseDecoder->AddGate("not0", new BasicGates::NOTGate());
		reverseDecoder->AddGate("not1", new BasicGates::NOTGate());
		reverseDecoder->AddGate("not2", new BasicGates::NOTGate());
		reverseDecoder->AddGate("not3", new BasicGates::NOTGate());
		reverseDecoder->AddGate("dec", cloned);

		reverseDecoder->GetPin("EN")->ConnectTo(cloned->GetPin("EN"));
		reverseDecoder->GetPin("I0")->ConnectTo(cloned->GetPin("I0"));
		reverseDecoder->GetPin("I1")->ConnectTo(cloned->GetPin("I1"));
		cloned->GetPin("Y0")->ConnectTo(reverseDecoder->GetGate("not0")->GetPin("in"));
		cloned->GetPin("Y1")->ConnectTo(reverseDecoder->GetGate("not1")->GetPin("in"));
		cloned->GetPin("Y2")->ConnectTo(reverseDecoder->GetGate("not2")->GetPin("in"));
		cloned->GetPin("Y3")->ConnectTo(reverseDecoder->GetGate("not3")->GetPin("in"));

		reverseDecoder->GetGate("not0")->GetPin("out")->ConnectTo(reverseDecoder->GetPin("N0"));
		reverseDecoder->GetGate("not1")->GetPin("out")->ConnectTo(reverseDecoder->GetPin("N1"));
		reverseDecoder->GetGate("not2")->GetPin("out")->ConnectTo(reverseDecoder->GetPin("N2"));
		reverseDecoder->GetGate("not3")->GetPin("out")->ConnectTo(reverseDecoder->GetPin("N3"));

		Tools::LogicTools::PrintTruthTable(base);
		Tools::LogicTools::PrintTruthTable(reverseDecoder);

		Core::CompositeGate* parity = new Core::CompositeGate("parity");
		parity->AddGate("xor1", new BasicGates::XORGate());
		parity->AddGate("xor2", new BasicGates::XORGate());
		parity->AddGate("xor3", new BasicGates::XORGate());
		parity->AddGate("xor4", new BasicGates::XORGate());
		parity->AddGate("xor5", new BasicGates::XORGate());
		parity->AddGate("xor6", new BasicGates::XORGate());
		parity->AddGate("xor7", new BasicGates::XORGate());
		parity->AddGate("xor8", new BasicGates::XORGate());
		parity->AddGate("xor9", new BasicGates::XORGate());
		parity->AddGate("xor10", new BasicGates::XORGate());
		parity->AddGate("not", new BasicGates::NOTGate());
		parity->AddInput("A");
		parity->AddInput("B");
		parity->AddInput("C");
		parity->AddInput("D");
		parity->AddInput("E");
		parity->AddInput("F");
		parity->AddInput("G");
		parity->AddInput("H");
		parity->AddInput("I");
		parity->AddOutput("EVEN");
		parity->AddOutput("ODD");

		parity->GetPin("A")->ConnectTo(parity->GetGate("xor1")->GetPin("in1"));
		parity->GetPin("B")->ConnectTo(parity->GetGate("xor1")->GetPin("in2"));
		parity->GetPin("C")->ConnectTo(parity->GetGate("xor2")->GetPin("in2"));

		parity->GetPin("D")->ConnectTo(parity->GetGate("xor3")->GetPin("in1"));
		parity->GetPin("E")->ConnectTo(parity->GetGate("xor3")->GetPin("in2"));
		parity->GetPin("F")->ConnectTo(parity->GetGate("xor4")->GetPin("in2"));

		parity->GetPin("G")->ConnectTo(parity->GetGate("xor5")->GetPin("in1"));
		parity->GetPin("H")->ConnectTo(parity->GetGate("xor5")->GetPin("in2"));
		parity->GetPin("I")->ConnectTo(parity->GetGate("xor6")->GetPin("in2"));

		parity->GetGate("xor1")->GetPin("out")->ConnectTo(parity->GetGate("xor2")->GetPin("in1"));
		parity->GetGate("xor3")->GetPin("out")->ConnectTo(parity->GetGate("xor4")->GetPin("in1"));
		parity->GetGate("xor5")->GetPin("out")->ConnectTo(parity->GetGate("xor6")->GetPin("in1"));

		parity->GetGate("xor2")->GetPin("out")->ConnectTo(parity->GetGate("xor7")->GetPin("in1"));
		parity->GetGate("xor2")->GetPin("out")->ConnectTo(parity->GetGate("xor9")->GetPin("in1"));

		parity->GetGate("xor4")->GetPin("out")->ConnectTo(parity->GetGate("xor7")->GetPin("in2"));
		parity->GetGate("xor4")->GetPin("out")->ConnectTo(parity->GetGate("xor9")->GetPin("in2"));

		parity->GetGate("xor6")->GetPin("out")->ConnectTo(parity->GetGate("not")->GetPin("in"));
		parity->GetGate("not")->GetPin("out")->ConnectTo(parity->GetGate("xor8")->GetPin("in2"));
		parity->GetGate("xor6")->GetPin("out")->ConnectTo(parity->GetGate("xor10")->GetPin("in2"));

		parity->GetGate("xor7")->GetPin("out")->ConnectTo(parity->GetGate("xor8")->GetPin("in1"));
		parity->GetGate("xor9")->GetPin("out")->ConnectTo(parity->GetGate("xor10")->GetPin("in1"));

		parity->GetGate("xor8")->GetPin("out")->ConnectTo(parity->GetPin("EVEN"));
		parity->GetGate("xor10")->GetPin("out")->ConnectTo(parity->GetPin("ODD"));

		Tools::LogicTools::PrintTruthTable(parity);

	}
	catch (std::exception & e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}

    return 0;
}

