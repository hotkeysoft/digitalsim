#include "stdafx.h"
#include "DFlipFlop.h"
#include "NANDGate.h"
#include "NOTGate.h"
#include "JKFlipFlop.h"

namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;

		JKFlipFlop::JKFlipFlop() noexcept : CompositeGate("jkflipflop")
		{
		}

		void JKFlipFlop::Init()
		{
			GateBase::Init();
			AddInput("j");
			AddInput("k");
			AddInput("clk");
			AddOutput("q");
			AddOutput("/q");

			AddGate("dflipflop", DFlipFlop::Create());
			AddGate("nand1", NANDGate::Create());
			AddGate("nand2", NANDGate::Create());
			AddGate("nand3", NANDGate::Create());

			FindPin("k")->ConnectTo(FindPin("nand2.in1"), true);					
			FindPin("j")->ConnectTo(FindPin("nand1.in2"));
			FindPin("clk")->ConnectTo(FindPin("dflipflop.clk"));

			FindPin("nand1.out")->ConnectTo(FindPin("nand3.in1"));
			FindPin("nand2.out")->ConnectTo(FindPin("nand3.in2"));
			FindPin("nand3.out")->ConnectTo(FindPin("dflipflop.d"));

			FindPin("dflipflop.q")->ConnectTo(FindPin("q"));
			FindPin("dflipflop.q")->ConnectTo(FindPin("nand2.in2"));
			FindPin("dflipflop./q")->ConnectTo(FindPin("/q"));
			FindPin("dflipflop./q")->ConnectTo(FindPin("nand1.in1"));
		}

		void JKFlipFlop::InitializeState()
		{
			CompositeGate::InitializeState();

			// To avoid initial oscillation, we set Q and notQ to opposite states.
			static IOState initialState = IOState::HI;
			GetGate("nand1")->GetPin("in1")->Set(initialState);
			GetGate("nand2")->GetPin("in2")->Set(initialState);
		}

		Core::GatePtr JKFlipFlop::Create()
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		Core::GatePtr JKFlipFlop::Clone(const char * name, bool deep)
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		struct JKFlipFlop::shared_enabler : public JKFlipFlop
		{
			template <typename... Args>
			shared_enabler(Args &&... args)
				: JKFlipFlop(std::forward<Args>(args)...)
			{
			}
		};
	}
}