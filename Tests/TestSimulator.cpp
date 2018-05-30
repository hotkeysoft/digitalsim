#include "pch.h"
#pragma warning( disable:4251 )

#include "TestCommon.h"
#include "Core\GateBase.h"
#include "Core\CompositeGate.h"
#include "Core\IOConnection.h"
#include "Core\Simulator.h"
#include "BasicGates\NOTGate.h"
#include "BasicGates\ANDGate.h"
#include "BasicGates\NANDGate.h"
#include "BasicGates\ORGate.h"
#include "BasicGates\XORGate.h"
#include "BasicGates\DLatch.h"
#include "BasicGates\SRLatch.h"
#include "BasicGates\WireGate.h"
#include "BasicGates\BufferGate.h"
#include "BasicGates\DFlipFlop.h"
#include "BasicGates\JKFlipFlop.h"
#include "BasicGates\TFlipFlop.h"
#include "Tools\LogicTools.h"

namespace UnitTests
{
	using namespace Core;

	// Not validated for now, just for code coverage
	TEST(TestSimulator, Create)
	{
		EXPECT_THROW(Simulator::Create(NULL), std::invalid_argument);
		EXPECT_THROW(Simulator::Create(""), std::invalid_argument);
		EXPECT_THROW(Simulator::Create(" "), std::invalid_argument);
		EXPECT_THROW(Simulator::Create("3"), std::invalid_argument);
		EXPECT_THROW(Simulator::Create("3d"), std::invalid_argument);
		EXPECT_THROW(Simulator::Create("a b"), std::invalid_argument);
		EXPECT_THROW(Simulator::Create("a "), std::invalid_argument);
		EXPECT_THROW(Simulator::Create(" a"), std::invalid_argument);
		EXPECT_THROW(Simulator::Create("a23456789012345678901234567890123"), std::invalid_argument);
		EXPECT_NE(nullptr, Simulator::Create("a2345678901234567890123456789012"));

		SimulatorPtr sim = Simulator::Create("sim");
		EXPECT_NE(nullptr, sim);
	}

	TEST(TestSimulator, AddGates)
	{
		SimulatorPtr sim = Simulator::Create("sim");
		GatePtr notGate = BasicGates::NOTGate::Create();
		sim->AddGate("not", notGate);
		sim->AddGate("and", BasicGates::ANDGate::Create());
		sim->AddGate("decoder", BuildDecoder());

		EXPECT_EQ(3, sim->GetGateCount());
		EXPECT_THROW(sim->AddGate(nullptr, BasicGates::NOTGate::Create()), std::invalid_argument);
		EXPECT_THROW(sim->AddGate("", BasicGates::NOTGate::Create()), std::invalid_argument);
		EXPECT_THROW(sim->AddGate(" ", BasicGates::NOTGate::Create()), std::invalid_argument);
		EXPECT_THROW(sim->AddGate("", BasicGates::NOTGate::Create()), std::invalid_argument);
		EXPECT_THROW(sim->AddGate("3", BasicGates::NOTGate::Create()), std::invalid_argument);
		EXPECT_THROW(sim->AddGate("3d", BasicGates::NOTGate::Create()), std::invalid_argument);
		EXPECT_THROW(sim->AddGate("a b", BasicGates::NOTGate::Create()), std::invalid_argument);
		EXPECT_THROW(sim->AddGate("a ", BasicGates::NOTGate::Create()), std::invalid_argument);
		EXPECT_THROW(sim->AddGate(" a", BasicGates::NOTGate::Create()), std::invalid_argument);
		EXPECT_THROW(sim->AddGate("a23456789012345678901234567890123", BasicGates::NOTGate::Create()), std::invalid_argument);
		
		sim->AddGate("a2345678901234567890123456789012", BasicGates::NOTGate::Create());
		EXPECT_EQ(4, sim->GetGateCount());

		// Same name
		EXPECT_THROW(sim->AddGate("not", BasicGates::NOTGate::Create()), std::invalid_argument);
		EXPECT_THROW(sim->AddGate("not", notGate), std::invalid_argument);

		// Already added under another name
		EXPECT_THROW(sim->AddGate("not2", notGate), std::invalid_argument);

		EXPECT_EQ(4, sim->GetGateCount());
	}

	TEST(TestSimulator, CheckMode)
	{
		SimulatorPtr sim = Simulator::Create("sim");
		GatePtr decoder = BuildDecoder();

		EXPECT_EQ(Simulator::ASYNC, decoder->GetMode());
		sim->AddGate("decoder", decoder);

		EXPECT_EQ(Simulator::SYNC, decoder->GetMode());

		for (auto subGate : std::dynamic_pointer_cast<CompositeGate>(decoder)->GetInternalGates())
		{
			EXPECT_EQ(Simulator::SYNC, subGate.second->GetMode());
		}

		// Has parent
		EXPECT_THROW(decoder->SetMode(Simulator::ASYNC), std::invalid_argument);
		EXPECT_THROW(decoder->SetMode(Simulator::SYNC), std::invalid_argument);
		EXPECT_THROW(decoder->SetMode(Simulator::SYNC, sim.get()), std::invalid_argument);

		GatePtr notGate = BasicGates::NOTGate::Create();
		notGate->SetMode(Simulator::ASYNC); // No-op
		EXPECT_THROW(notGate->SetMode(Simulator::SYNC) , std::invalid_argument);
		EXPECT_THROW(notGate->SetMode(Simulator::SYNC, nullptr), std::invalid_argument);

		EXPECT_THROW(notGate->SetMode(Simulator::ASYNC, sim.get()), std::invalid_argument);

		// Don't support outside gates from now
		EXPECT_THROW(notGate->SetMode(Simulator::SYNC, sim.get()), std::invalid_argument);

	}

