#include "stdafx.h"
#include "WireGate.h"

namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;

		WireGate::WireGate() noexcept : GateBase("wire")
		{
		}

		void WireGate::Init()
		{
			m_in = AddInput("in");
			m_out = AddOutput("out");
		}

		Core::GatePtr WireGate::Create()
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		Core::GatePtr WireGate::Clone(const char * name)
		{
			auto ptr = std::make_shared<shared_enabler>();
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		void WireGate::ComputeState()
		{
			m_out->Set(m_in->Get());
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