#include "stdafx.h"
#include <CppUnitTest.h>
#include "TestCommon.h"
#include "GateBase.h"
#include "CompositeGate.h"
#include "BasicGates\NOTGate.h"
#include "BasicGates\ANDGate.h"
#include "BasicGates\NANDGate.h"
#include "BasicGates\ORGate.h"
#include "BasicGates\XORGate.h"
#include "BasicGates\WireGate.h"
#include "BasicGates\BufferGate.h"
#include "LogicTools.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{		
	TEST_CLASS(TestGateBase)
	{
	public:
		TEST_METHOD(TestAddInput)
		{
			CompositeGate* gate = new CompositeGate("test");

			Logger::WriteMessage("TestAddInput: Bad input");
			Assert::ExpectException<std::exception>([gate] { gate->AddInput(NULL); });
			Assert::ExpectException<std::exception>([gate] { gate->AddInput(""); });
			Assert::ExpectException<std::exception>([gate] { gate->AddInput(" "); });
			Assert::ExpectException<std::exception>([gate] { gate->AddInput("3"); });
			Assert::ExpectException<std::exception>([gate] { gate->AddInput("3d"); });
			Assert::ExpectException<std::exception>([gate] { gate->AddInput("a b"); });
			Assert::ExpectException<std::exception>([gate] { gate->AddInput("a "); });
			Assert::ExpectException<std::exception>([gate] { gate->AddInput(" a"); });

			Logger::WriteMessage("TestAddInput: Good Input");
			gate->AddInput("a");
			Assert::AreEqual(1, (int)gate->GetInputCount());

			gate->AddInput("a12345");
			Assert::AreEqual(2, (int)gate->GetInputCount());
			Assert::AreEqual(0, (int)gate->GetOutputCount());

			Assert::IsNotNull(gate->GetPin("a"));
			Assert::IsNotNull(gate->GetPin("a12345"));
			
			Logger::WriteMessage("TestAddInput: Duplicate");
			Assert::ExpectException<std::exception>([gate] { gate->AddInput("a"); });
			Assert::ExpectException<std::exception>([gate] { gate->AddOutput("a"); });

			Assert::AreEqual(2, (int)gate->GetInputCount());
			Assert::AreEqual(0, (int)gate->GetOutputCount());

			delete gate;
		}

		TEST_METHOD(TestAddOutput)
		{
			CompositeGate* gate = new CompositeGate("test");

			Logger::WriteMessage("TestAddOutput: Bad Input");
			Assert::ExpectException<std::exception>([gate] { gate->AddOutput(NULL); });
			Assert::ExpectException<std::exception>([gate] { gate->AddOutput(""); });
			Assert::ExpectException<std::exception>([gate] { gate->AddOutput(" "); });
			Assert::ExpectException<std::exception>([gate] { gate->AddOutput("3"); });
			Assert::ExpectException<std::exception>([gate] { gate->AddOutput("3d"); });
			Assert::ExpectException<std::exception>([gate] { gate->AddOutput("a b"); });
			Assert::ExpectException<std::exception>([gate] { gate->AddOutput("a "); });
			Assert::ExpectException<std::exception>([gate] { gate->AddOutput(" a"); });

			Logger::WriteMessage("TestAddOutput: Good Input");
			gate->AddOutput("a");
			Assert::AreEqual(1, (int)gate->GetOutputCount());

			gate->AddOutput("a12345");
			Assert::AreEqual(2, (int)gate->GetOutputCount());
			Assert::AreEqual(0, (int)gate->GetInputCount());

			Assert::IsNotNull(gate->GetPin("a"));
			Assert::IsNotNull(gate->GetPin("a12345"));

			Logger::WriteMessage("TestAddOutput: Duplicate");
			Assert::ExpectException<std::exception>([gate] { gate->AddOutput("a"); });
			Assert::ExpectException<std::exception>([gate] { gate->AddInput("a"); });

			Assert::AreEqual(2, (int)gate->GetOutputCount());
			Assert::AreEqual(0, (int)gate->GetInputCount());

			delete gate;
		}

		TEST_METHOD(TestConnectTo)
		{
			CompositeGate* gate = new CompositeGate("test");
			gate->AddInput("in");
			gate->AddOutput("out");

			CompositeGate* gate2 = new CompositeGate("test2");
			gate2->AddInput("in");
			gate2->AddOutput("out");

			Logger::WriteMessage("TestConnectTo: Connect to self (out to in)");
			Assert::ExpectException<std::exception>([gate] { gate->GetPin("out")->ConnectTo(gate->GetPin("in")); });

			Logger::WriteMessage("TestConnectTo: Connect to self (in to in)");
			Assert::ExpectException<std::exception>([gate] { gate->GetPin("in")->ConnectTo(gate->GetPin("in")); });

			Logger::WriteMessage("TestConnectTo: Connect to self (out to out)");
			Assert::ExpectException<std::exception>([gate] { gate->GetPin("out")->ConnectTo(gate->GetPin("out")); });

			Logger::WriteMessage("TestConnectTo: Connect to self (in to out)");
			Assert::ExpectException<std::exception>([gate] { gate->GetPin("in")->ConnectTo(gate->GetPin("out")); });

			Logger::WriteMessage("TestConnectTo: Duplicate");
			gate->GetPin("out")->ConnectTo(gate2->GetPin("in"));
			Assert::ExpectException<std::exception>([&] { gate->GetPin("out")->ConnectTo(gate2->GetPin("in")); });

			delete gate;
			delete gate2;
		}

		TEST_METHOD(TestFeedback)
		{
			GateBase* gate = new NOTGate();
			GateBase* gate2 = new NOTGate();
			
			Logger::WriteMessage("TestFeedback: gate->gate2->gate");
			gate->GetPin("out")->ConnectTo(gate2->GetPin("in"));
			gate2->GetPin("out")->ConnectTo(gate->GetPin("in"));

			//gate->GetPin("in")->Set(IOPin::LOW);
			Assert::Fail(L"TODO");

			delete gate;
			delete gate2;
		}

		TEST_METHOD(TestAddGate)
		{
			CompositeGate * gate = new CompositeGate("test");
			GateBase * notGate = new NOTGate();
			GateBase * notGate2 = new NOTGate();

			Assert::AreEqual(0, (int)gate->GetGateCount());

			Logger::WriteMessage("TestAddGate: Bad Input");
			Assert::ExpectException<std::exception>([&] { gate->AddGate(NULL, notGate); });
			Assert::ExpectException<std::exception>([&] { gate->AddGate("", notGate); });
			Assert::ExpectException<std::exception>([&] { gate->AddGate(" ", notGate); });
			Assert::ExpectException<std::exception>([&] { gate->AddGate("3", notGate); });
			Assert::ExpectException<std::exception>([&] { gate->AddGate("3d", notGate); });
			Assert::ExpectException<std::exception>([&] { gate->AddGate("a b", notGate); });
			Assert::ExpectException<std::exception>([&] { gate->AddGate("a ", notGate); });
			Assert::ExpectException<std::exception>([&] { gate->AddGate(" a", notGate); });

			Assert::AreEqual(0, (int)gate->GetGateCount());

			gate->AddGate("not", notGate);
			Assert::AreEqual(1, (int)gate->GetGateCount());

			Logger::WriteMessage("TestAddGate: null gate");
			Assert::ExpectException<std::exception>([&] { gate->AddGate("null", NULL); });

			Logger::WriteMessage("TestAddGate: Duplicate name");
			Assert::ExpectException<std::exception>([&] { gate->AddGate("not", notGate2); });

			Assert::AreEqual(1, (int)gate->GetGateCount());

			Logger::WriteMessage("TestAddGate: Duplicate gate (TODO: Clone gate so this is possible)");
			Assert::ExpectException<std::exception>([&] { gate->AddGate("not2", notGate); });			

			Assert::AreEqual(1, (int)gate->GetGateCount());

			Logger::WriteMessage("TestAddGate: Self");
			Assert::ExpectException<std::exception>([&] { gate->AddGate("not2", gate); });
		}
		
		TEST_METHOD(TestComponentToTopLevel)
		{
			Logger::WriteMessage("TestComponentToTopLevel: Outside->Outside");
			CompositeGate * gate = new CompositeGate("test");
			gate->AddOutput("out");
			CompositeGate * gate2 = new CompositeGate("test2");
			gate2->AddInput("in");

			gate->GetPin("out")->ConnectTo(gate2->GetPin("in"));
		}

		TEST_METHOD(TestInnerToComponent)
		{
			CompositeGate * gate = new CompositeGate("test");
			gate->AddInput("in");
			gate->AddOutput("out");
			NOTGate * not1 = new NOTGate();
			NOTGate * not2 = new NOTGate();
			NOTGate * not3 = new NOTGate();
			NOTGate * not4 = new NOTGate();
			NOTGate * not5 = new NOTGate();
			gate->AddGate("not1", not1);
			gate->AddGate("not2", not2);
			gate->AddGate("not3", not3);
			gate->AddGate("not4", not4);
			gate->AddGate("not5", not5);
			Assert::AreEqual(5, (int)gate->GetGateCount());

			Logger::WriteMessage("TestInnerToComponent: Inside->Inside");
			not1->GetPin("out")->ConnectTo(not2->GetPin("in"));
			Assert::AreEqual(IOPin::UNDEF, not2->GetPin("out")->Get());
			not1->GetPin("in")->Set(IOPin::HI);
			Assert::AreEqual(IOPin::HI, not2->GetPin("out")->Get());
			not1->GetPin("in")->Set(IOPin::LOW);
			Assert::AreEqual(IOPin::LOW, not2->GetPin("out")->Get());

			// Outside gate
			GateBase * outsideNot = new NOTGate();

			Logger::WriteMessage("TestInnerToComponent: Inside->Input pin (not allowed)");			
			Assert::ExpectException<std::exception>([&] { not3->GetPin("out")->ConnectTo(gate->GetPin("in")); });

			Logger::WriteMessage("TestInnerToComponent: Inside->Output pin (ok)");
			gate->GetPin("in")->ConnectTo(not4->GetPin("in"));

			Logger::WriteMessage("TestInnerToComponent: Input pin->Inside (ok)");
			not4->GetPin("out")->ConnectTo(gate->GetPin("out"));

			Logger::WriteMessage("TestInnerToComponent: Validate inner gate working");
			gate->GetPin("in")->Set(IOPin::HI);
			Assert::AreEqual(IOPin::LOW, gate->GetPin("out")->Get());
			gate->GetPin("in")->Set(IOPin::LOW);
			Assert::AreEqual(IOPin::HI, gate->GetPin("out")->Get());

			Logger::WriteMessage("TestInnerToComponent: Output pin->inside (not allowed)");
			Assert::ExpectException<std::exception>([&] { gate->GetPin("out")->ConnectTo(not5->GetPin("in")); });

			Logger::WriteMessage("TestInnerToComponent: Inside->Outside gate (not allowed)");
			Assert::ExpectException<std::exception>([&] { not5->GetPin("out")->ConnectTo(outsideNot->GetPin("in")); });

			Logger::WriteMessage("TestInnerToComponent: Outside gate->Inside (not allowed)");
			Assert::ExpectException<std::exception>([&] { outsideNot->GetPin("out")->ConnectTo(not5->GetPin("in")); });


			NOTGate * not6 = new NOTGate();
			Logger::WriteMessage("TestInnerComponentToComponent: Outside gate->Outside pin (ok)");
			not6->GetPin("out")->ConnectTo(gate->GetPin("in"));
			Logger::WriteMessage("TestInnerComponentToComponent: Validate Outside gate->Outside pin");
			not6->GetPin("in")->Set(IOPin::HI);
			Assert::AreEqual(IOPin::HI, gate->GetPin("out")->Get());
			not6->GetPin("in")->Set(IOPin::LOW);
			Assert::AreEqual(IOPin::LOW, gate->GetPin("out")->Get());

			NOTGate * not7 = new NOTGate();
			Logger::WriteMessage("TestInnerComponentToComponent: Outside pin->Outside gate (ok)");
			gate->GetPin("out")->ConnectTo(not7->GetPin("in"));
			Logger::WriteMessage("TestInnerComponentToComponent: Validate Outside pin->Outside gate");
			gate->GetPin("in")->Set(IOPin::HI);
			Assert::AreEqual(IOPin::HI, not7->GetPin("out")->Get());
			gate->GetPin("in")->Set(IOPin::LOW);
			Assert::AreEqual(IOPin::LOW, not7->GetPin("out")->Get());
		}

		TEST_METHOD(TestFullName)
		{
			// TODO 
		}

		void AssertEqualOutputs(GateBase* g1, GateBase* g2)
		{
			Assert::AreEqual(g1->GetPin("Y0")->Get(), g2->GetPin("Y0")->Get());
			Assert::AreEqual(g1->GetPin("Y1")->Get(), g2->GetPin("Y1")->Get());
			Assert::AreEqual(g1->GetPin("Y2")->Get(), g2->GetPin("Y2")->Get());
			Assert::AreEqual(g1->GetPin("Y3")->Get(), g2->GetPin("Y3")->Get());
		}

		TEST_METHOD(TestClone)
		{
			CompositeGate* base = BuildDecoder();
			GateBase* clonedBase = base->Clone("clone");
			CompositeGate* cloned = dynamic_cast<CompositeGate*>(clonedBase);

			Assert::IsNotNull(cloned);
			Assert::AreNotEqual(base, cloned);

			Assert::AreEqual(std::string("clone"), cloned->GetName());
			Assert::AreEqual(base->GetInputCount(), cloned->GetInputCount());
			Assert::AreEqual(base->GetOutputCount(), cloned->GetOutputCount());
			Assert::AreEqual(base->GetGateCount(), cloned->GetGateCount());

			// Compare behavior
			base->GetPin("EN")->Set(IOPin::LOW);
			base->GetPin("I0")->Set(IOPin::LOW);
			base->GetPin("I1")->Set(IOPin::LOW);

			cloned->GetPin("EN")->Set(IOPin::LOW);
			cloned->GetPin("I0")->Set(IOPin::LOW);
			cloned->GetPin("I1")->Set(IOPin::LOW);

			AssertEqualOutputs(base, cloned);

			base->GetPin("EN")->Set(IOPin::HI);
			cloned->GetPin("EN")->Set(IOPin::HI);

			AssertEqualOutputs(base, cloned);

			base->GetPin("I0")->Set(IOPin::HI);
			cloned->GetPin("I0")->Set(IOPin::HI);

			AssertEqualOutputs(base, cloned);
		}	

		TEST_METHOD(TestHiZOutput)
		{
			CompositeGate* buffers = new CompositeGate("buffers");
			buffers->AddGate("b1", new BufferGate());
			buffers->AddGate("b2", new BufferGate());

			buffers->AddInput("en1")->ConnectTo(buffers->GetGate("b1")->GetPin("en"));
			buffers->AddInput("en2")->ConnectTo(buffers->GetGate("b2")->GetPin("en"));
			buffers->AddInput("i1")->ConnectTo(buffers->GetGate("b1")->GetPin("in"));
			buffers->AddInput("i2")->ConnectTo(buffers->GetGate("b2")->GetPin("in"));

			IOPin* out = buffers->AddOutput("out", 1, IOPin::OUTPUT_HI_Z);
			buffers->GetGate("b1")->GetPin("out")->ConnectTo(out);
			buffers->GetGate("b2")->GetPin("out")->ConnectTo(out);

			buffers->GetPin("en1")->Set(IOPin::LOW);
			buffers->GetPin("en2")->Set(IOPin::LOW);
			Assert::AreEqual(IOPin::HI_Z, buffers->GetPin("out")->Get());

			buffers->GetPin("i1")->Set(IOPin::HI);
			buffers->GetPin("en1")->Set(IOPin::HI);
			Assert::AreEqual(IOPin::HI, buffers->GetPin("out")->Get());
			buffers->GetPin("i1")->Set(IOPin::LOW);
			Assert::AreEqual(IOPin::LOW, buffers->GetPin("out")->Get());
			buffers->GetPin("i2")->Set(IOPin::HI);
			Assert::AreEqual(IOPin::LOW, buffers->GetPin("out")->Get());
			buffers->GetPin("en1")->Set(IOPin::LOW);
			Assert::AreEqual(IOPin::HI_Z, buffers->GetPin("out")->Get());
			buffers->GetPin("en2")->Set(IOPin::HI);
			Assert::AreEqual(IOPin::HI, buffers->GetPin("out")->Get());

			// Both output enabled
			buffers->GetPin("en1")->Set(IOPin::HI);
			Assert::AreEqual(IOPin::UNDEF, buffers->GetPin("out")->Get());

		}
	};
}