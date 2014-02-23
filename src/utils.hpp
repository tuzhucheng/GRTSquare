#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <QObject>

// Although QTime has parsing capabilities, there were some issues with parsing certain times
QString convertTo12HrFormat(const QString &time24HrFormat, const int option);

#endif /* UTILS_HPP_ */