	TEST(TestSimulator, Queue)
	{
		SimulatorPtr sim = Simulator::Create("sim");
		sim->AddGate("not1", BasicGates::NOTGate::Create());
		sim->AddGate("not2", BasicGates::NOTGate::Create());
		sim->AddGate("not3", BasicGates::NOTGate::Create());
		sim->AddGate("not4", BasicGates::NOTGate::Create());
		sim->AddGate("not5", BasicGates::NOTGate::Create());

		sim->GetGate("not1")->GetPin("out")->ConnectTo(sim->GetGate("not2")->GetPin("in"));
		sim->GetGate("not2")->GetPin("out")->ConnectTo(sim->GetGate("not3")->GetPin("in"));
		sim->GetGate("not3")->GetPin("out")->ConnectTo(sim->GetGate("not4")->GetPin("in"));
		sim->GetGate("not4")->GetPin("out")->ConnectTo(sim->GetGate("not5")->GetPin("in"));

		EXPECT_EQ(0, sim->GetEventQueue().size());

		sim->PostEvent({ IOState::LOW, sim->GetGate("not1")->GetPin("in") });
		sim->PostEvent({ IOState::HI, sim->GetGate("not1")->GetPin("in") }, 4);
		sim->PostEvent({ IOState::LOW, sim->GetGate("not1")->GetPin("in") }, 8);

		EXPECT_EQ(3, sim->GetEventQueue().size());

		std::cout << Tools::LogicTools::LogicAnalyser(sim, { sim->GetGate("not1")->GetPin("in"), sim->GetGate("not1")->GetPin("out"),
			sim->GetGate("not2")->GetPin("out"), sim->GetGate("not3")->GetPin("out"),
			sim->GetGate("not4")->GetPin("out"), sim->GetGate("not5")->GetPin("out") });

		EXPECT_EQ(0, sim->GetEventQueue().size());
	}
	
	TEST(TestSimulator, Reset)
	{
		SimulatorPtr sim = Simulator::Create("sim");
		sim->AddGate("not1", BasicGates::NOTGate::Create());
		sim->PostEvent({ IOState::HI, sim->GetGate("not1")->GetPin("in") });
		EXPECT_EQ(2, sim->Run(10));
		EXPECT_EQ(2, sim->GetTick());
		EXPECT_EQ(IOState::LOW, sim->GetGate("not1")->GetPin("out")->Get());
		EXPECT_EQ(0, sim->GetEventQueue().size());
		sim->PostEvent({ IOState::HI, sim->GetGate("not1")->GetPin("in") }, 10);
		EXPECT_EQ(1, sim->GetEventQueue().size());
			
		sim->Reset();
		EXPECT_EQ(0, sim->GetTick());
		EXPECT_EQ(0, sim->GetEventQueue().size());
		sim->PostEvent({ IOState::LOW, sim->GetGate("not1")->GetPin("in") });
		EXPECT_EQ(2, sim->Run(10));
		EXPECT_EQ(2, sim->GetTick());
		EXPECT_EQ(IOState::HI, sim->GetGate("not1")->GetPin("out")->Get());
		EXPECT_EQ(0, sim->GetEventQueue().size());
	}

	TEST(TestSimulator, Tick)
	{
		SimulatorPtr sim = Simulator::Create("sim");
		sim->AddGate("not1", BasicGates::NOTGate::Create());
		sim->PostEvent({ IOState::HI, sim->GetGate("not1")->GetPin("in") });
		EXPECT_NE(IOState::HI, sim->GetGate("not1")->GetPin("in")->Get());
		EXPECT_EQ(1, sim->Tick());
		EXPECT_EQ(1, sim->GetTick());
		EXPECT_EQ(IOState::HI, sim->GetGate("not1")->GetPin("in")->Get());
	}

	TEST(TestSimulator, Run)
	{
		SimulatorPtr sim = Simulator::Create("sim");
		sim->AddGate("not1", BasicGates::NOTGate::Create());
		sim->PostEvent({ IOState::HI, sim->GetGate("not1")->GetPin("in") }, 12);

		EXPECT_THROW(sim->Run(0), std::out_of_range);
		EXPECT_EQ(10, sim->Run(10));
		EXPECT_EQ(13, sim->Run(10));
		EXPECT_EQ(13, sim->GetTick());
	}

	TEST(TestSimulator, Pulse)
	{
		SimulatorPtr sim = Simulator::Create("sim");
		sim->AddGate("not1", BasicGates::NOTGate::Create());

		EXPECT_THROW(sim->Pulse(sim->GetGate("not1")->GetPin("in"), 2, 2), std::out_of_range);

		EXPECT_THROW(sim->Pulse(sim->GetGate("not1")->GetPin("in"), 3, 3), std::out_of_range);
		EXPECT_THROW(sim->Pulse(sim->GetGate("not1")->GetPin("in"), 0, 0), std::out_of_range);
		EXPECT_THROW(sim->Pulse(sim->GetGate("not1")->GetPin("in"), 1, 0), std::out_of_range);

		sim->Pulse(sim->GetGate("not1")->GetPin("in"), 2, 3);
		EXPECT_EQ(4, sim->Run(10));
		EXPECT_THROW(sim->Pulse(sim->GetGate("not1")->GetPin("in"), 3, 4), std::out_of_range);
		EXPECT_THROW(sim->Pulse(sim->GetGate("not1")->GetPin("in"), 4, 5), std::out_of_range);

		sim->Pulse(sim->GetGate("not1")->GetPin("in"), 5, 6);
		EXPECT_EQ(7, sim->Run(10));
	}

