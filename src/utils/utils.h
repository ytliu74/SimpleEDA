#if !defined(UTILS_H)
#define UTILS_H

#include <math.h>

#include <QRegExp>
#include <QString>
#include <iostream>
#include <vector>

std::ostream& operator<<(std::ostream& os, const QString& qstr);

const std::string str(const QString qstr);

QString qstr(const std::string str);

#endif  // UTILS_H
