#pragma once
#include <set>

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
		class GateBase;

		class DllExport IOPin
		{
		public:
			enum IO_STATE { LOW = 0, HI = 1, UNDEF = -1, HI_Z = -100 };
			enum IO_DIRECTION { INPUT, OUTPUT, OUTPUT_HI_Z };

			IOPin(GateBase *parentGate, const char * name, IO_DIRECTION direction);

			std::string GetName() { return m_name; }
			std::string GetFullName();
			GateBase* GetParent() noexcept { return m_parentGate; }
			IO_DIRECTION GetDirection() noexcept { return m_direction; }

			virtual IO_STATE Get() noexcept { return m_state; };
			virtual void Set(IO_STATE state);

			virtual void ConnectTo(IOPin*);

		protected:
			GateBase * m_parentGate;

			std::string m_name;
			IO_DIRECTION m_direction;

			IO_STATE m_state;
		};
	}
}