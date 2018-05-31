#include "stdafx.h"
#include "Core/Simulator.h"
#include "XORGate.h"

namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;

		XORGate::XORGate() noexcept : GateBase("xor")
		{
		}

		void XORGate::Init()
		{
			GateBase::Init();
			m_in1 = AddInput("in1");
			m_in2 = AddInput("in2");
			m_out = AddOutput("out");
		}

		Core::GatePtr XORGate::Create()
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;

		}

		Core::GatePtr XORGate::Clone(const char * name, bool deep)
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;

		}

		void XORGate::ComputeState()
		{
			IOState newState;
			if ((m_in1->Get() == IOState::LOW && m_in2->Get() == IOState::HI) ||
				(m_in1->Get() == IOState::HI && m_in2->Get() == IOState::LOW))
			{
				newState = IOState::HI;
			}
			else
			{
				newState = IOState::LOW;
			}

			if (m_in1->Get() == IOState::UNDEF || m_in2->Get() == IOState::UNDEF)
			{
				newState = IOState::UNDEF;
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

		struct XORGate::shared_enabler : public XORGate
		{
			template <typename... Args>
			shared_enabler(Args &&... args)
				: XORGate(std::forward<Args>(args)...)
			{
			}
		};
	}
}