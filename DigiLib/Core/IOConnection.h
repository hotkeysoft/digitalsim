#pragma once
#include "Common.h"
#include "IOPin.h"
#include "IOState.h"

namespace DigiLib
{
	namespace Core
	{
		class DllExport IOConnection
		{
		public:
			enum Type { LINK, SETPIN };

			IOConnection(IOPinPtr source, IOPinPtr target, bool inverted = false);
			IOConnection(IOState source, IOPinPtr target);

			IOState GetStatus() const noexcept { return m_status;  }
			IOPinPtr GetSource() const noexcept { return m_source; }
			IOPinPtr GetTarget() const noexcept { return m_target; }

			bool operator<(const IOConnection& rhs) const noexcept;

			void Run();

			std::string ToString() const;

		protected:
			Type m_type;
			bool m_inverted;
			IOState m_status;
			IOPinPtr m_source;
			IOPinPtr m_target;
		};

		inline std::ostream & operator << (std::ostream & os, const IOConnection& link)
		{
			os << link.ToString();
			return os;
		}
	}
}