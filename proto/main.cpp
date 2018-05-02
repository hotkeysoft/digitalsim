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
#include "BasicGates\BufferGate.h"
#include "Tools\LogicTools.h"

using namespace DigiLib;
using namespace DigiLib::Core;

CompositeGate* BuildFullAdder()
{
	CompositeGate * component = new CompositeGate("FULLADDER");
	component->AddInput("x");
	component->AddInput("y");
	component->AddInput("cin");
	component->AddOutput("s");
	component->AddOutput("cout");

	BasicGates::XORGate* xor1 = new BasicGates::XORGate();
	BasicGates::XORGate* xor2 = new BasicGates::XORGate();

	BasicGates::ANDGate* and1 = new BasicGates::ANDGate();
	BasicGates::ANDGate* and2 = new BasicGates::ANDGate();
	BasicGates::ANDGate* and3 = new BasicGates::ANDGate();

	BasicGates::ORGate* or1 = new BasicGates::ORGate(3);

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

CompositeGate* Build4BitAdder()
{
	CompositeGate * adder4 = new CompositeGate("adder4");
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

GateBase* BuildDecoder()
{
	CompositeGate * component = new CompositeGate("DECODER");
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

GateBase* Build3to8Decoder()
{
	GateBase * decoder1 = BuildDecoder();
	GateBase * decoder2 = BuildDecoder();

	BasicGates::NOTGate *notI1 = new BasicGates::NOTGate();

	CompositeGate * decoder3to8 = new CompositeGate("Decoder3to8");
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
		std::cout << Tools::LogicTools::PrintTruthTable({ andGate.GetPin("in1"), andGate.GetPin("in2") }, { andGate.GetPin("out") });

		std::cout << "NAND Gate" << std::endl;
		BasicGates::NANDGate nandGate;
		std::cout << Tools::LogicTools::PrintTruthTable({ nandGate.GetPin("in1"), nandGate.GetPin("in2") }, { nandGate.GetPin("out") });

		std::cout << "OR Gate" << std::endl;
		BasicGates::ORGate orGate;
		std::cout << Tools::LogicTools::PrintTruthTable({ orGate.GetPin("in1"), orGate.GetPin("in2") }, { orGate.GetPin("out") });

		std::cout << "Quad input OR Gate" << std::endl;
		BasicGates::ORGate or1;
		BasicGates::ORGate or2;
		BasicGates::ORGate or3;

		or1.GetPin("out")->ConnectTo(or3.GetPin("in1"));
		or2.GetPin("out")->ConnectTo(or3.GetPin("in2"));
		
		std::cout << Tools::LogicTools::PrintTruthTable({ or1.GetPin("in1"), or1.GetPin("in2"), or2.GetPin("in1"), or2.GetPin("in2") }, { or3.GetPin("out") });

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

		std::cout << Tools::LogicTools::PrintTruthTable({ EN.GetPin("in"), I1.GetPin("in"), I0.GetPin("in") }, { Y3.GetPin("out"), Y2.GetPin("out"), Y1.GetPin("out"), Y0.GetPin("out") });

		// Create component
		CompositeGate component("DECODER");
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

		std::cout << Tools::LogicTools::PrintTruthTable(
			{ component.GetPin("EN"), component.GetPin("I1"), component.GetPin("I0") },
			{ component.GetPin("Y3"), component.GetPin("Y2"), component.GetPin("Y1"), component.GetPin("Y0") });

		std::cout << Tools::LogicTools::PrintTruthTable(&component);

		GateBase* decoder = Build3to8Decoder();
		std::cout << Tools::LogicTools::PrintInternalConnections(decoder);
		std::cout << std::endl;
		GateBase* clone = decoder->Clone("clone");
		std::cout << Tools::LogicTools::PrintInternalConnections(clone);

		GateBase* base = BuildDecoder();
		GateBase* cloned = base->Clone("clone");

		base->GetPin("EN")->Set(IOState::LOW);
		base->GetPin("I0")->Set(IOState::LOW);
		base->GetPin("I1")->Set(IOState::LOW);

		cloned->GetPin("EN")->Set(IOState::LOW);
		cloned->GetPin("I0")->Set(IOState::LOW);
		cloned->GetPin("I1")->Set(IOState::LOW);
		
		CompositeGate* reverseDecoder = new CompositeGate("reverseDecoder");
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

		std::cout << Tools::LogicTools::PrintTruthTable(base);
		std::cout << Tools::LogicTools::PrintTruthTable(reverseDecoder);

		CompositeGate* parity = new CompositeGate("parity");
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

		std::cout << Tools::LogicTools::PrintTruthTable(parity);

		BasicGates::BufferGate* bufferGate = new BasicGates::BufferGate();
		std::cout << "-----------" << std::endl;
		std::cout << "Buffer gate" << std::endl;
		std::cout << Tools::LogicTools::PrintTruthTable(bufferGate);


		std::cout << "---------------------" << std::endl;
		std::cout << "Building 16 bit adder" << std::endl;
		CompositeGate* adder16 = new CompositeGate("adder16");
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

		adder16->GetGate("a1")->GetPin("cin")->Set(IOState::LOW);
		adder16->GetGate("a1")->GetPin("cout")->ConnectTo(adder16->GetGate("a2")->GetPin("cin"));
		adder16->GetGate("a2")->GetPin("cout")->ConnectTo(adder16->GetGate("a3")->GetPin("cin"));
		adder16->GetGate("a3")->GetPin("cout")->ConnectTo(adder16->GetGate("a4")->GetPin("cin"));
	
		adder16->GetPin("x")->Set(IOState::FromInt(12345, 16));
		adder16->GetPin("y")->Set(IOState::FromInt(34567, 16));
		std::cout << "12345 + 34567 = " << adder16->GetPin("s")->Get().ToInt16() << std::endl;

		std::string toto;
		std::cout << "Pause" << std::endl;
		getchar();

		GateBase* cloneAdder = adder16->Clone("clone");
		cloneAdder->GetPin("x")->Set(IOState::FromInt(123, 16));
		cloneAdder->GetPin("y")->Set(IOState::FromInt(321, 16));
		std::cout << "123 + 321 = " << cloneAdder->GetPin("s")->Get().ToInt16() << std::endl;
		std::cout << "Pause" << std::endl;
		getchar();
		
		delete cloneAdder;
		std::cout << "Pause" << std::endl;
		getchar();


	}
	catch (std::exception & e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}

    return 0;
}

