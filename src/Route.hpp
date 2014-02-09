#ifndef ROUTE_HPP_
#define ROUTE_HPP_

#include <QObject>

class Route : public QObject
{
	Q_OBJECT

    // These are the properties that will be accessible by the datamodel in the view.
    Q_PROPERTY(QString routeID READ routeID WRITE setRouteID NOTIFY routeIDChanged FINAL)
    Q_PROPERTY(QString routeLongName READ routeLongName WRITE setRouteLongName NOTIFY routeLongNameChanged FINAL)
    Q_PROPERTY(QString routeNumber READ routeNumber WRITE setRouteNumber NOTIFY routeNumberChanged FINAL)

public:
    Route(QObject *parent = 0);
    Route(const QString &routeId, const QString &routeLongName, const QString &routeNumber, QObject *parent = 0);

    QString routeID() const;
    QString routeLongName() const;
    QString routeNumber() const;

    void setRouteID(const QString &newId);
    void setRouteLongName(const QString &newLongName);
    void setRouteNumber(const QString &newNumber);

Q_SIGNALS:
	void routeIDChanged(const QString &newId);
	void routeLongNameChanged(const QString &newLongName);
	void routeNumberChanged(const QString &newNumber);

private:
	QString m_routeId;
	QString m_routeLongName;
	QString m_routeNumber;
};


#endif
