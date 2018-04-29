#include "stdafx.h"
#include "IOPin.h"
#include "GateBase.h"

namespace DigiLib
{
	namespace Core
	{
		IOPin::IOPin(GateBase *parentGate, const char* name, IO_DIRECTION direction) :
			m_name(name), m_direction(direction), m_state(IOPin::UNDEF), m_parentGate(parentGate)
		{
			assert(parentGate != NULL);
			assert(name != NULL);
		}

		std::string IOPin::GetFullName()
		{
			std::ostringstream os;
			os << m_parentGate->GetFullName() << "." << m_name;
			return os.str();
		}

		void IOPin::Set(IO_STATE state)
		{
			auto & connectedPins = m_parentGate->GetConnectedToPins(this);

			m_state = state;
			if (m_direction == IO_DIRECTION::INPUT && connectedPins.size() > 0)
			{
				// Connect to internal gates
				for (auto connected : connectedPins)
				{
					connected.GetTarget()->Set(state);
				}
			}

			if (m_direction == IO_DIRECTION::INPUT)
			{
				// Handle HI_Z multiple input connections 
				ComputePinState();

				m_parentGate->ComputeState();
			}

			if (m_direction == IO_DIRECTION::OUTPUT || m_direction == IO_DIRECTION::OUTPUT_HI_Z)
			{
				for (auto connected : connectedPins)
				{
					connected.GetTarget()->Set(state);
				}
			}
		}

		void IOPin::ComputePinState()
		{
			auto & connectedFrom = m_parentGate->GetConnectedFromPins(this);
			if (connectedFrom.size() > 1)
			{
				m_state = IOPin::HI_Z;
				for (auto pin : connectedFrom)
				{
					IOPin::IO_STATE pinState = pin.GetSource()->Get();

					if (m_state == IOPin::HI_Z)
					{
						m_state = pinState;
					}
					else if (pinState == IOPin::HI_Z)
					{
						// Keep existing state
					}
					else
					{
						m_state = IOPin::UNDEF; // Conflict
					}
				}
			}
		}

		void IOPin::ConnectTo(IOPin* target)
		{
			m_parentGate->ConnectPins(this, target);
		}
	}
}