	TEST(TestSimulator, Clock)
	{
		SimulatorPtr sim = Simulator::Create("sim");
		sim->AddGate("not1", BasicGates::NOTGate::Create());

		EXPECT_THROW(sim->Clock(sim->GetGate("not1")->GetPin("in"), 0, 0, 0, 0), std::out_of_range);
		EXPECT_THROW(sim->Clock(sim->GetGate("not1")->GetPin("in"), 1, 0, 0, 0), std::out_of_range);
		EXPECT_THROW(sim->Clock(sim->GetGate("not1")->GetPin("in"), 0, 1, 0, 0), std::out_of_range);
		EXPECT_THROW(sim->Clock(sim->GetGate("not1")->GetPin("in"), 0, 0, 1, 0), std::out_of_range);
		EXPECT_THROW(sim->Clock(sim->GetGate("not1")->GetPin("in"), 0, 0, 0, 1), std::out_of_range);
		EXPECT_THROW(sim->Clock(sim->GetGate("not1")->GetPin("in"), 1, 1, 0, 1), std::out_of_range);
		EXPECT_THROW(sim->Clock(sim->GetGate("not1")->GetPin("in"), 1, 1, 1, 1), std::out_of_range);
		
		sim->Clock(sim->GetGate("not1")->GetPin("in"), 1, 1, 1, 2);

		EXPECT_EQ(3, sim->Run(10));
		EXPECT_THROW(sim->Clock(sim->GetGate("not1")->GetPin("in"), 1, 1, 2, 3), std::out_of_range);
		EXPECT_THROW(sim->Clock(sim->GetGate("not1")->GetPin("in"), 1, 1, 3, 4), std::out_of_range);

		sim->Clock(sim->GetGate("not1")->GetPin("in"), 1, 1, 5, 10);
		EXPECT_EQ(11, sim->Run(20));
	}

	TEST(TestSimulator, Clone)
	{
		SimulatorPtr sim = Simulator::Create("sim");
		sim->AddInput("x", 4);
		sim->AddInput("y", 4);
		sim->AddInput("cin");
		sim->AddOutput("s", 4);
		sim->AddOutput("cout");
		sim->AddGate("add4", Build4BitAdder());
		sim->AddGate("dlatch", BasicGates::DLatch::Create());
		sim->AddGate("dflip", BasicGates::DFlipFlop::Create());
		sim->AddGate("srlatch", BasicGates::SRLatch::Create());
		sim->AddGate("tflip", BasicGates::TFlipFlop::Create());
		sim->AddGate("jkflip", BasicGates::JKFlipFlop::Create());
		
		sim->GetPin("x")->ConnectTo(sim->GetGate("add4")->GetPin("x"));
		sim->GetPin("y")->ConnectTo(sim->GetGate("add4")->GetPin("y"));
		sim->GetPin("cin")->ConnectTo(sim->GetGate("add4")->GetPin("cin"));
		sim->GetGate("add4")->GetPin("cout")->ConnectTo(sim->GetPin("cout"));
		sim->GetGate("add4")->GetPin("s")->ConnectTo(sim->GetPin("s"));

		sim->PostEvent({ IOState::LOW, sim->GetPin("cin") });
		sim->PostEvent({ IOState::FromInt(5, 4), sim->GetPin("x") });
		sim->PostEvent({ IOState::FromInt(7, 4), sim->GetPin("y") });

		EXPECT_LT(sim->Run(50), 40);
		EXPECT_EQ(IOState::FromInt(12, 4), sim->GetPin("s")->Get());
		EXPECT_EQ(IOState::LOW, sim->GetPin("cout")->Get());

		size_t tickCount1 = sim->GetTick();
		long useCount = sim.use_count();

		SimulatorPtr clone = std::dynamic_pointer_cast<Simulator>(sim->Clone("clone"));
		
		EXPECT_EQ(useCount, sim.use_count());
		EXPECT_NE(nullptr, clone);
		
		EXPECT_EQ(0, clone->GetTick());
		EXPECT_STREQ("clone", clone->GetName().c_str());
		EXPECT_STREQ("clone", clone->GetFullName().c_str());
		EXPECT_EQ(sim->GetGateCount(), clone->GetGateCount());
		EXPECT_EQ(sim->GetInputCount(), clone->GetInputCount());
		EXPECT_EQ(sim->GetOutputCount(), clone->GetOutputCount());
		EXPECT_EQ(sim->GetGateCount(), clone->GetGateCount());

		EXPECT_STREQ(sim->GetGate("add4")->GetName().c_str(), clone->GetGate("add4")->GetName().c_str());
		EXPECT_STREQ("clone.add4", clone->GetGate("add4")->GetFullName().c_str());
		
		// Try it...
		clone->PostEvent({ IOState::LOW, clone->GetPin("cin") });
		clone->PostEvent({ IOState::FromInt(5, 4), clone->GetPin("x") });
		clone->PostEvent({ IOState::FromInt(7, 4), clone->GetPin("y") });

		EXPECT_LT(clone->Run(50), 40);
		EXPECT_EQ(IOState::FromInt(12, 4), clone->GetPin("s")->Get());
		EXPECT_EQ(IOState::LOW, clone->GetPin("cout")->Get());

		EXPECT_EQ(tickCount1, clone->GetTick());
	}

