#include "Route.hpp"

Route::Route(QObject *parent)
    : QObject(parent)
{
}

Route::Route(const QString &routeId, const QString &routeLongName, const QString &routeNumber, QObject *parent)
    : QObject(parent)
    , m_routeId(routeId)
    , m_routeLongName(routeLongName)
    , m_routeNumber(routeNumber)
{
}

QString Route::routeID() const
{
    return m_routeId;
}

QString Route::routeLongName() const
{
    return m_routeLongName;
}

QString Route::routeNumber() const
{
    return m_routeNumber;
}

void Route::setRouteID(const QString &newId)
{
    if (newId != m_routeId) {
    	m_routeId = newId;
        emit routeIDChanged(newId);
    }
}

void Route::setRouteLongName(const QString &newLongName)
{
    if (newLongName != m_routeLongName) {
    	m_routeLongName = newLongName;
        emit routeLongNameChanged(newLongName);
    }
}

void Route::setRouteNumber(const QString &newNumber)
{
    if (newNumber != m_routeNumber) {
    	m_routeNumber = newNumber;
        emit routeIDChanged(newNumber);
    }
}




