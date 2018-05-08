#include "stdafx.h"
#include "IOConnection.h"
#include "IOPin.h"

namespace DigiLib
{
	namespace Core
	{
		IOConnection::IOConnection(IOPinPtr source, IOPinPtr target, bool inverted) : m_type(LINK), m_source(source), m_target(target), m_inverted(inverted)
		{
			if (source == nullptr || target == nullptr)
				throw std::invalid_argument("source or target is null");
			if (source->GetWidth() != target->GetWidth())
				throw std::invalid_argument("connection size mismatch");
		}

		IOConnection::IOConnection(IOState status, IOPinPtr target) : m_type(SETPIN), m_status(status), m_target(target)
		{
			if (target == nullptr)
				throw std::invalid_argument("source or target is null");
			if (status.GetWidth() != target->GetWidth())
				throw std::invalid_argument("connection size mismatch");
		}

		//TODO: We only check the pointers, which won't work well with IOPinSubset. 
		//      Data should be compared, tests added to validate

		bool IOConnection::operator<(const IOConnection & rhs) const noexcept
		{
			if (m_type != rhs.m_type)
			{
				return m_type < rhs.m_type;
			}

			if (m_type == SETPIN)
			{
				return m_target < rhs.m_target;
			}

			if (m_type == SETPIN || m_source == rhs.m_source)
			{
				return m_target < rhs.m_target;
			}

			return m_source < rhs.m_source;
		}

		void IOConnection::Run()
		{
			if (m_type == LINK)
			{
				m_target->Set(m_inverted? !m_source->Get():m_source->Get());
			}
			else
			{
				m_target->Set(m_status);
			}
		}

		std::string IOConnection::ToString() const
		{
			std::ostringstream os;
			if (m_type == LINK)
			{
				os << GetSource()->GetFullName() << " ->" << (m_inverted?" /":" ") << GetTarget()->GetFullName();
			}
			else
			{
				os << m_status << " -> " << GetTarget()->GetFullName();
			}
			return os.str();
		}
	}
}