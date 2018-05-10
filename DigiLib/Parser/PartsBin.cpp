#include "stdafx.h"
#include "PartsBin.h"
#include "Core\IOPin.h"
#include "Core\GateBase.h"
#include "Core\CompositeGate.h"

namespace DigiLib {
	namespace Parser {
		using namespace DigiLib::Core;

		PartsBinPtr PartsBin::Create()
		{
			auto ptr = std::make_shared<shared_enabler>();
			return std::static_pointer_cast<PartsBin>(ptr);
		}

		void PartsBin::AddPart(const char* name, GatePtr gate)
		{
			if (name == nullptr)
				throw std::invalid_argument("name is null");	
			if (gate == nullptr)
				throw std::invalid_argument("gate is null");
			if (!GateBase::IsValidGateName(name))
				throw std::invalid_argument("invalid gate name");

			if (m_gates.find(name) != m_gates.end())
			{
				throw std::invalid_argument("part with this name already exists");
			}

			this->m_gates[name] = gate->Clone(name);
		}

		bool PartsBin::HasGate(const char * name)
		{
			if (name == nullptr)
			{
				throw std::invalid_argument("name is null");
			}

			return m_gates.find(name) != m_gates.end();
		}

		GatePtr PartsBin::Find(const char * name)
		{
			if (name == nullptr)
			{
				throw std::invalid_argument("name is null");
			}

			auto part = m_gates.find(name);
			if (part != m_gates.end())
			{
				return part->second;
			}
			else if (m_partFinderFunc)
			{
				GatePtr gate = m_partFinderFunc(shared_from_this(), name);
				if (gate != nullptr)
				{
					this->m_gates[name] = gate->Clone(name);
					return this->m_gates[name];
				}
			}

			return nullptr;
		}

		struct PartsBin::shared_enabler : public PartsBin
		{
			template <typename... Args>
			shared_enabler(Args &&... args)
				: PartsBin(std::forward<Args>(args)...)
			{
			}
		};

	}
}