#include "stdafx.h"
#include "Core\Simulator.h"
#include "WireGate.h"

namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;

		WireGate::WireGate(size_t delay) noexcept : GateBase("wire", delay)
		{
		}

		void WireGate::Init()
		{
			GateBase::Init();
			m_in = AddInput("in");
			m_out = AddOutput("out");
		}

		Core::GatePtr WireGate::Create(size_t delay)
		{
			auto ptr = std::make_shared<shared_enabler>(delay);
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		Core::GatePtr WireGate::Clone(const char * name, bool deep)
		{
			auto ptr = std::make_shared<shared_enabler>(this->m_delay);
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		void WireGate::ComputeState()
		{
			IOState newState = m_in->Get();
			if (GetMode() == ASYNC)
			{
				m_out->Set(newState);
			}
			else
			{
				GetSimulator()->PostEventRelative({ newState, m_out }, m_delay);
			}
		}

		struct WireGate::shared_enabler : public WireGate
		{
			template <typename... Args>
			shared_enabler(Args &&... args)
				: WireGate(std::forward<Args>(args)...)
			{
			}
		};
	}
}