#include "stdafx.h"
#include "XORGate.h"

namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;

		XORGate::XORGate() noexcept : GateBase("xor")
		{
		}

		void XORGate::Init()
		{
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

		Core::GatePtr XORGate::Clone(const char * name)
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;

		}

		void XORGate::ComputeState()
		{
			if ((m_in1->Get() == IOState::LOW && m_in2->Get() == IOState::HI) ||
				(m_in1->Get() == IOState::HI && m_in2->Get() == IOState::LOW))
			{
				m_out->Set(IOState::HI);
			}
			else
			{
				m_out->Set(IOState::LOW);
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