	//////Gate tests
	TEST(TestSimulator, SRLatch)
	{
		SimulatorPtr sim = Simulator::Create("sim");

		sim->AddGate("srlatch", BasicGates::SRLatch::Create());

		// Multiple tests because initial state is random
		for (int i = 0; i < 10; ++i)
		{
			// When powered up in this state in can oscillate until we assert R or S
			sim->PostEvent({ IOState::LOW, sim->GetGate("srlatch")->GetPin("s") });
			sim->PostEvent({ IOState::LOW, sim->GetGate("srlatch")->GetPin("r") });

			sim->Pulse(sim->GetGate("srlatch")->GetPin("r"), 20, 25);
			EXPECT_LT(sim->Run(50), 30);
			EXPECT_EQ(IOState::LOW, sim->GetGate("srlatch")->GetPin("q")->Get());
			EXPECT_EQ(IOState::HI, sim->GetGate("srlatch")->GetPin("/q")->Get());

			sim->Pulse(sim->GetGate("srlatch")->GetPin("s"), 30, 35);
			EXPECT_LT(sim->Run(50), 40);
			EXPECT_EQ(IOState::HI, sim->GetGate("srlatch")->GetPin("q")->Get());
			EXPECT_EQ(IOState::LOW, sim->GetGate("srlatch")->GetPin("/q")->Get());

			sim->Pulse(sim->GetGate("srlatch")->GetPin("r"), 40, 45);
			EXPECT_LT(sim->Run(50), 50);
			EXPECT_EQ(IOState::LOW, sim->GetGate("srlatch")->GetPin("q")->Get());
			EXPECT_EQ(IOState::HI, sim->GetGate("srlatch")->GetPin("/q")->Get());

			sim->Reset();
		}
	}

	TEST(TestSimulator, DLatch)
	{
		SimulatorPtr sim = Simulator::Create("sim");
		CompositeGatePtr dlatch = std::dynamic_pointer_cast<CompositeGate>(BasicGates::DLatch::Create());
		sim->AddGate("dlatch", dlatch);

		sim->PostEvent({ IOState::LOW, sim->GetGate("dlatch")->GetPin("d") });
		sim->PostEvent({ IOState::LOW, sim->GetGate("dlatch")->GetPin("c") });

		// p457 fig.7-13
		sim->Pulse(sim->GetGate("dlatch")->GetPin("d"), 2 * 6, 2 * 14);
		sim->Pulse(sim->GetGate("dlatch")->GetPin("d"), 2 * 25, 2 * 34);
		sim->Pulse(sim->GetGate("dlatch")->GetPin("d"), 2 * 25, 2 * 34);
		sim->Pulse(sim->GetGate("dlatch")->GetPin("d"), 2 * 52, 2 * 59);
		sim->Pulse(sim->GetGate("dlatch")->GetPin("d"), 2 * 63, 2 * 78);
		sim->Pulse(sim->GetGate("dlatch")->GetPin("d"), 2 * 81, 2 * 101);
		sim->Pulse(sim->GetGate("dlatch")->GetPin("d"), 2 * 115, 2 * 130);

		sim->Pulse(sim->GetGate("dlatch")->GetPin("c"), 2 * 16, 2 * 42);
		sim->Pulse(sim->GetGate("dlatch")->GetPin("c"), 2 * 69, 2 * 92);
		sim->Pulse(sim->GetGate("dlatch")->GetPin("c"), 2 * 118, 2 * 144);

		//std::cout << Tools::LogicTools::LogicAnalyser(sim, {
		//	sim->GetGate("dlatch")->GetPin("d"),
		//	sim->GetGate("dlatch")->GetPin("c"),
		//	nullptr,
		//	sim->GetGate("dlatch")->GetPin("q") }, 300, 2);

		ASSERT_LT(sim->Run(500), 300);
	}

	TEST(TestSimulator, DLatch2)
	{
		// Multiple tests because initial state is random
		for (int i = 0; i < 10; ++i)
		{
			SimulatorPtr sim = Simulator::Create("sim");

			sim->AddGate("dlatch", BasicGates::DLatch::Create());				
			sim->PostEvent({ IOState::LOW, sim->GetGate("dlatch")->GetPin("c") });
			sim->PostEvent({ IOState::LOW, sim->GetGate("dlatch")->GetPin("d") });

			// Reset
			sim->Pulse(sim->GetGate("dlatch")->GetPin("c"), 20, 30);
			EXPECT_LT(sim->Run(50), 40);
			EXPECT_EQ(IOState::LOW, sim->GetGate("dlatch")->GetPin("q")->Get());
			EXPECT_EQ(IOState::HI, sim->GetGate("dlatch")->GetPin("/q")->Get());

			sim->Pulse(sim->GetGate("dlatch")->GetPin("d"), 40, 60);
			sim->Pulse(sim->GetGate("dlatch")->GetPin("c"), 45, 55);
			EXPECT_LT(sim->Run(40), 70);
			EXPECT_EQ(IOState::HI, sim->GetGate("dlatch")->GetPin("q")->Get());
			EXPECT_EQ(IOState::LOW, sim->GetGate("dlatch")->GetPin("/q")->Get());

			sim->Pulse(sim->GetGate("dlatch")->GetPin("d"), 70, 80);
			sim->Pulse(sim->GetGate("dlatch")->GetPin("c"), 75, 90);
			EXPECT_LT(sim->Run(40), 100);
			EXPECT_EQ(IOState::LOW, sim->GetGate("dlatch")->GetPin("q")->Get());
			EXPECT_EQ(IOState::HI, sim->GetGate("dlatch")->GetPin("/q")->Get());

			sim->Pulse(sim->GetGate("dlatch")->GetPin("d"), 100, 120);
			EXPECT_LT(sim->Run(40), 130);
			EXPECT_EQ(IOState::LOW, sim->GetGate("dlatch")->GetPin("q")->Get());
			EXPECT_EQ(IOState::HI, sim->GetGate("dlatch")->GetPin("/q")->Get());
		}
	}

