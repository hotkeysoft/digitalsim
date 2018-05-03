#include "stdafx.h"
#include "NOTGate.h"

namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;

		NOTGate::NOTGate() noexcept : GateBase("not")
		{
		}

		void NOTGate::Init()
		{
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

		Core::GatePtr NOTGate::Clone(const char * name)
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		void NOTGate::ComputeState()
		{
			if (m_in->Get() == IOState::HI)
			{
				m_out->Set(IOState::LOW);
			}
			else
			{
				m_out->Set(IOState::HI);
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