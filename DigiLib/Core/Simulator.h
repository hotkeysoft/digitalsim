#pragma once

#include <set>
#include <map>
#include "Common.h"
#include "IOPin.h"
#include "IOConnection.h"
#include "CompositeGate.h"

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
		typedef std::map<std::string, GatePtr > GateMapType;

		typedef std::multimap<size_t, IOConnection> EventMapType;

		class DllExport Simulator : public CompositeGate
		{
		public:
			static SimulatorPtr Create(const char* name);
			GatePtr Clone(const char* name, bool deep = true) override;

			virtual void Reset();
			virtual size_t Tick();
			virtual size_t Run(size_t maxTicks);
			
			void Pulse(IOPinPtr pin, size_t begin, size_t end, bool negative = false);
			void Pulse(IOPinPtr pin, IOState state, size_t begin, size_t end);
			void Clock(IOPinPtr pin, size_t hiWidth, size_t lowWidth, size_t begin, size_t end, bool negative = false);

			using GateBase::AddInput;
			using GateBase::AddOutput;

			virtual void AddGate(const char* name, GatePtr gate);

			size_t GetTick() noexcept { return m_currentTick; }
			virtual EventMapType& GetEventQueue() noexcept { return m_eventQueue; }
			virtual void PostEvent(const IOConnection&);
			virtual void PostEvent(const IOConnection&, size_t at);
			virtual void PostEventRelative(const IOConnection&, size_t delay);

		protected:
			Simulator(const char* name);
			SimulatorPtr thisSimulator() { return std::dynamic_pointer_cast<Simulator>(shared_from_this()); }

			size_t m_currentTick;
			EventMapType m_eventQueue;			

			struct shared_enabler;
		};
	}
}