	TEST(TestSimulator, DFlipFLop)
	{
		SimulatorPtr sim = Simulator::Create("sim");
		CompositeGatePtr dflip = std::dynamic_pointer_cast<CompositeGate>(BasicGates::DFlipFlop::Create());
		sim->AddGate("dflip", dflip);

		sim->PostEvent({ IOState::LOW, sim->GetGate("dflip")->GetPin("d") });
		sim->PostEvent({ IOState::LOW, sim->GetGate("dflip")->GetPin("clk") });

		// p459 fig.7-13
		sim->Pulse(sim->GetGate("dflip")->GetPin("d"), 2 * 5, 2 * 17);
		sim->Pulse(sim->GetGate("dflip")->GetPin("d"), 2 * 33, 2 * 39);
		sim->Pulse(sim->GetGate("dflip")->GetPin("d"), 2 * 50, 2 * 67);
		sim->Pulse(sim->GetGate("dflip")->GetPin("d"), 2 * 106, 2 * 140);

		sim->Clock(sim->GetGate("dflip")->GetPin("clk"), 2 * 17, 2 * 17, 2 * 11, 2 * 150);

		//std::cout << Tools::LogicTools::LogicAnalyser(sim, {
		//	sim->GetGate("dflip")->GetPin("d"),
		//	sim->GetGate("dflip")->GetPin("clk"),
		//	nullptr,
		//	sim->GetGate("dflip")->GetPin("q") }, 340, 2);

		ASSERT_LT(sim->Run(500), 340);
	}

	TEST(TestSimulator, DFlipFlopOscillation)
	{
		for (int i = 0; i < 10; ++i)
		{
			SimulatorPtr sim = Simulator::Create("sim");
			CompositeGatePtr dflip = std::dynamic_pointer_cast<CompositeGate>(BasicGates::DFlipFlop::Create());
			sim->AddGate("dflip", dflip);

			sim->PostEvent({ IOState::LOW, sim->GetGate("dflip")->GetPin("d") });
			sim->PostEvent({ IOState::LOW, sim->GetGate("dflip")->GetPin("clk") });

			sim->Pulse(sim->GetGate("dflip")->GetPin("d"), 10, 15);

			//std::cout << Tools::LogicTools::LogicAnalyser(sim, {
			//	sim->GetGate("dflip")->GetPin("d"),
			//	sim->GetGate("dflip")->GetPin("clk"),
			//	nullptr,
			//	sim->GetGate("dflip")->GetPin("q") }, 50);

			EXPECT_LT(sim->Run(100), 40);
		}
	}

	TEST(TestSimulator, JKFlipFLop)
	{
		SimulatorPtr sim = Simulator::Create("sim");
		CompositeGatePtr jk = std::dynamic_pointer_cast<CompositeGate>(BasicGates::JKFlipFlop::Create());
		sim->AddGate("jk", jk);

		sim->PostEvent({ IOState::LOW, sim->FindPin("jk.j") });
		sim->PostEvent({ IOState::HI, sim->FindPin("jk.k") });
		sim->PostEvent({ IOState::LOW, sim->FindPin("jk.clk") });

		// p465 fig.7-26, modified to init state
		const size_t mul = 8;
		sim->Pulse(sim->FindPin("jk.j"), mul * 4, mul * 10);
		sim->Pulse(sim->FindPin("jk.j"), mul * 25, mul * 73);
		sim->Pulse(sim->FindPin("jk.j"), mul * 84, mul * 90);
		sim->Pulse(sim->FindPin("jk.j"), mul * 112, mul * 120);

		sim->Pulse(sim->FindPin("jk.k"), mul * 54, mul * 85);
		sim->Pulse(sim->FindPin("jk.k"), mul * 112, mul * 120);
		sim->PostEvent({ IOState::HI, sim->FindPin("jk.k") }, mul * 130);

		sim->Clock(sim->FindPin("jk.clk"), mul * 17, mul * 17, mul * 15, mul * 150);

//		std::cout << Tools::LogicTools::LogicAnalyser(sim, {
//			sim->FindPin("jk.j"),
//			sim->FindPin("jk.k"),
//			sim->FindPin("jk.clk"),
//			nullptr,
//			sim->FindPin("jk.q") }, 150 * mul, 32);

		ASSERT_LT(sim->Run(1500), 1100);
	}


	TEST(TestSimulator, TFlipFlop)
	{
		for (int i = 0; i < 10; ++i)
		{
			SimulatorPtr sim = Simulator::Create("sim");
			sim->AddGate("tflip", BasicGates::TFlipFlop::Create());

			sim->PostEvent({ IOState::LOW, sim->GetGate("tflip")->GetPin("t") });

			sim->Pulse(sim->GetGate("tflip")->GetPin("t"), 20, 40);
			EXPECT_LT(sim->Run(50), 60);
			EXPECT_EQ(IOState::LOW, sim->GetGate("tflip")->GetPin("q")->Get());

			sim->Pulse(sim->GetGate("tflip")->GetPin("t"), 60, 80);
			EXPECT_LT(sim->Run(50), 100);
			EXPECT_EQ(IOState::HI, sim->GetGate("tflip")->GetPin("q")->Get());

			sim->Pulse(sim->GetGate("tflip")->GetPin("t"), 100, 120);
			EXPECT_LT(sim->Run(50), 140);
			EXPECT_EQ(IOState::LOW, sim->GetGate("tflip")->GetPin("q")->Get());

			sim->Pulse(sim->GetGate("tflip")->GetPin("t"), 140, 160);
			EXPECT_LT(sim->Run(50), 180);
			EXPECT_EQ(IOState::HI, sim->GetGate("tflip")->GetPin("q")->Get());
		}
	}

