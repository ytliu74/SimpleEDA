#if !defined(UTILS_H)
#define UTILS_H

#include <math.h>

#include <QRegExp>
#include <QString>
#include <iostream>
#include <vector>

std::ostream& operator<<(std::ostream& os, const QString& qstr);

const std::string str(const QString qstr);

template <typename T>
int findFirstElement(const std::vector<T> vec, T element);

#endif  // UTILS_H
