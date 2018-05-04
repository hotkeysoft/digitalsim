#include "stdafx.h"
#include "NOTGate.h"
#include "DLatch.h"
#include "DFlipFlop.h"

namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;

		DFlipFlop::DFlipFlop() noexcept : CompositeGate("dflipflop")
		{
		}

		void DFlipFlop::Init()
		{
			AddInput("d");
			AddInput("clk");
			AddOutput("q");
			AddOutput("notq");

			AddGate("dlatch1", DLatch::Create());
			AddGate("dlatch2", DLatch::Create());
			AddGate("not1", NOTGate::Create());
			AddGate("not2", NOTGate::Create());

			GetPin("d")->ConnectTo(GetGate("dlatch1")->GetPin("d"));
			GetPin("clk")->ConnectTo(GetGate("not1")->GetPin("in"));
			GetGate("not1")->GetPin("out")->ConnectTo(GetGate("dlatch1")->GetPin("c"));
			GetGate("not1")->GetPin("out")->ConnectTo(GetGate("not2")->GetPin("in"));
			GetGate("not2")->GetPin("out")->ConnectTo(GetGate("dlatch2")->GetPin("c"));
			GetGate("dlatch1")->GetPin("q")->ConnectTo(GetGate("dlatch2")->GetPin("d"));
			GetGate("dlatch2")->GetPin("q")->ConnectTo(GetPin("q"));
			GetGate("dlatch2")->GetPin("notq")->ConnectTo(GetPin("notq"));
		}

		Core::GatePtr DFlipFlop::Create()
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		Core::GatePtr DFlipFlop::Clone(const char * name)
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		void DFlipFlop::ComputeState()
		{
		}

		struct DFlipFlop::shared_enabler : public DFlipFlop
		{
			template <typename... Args>
			shared_enabler(Args &&... args)
				: DFlipFlop(std::forward<Args>(args)...)
			{
			}
		};
	}
}