	TEST(TestSimulator, FullAdder)
	{
		SimulatorPtr sim = Simulator::Create("sim");
		sim->AddGate("add", BuildFullAdder());

		sim->PostEvent({ IOState::LOW, sim->GetGate("add")->GetPin("cin") });
		sim->PostEvent({ IOState::LOW, sim->GetGate("add")->GetPin("x") });
		sim->PostEvent({ IOState::LOW, sim->GetGate("add")->GetPin("y") });

		EXPECT_LT(sim->Run(20), 10);
		EXPECT_EQ(IOState::LOW, sim->GetGate("add")->GetPin("s")->Get());
		EXPECT_EQ(IOState::LOW, sim->GetGate("add")->GetPin("cout")->Get());

		sim->PostEvent({ IOState::LOW, sim->GetGate("add")->GetPin("cin") }, 10);
		sim->PostEvent({ IOState::HI, sim->GetGate("add")->GetPin("x") }, 10);
		sim->PostEvent({ IOState::LOW, sim->GetGate("add")->GetPin("y")}, 10);

		EXPECT_LT(sim->Run(20), 20);
		EXPECT_EQ(IOState::HI, sim->GetGate("add")->GetPin("s")->Get());
		EXPECT_EQ(IOState::LOW, sim->GetGate("add")->GetPin("cout")->Get());

		sim->PostEvent({ IOState::LOW, sim->GetGate("add")->GetPin("cin") }, 20);
		sim->PostEvent({ IOState::LOW, sim->GetGate("add")->GetPin("x") }, 20);
		sim->PostEvent({ IOState::HI, sim->GetGate("add")->GetPin("y") }, 20);

		EXPECT_LT(sim->Run(20), 30);
		EXPECT_EQ(IOState::HI, sim->GetGate("add")->GetPin("s")->Get());
		EXPECT_EQ(IOState::LOW, sim->GetGate("add")->GetPin("cout")->Get());

		sim->PostEvent({ IOState::LOW, sim->GetGate("add")->GetPin("cin") }, 30);
		sim->PostEvent({ IOState::HI, sim->GetGate("add")->GetPin("x") }, 30);
		sim->PostEvent({ IOState::HI, sim->GetGate("add")->GetPin("y") }, 30);

		EXPECT_LT(sim->Run(20), 40);
		EXPECT_EQ(IOState::LOW, sim->GetGate("add")->GetPin("s")->Get());
		EXPECT_EQ(IOState::HI, sim->GetGate("add")->GetPin("cout")->Get());

		sim->PostEvent({ IOState::HI, sim->GetGate("add")->GetPin("cin") }, 40);
		sim->PostEvent({ IOState::LOW, sim->GetGate("add")->GetPin("x") }, 40);
		sim->PostEvent({ IOState::LOW, sim->GetGate("add")->GetPin("y") }, 40);

		EXPECT_LT(sim->Run(20), 50);
		EXPECT_EQ(IOState::HI, sim->GetGate("add")->GetPin("s")->Get());
		EXPECT_EQ(IOState::LOW, sim->GetGate("add")->GetPin("cout")->Get());

		sim->PostEvent({ IOState::HI, sim->GetGate("add")->GetPin("cin") }, 50);
		sim->PostEvent({ IOState::HI, sim->GetGate("add")->GetPin("x") }, 50);
		sim->PostEvent({ IOState::LOW, sim->GetGate("add")->GetPin("y") }, 50);

		EXPECT_LT(sim->Run(20), 60);
		EXPECT_EQ(IOState::LOW, sim->GetGate("add")->GetPin("s")->Get());
		EXPECT_EQ(IOState::HI, sim->GetGate("add")->GetPin("cout")->Get());

		sim->PostEvent({ IOState::HI, sim->GetGate("add")->GetPin("cin") }, 60);
		sim->PostEvent({ IOState::LOW, sim->GetGate("add")->GetPin("x") }, 60);
		sim->PostEvent({ IOState::HI, sim->GetGate("add")->GetPin("y") }, 60);

		EXPECT_LT(sim->Run(20), 70);
		EXPECT_EQ(IOState::LOW, sim->GetGate("add")->GetPin("s")->Get());
		EXPECT_EQ(IOState::HI, sim->GetGate("add")->GetPin("cout")->Get());

		sim->PostEvent({ IOState::HI, sim->GetGate("add")->GetPin("cin") }, 70);
		sim->PostEvent({ IOState::HI, sim->GetGate("add")->GetPin("x") }, 70);
		sim->PostEvent({ IOState::HI, sim->GetGate("add")->GetPin("y") }, 70);

		EXPECT_LT(sim->Run(20), 80);
		EXPECT_EQ(IOState::HI, sim->GetGate("add")->GetPin("s")->Get());
		EXPECT_EQ(IOState::HI, sim->GetGate("add")->GetPin("cout")->Get());
	}
	
