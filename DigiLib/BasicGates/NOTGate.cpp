#include "stdafx.h"
#include "Core/Simulator.h"
#include "NOTGate.h"

namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;

		NOTGate::NOTGate() noexcept : GateBase("not")
		{
		}

		void NOTGate::Init()
		{
			GateBase::Init();
			m_in = AddInput("in");
			m_out = AddOutput("out");
		}

		Core::GatePtr NOTGate::Create()
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		Core::GatePtr NOTGate::Clone(const char * name, bool deep)
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		void NOTGate::ComputeState()
		{
			IOState newState;
			if (m_in->Get() == IOState::UNDEF)
			{
				newState = IOState::UNDEF;
			}
			else if (m_in->Get() == IOState::HI)
			{
				newState = IOState::LOW;
			}
			else
			{
				newState = IOState::HI;
			}

			if (GetMode() == ASYNC)
			{
				m_out->Set(newState);
			}
			else
			{
				GetSimulator()->PostEvent({ newState, m_out });
			}
		}

		struct NOTGate::shared_enabler : public NOTGate
		{
			template <typename... Args>
			shared_enabler(Args &&... args)
				: NOTGate(std::forward<Args>(args)...)
			{
			}
		};
	}
}