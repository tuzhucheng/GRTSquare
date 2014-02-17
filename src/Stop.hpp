#ifndef STOP_HPP_
#define STOP_HPP_

#include <QObject>

class Stop : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString stopNumber READ stopNumber CONSTANT)
	Q_PROPERTY(QString stopName READ stopName CONSTANT)

public:
	Stop(QObject *parent = 0);
	Stop(const QString &stopNumber, const QString &stopName, QObject *parent = 0);

	QString stopNumber() const;
	QString stopName() const;

private:
	QString m_stopNumber;
	QString m_stopName;
};

#endif