	TEST(TestSimulator, Adder4Bit)
	{
		SimulatorPtr sim = Simulator::Create("sim");
		sim->AddGate("add4", Build4BitAdder());

		sim->PostEvent({ IOState::LOW, sim->GetGate("add4")->GetPin("cin") });
		sim->PostEvent({ IOState::FromInt(5, 4), sim->GetGate("add4")->GetPin("x") });
		sim->PostEvent({ IOState::FromInt(7, 4), sim->GetGate("add4")->GetPin("y") });

		EXPECT_LT(sim->Run(50), 40);
		EXPECT_EQ(IOState::FromInt(12, 4), sim->GetGate("add4")->GetPin("s")->Get());
		EXPECT_EQ(IOState::LOW, sim->GetGate("add4")->GetPin("cout")->Get());

		sim->PostEvent({ IOState::HI, sim->GetGate("add4")->GetPin("cin") }, 40);
		sim->PostEvent({ IOState::FromInt(3, 4), sim->GetGate("add4")->GetPin("x") }, 40);
		sim->PostEvent({ IOState::FromInt(3, 4), sim->GetGate("add4")->GetPin("y") }, 40);

		EXPECT_LT(sim->Run(50), 80);
		EXPECT_EQ(IOState::FromInt(3+3+1, 4), sim->GetGate("add4")->GetPin("s")->Get());
		EXPECT_EQ(IOState::LOW, sim->GetGate("add4")->GetPin("cout")->Get());

		sim->PostEvent({ IOState::HI, sim->GetGate("add4")->GetPin("cin") }, 80);
		sim->PostEvent({ IOState::FromInt(15, 4), sim->GetGate("add4")->GetPin("x") }, 80);
		sim->PostEvent({ IOState::FromInt(15, 4), sim->GetGate("add4")->GetPin("y") }, 80);

		EXPECT_LT(sim->Run(50), 120);
		EXPECT_EQ(IOState::FromInt(15, 4), sim->GetGate("add4")->GetPin("s")->Get());
		EXPECT_EQ(IOState::HI, sim->GetGate("add4")->GetPin("cout")->Get());
	}

	TEST(TestSimulator, GateDelay)
	{
		// Not much used at the moment, to be developped... (only NAND Gate and wire have it for now)
		SimulatorPtr sim = Simulator::Create("sim");

		sim->AddInput("in1");
		sim->AddInput("in2");
		sim->AddOutput("out1");
		sim->AddOutput("out2");
		sim->AddOutput("out3");

		GatePtr nand1 = BasicGates::NANDGate::Create();
		GatePtr nand2 = BasicGates::NANDGate::Create(2, 2);
		GatePtr nand3 = BasicGates::NANDGate::Create(2, 5);

		GatePtr wire = BasicGates::WireGate::Create(10);
		EXPECT_EQ(10, wire->GetDelay());

		sim->AddGate("nand1", nand1);
		sim->AddGate("nand2", nand2);
		sim->AddGate("nand3", nand3);
		sim->AddGate("wire", wire);

		sim->GetPin("in1")->ConnectTo(wire->GetPin("in"));
		wire->GetPin("out")->ConnectTo(nand1->GetPin("in1"));

		sim->GetPin("in1")->ConnectTo(nand2->GetPin("in1"));
		sim->GetPin("in1")->ConnectTo(nand3->GetPin("in1"));

		sim->GetPin("in2")->ConnectTo(nand1->GetPin("in2"));
		sim->GetPin("in2")->ConnectTo(nand2->GetPin("in2"));
		sim->GetPin("in2")->ConnectTo(nand3->GetPin("in2"));

		nand1->GetPin("out")->ConnectTo(sim->GetPin("out1"));
		nand2->GetPin("out")->ConnectTo(sim->GetPin("out2"));
		nand3->GetPin("out")->ConnectTo(sim->GetPin("out3"));
		
		sim->PostEvent({ IOState::LOW, sim->GetPin("in1") });
		sim->PostEvent({ IOState::HI, sim->GetPin("in2") });

		sim->PostEvent({ IOState::HI, sim->GetPin("in1") }, 30);


		std::cout << Tools::LogicTools::LogicAnalyser(sim, {
			sim->GetPin("in1"),
			sim->GetPin("in2"),		
			nullptr,
			wire->GetPin("out"),
			sim->GetPin("out1"),
			sim->GetPin("out2"),
			sim->GetPin("out3") });

		EXPECT_GT(sim->GetTick(), 40);
		EXPECT_LT(sim->GetTick(), 50);

	}

	TEST(TestSimulator, Counter1)
	{
		SimulatorPtr sim = Simulator::Create("sim");

		GatePtr counter = Build74163Counter();
		sim->AddGate("cnt", counter);
		
		sim->PostEvent({ IOState::LOW, counter->GetPin("clk") });
		sim->PostEvent({ IOState::HI, counter->GetPin("/ld") });
		sim->PostEvent({ IOState::HI, counter->GetPin("enp") });
		sim->PostEvent({ IOState::HI, counter->GetPin("ent") });
		
		sim->Pulse(counter->GetPin("/clr"), 1, 30, true);

		sim->Pulse(counter->GetPin("clk"), 20, 30);
		EXPECT_LT(sim->Run(200), 60);
		EXPECT_EQ(IOState::FromInt(0, 4), counter->GetPin("q")->Get());
		EXPECT_EQ(IOState::LOW, counter->GetPin("rco")->Get());
		
		for (int i = 1; i < 16; ++i)
		{
			sim->Pulse(counter->GetPin("clk"), 60*i, 60*i+10);
			EXPECT_LT(sim->Run(200), 60*(i+1));
			EXPECT_EQ(IOState::FromInt(i, 4), counter->GetPin("q")->Get());
			EXPECT_EQ((i == 15)?IOState::HI:IOState::LOW, counter->GetPin("rco")->Get());
		}
	}

