#pragma once

#include <vector>
#include <array>
#include <string>
#include <ostream>
#include <initializer_list>
#include "Common.h"

namespace DigiLib
{
	namespace Core
	{
		class DllExport IOState
		{
		public:
			enum IO_STATE : int8_t { LOW = 0, HI = 1, UNDEF = -1, HI_Z = -100 };

			IOState(IO_STATE state = UNDEF, size_t width = 1);
			IOState(std::initializer_list<IO_STATE> init_list);
			IOState(const IOState& rhs) : m_width(rhs.m_width), m_states(rhs.m_states) {};
			IOState operator=(const IOState&);
			IOState operator=(IO_STATE);

			IOState operator!() const;
			IOState operator|(const IOState& rhs) const;

			size_t GetWidth() const { return m_width; }

			bool operator==(const IOState& rhs) const;
			bool operator!=(const IOState& rhs) const;
			bool operator==(IO_STATE rhs) const;
			bool operator!=(IO_STATE rhs) const;

			IO_STATE operator [](size_t pin) const;
			IO_STATE & operator [](size_t pin);

			bool IsUndef() const;

			IO_STATE Get(size_t pin = 0) const;
			IOState Get(size_t low, size_t hi) const;

			void Set(std::vector<IO_STATE> pins);
			void Set(size_t pin, IO_STATE);
			void Set(IO_STATE);

			std::string ToString() const;
			static std::string ToString(IO_STATE);
			
			uint8_t ToInt8() const;
			uint16_t ToInt16() const;

			static IOState FromInt(int value, size_t bitCount);

			static IOState Random(size_t bitCount = 1);

		protected:
			IOState(size_t width) : m_width(width) {}

			size_t m_width;
			std::array<IO_STATE, MAX_PINS> m_states;
		};

		inline bool operator==(IOState::IO_STATE lhs, const IOState& rhs) { return rhs == lhs; }
		inline bool operator!=(IOState::IO_STATE lhs, const IOState& rhs) { return rhs != lhs; }

		inline std::ostream & operator << (std::ostream & os, const IOState& state)
		{
			os << state.ToString();
			return os;
		}
	}
}