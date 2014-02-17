#include "Stop.hpp"

Stop::Stop(QObject *parent) : QObject(parent)
{
}

Stop::Stop(const QString &stopNumber, const QString &stopName, QObject *parent)
	: QObject(parent)
	, m_stopNumber(stopNumber)
	, m_stopName(stopName)
{
}

QString Stop::stopNumber() const
{
	return m_stopNumber;
}

QString Stop::stopName() const
{
	return m_stopName;
}



