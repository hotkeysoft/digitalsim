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
			GateBase::Init();
			AddInput("r");
			AddInput("s");
			AddOutput("q");
			AddOutput("/q");
			AddGate("nor1", NORGate::Create());
			AddGate("nor2", NORGate::Create());

			GetPin("r")->ConnectTo(GetGate("nor1")->GetPin("in1"));
			GetPin("s")->ConnectTo(GetGate("nor2")->GetPin("in2"));
			GetGate("nor1")->GetPin("out")->ConnectTo(GetPin("q"));
			GetGate("nor1")->GetPin("out")->ConnectTo(GetGate("nor2")->GetPin("in1"));
			GetGate("nor2")->GetPin("out")->ConnectTo(GetPin("/q"));
			GetGate("nor2")->GetPin("out")->ConnectTo(GetGate("nor1")->GetPin("in2"));
		}

		void SRLatch::InitializeState()
		{
			CompositeGate::InitializeState();
			// To avoit initial oscillation, we set Q and notQ to opposite (random) states.
			IOState initialState = IOState::IOState::Random();
			IOState notState = !initialState;
			GetPin("q")->Set(initialState);
			GetPin("/q")->Set(!initialState);
		}

		Core::GatePtr SRLatch::Create()
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		Core::GatePtr SRLatch::Clone(const char * name, bool deep)
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
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