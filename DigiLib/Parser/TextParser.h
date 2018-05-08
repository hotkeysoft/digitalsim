#pragma once
#include <vector>
#include <sstream>
#include "Core\Common.h"
#include "Core\IOState.h"
#include "Core\IOPin.h"

#ifdef  DIGILIB_EXPORTS 
/*Enabled as "export" while compiling the dll project*/
#define DllExport __declspec(dllexport)  
#else
/*Enabled as "import" in the Client side for using already created dll file*/
#define DllExport __declspec(dllimport)  
#endif

namespace DigiLib {
	namespace Parser {
		class DllExport TextParser
		{
		public:
			TextParser() {};

			void Attach(Core::GatePtr gate) 
			{
				m_gate = gate;
			}

			Core::GatePtr Get() { return m_gate;  }

			void ParseConnection(const char *);
			
		private:
			Core::GatePtr m_gate;
		};
	}
}