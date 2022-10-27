#if !defined(UTILS_H)
#define UTILS_H

#include <math.h>

#include <QRegExp>
#include <QString>
#include <QVector>
#include <armadillo>
#include <iostream>
#include <vector>

std::ostream& operator<<(std::ostream& os, const QString& qstr);

const std::string str(const QString qstr);

QString qstr(const std::string str);

/**
 * @brief Get the real part of matrix from a cx_mat
 *
 * @param cx_mat
 * @return arma::mat
 */
arma::mat GetReal(const arma::cx_mat cx_mat);

#endif  // UTILS_H
