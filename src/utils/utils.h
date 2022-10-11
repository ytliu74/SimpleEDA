#if !defined(UTILS_H)
#define UTILS_H

#include <math.h>
#include <iostream>
#include <vector>
#include <QString>
#include <QRegExp>

void printQString(const std::string comment, const QString qstring);

struct scaledUnit {
    const QString unit;
    const double scaledValue;
};

double parseValue(const QString value_in_str);


#endif // UTILS_H
