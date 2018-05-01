#include "stdafx.h"
#include "IOState.h"
#include <algorithm>

namespace DigiLib
{
	namespace Core
	{
		IOState::IOState(IO_STATE state, size_t width) : m_states(width, state)
		{
			if (width < 1 || width > MAX_PINS)
				throw std::out_of_range("invalid pin width");
		}

		IOState::IOState(std::initializer_list<IO_STATE> init_list) : m_states(init_list.size())
		{
			m_states = init_list;
		}

		IOState IOState::operator=(const IOState &rhs)
		{
			if (rhs.GetWidth() != this->GetWidth())
			{
				throw std::invalid_argument("source and target must be same width");
			}
			this->m_states = rhs.m_states;
			return *this;
		}

		IOState IOState::operator=(IO_STATE state)
		{
			Set(state);
			return *this;
		}

		bool IOState::operator==(const IOState & rhs) const
		{
			return m_states == rhs.m_states;
		}

		bool IOState::operator!=(const IOState & rhs) const
		{
			return m_states != rhs.m_states;
		}

		bool IOState::operator==(IO_STATE rhs) const
		{
			return m_states.size() == 1 && m_states[0] == rhs;
		}

		bool IOState::operator!=(IO_STATE rhs) const
		{
			if (m_states.size() != 1)
				return true;

			return m_states[0] != rhs;
		}

		std::string IOState::ToString() const
		{
			std::ostringstream os;
			if (m_states.size() == 1)
			{
				os << ToString(m_states[0]);
			}
			else
			{
				const char* separator = "";
				os << "{";
				for (IO_STATE state : m_states)
				{
					os << separator << ToString(state);
					separator = ", ";
				}
				os << "}";
			}
			return os.str();
		}

		std::string IOState::ToString(IO_STATE state)
		{
			switch (state)
			{
			case IO_STATE::HI: return "1";
			case IO_STATE::HI_Z: return "Z";
			case IO_STATE::LOW: return "0";
			default: return "x";
			}
		}

		uint8_t IOState::ToInt8() const
		{
			uint8_t ret = 0;
			for (size_t i = 0; i < std::min((size_t)8, m_states.size()); ++i)
			{
				switch (m_states[i])
				{
				case IO_STATE::HI:
					ret += 1 << i;
					break;
				case IO_STATE::LOW:
					break;
				default:
					throw std::invalid_argument("Cannot be converted to number");
				}
			}

			return ret;
		}

		uint16_t IOState::ToInt16() const
		{
			uint16_t ret = 0;
			for (size_t i = 0; i < std::min((size_t)16, m_states.size()); ++i)
			{
				switch (m_states[i])
				{
				case IO_STATE::HI:
					ret += 1 << i;
					break;
				case IO_STATE::LOW:
					break;
				default:
					throw std::invalid_argument("Cannot be converted to number");
				}
			}

			return ret;
		}

		IOState IOState::FromInt(int value, size_t bitCount)
		{
			if (bitCount <1 || bitCount > MAX_PINS)
			{
				throw std::out_of_range("bitCount out of range");
			}
			if (value < 0)
			{
				throw std::out_of_range("value can not be negative");
			}

			IOState ret(IOState::LOW, bitCount);
			for (size_t i = 0; i < bitCount; ++i)
			{
				if (value & (1 << i))
				{
					ret[i] = IOState::HI;
				}
			}
			
			return ret;
		}

		IOState::IO_STATE IOState::Get(size_t pin) const
		{
			if (pin < 0 || pin >= m_states.size())
			{
				throw std::out_of_range("invalid pin");
			}

			return m_states[pin];
		}

		IOState IOState::Get(size_t low, size_t hi) const
		{
			if (low < 0 || low >= m_states.size() ||
				hi < 0 || hi >= m_states.size() ||
				low > hi)
			{
				throw std::out_of_range("invalid range");
			}

			size_t width = hi - low + 1;
			IOState state(IOState::UNDEF, width);
			for (size_t out=0, in = low; in <= hi; ++in, ++out)
			{
				state[out] = m_states[in];
			}
			return state;
		}

		void IOState::Set(std::vector<IO_STATE> pins)
		{
			if (pins.size() != m_states.size())
			{
				throw std::invalid_argument("source and target must be same width");
			}

			m_states = pins;
		}

		void IOState::Set(size_t pin, IO_STATE state)
		{
			if (pin < 0 || pin >= m_states.size())
			{
				throw std::out_of_range("invalid pin");
			}

			m_states[pin] = state;
		}

		void IOState::Set(IO_STATE state)
		{
			if (m_states.size() != 1)
			{
				throw std::invalid_argument("Can not be used on bus");
			}

			m_states[0] = state;
		}

		IOState::IO_STATE & IOState::operator[](size_t pin)
		{
			if (pin < 0 || pin >= m_states.size())
			{
				throw std::out_of_range("invalid pin");
			}

			return m_states[pin];
		}
	}
}