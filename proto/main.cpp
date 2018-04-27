// proto.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <initializer_list>
#include <vector>
#include "CompositeGate.h"
#include "BasicGates\ANDGate.h"
#include "BasicGates\NANDGate.h"
#include "BasicGates\ORGate.h"
#include "BasicGates\NOTGate.h"
#include "BasicGates\WireGate.h"
#include "LogicTools.h"

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

	component->AddGate("NOTI0", new NOTGate());
	component->AddGate("NOTI1", new NOTGate());
	component->AddGate("AND0", new ANDGate(3));
	component->AddGate("AND1", new ANDGate(3));
	component->AddGate("AND2", new ANDGate(3));
	component->AddGate("AND3", new ANDGate(3));

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

	NOTGate *notI1 = new NOTGate();

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
		CompositeGate component("DECODER");
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

		GateBase* decoder = Build3to8Decoder();		
		LogicTools::PrintInternalConnections(decoder);
		std::cout << std::endl;
		GateBase* clone = decoder->Clone("clone");
		LogicTools::PrintInternalConnections(clone);

		GateBase* base = BuildDecoder();
		GateBase* cloned = base->Clone("clone");

		base->GetPin("EN")->Set(IOPin::LOW);
		base->GetPin("I0")->Set(IOPin::LOW);
		base->GetPin("I1")->Set(IOPin::LOW);

		cloned->GetPin("EN")->Set(IOPin::LOW);
		cloned->GetPin("I0")->Set(IOPin::LOW);
		cloned->GetPin("I1")->Set(IOPin::LOW);
		
		CompositeGate* reverseDecoder = new CompositeGate("reverseDecoder");
		reverseDecoder->AddInput("EN");
		reverseDecoder->AddInput("I0");
		reverseDecoder->AddInput("I1");
		reverseDecoder->AddOutput("N0");
		reverseDecoder->AddOutput("N1");
		reverseDecoder->AddOutput("N2");
		reverseDecoder->AddOutput("N3");

		reverseDecoder->AddGate("not0", new NOTGate());
		reverseDecoder->AddGate("not1", new NOTGate());
		reverseDecoder->AddGate("not2", new NOTGate());
		reverseDecoder->AddGate("not3", new NOTGate());
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

		LogicTools::PrintTruthTable(base);
		LogicTools::PrintTruthTable(reverseDecoder);

	}
	catch (std::exception & e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}

    return 0;
}

