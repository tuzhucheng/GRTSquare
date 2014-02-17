#include "Route.hpp"

Route::Route(QObject *parent)
    : QObject(parent)
{
}

Route::Route(const QString &routeName, const QString &routeNumber, QObject *parent)
    : QObject(parent)
    , m_routeName(routeName)
    , m_routeNumber(routeNumber)
{
}

QString Route::routeName() const
{
    return m_routeName;
}

QString Route::routeNumber() const
{
    return m_routeNumber;
}

void Route::setRouteName(const QString &newName)
{
    if (newName != m_routeName) {
    	m_routeName = newName;
        emit routeNameChanged(newName);
    }
}

void Route::setRouteNumber(const QString &newNumber)
{
    if (newNumber != m_routeNumber) {
    	m_routeNumber = newNumber;
        emit routeNumberChanged(newNumber);
    }
}




