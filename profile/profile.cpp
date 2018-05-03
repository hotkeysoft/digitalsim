// profile.cpp : Defines the entry point for the console application.
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

int main()
{
	CompositeGatePtr adder16 = CompositeGate::Create("adder16");
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

	adder16->GetPin("x")->Set(IOState::FromInt(56789, 16));
	adder16->GetPin("y")->Set(IOState::FromInt(11111, 16));
	std::cout << "56789 + 11111 = " << adder16->GetPin("s")->Get().ToInt16() << std::endl;

	adder16->GetPin("x")->Set(IOState::FromInt(3333, 16));
	adder16->GetPin("y")->Set(IOState::FromInt(4444, 16));
	std::cout << "3333 + 4444 = " << adder16->GetPin("s")->Get().ToInt16() << std::endl;

	for (int i = 0; i < 100; ++i)
	{
		adder16->GetPin("x")->Set(IOState::FromInt(i*10, 16));
		adder16->GetPin("y")->Set(IOState::FromInt(i*300, 16));
		std::cout << i*10 << " + " << i*300 << " = " << adder16->GetPin("s")->Get().ToInt16() << std::endl;
	}


    return 0;
}

