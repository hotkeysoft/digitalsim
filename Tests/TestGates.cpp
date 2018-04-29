#include "pch.h"
#pragma warning( disable:4251 )

#include "TestCommon.h"
#include "Core\GateBase.h"
#include "Core\CompositeGate.h"
#include "BasicGates\NOTGate.h"
#include "BasicGates\ANDGate.h"
#include "BasicGates\NANDGate.h"
#include "BasicGates\ORGate.h"
#include "BasicGates\XORGate.h"
#include "BasicGates\WireGate.h"
#include "BasicGates\BufferGate.h"
#include "Tools\LogicTools.h"

namespace UnitTests
{
	Core::CompositeGate* BuildDecoder()
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

	TEST(TestGates, TestNOTGate)
	{
		BasicGates::NOTGate * gate = new BasicGates::NOTGate();
		Tools::LogicTools::IOStateList out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::IOStateList compare = { Core::IOPin::HI, Core::IOPin::LOW };
		ASSERT_EQ(compare, out);
		delete gate;
	}

	TEST(TestGates, TestWireGate)
	{
		BasicGates::WireGate * gate = new BasicGates::WireGate();
		Tools::LogicTools::IOStateList out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::IOStateList compare = { Core::IOPin::LOW, Core::IOPin::HI };
		ASSERT_EQ(compare, out);
		delete gate;
	}

	TEST(TestGates, TestANDGate)
	{
		BasicGates::ANDGate * gate = new BasicGates::ANDGate();
		Tools::LogicTools::IOStateList out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::IOStateList compare = { Core::IOPin::LOW, Core::IOPin::LOW, Core::IOPin::LOW, Core::IOPin::HI };
		ASSERT_EQ(compare, out);
		delete gate;
	}

	TEST(TestGates, TestANDGate4)
	{
		BasicGates::ANDGate * gate = new BasicGates::ANDGate(4);
		Tools::LogicTools::IOStateList out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::IOStateList compare = { 
			Core::IOPin::LOW, Core::IOPin::LOW, Core::IOPin::LOW, Core::IOPin::LOW,
			Core::IOPin::LOW, Core::IOPin::LOW, Core::IOPin::LOW, Core::IOPin::LOW,
			Core::IOPin::LOW, Core::IOPin::LOW, Core::IOPin::LOW, Core::IOPin::LOW,
			Core::IOPin::LOW, Core::IOPin::LOW, Core::IOPin::LOW, Core::IOPin::HI,
		};
		ASSERT_EQ(compare, out);
		delete gate;
	}

	TEST(TestGates, TestORGate)
	{
		BasicGates::ORGate * gate = new BasicGates::ORGate();
		Tools::LogicTools::IOStateList out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::IOStateList compare = { Core::IOPin::LOW, Core::IOPin::HI, Core::IOPin::HI, Core::IOPin::HI };
		ASSERT_EQ(compare, out);
		delete gate;
	}

	TEST(TestGates, TestORGate4)
	{
		BasicGates::ORGate * gate = new BasicGates::ORGate(4);
		Tools::LogicTools::IOStateList out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::IOStateList compare = {
			Core::IOPin::LOW, Core::IOPin::HI, Core::IOPin::HI, Core::IOPin::HI,
			Core::IOPin::HI, Core::IOPin::HI, Core::IOPin::HI, Core::IOPin::HI,
			Core::IOPin::HI, Core::IOPin::HI, Core::IOPin::HI, Core::IOPin::HI,
			Core::IOPin::HI, Core::IOPin::HI, Core::IOPin::HI, Core::IOPin::HI,
		};
		ASSERT_EQ(compare, out);
		delete gate;
	}

	TEST(TestGates, TestXORGate)
	{
		BasicGates::XORGate * gate = new BasicGates::XORGate();
		Tools::LogicTools::IOStateList out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::IOStateList compare = { Core::IOPin::LOW, Core::IOPin::HI, Core::IOPin::HI, Core::IOPin::LOW };
		ASSERT_EQ(compare, out);
		delete gate;
	}

	TEST(TestGates, TestNANDGate)
	{
		BasicGates::NANDGate * gate = new BasicGates::NANDGate();
		Tools::LogicTools::IOStateList out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::IOStateList compare = { Core::IOPin::HI, Core::IOPin::HI, Core::IOPin::HI, Core::IOPin::LOW };
		ASSERT_EQ(compare, out);
		delete gate;
	}

	TEST(TestGates, TestNANDGate4)
	{
		BasicGates::NANDGate * gate = new BasicGates::NANDGate(4);
		Tools::LogicTools::IOStateList out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::IOStateList compare = {
			Core::IOPin::HI, Core::IOPin::HI, Core::IOPin::HI, Core::IOPin::HI,
			Core::IOPin::HI, Core::IOPin::HI, Core::IOPin::HI, Core::IOPin::HI,
			Core::IOPin::HI, Core::IOPin::HI, Core::IOPin::HI, Core::IOPin::HI,
			Core::IOPin::HI, Core::IOPin::HI, Core::IOPin::HI, Core::IOPin::LOW,
		};
		ASSERT_EQ(compare, out);
		delete gate;
	}

