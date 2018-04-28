#pragma once

#include <set>
#include <map>
#include "IOPin.h"
#include "IOConnection.h"
#include "GateBase.h"

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
		typedef std::map<std::string, GateBase*> GateMapType;

		class DllExport CompositeGate : public GateBase
		{
		public:
			CompositeGate(const char* name);
			virtual void SetName(const char *name);

			virtual GateBase* Clone(const char* name);

			using GateBase::AddInput;
			using GateBase::AddOutput;

			virtual void AddGate(const char* name, GateBase* gate);
			virtual size_t GetGateCount() { return m_internalGates.size(); }
			virtual GateBase* GetGate(const char*  name);
			virtual GateMapType& GetInternalGates() noexcept { return m_internalGates; }

		protected:
			GateMapType m_internalGates;

			static void InternalCloneOutputs(CompositeGate* source, CompositeGate* clone);
			static void InternalCloneInputs(CompositeGate* source, CompositeGate* clone);
			static void InternalCloneGates(CompositeGate* source, CompositeGate* clone);
			static void InternalCloneLinks(GateBase* source, GateBase* clone);
			static void InternalCloneInnerLinks(GateBase* source, GateBase* clone);

			void ValidateGateName(const char* name, bool checkDuplicate = true);
		};
	}
}
