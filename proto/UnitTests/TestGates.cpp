#include "stdafx.h"
#include <CppUnitTest.h>
#include "TestCommon.h"
#include "GateBase.h"
#include "BasicGates\NOTGate.h"
#include "BasicGates\ANDGate.h"
#include "BasicGates\NANDGate.h"
#include "BasicGates\ORGate.h"
#include "BasicGates\XORGate.h"
#include "BasicGates\WireGate.h"
#include "LogicTools.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
	GateBase* BuildDecoder()
	{
		GateBase * component = new GateBase("DECODER");
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

	TEST_CLASS(TestGates)
	{
	public:
		TEST_METHOD(TestNOTGate)
		{
			NOTGate * gate = new NOTGate();
			LogicTools::IOStateList out = LogicTools::GetTruthTable(gate);
			LogicTools::IOStateList compare = { IOPin::HI, IOPin::LOW };
			Assert::AreEqual(compare, out);
			delete gate;
		}

		TEST_METHOD(TestWireGate)
		{
			WireGate * gate = new WireGate();
			LogicTools::IOStateList out = LogicTools::GetTruthTable(gate);
			LogicTools::IOStateList compare = { IOPin::LOW, IOPin::HI };
			Assert::AreEqual(compare, out);
			delete gate;
		}

		TEST_METHOD(TestANDGate)
		{
			ANDGate * gate = new ANDGate();
			LogicTools::IOStateList out = LogicTools::GetTruthTable(gate);
			LogicTools::IOStateList compare = { IOPin::LOW, IOPin::LOW, IOPin::LOW, IOPin::HI };
			Assert::AreEqual(compare, out);
			delete gate;
		}


		TEST_METHOD(TestANDGate4)
		{
			ANDGate * gate = new ANDGate(4);
			LogicTools::IOStateList out = LogicTools::GetTruthTable(gate);
			LogicTools::IOStateList compare = { 
				IOPin::LOW, IOPin::LOW, IOPin::LOW, IOPin::LOW,
				IOPin::LOW, IOPin::LOW, IOPin::LOW, IOPin::LOW,
				IOPin::LOW, IOPin::LOW, IOPin::LOW, IOPin::LOW,
				IOPin::LOW, IOPin::LOW, IOPin::LOW, IOPin::HI,
			};
			Assert::AreEqual(compare, out);
			delete gate;
		}

		TEST_METHOD(TestORGate)
		{
			ORGate * gate = new ORGate();
			LogicTools::IOStateList out = LogicTools::GetTruthTable(gate);
			LogicTools::IOStateList compare = { IOPin::LOW, IOPin::HI, IOPin::HI, IOPin::HI };
			Assert::AreEqual(compare, out);
			delete gate;
		}

		TEST_METHOD(TestORGate4)
		{
			ORGate * gate = new ORGate(4);
			LogicTools::IOStateList out = LogicTools::GetTruthTable(gate);
			LogicTools::IOStateList compare = {
				IOPin::LOW, IOPin::HI, IOPin::HI, IOPin::HI,
				IOPin::HI, IOPin::HI, IOPin::HI, IOPin::HI,
				IOPin::HI, IOPin::HI, IOPin::HI, IOPin::HI,
				IOPin::HI, IOPin::HI, IOPin::HI, IOPin::HI,
			};
			Assert::AreEqual(compare, out);
			delete gate;
		}

		TEST_METHOD(TestXORGate)
		{
			XORGate * gate = new XORGate();
			LogicTools::IOStateList out = LogicTools::GetTruthTable(gate);
			LogicTools::IOStateList compare = { IOPin::LOW, IOPin::HI, IOPin::HI, IOPin::LOW };
			Assert::AreEqual(compare, out);
			delete gate;
		}

		TEST_METHOD(TestNANDGate)
		{
			NANDGate * gate = new NANDGate();
			LogicTools::IOStateList out = LogicTools::GetTruthTable(gate);
			LogicTools::IOStateList compare = { IOPin::HI, IOPin::HI, IOPin::HI, IOPin::LOW };
			Assert::AreEqual(compare, out);
			delete gate;
		}

		TEST_METHOD(TestNANDGate4)
		{
			NANDGate * gate = new NANDGate(4);
			LogicTools::IOStateList out = LogicTools::GetTruthTable(gate);
			LogicTools::IOStateList compare = {
				IOPin::HI, IOPin::HI, IOPin::HI, IOPin::HI,
				IOPin::HI, IOPin::HI, IOPin::HI, IOPin::HI,
				IOPin::HI, IOPin::HI, IOPin::HI, IOPin::HI,
				IOPin::HI, IOPin::HI, IOPin::HI, IOPin::LOW,
			};
			Assert::AreEqual(compare, out);
			delete gate;
		}

		TEST_METHOD(TestCustomDecoder)
		{
			GateBase * component = BuildDecoder();
			Assert::IsNotNull(component);

			LogicTools::IOStateList out = LogicTools::GetTruthTable(component);
			LogicTools::IOStateList compare = {
				IOPin::LOW, IOPin::LOW, IOPin::LOW, IOPin::LOW,
				IOPin::LOW, IOPin::LOW, IOPin::LOW, IOPin::LOW,
				IOPin::LOW, IOPin::LOW, IOPin::LOW, IOPin::LOW,
				IOPin::LOW, IOPin::LOW, IOPin::LOW, IOPin::LOW,
				IOPin::HI, IOPin::LOW, IOPin::LOW, IOPin::LOW,
				IOPin::LOW, IOPin::LOW, IOPin::HI, IOPin::LOW,
				IOPin::LOW, IOPin::HI, IOPin::LOW, IOPin::LOW,
				IOPin::LOW, IOPin::LOW, IOPin::LOW, IOPin::HI,
			};
			Assert::AreEqual(compare, out);
			delete component;
		}

		TEST_METHOD(TestCustomComplexComponent)
		{
			GateBase * decoder1 = BuildDecoder();
			Assert::IsNotNull(decoder1);

			GateBase * decoder2 = BuildDecoder();
			Assert::IsNotNull(decoder2);

			NOTGate *notI1 = new NOTGate();

			GateBase * decoder3to8 = new GateBase("Decoder3to8");
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

			LogicTools::IOStateList out = LogicTools::GetTruthTable(decoder3to8);

			LogicTools::IOStateList compare = {
				IOPin::HI, IOPin::LOW,IOPin::LOW,IOPin::LOW,IOPin::LOW,IOPin::LOW,IOPin::LOW,IOPin::LOW,
				IOPin::LOW,IOPin::LOW,IOPin::LOW,IOPin::LOW,IOPin::HI ,IOPin::LOW,IOPin::LOW,IOPin::LOW,
				IOPin::LOW,IOPin::LOW,IOPin::HI ,IOPin::LOW,IOPin::LOW,IOPin::LOW,IOPin::LOW,IOPin::LOW,
				IOPin::LOW,IOPin::LOW,IOPin::LOW,IOPin::LOW,IOPin::LOW,IOPin::LOW,IOPin::HI ,IOPin::LOW,
				IOPin::LOW,IOPin::HI ,IOPin::LOW,IOPin::LOW,IOPin::LOW,IOPin::LOW,IOPin::LOW,IOPin::LOW,
				IOPin::LOW,IOPin::LOW,IOPin::LOW,IOPin::LOW,IOPin::LOW,IOPin::HI ,IOPin::LOW,IOPin::LOW,
				IOPin::LOW,IOPin::LOW,IOPin::LOW,IOPin::HI ,IOPin::LOW,IOPin::LOW,IOPin::LOW,IOPin::LOW,
				IOPin::LOW,IOPin::LOW,IOPin::LOW,IOPin::LOW,IOPin::LOW,IOPin::LOW,IOPin::LOW,IOPin::HI 
			};

			Assert::AreEqual(compare, out);
			delete decoder3to8;
		}

	};
}