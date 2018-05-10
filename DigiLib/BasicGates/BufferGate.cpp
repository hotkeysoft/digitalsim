#include "stdafx.h"
#include "BufferGate.h"
#include <sstream>

namespace DigiLib {
	namespace BasicGates {
		using namespace DigiLib::Core;

		BufferGate::BufferGate(size_t width) : GateBase("buffer"), m_width(width)
		{
		}
		
		void BufferGate::Init()
		{
			GateBase::Init();
			m_in = AddInput("in", m_width);
			m_en = AddInput("en");
			m_out = AddOutput("out", m_width, IOPin::OUTPUT_HI_Z);
		}

		Core::GatePtr BufferGate::Create(size_t width)
		{
			auto ptr = std::make_shared<shared_enabler>(width);
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		Core::GatePtr BufferGate::Clone(const char * name, bool deep)
		{
			auto ptr = std::make_shared<shared_enabler>(this->m_width);
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		void BufferGate::ComputeState()
		{
			if (m_en->Get() == IOState::LOW)
			{
				IOState hiz(IOState::HI_Z, m_width);
				m_out->Set(hiz);
			}
			else
			{
				m_out->Set(m_in->Get());
			}
		}

		struct BufferGate::shared_enabler : public BufferGate
		{
			template <typename... Args>
			shared_enabler(Args &&... args)
				: BufferGate(std::forward<Args>(args)...)
			{
			}
		};
	}
}