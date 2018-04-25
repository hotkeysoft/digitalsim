#include "stdafx.h"
#include "CppUnitTest.h"
#include "GateBase.h"
#include "NOTGate.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{		
	TEST_CLASS(TestGateBase)
	{
	public:
		TEST_METHOD(TestAddInput)
		{
			GateBase* gate = new GateBase("test");

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
			GateBase* gate = new GateBase("test");

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
			GateBase* gate = new GateBase("test");
			gate->AddInput("in");
			gate->AddOutput("out");

			GateBase* gate2 = new GateBase("test2");
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
			Assert::Fail(L"test");

			delete gate;
			delete gate2;
		}

		TEST_METHOD(TestAddGate)
		{
			GateBase * gate = new GateBase("test");
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


	};
}