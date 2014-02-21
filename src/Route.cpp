#include "Route.hpp"

Route::Route(QObject *parent)
    : QObject(parent)
{
}

Route::Route(const QString &routeName, const int routeNumber, QObject *parent)
    : QObject(parent)
    , m_routeName(routeName)
    , m_routeNumber(routeNumber)
{
}

QString Route::routeName() const
{
    return m_routeName;
}

int Route::routeNumber() const
{
    return m_routeNumber;
}




