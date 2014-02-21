#ifndef ROUTE_HPP_
#define ROUTE_HPP_

#include <QObject>

class Route : public QObject
{
	Q_OBJECT

    // These are the properties that will be accessible by the datamodel in the view.
    Q_PROPERTY(QString routeName READ routeName CONSTANT)
    Q_PROPERTY(int routeNumber READ routeNumber CONSTANT)

public:
    Route(QObject *parent = 0);
    Route(const QString &routeName, const int routeNumber, QObject *parent = 0);

    QString routeName() const;
    int routeNumber() const;

private:
	QString m_routeName;
	int m_routeNumber;
};


#endif
