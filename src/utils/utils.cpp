#include "utils.h"

std::ostream& operator<<(std::ostream& os, const QString& qstr) {
    os << qstr.toStdString();
    return os;
}

const std::string str(const QString qstr) { return qstr.toStdString(); }

QString qstr(const std::string str) { return QString::fromStdString(str); }

arma::mat GetReal(const arma::cx_mat cx_mat) {
    int n_rows = cx_mat.n_rows;
    int n_cols = cx_mat.n_cols;

    arma::mat real(n_rows, n_cols);

    for (int r = 0; r < cx_mat.n_rows; r++) {
        for (int c = 0; c < cx_mat.n_cols; c++) {
            real(r, c) = cx_mat(r, c).real();
        }
    }

    return real;
}