	TEST(TestGates, TestCustomDecoder)
	{
		Core::GateBase * component = BuildDecoder();
		
		ASSERT_NE(nullptr, component);

		Tools::LogicTools::IOStateList out = Tools::LogicTools::GetTruthTable(component);
		Tools::LogicTools::IOStateList compare = {
			Core::IOPin::LOW, Core::IOPin::LOW, Core::IOPin::LOW, Core::IOPin::LOW,
			Core::IOPin::LOW, Core::IOPin::LOW, Core::IOPin::LOW, Core::IOPin::LOW,
			Core::IOPin::LOW, Core::IOPin::LOW, Core::IOPin::LOW, Core::IOPin::LOW,
			Core::IOPin::LOW, Core::IOPin::LOW, Core::IOPin::LOW, Core::IOPin::LOW,
			Core::IOPin::HI, Core::IOPin::LOW, Core::IOPin::LOW, Core::IOPin::LOW,
			Core::IOPin::LOW, Core::IOPin::LOW, Core::IOPin::HI, Core::IOPin::LOW,
			Core::IOPin::LOW, Core::IOPin::HI, Core::IOPin::LOW, Core::IOPin::LOW,
			Core::IOPin::LOW, Core::IOPin::LOW, Core::IOPin::LOW, Core::IOPin::HI,
		};
		ASSERT_EQ(compare, out);
		delete component;
	}

	TEST(TestGates, TestCustomComplexComponent)
	{
		Core::GateBase * decoder1 = BuildDecoder();
		ASSERT_NE(nullptr, decoder1);

		Core::GateBase * decoder2 = BuildDecoder();
		ASSERT_NE(nullptr, decoder2);

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

		Tools::LogicTools::IOStateList out = Tools::LogicTools::GetTruthTable(decoder3to8);

		Tools::LogicTools::IOStateList compare = {
			Core::IOPin::HI, Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,
			Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::HI ,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,
			Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::HI ,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,
			Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::HI ,Core::IOPin::LOW,
			Core::IOPin::LOW,Core::IOPin::HI ,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,
			Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::HI ,Core::IOPin::LOW,Core::IOPin::LOW,
			Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::HI ,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,
			Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::LOW,Core::IOPin::HI 
		};

		ASSERT_EQ(compare, out);
		delete decoder3to8;
	}

	TEST(TestGates, TestClone)
	{
		Core::CompositeGate* comp = new Core::CompositeGate("comp");

		BasicGates::ANDGate* andGate = new BasicGates::ANDGate();

		// Share one input to avoir truth table explosion
		Core::IOPin* input = comp->AddInput("masterIn");

		comp->AddGate("and", andGate);
		input->ConnectTo(andGate->GetPin("in1"));
		comp->AddInput("and2")->ConnectTo(andGate->GetPin("in2"));
		andGate->GetPin("out")->ConnectTo(comp->AddOutput("andout"));

		BasicGates::ORGate* orGate = new BasicGates::ORGate();
		comp->AddGate("or", orGate);
		input->ConnectTo(orGate->GetPin("in1"));
		comp->AddInput("or2")->ConnectTo(orGate->GetPin("in2"));
		orGate->GetPin("out")->ConnectTo(comp->AddOutput("orout"));

		BasicGates::XORGate* xorGate = new BasicGates::XORGate();
		comp->AddGate("xor", xorGate);
		input->ConnectTo(xorGate->GetPin("in1"));
		comp->AddInput("xor2")->ConnectTo(xorGate->GetPin("in2"));
		xorGate->GetPin("out")->ConnectTo(comp->AddOutput("xorout"));

		BasicGates::NANDGate* nandGate = new BasicGates::NANDGate();
		comp->AddGate("nand", nandGate);
		input->ConnectTo(nandGate->GetPin("in1"));
		comp->AddInput("nand2")->ConnectTo(nandGate->GetPin("in2"));
		nandGate->GetPin("out")->ConnectTo(comp->AddOutput("nandout"));

		BasicGates::NOTGate* notGate = new BasicGates::NOTGate();
		comp->AddGate("not", notGate);
		input->ConnectTo(notGate->GetPin("in"));
		notGate->GetPin("out")->ConnectTo(comp->AddOutput("notout"));

		BasicGates::WireGate* wireGate = new BasicGates::WireGate();
		comp->AddGate("wire", wireGate);
		input->ConnectTo(wireGate->GetPin("in"));
		wireGate->GetPin("out")->ConnectTo(comp->AddOutput("wireout"));

		BasicGates::BufferGate* bufferGate = new BasicGates::BufferGate();
		comp->AddGate("buffer", bufferGate);
		input->ConnectTo(bufferGate->GetPin("in"));
		comp->AddInput("bufferEN")->ConnectTo(bufferGate->GetPin("en"));
		bufferGate->GetPin("out")->ConnectTo(comp->AddOutput("bufferout"));

		ASSERT_EQ(7, comp->GetGateCount());

		Tools::LogicTools::IOStateList out = Tools::LogicTools::GetTruthTable(comp);
		ASSERT_EQ(448, out.size());

		Core::GateBase* clone = comp->Clone("clone");
		ASSERT_NE(nullptr, clone);
		ASSERT_NE(comp, clone);

		Tools::LogicTools::IOStateList outClone = Tools::LogicTools::GetTruthTable(clone);
		ASSERT_EQ(448, outClone.size());

		ASSERT_EQ(out, outClone);
	}

}