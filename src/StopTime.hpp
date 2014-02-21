#ifndef STOPTIME_HPP_
#define STOPTIME_HPP_

#include <QObject>

class StopTime : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString time READ time CONSTANT)
	Q_PROPERTY(int route READ route CONSTANT)

public:
	StopTime(QObject *parent = 0);
	StopTime(const int route, const QString &time, QObject *parent = 0);

	QString time() const;
	int route() const;

private:
	int m_route;
	QString m_time;
};


#endif /* STOPTIME_HPP_ */
