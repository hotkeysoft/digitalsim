#include "stdafx.h"
#include "IOPin.h"
#include "GateBase.h"
#include "Simulator.h"

namespace DigiLib
{
	namespace Core
	{
		IOPin::IOPin(GateRef parentGate, size_t id, const char* name, size_t width, IO_DIRECTION direction) :
			m_name(name), m_id(id), m_width(width), m_direction(direction), 
			m_state(IOState::UNDEF, width), m_parentGate(parentGate)
		{
			assert(name != NULL);
		}

		IOPinPtr IOPin::Clone(GateRef cloneParent)
		{
			return cloneParent->GetPin(GetRawName().c_str());
		}

		std::string IOPin::GetFullName()
		{
			std::ostringstream os;	
			os << m_parentGate->GetFullName() << "." << GetName();
			return os.str();
		}

		IOState IOPin::GetMask() noexcept
		{
			IOState::IO_STATE state = IOState::UNDEF;
			switch (m_direction)
			{
			case INPUT:
				state = IOState::LOW; break;
			case OUTPUT: 
				state = IOState::HI; break;
			case OUTPUT_HI_Z:
				state = IOState::HI_Z; break;
			}

			return IOState(state, m_width);
		}

		bool IOPin::Overlaps(IOState state)
		{
			if (state.GetWidth() != m_width)
				throw std::invalid_argument("pin width mismatch");

			for (size_t i = 0; i < m_width; ++i)
			{
				if (state[i] != IOState::UNDEF)
					return true;
			}

			return false;
		}

		void IOPin::Reset(IOState::IO_STATE state)
		{
			m_state = IOState(state, m_width);
		}

		void IOPin::Set(IOState state)
		{
			if (state.GetWidth() != this->GetWidth())
			{
				throw std::invalid_argument("pin width mismatch");
			}
			
			if (m_state == state)
			{
				return;
			}
			m_state = state;

			const auto mode = m_parentGate->GetMode();

			auto & connectedPins = m_parentGate->GetConnectedToPin(shared_from_this());
			
			if (m_direction == IO_DIRECTION::INPUT && connectedPins.size() > 0)
			{
				// Connect to internal gates
				for (auto & connected : connectedPins)
				{
					if (mode == GateBase::ASYNC)
					{
						connected.GetTarget()->Set(connected.GetSource()->Get());
					}
					else
					{
						m_parentGate->GetSimulator()->PostEvent(connected);
					}					
				}
			}
			// TODO: Not sure about this.. could it be both?
			else if (m_direction == IO_DIRECTION::INPUT)
			{
				// Handle HI_Z multiple input connections 
				ComputePinState();

				m_parentGate->ComputeState();
			}

			if (m_direction == IO_DIRECTION::POWER || 
				m_direction == IO_DIRECTION::OUTPUT || 
				m_direction == IO_DIRECTION::OUTPUT_HI_Z)
			{
				for (auto & connected : connectedPins)
				{
					if (mode == GateBase::ASYNC)
					{
						connected.GetTarget()->Set(connected.GetSource()->Get());
					}
					else
					{
						m_parentGate->GetSimulator()->PostEvent(connected);
					}
				}
			}
		}

		void IOPin::ComputePinState()
		{
			auto & connectedFrom = m_parentGate->GetConnectedFromPin(shared_from_this());
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

		void IOPin::ConnectTo(IOPinPtr target, bool inverted)
		{
			IOPinPtr source = this->shared_from_this();
			if (!m_parentGate->ConnectPins(source, target, inverted))
			{
				throw std::invalid_argument("can not create connnection");
			}
		}
	}
}