#pragma once

#include "IOPin.h"

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
		class IOConnection
		{
		public:
			IOConnection(IOPinPtr source, IOPinPtr target);

			IOPinPtr GetSource() const noexcept { return m_source; }
			IOPinPtr GetTarget() const noexcept { return m_target; }

			bool operator<(const IOConnection& rhs) const noexcept;

		protected:
			IOPinPtr m_source;
			IOPinPtr m_target;
		};
	}
}