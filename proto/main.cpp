// proto.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <initializer_list>
#include <vector>
#include "ANDGate.h"
#include "NANDGate.h"
#include "ORGate.h"
#include "NOTGate.h"
#include "WireGate.h"
#include "Component.h"

void TruthTable(size_t level, std::vector<IOPin*> const& inputs, std::initializer_list<IOPin*> const& outputs)
{
	if (level <= inputs.size()-1)
	{

		inputs[level]->Set(IOPin::LOW);
		TruthTable(level+1, inputs, outputs);

		inputs[level]->Set(IOPin::HI);
		TruthTable(level+1, inputs, outputs);
	}
	else
	{
		for (auto pin : inputs)
		{
			std::cout << pin->Get() << "\t";
		}
		for (auto pin : outputs)
		{	
			std::cout << pin->Get() << "\t";
		}
		std::cout << std::endl;
	}
}

void TruthTable(std::initializer_list<IOPin*> const& inputs, std::initializer_list<IOPin*> const& outputs)
{
	std::vector<IOPin*> inputsVect;

	for (auto pin : inputs)
	{
		inputsVect.push_back(pin);
	}

	for (auto pin : inputs)
	{
		std::cout << pin->GetName() << "\t";
	}

	for (auto pin : outputs)
	{
		std::cout << pin->GetName() << "\t";
	}
	std::cout << std::endl;

	TruthTable(0, inputsVect, outputs);
	std::cout << std::endl;
}

int main()
{
	try
	{
		std::cout << "AND Gate" << std::endl;
		ANDGate andGate;
		TruthTable({ andGate.GetPin("in1"), andGate.GetPin("in2") }, { andGate.GetPin("out") });

		std::cout << "NAND Gate" << std::endl;
		NANDGate nandGate;
		TruthTable({ nandGate.GetPin("in1"), nandGate.GetPin("in2") }, { nandGate.GetPin("out") });

		std::cout << "OR Gate" << std::endl;
		ORGate orGate;
		TruthTable({ orGate.GetPin("in1"), orGate.GetPin("in2") }, { orGate.GetPin("out") });

		std::cout << "Quad input OR Gate" << std::endl;
		ORGate or1;
		ORGate or2;
		ORGate or3;

		or1.GetPin("out")->ConnectTo(or3.GetPin("in1"));
		or2.GetPin("out")->ConnectTo(or3.GetPin("in2"));
		
		TruthTable({ or1.GetPin("in1"), or1.GetPin("in2"), or2.GetPin("in1"), or2.GetPin("in2") }, { or3.GetPin("out") });

		std::cout << "2-to-4 decoder" << std::endl;
		WireGate I0, I1, EN;
		NOTGate NOTI0, NOTI1;
		ANDGate Y0(3), Y1(3), Y2(3), Y3(3);

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

		TruthTable({ EN.GetPin("in"), I1.GetPin("in"), I0.GetPin("in") }, { Y3.GetPin("out"), Y2.GetPin("out"), Y1.GetPin("out"), Y0.GetPin("out") });

		// Create component
		Component component;
		component.AddInput("EN");
		component.AddInput("I0");
		component.AddInput("I1");
		component.AddOutput("Y0");
		component.AddOutput("Y1");
		component.AddOutput("Y2");
		component.AddOutput("Y3");

	}
	catch (std::exception & e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}

    return 0;
}

