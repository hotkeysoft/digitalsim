#pragma once

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
		class IOPin;

		class DllExport IOConnection
		{
		public:
			IOConnection(IOPin* source, IOPin* target);

			IOPin* GetSource() noexcept { return m_source; }
			IOPin* GetTarget() noexcept { return m_target; }

			bool operator==(const IOConnection& rhs) const noexcept;
			bool operator<(const IOConnection& rhs) const noexcept;

		protected:
			IOPin * m_source;
			IOPin * m_target;
		};
	}
}