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
			AddInput("j");
			AddInput("k");
			AddInput("clk");
			AddOutput("q");
			AddOutput("notq");

			AddGate("dflipflop", DFlipFlop::Create());
			AddGate("nand1", NANDGate::Create());
			AddGate("nand2", NANDGate::Create());
			AddGate("nand3", NANDGate::Create());
			AddGate("not", NOTGate::Create());

			GetPin("k")->ConnectTo(GetGate("not")->GetPin("in"));
			GetGate("not")->GetPin("out")->ConnectTo(GetGate("nand2")->GetPin("in1"));
			GetPin("j")->ConnectTo(GetGate("nand1")->GetPin("in2"));
			GetPin("clk")->ConnectTo(GetGate("dflipflop")->GetPin("clk"));

			GetGate("nand1")->GetPin("out")->ConnectTo(GetGate("nand3")->GetPin("in1"));
			GetGate("nand2")->GetPin("out")->ConnectTo(GetGate("nand3")->GetPin("in2"));
			GetGate("nand3")->GetPin("out")->ConnectTo(GetGate("dflipflop")->GetPin("d"));

			GetGate("dflipflop")->GetPin("q")->ConnectTo(GetPin("q"));
			GetGate("dflipflop")->GetPin("q")->ConnectTo(GetGate("nand2")->GetPin("in2"));
			GetGate("dflipflop")->GetPin("notq")->ConnectTo(GetPin("notq"));
			GetGate("dflipflop")->GetPin("notq")->ConnectTo(GetGate("nand1")->GetPin("in1"));
		}

		Core::GatePtr JKFlipFlop::Create()
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		Core::GatePtr JKFlipFlop::Clone(const char * name)
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		void JKFlipFlop::ComputeState()
		{
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