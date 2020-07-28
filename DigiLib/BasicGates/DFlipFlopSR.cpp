#include "stdafx.h"
#include "NANDGate.h"
#include "NOTGate.h"
#include "DFlipFlopSR.h"

namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;

		DFlipFlopSR::DFlipFlopSR() noexcept : CompositeGate("dflipflopsr")
		{
		}

		void DFlipFlopSR::Init()
		{
			GateBase::Init();
			AddInput("/set");
			AddInput("/reset");
			AddInput("d");
			AddInput("clk");
			AddOutput("q");
			AddOutput("/q");


			AddGate("nand1", NANDGate::Create(3));
			AddGate("nand2", NANDGate::Create(3));
			AddGate("nand3", NANDGate::Create(3));
			AddGate("nand4", NANDGate::Create(3));
			AddGate("nand5", NANDGate::Create(3));
			AddGate("nand6", NANDGate::Create(3));

			GetPin("/set")->ConnectTo(GetGate("nand1")->GetPin("in1"));
			GetPin("/set")->ConnectTo(GetGate("nand5")->GetPin("in1"));

			GetPin("/reset")->ConnectTo(GetGate("nand2")->GetPin("in2"));
			GetPin("/reset")->ConnectTo(GetGate("nand6")->GetPin("in2"));
			GetPin("/reset")->ConnectTo(GetGate("nand4")->GetPin("in2"));

			GetPin("d")->ConnectTo(GetGate("nand4")->GetPin("in3"));

			GetPin("clk")->ConnectTo(GetGate("nand3")->GetPin("in2"));
			GetPin("clk")->ConnectTo(GetGate("nand2")->GetPin("in3"));

			GetGate("nand1")->GetPin("out")->ConnectTo(GetGate("nand2")->GetPin("in1"));

			GetGate("nand2")->GetPin("out")->ConnectTo(GetGate("nand1")->GetPin("in3"));
			GetGate("nand2")->GetPin("out")->ConnectTo(GetGate("nand5")->GetPin("in2"));
			GetGate("nand2")->GetPin("out")->ConnectTo(GetGate("nand3")->GetPin("in1"));

			GetGate("nand3")->GetPin("out")->ConnectTo(GetGate("nand6")->GetPin("in3"));
			GetGate("nand3")->GetPin("out")->ConnectTo(GetGate("nand4")->GetPin("in1"));

			GetGate("nand4")->GetPin("out")->ConnectTo(GetGate("nand3")->GetPin("in3"));
			GetGate("nand4")->GetPin("out")->ConnectTo(GetGate("nand1")->GetPin("in2"));

			GetGate("nand5")->GetPin("out")->ConnectTo(GetGate("nand6")->GetPin("in1"));
			GetGate("nand5")->GetPin("out")->ConnectTo(GetPin("q"));

			GetGate("nand6")->GetPin("out")->ConnectTo(GetGate("nand5")->GetPin("in3"));
			GetGate("nand6")->GetPin("out")->ConnectTo(GetPin("/q"));
		}

		void DFlipFlopSR::InitializeState()
		{
			CompositeGate::InitializeState();
			
			// To avoit initial oscillation, we set Q and notQ to opposite (random) states.
			IOState initialState = IOState::IOState::Random();
			GetGate("nand1")->GetPin("in3")->Set(initialState);
			GetGate("nand2")->GetPin("in1")->Set(!initialState);

			GetGate("nand3")->GetPin("in3")->Set(initialState);
			GetGate("nand4")->GetPin("in1")->Set(!initialState);

			GetGate("nand5")->GetPin("in3")->Set(initialState);
			GetGate("nand6")->GetPin("in1")->Set(!initialState);
		}

		Core::GatePtr DFlipFlopSR::Create()
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		Core::GatePtr DFlipFlopSR::Clone(const char * name, bool deep)
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		struct DFlipFlopSR::shared_enabler : public DFlipFlopSR
		{
			template <typename... Args>
			shared_enabler(Args &&... args)
				: DFlipFlopSR(std::forward<Args>(args)...)
			{
			}
		};
	}
}