	// Counts from 3 to 12
	TEST(TestSimulator, Counter2)
	{
		// page 601 fig.9.21-9.22
		SimulatorPtr sim = Simulator::Create("sim");

		GatePtr counter = Build74163Counter();
		sim->AddGate("cnt", counter);

		sim->AddGate("nand", BasicGates::NANDGate::Create());
		counter->GetPin("q", 2)->ConnectTo(sim->GetGate("nand")->GetPin("in1"));
		counter->GetPin("q", 3)->ConnectTo(sim->GetGate("nand")->GetPin("in2"));
		sim->GetGate("nand")->GetPin("out")->ConnectTo(counter->GetPin("/ld"));

		sim->Pulse(counter->GetPin("/clr"), 1, 200, true);
		sim->PostEvent({ IOState::HI, counter->GetPin("/clr") });
		sim->PostEvent({ IOState::HI, counter->GetPin("data",0) });
		sim->PostEvent({ IOState::HI, counter->GetPin("data",1) });
		sim->PostEvent({ IOState::LOW, counter->GetPin("data",2) });
		sim->PostEvent({ IOState::LOW, counter->GetPin("data",3) });
		sim->PostEvent({ IOState::HI, counter->GetPin("enp") });
		sim->PostEvent({ IOState::HI, counter->GetPin("ent") });
		sim->PostEvent({ IOState::HI, counter->GetPin("/ld") });
		
//		sim->Pulse(counter->GetPin("/ld"), 10, 200, true);

		size_t mul = 20;
		sim->Clock(counter->GetPin("clk"), mul * 4, mul * 4, 1, mul * 4 * 36);

		std::cout << Tools::LogicTools::LogicAnalyser(sim, {
			counter->GetPin("/clr"),
			counter->GetPin("/ld"),
			counter->GetPin("clk"),
			nullptr,
			counter->GetPin("q") }, 36 * 4 * mul, mul * 4);
	}

	TEST(TestSimulator, Counter3)
	{
		// page 634 fig.9.60
		SimulatorPtr sim = Simulator::Create("sim");

		GatePtr counter = Build74163Counter();
		sim->AddGate("cnt", counter);

		sim->AddGate("nand", BasicGates::NANDGate::Create());		
		counter->GetPin("q", 2)->ConnectTo(sim->GetGate("nand")->GetPin("in1"), true);
		counter->GetPin("q", 3)->ConnectTo(sim->GetGate("nand")->GetPin("in2"), true);
		sim->GetGate("nand")->GetPin("out")->ConnectTo(counter->GetPin("/ld"));

		sim->AddGate("not", BasicGates::NOTGate::Create());
		counter->GetPin("q", 1)->ConnectTo(sim->GetGate("not")->GetPin("in"));
		sim->GetGate("not")->GetPin("out")->ConnectTo(counter->GetPin("/clr"));

		sim->AddGate("or1", BasicGates::ORGate::Create());
		sim->AddGate("or2", BasicGates::ORGate::Create());

		counter->GetPin("q")->ConnectTo(sim->AddOutput("q", 4));
		sim->AddInput("clk")->ConnectTo(counter->GetPin("clk"));
		sim->AddInput("haz");
		sim->AddInput("left");
		sim->AddInput("right");
		
		sim->GetPin("haz")->ConnectTo(counter->GetPin("data", 0));
		sim->GetPin("haz")->ConnectTo(counter->GetPin("data", 1));
		sim->GetPin("haz")->ConnectTo(sim->GetGate("or1")->GetPin("in1"));
		sim->GetPin("haz")->ConnectTo(sim->GetGate("or2")->GetPin("in1"));
		sim->GetPin("left")->ConnectTo(sim->GetGate("or1")->GetPin("in2"));
		sim->GetPin("right")->ConnectTo(sim->GetGate("or2")->GetPin("in2"));
		sim->GetGate("or1")->GetPin("out")->ConnectTo(counter->GetPin("data", 2));
		sim->GetGate("or2")->GetPin("out")->ConnectTo(counter->GetPin("data", 3));

		sim->PostEvent({ IOState::HI, counter->GetPin("/ld") });
		sim->PostEvent({ IOState::HI, counter->GetPin("/clr") });
		sim->PostEvent({ IOState::HI, counter->GetPin("enp") });
		sim->PostEvent({ IOState::HI, counter->GetPin("ent") });
		
		sim->PostEvent({ IOState::LOW, sim->GetPin("left") });
		sim->PostEvent({ IOState::LOW, sim->GetPin("right") });
		sim->PostEvent({ IOState::LOW, sim->GetPin("haz") });

		sim->Pulse( sim->GetPin("left"), 200, 1500);
		sim->Pulse( sim->GetPin("right"), 2000, 3500);
		sim->Pulse(sim->GetPin("haz"), 4000, 6500);

		size_t mul = 20;
		sim->Clock(sim->GetPin("clk"), mul * 4, mul * 4, 1, mul * 4 * 100);

		std::cout << Tools::LogicTools::LogicAnalyser(sim, {
			sim->GetPin("haz"),
			sim->GetPin("left"),
			sim->GetPin("right"),
			sim->GetPin("clk"),
			nullptr,
			sim->GetPin("q") }, 100 * 4 * mul, mul * 8);
	}
}