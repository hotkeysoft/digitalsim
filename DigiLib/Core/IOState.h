#pragma once

#include <vector>
#include <array>
#include <string>
#include <ostream>
#include <initializer_list>

#ifdef  DIGILIB_EXPORTS 
/*Enabled as "export" while compiling the dll project*/
#define DllExport __declspec(dllexport)  
#else
/*Enabled as "import" in the Client side for using already created dll file*/
#define DllExport __declspec(dllimport)  
#endif

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
			IOState(const IOState& rhs) : m_states(rhs.m_states) {};
			IOState operator=(const IOState&);
			IOState operator=(IO_STATE);

			size_t GetWidth() const { return m_states.size(); }

			bool operator==(const IOState& rhs) const;
			bool operator!=(const IOState& rhs) const;
			bool operator==(IO_STATE rhs) const;
			bool operator!=(IO_STATE rhs) const;

			IO_STATE & operator [](size_t pin);

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

		private:
			std::vector<IO_STATE> m_states;
			//std::array<IO_STATE, MAX_PINS> m_states2;
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