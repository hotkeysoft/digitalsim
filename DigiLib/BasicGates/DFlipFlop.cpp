#include "stdafx.h"
#include "NOTGate.h"
#include "DLatch.h"
#include "DFlipFlop.h"

namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;

		DFlipFlop::DFlipFlop(bool positiveTriggered) noexcept : CompositeGate("dflipflop"), m_positiveTriggered(positiveTriggered)
		{
		}

		void DFlipFlop::Init()
		{
			AddInput("d");
			AddInput("clk");
			AddOutput("q");
			AddOutput("/q");

			AddGate("d1", DLatch::Create());
			AddGate("d2", DLatch::Create());
			AddGate("not2", NOTGate::Create());

			GetPin("d")->ConnectTo(GetGate("d1")->GetPin("d"));
			if (m_positiveTriggered)
			{
				AddGate("not1", NOTGate::Create());
				GetPin("clk")->ConnectTo(GetGate("not1")->GetPin("in"));
				GetGate("not1")->GetPin("out")->ConnectTo(GetGate("d1")->GetPin("c"));
				GetGate("not1")->GetPin("out")->ConnectTo(GetGate("not2")->GetPin("in"));
			}
			else
			{
				GetPin("clk")->ConnectTo(GetGate("d1")->GetPin("c"));
				GetPin("clk")->ConnectTo(GetGate("not2")->GetPin("in"));
			}

			GetGate("not2")->GetPin("out")->ConnectTo(GetGate("d2")->GetPin("c"));
			GetGate("d2")->GetPin("q")->ConnectTo(GetPin("q"));
			GetGate("d1")->GetPin("q")->ConnectTo(GetGate("d2")->GetPin("d"));
			GetGate("d2")->GetPin("/q")->ConnectTo(GetPin("/q"));
		}

		Core::GatePtr DFlipFlop::Create(bool positiveTriggered)
		{
			auto ptr = std::make_shared<shared_enabler>(positiveTriggered);
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		Core::GatePtr DFlipFlop::Clone(const char * name, bool deep)
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
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