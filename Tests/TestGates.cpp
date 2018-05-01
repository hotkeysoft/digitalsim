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

	CompositeGate* BuildDecoder()
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

	TEST(TestGates, TestNOTGate)
	{
		BasicGates::NOTGate * gate = new BasicGates::NOTGate();
		Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::ResultListType compare({ IOState::HI, IOState::LOW });
		ASSERT_EQ(compare, out);
		delete gate;
	}

	TEST(TestGates, TestWireGate)
	{
		BasicGates::WireGate * gate = new BasicGates::WireGate();
		Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::ResultListType compare({ IOState::LOW, IOState::HI });
		ASSERT_EQ(compare, out);
		delete gate;
	}

	TEST(TestGates, TestANDGate)
	{
		BasicGates::ANDGate * gate = new BasicGates::ANDGate();
		Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::ResultListType compare({ IOState::LOW, IOState::LOW, IOState::LOW, IOState::HI });
		ASSERT_EQ(compare, out);
		delete gate;
	}

	TEST(TestGates, TestANDGate4)
	{
		BasicGates::ANDGate * gate = new BasicGates::ANDGate(4);
		Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::ResultListType compare({
			IOState::LOW, IOState::LOW, IOState::LOW, IOState::LOW,
			IOState::LOW, IOState::LOW, IOState::LOW, IOState::LOW,
			IOState::LOW, IOState::LOW, IOState::LOW, IOState::LOW,
			IOState::LOW, IOState::LOW, IOState::LOW, IOState::HI,
		});
		ASSERT_EQ(compare, out);
		delete gate;
	}

	TEST(TestGates, TestORGate)
	{
		BasicGates::ORGate * gate = new BasicGates::ORGate();
		Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::ResultListType compare({ IOState::LOW, IOState::HI, IOState::HI, IOState::HI });
		ASSERT_EQ(compare, out);
		delete gate;
	}

	TEST(TestGates, TestORGate4)
	{
		BasicGates::ORGate * gate = new BasicGates::ORGate(4);
		Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::ResultListType compare = {
			IOState::LOW, IOState::HI, IOState::HI, IOState::HI,
			IOState::HI, IOState::HI, IOState::HI, IOState::HI,
			IOState::HI, IOState::HI, IOState::HI, IOState::HI,
			IOState::HI, IOState::HI, IOState::HI, IOState::HI,
		};
		ASSERT_EQ(compare, out);
		delete gate;
	}

	TEST(TestGates, TestXORGate)
	{
		BasicGates::XORGate * gate = new BasicGates::XORGate();
		Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::ResultListType compare({ IOState::LOW, IOState::HI, IOState::HI, IOState::LOW });
		ASSERT_EQ(compare, out);
		delete gate;
	}

	TEST(TestGates, TestNANDGate)
	{
		BasicGates::NANDGate * gate = new BasicGates::NANDGate();
		Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::ResultListType compare({ IOState::HI, IOState::HI, IOState::HI, IOState::LOW });
		ASSERT_EQ(compare, out);
		delete gate;
	}

	TEST(TestGates, TestNANDGate4)
	{
		BasicGates::NANDGate * gate = new BasicGates::NANDGate(4);
		Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::ResultListType compare({
			IOState::HI, IOState::HI, IOState::HI, IOState::HI,
			IOState::HI, IOState::HI, IOState::HI, IOState::HI,
			IOState::HI, IOState::HI, IOState::HI, IOState::HI,
			IOState::HI, IOState::HI, IOState::HI, IOState::LOW,
		});
		ASSERT_EQ(compare, out);
		delete gate;
	}

	TEST(TestGates, TestBufferGate)
	{
		BasicGates::BufferGate * gate = new BasicGates::BufferGate();
		Tools::LogicTools::IOStateList out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::IOStateList compare = { Core::IOPin::HI_Z, Core::IOPin::HI_Z, Core::IOPin::LOW, Core::IOPin::HI };
		ASSERT_EQ(compare, out);
		delete gate;
	}
	
	TEST(TestGates, TestCustomDecoder)
	{
		GateBase * component = BuildDecoder();
		
		ASSERT_NE(nullptr, component);

		Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(component);
		Tools::LogicTools::ResultListType compare = {
			IOState::LOW, IOState::LOW, IOState::LOW, IOState::LOW,
			IOState::LOW, IOState::LOW, IOState::LOW, IOState::LOW,
			IOState::LOW, IOState::LOW, IOState::LOW, IOState::LOW,
			IOState::LOW, IOState::LOW, IOState::LOW, IOState::LOW,
			IOState::HI, IOState::LOW, IOState::LOW, IOState::LOW,
			IOState::LOW, IOState::LOW, IOState::HI, IOState::LOW,
			IOState::LOW, IOState::HI, IOState::LOW, IOState::LOW,
			IOState::LOW, IOState::LOW, IOState::LOW, IOState::HI,
		};
		ASSERT_EQ(compare, out);
		delete component;
	}

	TEST(TestGates, TestCustomComplexComponent)
	{
		GateBase * decoder1 = BuildDecoder();
		ASSERT_NE(nullptr, decoder1);

		GateBase * decoder2 = BuildDecoder();
		ASSERT_NE(nullptr, decoder2);

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

		Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(decoder3to8);

		Tools::LogicTools::ResultListType compare = {
			IOState::HI, IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,
			IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::HI ,IOState::LOW,IOState::LOW,IOState::LOW,
			IOState::LOW,IOState::LOW,IOState::HI ,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,
			IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::HI ,IOState::LOW,
			IOState::LOW,IOState::HI ,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,
			IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::HI ,IOState::LOW,IOState::LOW,
			IOState::LOW,IOState::LOW,IOState::LOW,IOState::HI ,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,
			IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::LOW,IOState::HI 
		};

		ASSERT_EQ(compare, out);
		delete decoder3to8;
	}

	TEST(TestGates, TestClone)
	{
		CompositeGate* comp = new CompositeGate("comp");

		BasicGates::ANDGate* andGate = new BasicGates::ANDGate();

		// Share one input to avoir truth table explosion
		IOPin* input = comp->AddInput("masterIn");

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

		Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(comp);
		ASSERT_EQ(448, out.size());

		GateBase* clone = comp->Clone("clone");
		ASSERT_NE(nullptr, clone);
		ASSERT_NE(comp, clone);

		Tools::LogicTools::ResultListType outClone = Tools::LogicTools::GetTruthTable(clone);
		ASSERT_EQ(448, outClone.size());

		ASSERT_EQ(out, outClone);
	}

	TEST(TestGates, TestFullAdder)
	{
		GateBase* gate = BuildFullAdder();		
		Tools::LogicTools::ResultListType out = Tools::LogicTools::GetTruthTable(gate);
		Tools::LogicTools::ResultListType compare({ IOState::LOW, IOState::LOW, IOState::LOW, IOState::HI, 
			IOState::LOW, IOState::HI, IOState::HI, IOState::LOW,
			IOState::LOW, IOState::HI, IOState::HI, IOState::LOW,
			IOState::HI, IOState::LOW, IOState::HI, IOState::HI });
		ASSERT_EQ(compare, out);

		TEST_COUT << "Building 4 bit adder";
		CompositeGate* adder4 = Build4BitAdder();

		adder4->GetPin("cin")->Set(IOState::LOW);
		adder4->GetPin("x")->Set(IOState::FromInt(2, 4));
		adder4->GetPin("y")->Set(IOState::FromInt(2, 4));
		ASSERT_EQ(IOState::FromInt(4, 4), adder4->GetPin("s")->Get());
		ASSERT_EQ(IOState(IOState::LOW), adder4->GetPin("cout")->Get());

		adder4->GetPin("x")->Set(IOState::FromInt(10, 4));
		adder4->GetPin("y")->Set(IOState::FromInt(5, 4));
		ASSERT_EQ(IOState::FromInt(15, 4), adder4->GetPin("s")->Get());
		ASSERT_EQ(IOState(IOState::LOW), adder4->GetPin("cout")->Get());

		adder4->GetPin("x")->Set(IOState::FromInt(15, 4));
		adder4->GetPin("y")->Set(IOState::FromInt(1, 4));
		ASSERT_EQ(IOState::FromInt(0, 4), adder4->GetPin("s")->Get());
		ASSERT_EQ(IOState(IOState::HI), adder4->GetPin("cout")->Get());

		adder4->GetPin("x")->Set(IOState::FromInt(15, 4));
		adder4->GetPin("y")->Set(IOState::FromInt(15, 4));
		ASSERT_EQ(IOState::FromInt(14, 4), adder4->GetPin("s")->Get());
		ASSERT_EQ(IOState(IOState::HI), adder4->GetPin("cout")->Get());

		adder4->GetPin("cin")->Set(IOState::HI);
		adder4->GetPin("x")->Set(IOState::FromInt(15, 4));
		adder4->GetPin("y")->Set(IOState::FromInt(15, 4));
		ASSERT_EQ(IOState::FromInt(15, 4), adder4->GetPin("s")->Get());
		ASSERT_EQ(IOState(IOState::HI), adder4->GetPin("cout")->Get());
	}
}