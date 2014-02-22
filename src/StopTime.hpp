#ifndef STOPTIME_HPP_
#define STOPTIME_HPP_

#include <QObject>

class StopTime : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString time READ time CONSTANT)
	Q_PROPERTY(int route READ route CONSTANT)
	Q_PROPERTY(QString headSign READ headSign CONSTANT)

public:
	StopTime(QObject *parent = 0);
	StopTime(const int route, const QString &time, const QString &headSign, QObject *parent = 0);

	QString time() const;
	int route() const;
	QString headSign() const;

private:
	int m_route;
	QString m_time;
	QString m_headSign;
};


#endif /* STOPTIME_HPP_ */
