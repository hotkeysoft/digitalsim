#pragma once

class IOPin;

class IOConnection
{
public:
	IOConnection() : m_source(nullptr), m_target(nullptr) {}
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

