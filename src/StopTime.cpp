#include "StopTime.hpp"
#include "utils.hpp"
#include <QDebug>
#include <QStringList>

StopTime::StopTime(QObject *parent) : QObject(parent)
{
}

StopTime::StopTime(const int route, const QString &time, const QString &headSign, QObject *parent)
	: QObject(parent)
	, m_route(route)
	, m_time(time)
	, m_headSign(headSign)
{
	m_timeFormatted =  convertTo12HrFormat(m_time, 0);
}

QString StopTime::time() const
{
	return m_time;
}

QString StopTime::timeFormatted() const
{
	return m_timeFormatted;
}

int StopTime::route() const
{
	return m_route;
}

QString StopTime::headSign() const
{
	return m_headSign;
}


