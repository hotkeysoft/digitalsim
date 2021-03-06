#include "stdafx.h"
#include "NORGate.h"
#include "NANDGate.h"
#include "NOTGate.h"
#include "DLatch.h"

namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;

		DLatch::DLatch() noexcept : CompositeGate("dlatch")
		{
		}

		void DLatch::Init()
		{
			GateBase::Init();
			AddInput("d");
			AddInput("c");
			AddOutput("q");
			AddOutput("/q");
			AddGate("nand1", NANDGate::Create());
			AddGate("nand2", NANDGate::Create());
			AddGate("nand3", NANDGate::Create());
			AddGate("nand4", NANDGate::Create());
			AddGate("not", NOTGate::Create());

			GetPin("d")->ConnectTo(GetGate("nand1")->GetPin("in1"));
			GetPin("d")->ConnectTo(GetGate("not")->GetPin("in"));
			GetPin("c")->ConnectTo(GetGate("nand1")->GetPin("in2"));
			GetPin("c")->ConnectTo(GetGate("nand2")->GetPin("in1"));
			GetGate("not")->GetPin("out")->ConnectTo(GetGate("nand2")->GetPin("in2"));
			GetGate("nand1")->GetPin("out")->ConnectTo(GetGate("nand3")->GetPin("in1"));
			GetGate("nand2")->GetPin("out")->ConnectTo(GetGate("nand4")->GetPin("in2"));
			GetGate("nand3")->GetPin("out")->ConnectTo(GetPin("q"));
			GetGate("nand3")->GetPin("out")->ConnectTo(GetGate("nand4")->GetPin("in1"));
			GetGate("nand4")->GetPin("out")->ConnectTo(GetPin("/q"));
			GetGate("nand4")->GetPin("out")->ConnectTo(GetGate("nand3")->GetPin("in2"));
		}

		void DLatch::InitializeState()
		{
			CompositeGate::InitializeState();
			
			// To avoid initial oscillation, we set Q and notQ to opposite states.
			static IOState initialState = IOState::HI;
			GetGate("nand4")->GetPin("in1")->Set(initialState);
			GetGate("nand3")->GetPin("in2")->Set(!initialState);
		}

		Core::GatePtr DLatch::Create()
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		Core::GatePtr DLatch::Clone(const char * name, bool deep)
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		struct DLatch::shared_enabler : public DLatch
		{
			template <typename... Args>
			shared_enabler(Args &&... args)
				: DLatch(std::forward<Args>(args)...)
			{
			}
		};
	}
}