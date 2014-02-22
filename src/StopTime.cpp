#include "StopTime.hpp"
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
}

QString StopTime::time() const
{
	// Although QTime has parsing capabilities, there were some issues with parsing certain times

	QStringList parts = m_time.split(":");
	QString ap;
	int hour = parts.at(0).toInt();
	QString adjustedHour;
	if (hour > 12) {
		ap = "PM";
		adjustedHour = QString::number(hour-12);
	} else {
		ap = "AM";
		adjustedHour = parts.at(0);
	}

	return adjustedHour + ":" + parts.at(1) + " " + ap;
}

int StopTime::route() const
{
	return m_route;
}

QString StopTime::headSign() const
{
	return m_headSign;
}


