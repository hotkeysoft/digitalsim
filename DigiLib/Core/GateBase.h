#pragma once

#include <set>
#include <map>
#include <memory>
#include "IOPin.h"
#include "IOConnection.h"
#pragma warning ( disable:26426 )

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
		typedef std::vector<std::unique_ptr<IOPin> > IOPinListType;
		typedef std::map<std::string, size_t> IOPinNameToIDMapType;

		typedef std::set<IOConnection> PinConnectionsType;
		typedef std::map<std::string, PinConnectionsType> ConnectedPinsType;
		//typedef std::vector<PinConnectionsType> ConnectedPinsType;
		typedef std::map<IOPin::IO_DIRECTION, std::map<IOPin::IO_DIRECTION, bool> > AllowedConnectionMapType;

		class DllExport GateBase
		{
		public:
			GateBase(const char* name);
			virtual ~GateBase() = default;
			GateBase(const GateBase&) = delete;
			GateBase& operator=(const GateBase&) = delete;
			GateBase(GateBase&&) = delete;
			GateBase& operator=(GateBase&&) = delete;

			std::string GetName() { return m_name; }
			std::string GetFullName();
			virtual void SetName(const char*);

			virtual GateBase* Clone(const char* name) = 0;

			virtual IOPin* GetPin(size_t pinID) { return m_ioPins[pinID].get(); }
			virtual IOPin* GetPin(const char* name);
			virtual IOPin* GetPin(const char* name, size_t pin);
			virtual IOPin* GetPin(const char* name, size_t low, size_t hi);

			virtual PinConnectionsType& GetConnectedFromPins(size_t pinID);
			virtual PinConnectionsType& GetConnectedToPins(size_t pinID);

			virtual PinConnectionsType& GetConnectedFromPins(const char* sourcePin);
			virtual PinConnectionsType& GetConnectedToPins(const char* sourcePin);
			
			virtual PinConnectionsType& GetConnectedFromPins(IOPin* sourcePin);
			virtual PinConnectionsType& GetConnectedToPins(IOPin* sourcePin);

			virtual ConnectedPinsType& GetConnectedFromPins() noexcept { return m_connectedFromPins; }
			virtual ConnectedPinsType& GetConnectedToPins() noexcept { return m_connectedToPins; }

			GateBase* GetParent() noexcept { return m_parent; }
			virtual void SetParent(GateBase* parent);

			virtual void ComputeState() noexcept(false) {};

			size_t GetInputCount() noexcept { return m_inputPinsNames.size(); }
			size_t GetOutputCount() noexcept { return m_outputPinsNames.size(); }

			const IOPinNameToIDMapType& GetInputPins() noexcept { return m_inputPinsNames; }
			const IOPinNameToIDMapType& GetOutputPins() noexcept { return m_outputPinsNames; }

			void ConnectPins(IOPin* source, IOPin* target);

		protected:
			virtual IOPin* AddInput(const char*  name, size_t width = 1);
			virtual IOPin* AddOutput(const char*  name, size_t width = 1, IOPin::IO_DIRECTION dir = IOPin::IO_DIRECTION::OUTPUT);

			std::string m_name;
			GateBase* m_parent;

			ConnectedPinsType m_connectedToPins;
			ConnectedPinsType m_connectedFromPins;

			IOPinNameToIDMapType m_inputPinsNames;
			IOPinNameToIDMapType m_outputPinsNames;

			IOPinListType m_ioPins;

			static AllowedConnectionMapType m_insideInsideMap;
			static AllowedConnectionMapType m_insideParentMap;
			static AllowedConnectionMapType m_parentInsideMap;
			static void InitAllowedConnectionMaps();

			bool IsValidPinName(const char* name);
			void ValidatePinName(const char* name);
			void ValidatePinWidth(size_t width);

			virtual bool IsValidGateName(const char* name);
			virtual void ValidateGateName(const char* name);
		};
	}
}