#include "stdafx.h"
#include "IOPin.h"
#include "GateBase.h"

namespace DigiLib
{
	namespace Core
	{
		//IOPinPtr IOPin::Create(GateBase *parentGate, size_t id, const char * name, size_t width, IO_DIRECTION direction)
		//{
		//	struct make_shared_enabler : public IOPin {};
		//	IOPinPtr created = std::make_shared<make_shared_enabler>();
		//	created->m_parentGate = parentGate;
		//	created->m_id = id;
		//	created->m_name = name;
		//	created->m_direction = direction;
		//	created->m_width = width;

		//	return created;
		//}

		IOPin::IOPin(GateBase *parentGate, size_t id, const char* name, size_t width, IO_DIRECTION direction) :
			m_name(name), m_id(id), m_width(width), m_direction(direction), m_state(IOState::UNDEF, width), m_parentGate(parentGate)
		{
			assert(parentGate != NULL);
			assert(name != NULL);
		}

		IOPinPtr IOPin::Clone(GateBase * cloneParent)
		{
			return cloneParent->GetPin(GetRawName().c_str());
		}

		std::string IOPin::GetFullName()
		{
			std::ostringstream os;
			os << m_parentGate->GetFullName() << "." << GetName();
			return os.str();
		}

		void IOPin::Set(IOState state)
		{
			if (state.GetWidth() != this->GetWidth())
			{
				throw std::invalid_argument("pin width mismatch");
			}

			auto & connectedPins = m_parentGate->GetConnectedToPins(m_id);

			m_state = state;
			if (m_direction == IO_DIRECTION::INPUT && connectedPins.size() > 0)
			{
				// Connect to internal gates
				for (auto & connected : connectedPins)
				{
					connected.GetTarget()->Set(connected.GetSource()->Get());
				}
			}
			// TODO: Not sure about this.. could it be both?
			else if (m_direction == IO_DIRECTION::INPUT)
			{
				// Handle HI_Z multiple input connections 
				ComputePinState();

				m_parentGate->ComputeState();
			}

			if (m_direction == IO_DIRECTION::OUTPUT || m_direction == IO_DIRECTION::OUTPUT_HI_Z)
			{
				for (auto & connected : connectedPins)
				{
					connected.GetTarget()->Set(connected.GetSource()->Get());
				}
			}
		}

		void IOPin::ComputePinState()
		{
			auto & connectedFrom = m_parentGate->GetConnectedFromPins(m_id);
			if (connectedFrom.size() > 1)
			{
				m_state = IOState(IOState::HI_Z, m_width);
				for (auto pin : connectedFrom)
				{
					size_t offset = pin.GetTarget()->GetOffset();
					size_t width = pin.GetSource()->GetWidth();
					IOState state = pin.GetSource()->Get();

					for (size_t i = 0; i < width; ++i, ++offset)
					{
						if (m_state[offset] == IOState::HI_Z)
						{
							m_state[offset] = state[i];
						}
						else if (state[i] == IOState::HI_Z)
						{
							// Keep existing state
						}
						else
						{
							m_state[offset] = IOState::UNDEF; // Conflict
						}
					}
				}
			}
		}

		void IOPin::ConnectTo(IOPinPtr target)
		{
			IOPinPtr source = this->shared_from_this();
			m_parentGate->ConnectPins(source, target);
		}
	}
}