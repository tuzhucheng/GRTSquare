#include "StopTime.hpp"

StopTime::StopTime(QObject *parent) : QObject(parent)
{
}

StopTime::StopTime(const QString &route, const QString &time, QObject *parent)
	: QObject(parent)
	, m_route(route)
	, m_time(time)
{
}

QString StopTime::time() const
{
	return m_time;
}

QString StopTime::route() const
{
	return m_route;
}


