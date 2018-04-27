// proto.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <initializer_list>
#include <vector>
#include "BasicGates\ANDGate.h"
#include "BasicGates\NANDGate.h"
#include "BasicGates\ORGate.h"
#include "BasicGates\NOTGate.h"
#include "BasicGates\WireGate.h"
#include "LogicTools.h"

int main()
{
	try
	{
		std::cout << "AND Gate" << std::endl;
		ANDGate andGate;
		LogicTools::PrintTruthTable({ andGate.GetPin("in1"), andGate.GetPin("in2") }, { andGate.GetPin("out") });

		std::cout << "NAND Gate" << std::endl;
		NANDGate nandGate;
		LogicTools::PrintTruthTable({ nandGate.GetPin("in1"), nandGate.GetPin("in2") }, { nandGate.GetPin("out") });

		std::cout << "OR Gate" << std::endl;
		ORGate orGate;
		LogicTools::PrintTruthTable({ orGate.GetPin("in1"), orGate.GetPin("in2") }, { orGate.GetPin("out") });

		std::cout << "Quad input OR Gate" << std::endl;
		ORGate or1;
		ORGate or2;
		ORGate or3;

		or1.GetPin("out")->ConnectTo(or3.GetPin("in1"));
		or2.GetPin("out")->ConnectTo(or3.GetPin("in2"));
		
		LogicTools::PrintTruthTable({ or1.GetPin("in1"), or1.GetPin("in2"), or2.GetPin("in1"), or2.GetPin("in2") }, { or3.GetPin("out") });

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

		LogicTools::PrintTruthTable({ EN.GetPin("in"), I1.GetPin("in"), I0.GetPin("in") }, { Y3.GetPin("out"), Y2.GetPin("out"), Y1.GetPin("out"), Y0.GetPin("out") });

		// Create component
		GateBase component("DECODER");
		component.AddInput("EN");
		component.AddInput("I0");
		component.AddInput("I1");
		component.AddOutput("Y0");
		component.AddOutput("Y1");
		component.AddOutput("Y2");
		component.AddOutput("Y3");

		component.AddGate("NOTI0", new NOTGate());
		component.AddGate("NOTI1", new NOTGate());
		component.AddGate("AND0", new ANDGate(3));
		component.AddGate("AND1", new ANDGate(3));
		component.AddGate("AND2", new ANDGate(3));
		component.AddGate("AND3", new ANDGate(3));

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

		LogicTools::PrintTruthTable(	
			{ component.GetPin("EN"), component.GetPin("I1"), component.GetPin("I0") },
			{ component.GetPin("Y3"), component.GetPin("Y2"), component.GetPin("Y1"), component.GetPin("Y0") });

		LogicTools::PrintTruthTable(&component);
	}
	catch (std::exception & e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}

    return 0;
}

