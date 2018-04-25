#include "stdafx.h"
#include "CppUnitTest.h"
#include "GateBase.h"
#include "NOTGate.h"
#include "ANDGate.h"
#include "NANDGate.h"
#include "ORGate.h"
#include "XORGate.h"
#include "WireGate.h"
#include "LogicTools.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework {
			template<> std::wstring ToString<IOPin::IO_DIRECTION>(const IOPin::IO_DIRECTION& t) { return t == IOPin::INPUT ? L"INPUT" : L"OUTPUT"; }
			template<> std::wstring ToString<IOPin::IO_STATE>(const IOPin::IO_STATE& t)
			{
				switch (t)
				{
				case IOPin::HI: return L"HI";
				case IOPin::LOW: return L"LOW";
				case IOPin::UNDEF: return L"UNDEF";
				}
				return L"undefined state";
			}
			template<> std::wstring ToString<LogicTools::IOStateList>(const LogicTools::IOStateList& t)
			{
				std::wostringstream os;
				for (auto state : t)
				{
					switch (state)
					{
					case IOPin::HI:
						os << L"HI";
						break;
					case IOPin::LOW:
						os << L"LOW";
						break;
					case IOPin::UNDEF:
						os << L"UNDEF";
						break;
					default:
						os << L"*INVALID*";
					}
					os << ",";
				}
				std::wstring ret = os.str();
				if (ret.back() == L',')
				{
					ret.erase(ret.size() - 1);
				}
				return ret;
			}

		}
	}
}

namespace UnitTests
{
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


	};
}