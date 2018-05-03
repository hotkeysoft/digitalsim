#include "stdafx.h"
#include "NORGate.h"
#include "SRLatch.h"


namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;

		SRLatch::SRLatch() noexcept : CompositeGate("srlatch")
		{
		}

		void SRLatch::Init()
		{
			AddInput("r");
			AddInput("s");
			AddOutput("q");
			AddOutput("notq");
			AddGate("nor1", NORGate::Create());
			AddGate("nor2", NORGate::Create());

			GetPin("r")->ConnectTo(GetGate("nor1")->GetPin("in1"));
			GetPin("s")->ConnectTo(GetGate("nor2")->GetPin("in2"));
			GetGate("nor1")->GetPin("out")->ConnectTo(GetPin("q"));
			GetGate("nor1")->GetPin("out")->ConnectTo(GetGate("nor2")->GetPin("in1"));
			GetGate("nor2")->GetPin("out")->ConnectTo(GetPin("notq"));
			GetGate("nor2")->GetPin("out")->ConnectTo(GetGate("nor1")->GetPin("in2"));
		}

		Core::GatePtr SRLatch::Create()
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		Core::GatePtr SRLatch::Clone(const char * name)
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		void SRLatch::ComputeState()
		{
		}

		struct SRLatch::shared_enabler : public SRLatch
		{
			template <typename... Args>
			shared_enabler(Args &&... args)
				: SRLatch(std::forward<Args>(args)...)
			{
			}
		};
	}
}