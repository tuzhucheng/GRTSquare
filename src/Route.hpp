#ifndef ROUTE_HPP_
#define ROUTE_HPP_

#include <QObject>

class Route : public QObject
{
	Q_OBJECT

    // These are the properties that will be accessible by the datamodel in the view.
    Q_PROPERTY(QString routeName READ routeName WRITE setRouteName NOTIFY routeNameChanged FINAL)
    Q_PROPERTY(QString routeNumber READ routeNumber WRITE setRouteNumber NOTIFY routeNumberChanged FINAL)

public:
    Route(QObject *parent = 0);
    Route(const QString &routeName, const QString &routeNumber, QObject *parent = 0);

    QString routeName() const;
    QString routeNumber() const;

    void setRouteName(const QString &newName);
    void setRouteNumber(const QString &newNumber);

Q_SIGNALS:
	void routeNameChanged(const QString &newName);
	void routeNumberChanged(const QString &newNumber);

private:
	QString m_routeName;
	QString m_routeNumber;
};


#endif
