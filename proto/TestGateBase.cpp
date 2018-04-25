#include "stdafx.h"
#include "CppUnitTest.h"
#include "GateBase.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{		
	TEST_CLASS(TestGateBase)
	{
	public:
		TEST_METHOD(TestAddInput)
		{
			GateBase* gate = new GateBase("test");

			// Bad input
			Assert::ExpectException<std::exception>([gate] { gate->AddInput(NULL); });
			Assert::ExpectException<std::exception>([gate] { gate->AddInput(""); });
			Assert::ExpectException<std::exception>([gate] { gate->AddInput(" "); });
			Assert::ExpectException<std::exception>([gate] { gate->AddInput("3"); });
			Assert::ExpectException<std::exception>([gate] { gate->AddInput("3d"); });
			Assert::ExpectException<std::exception>([gate] { gate->AddInput("a b"); });
			Assert::ExpectException<std::exception>([gate] { gate->AddInput("a "); });
			Assert::ExpectException<std::exception>([gate] { gate->AddInput(" a"); });

			//// Good input			
			gate->AddInput("a");
			gate->AddInput("a12345");
			Assert::IsNotNull(gate->GetPin("a"));
			Assert::IsNotNull(gate->GetPin("a12345"));
			
			// Duplicate			
			Assert::ExpectException<std::exception>([gate] { gate->AddInput("a"); });
			Assert::ExpectException<std::exception>([gate] { gate->AddOutput("a"); });

			delete gate;
		}

		TEST_METHOD(TestAddOutput)
		{
			GateBase* gate = new GateBase("test");

			// Bad input
			Assert::ExpectException<std::exception>([gate] { gate->AddOutput(NULL); });
			Assert::ExpectException<std::exception>([gate] { gate->AddOutput(""); });
			Assert::ExpectException<std::exception>([gate] { gate->AddOutput(" "); });
			Assert::ExpectException<std::exception>([gate] { gate->AddOutput("3"); });
			Assert::ExpectException<std::exception>([gate] { gate->AddOutput("3d"); });
			Assert::ExpectException<std::exception>([gate] { gate->AddOutput("a b"); });
			Assert::ExpectException<std::exception>([gate] { gate->AddOutput("a "); });
			Assert::ExpectException<std::exception>([gate] { gate->AddOutput(" a"); });

			//// Good input			
			gate->AddOutput("a");
			gate->AddOutput("a12345");
			Assert::IsNotNull(gate->GetPin("a"));
			Assert::IsNotNull(gate->GetPin("a12345"));

			// Duplicate			
			Assert::ExpectException<std::exception>([gate] { gate->AddOutput("a"); });
			Assert::ExpectException<std::exception>([gate] { gate->AddInput("a"); });

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


			Assert::ExpectException<std::exception>([gate] { gate->GetPin("out")->ConnectTo(gate->GetPin("in")); });
			Assert::ExpectException<std::exception>([gate] { gate->GetPin("in")->ConnectTo(gate->GetPin("in")); });
			Assert::ExpectException<std::exception>([gate] { gate->GetPin("out")->ConnectTo(gate->GetPin("out")); });
			Assert::ExpectException<std::exception>([gate] { gate->GetPin("in")->ConnectTo(gate->GetPin("out")); });

			gate->GetPin("out")->ConnectTo(gate2->GetPin("in"));
			Assert::ExpectException<std::exception>([&] { gate->GetPin("out")->ConnectTo(gate2->GetPin("in")); });

			delete gate;
			delete gate2;
		}

	};
}