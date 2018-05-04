#include "stdafx.h"
#include "DFlipFlop.h"
#include "XORGate.h"
#include "WireGate.h"
#include "TFlipFlop.h"

namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;

		TFlipFlop::TFlipFlop() noexcept : CompositeGate("tflipflop")
		{
		}

		void TFlipFlop::Init()
		{
			AddInput("t");
			AddOutput("q");
			AddOutput("notq");

			AddGate("dflipflop", DFlipFlop::Create());
			AddGate("wire", WireGate::Create());

			GetPin("t")->ConnectTo(GetGate("dflipflop")->GetPin("clk"));
			GetGate("dflipflop")->GetPin("q")->ConnectTo(GetPin("q"));
			GetGate("dflipflop")->GetPin("notq")->ConnectTo(GetGate("wire")->GetPin("in"));
			GetGate("wire")->GetPin("out")->ConnectTo(GetGate("dflipflop")->GetPin("d"));
			GetGate("dflipflop")->GetPin("notq")->ConnectTo(GetPin("notq"));
		}

		Core::GatePtr TFlipFlop::Create()
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		Core::GatePtr TFlipFlop::Clone(const char * name)
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		void TFlipFlop::ComputeState()
		{
		}

		struct TFlipFlop::shared_enabler : public TFlipFlop
		{
			template <typename... Args>
			shared_enabler(Args &&... args)
				: TFlipFlop(std::forward<Args>(args)...)
			{
			}
		};
	}
}