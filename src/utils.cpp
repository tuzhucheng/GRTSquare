#include "utils.hpp"
#include <QStringList>

/* Option 0 means do not display seconds, 1 means include seconds */
QString convertTo12HrFormat(const QString &time24HrFormat, const int option)
{
	QStringList parts = time24HrFormat.split(":");
	QString ap;
	int hour = parts.at(0).toInt();
	QString adjustedHour;
	if (hour > 12) {
		ap = "PM";
		adjustedHour = QString::number(hour-12);
	} else if (hour == 12) {
		ap = "PM";
		adjustedHour = parts.at(0);
	} else {
		ap = "AM";
		adjustedHour = parts.at(0);
	}

	switch(option)
	{
	case 0:
		return adjustedHour + ":" + parts.at(1) + " " + ap;
	case 1:
		return adjustedHour + ":" + parts.at(1) + ":" + parts.at(2) + " " + ap;
	default:
		return "";
	}
}
