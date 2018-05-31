#include "stdafx.h"
#include "IOPinSubset.h"
#include "Core/GateBase.h"

namespace DigiLib
{
	namespace Core
	{
		IOPinSubset::IOPinSubset(IOPinRef parentPin, size_t offset)
			: IOPin(parentPin->GetParent(), parentPin->GetID(), parentPin->GetName().c_str(), 1, parentPin->GetDirection()),
			 m_parentPin(parentPin), m_low(offset), m_hi(offset)
		{
		}

		IOPinSubset::IOPinSubset(IOPinRef parentPin, size_t low, size_t hi)
			: IOPin(parentPin->GetParent(), parentPin->GetID(), parentPin->GetName().c_str(), hi-low+1, parentPin->GetDirection()),
			 m_parentPin(parentPin), m_low(low), m_hi(hi)
		{
		}

		IOPinPtr IOPinSubset::Clone(GateRef cloneParent)
		{
			return cloneParent->GetPin(GetRawName().c_str(), m_low, m_hi);
		}

		std::string IOPinSubset::GetName()
		{
			std::ostringstream os;
			os << m_name << '[' << m_low;
			if (m_hi != m_low)
				os << ':' << m_hi;
			os << ']';
			return os.str();
		}

		IOState IOPinSubset::Get() noexcept
		{
			return m_parentPin->Get().Get(m_low, m_hi);
		}
		
		void IOPinSubset::Set(IOState state)
		{
			if (state.GetWidth() != this->m_width)
			{
				throw std::invalid_argument("bus width mismatch");
			}

			IOState parentState = m_parentPin->Get();
			
			for (size_t in = 0, out = m_low; out <= m_hi; ++in, ++out)
			{
				parentState[out] = state[in];
			}
						
			m_parentPin->Set(parentState);
		}

		IOState IOPinSubset::GetMask() noexcept
		{
			IOState::IO_STATE state;
			switch (m_direction)
			{
			case INPUT:
				state = IOState::LOW; break;
			case OUTPUT:
				state = IOState::HI; break;
			case OUTPUT_HI_Z:
				state = IOState::HI_Z; break;
			default: 
				state = IOState::UNDEF;
			}

			IOState ret(IOState::UNDEF, m_parentPin->GetWidth());
			for (size_t i = m_low; i <= m_hi; ++i)
			{
				ret[i] = state;
			}
			return ret;
		}

		bool IOPinSubset::Overlaps(IOState state)
		{
			if (m_parentPin->GetWidth() != state.GetWidth())
				throw std::invalid_argument("pin width mismatch");

			for (size_t i = m_low; i <= m_hi; ++i)
			{
				if (state[i] != IOState::UNDEF)
					return true;
			}

			return false;
		}

	}
}