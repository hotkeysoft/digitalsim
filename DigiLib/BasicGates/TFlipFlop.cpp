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
			AddOutput("/q");

			AddGate("dflip", DFlipFlop::Create());

			GetPin("t")->ConnectTo(GetGate("dflip")->GetPin("clk"));
			GetGate("dflip")->GetPin("q")->ConnectTo(GetPin("q"));

			GetGate("dflip")->GetPin("/q")->ConnectTo(GetGate("dflip")->GetPin("d"));
			GetGate("dflip")->GetPin("/q")->ConnectTo(GetPin("/q"));
		}

		Core::GatePtr TFlipFlop::Create()
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		Core::GatePtr TFlipFlop::Clone(const char * name, bool deep)
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
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