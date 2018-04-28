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
			virtual ~IOConnection() {}

			IOPin* GetSource() { return m_source; }
			IOPin* GetTarget() { return m_target; }

			bool operator==(const IOConnection& rhs) const;
			bool operator<(const IOConnection& rhs) const;

		protected:
			IOPin * m_source;
			IOPin * m_target;
		};
	}
}