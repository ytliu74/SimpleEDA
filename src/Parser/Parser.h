/**
 * @file Parser.h
 * @author Yaotian Liu
 * @brief Header of Parser
 * @date 2022-10-11
 */

#if !defined(PARSER_H)
#define PARSER_H

#include <iostream>
#include <vector>
#include <QString>
#include <QStringList>


struct Vsrc {
    std::string name;
    double value;
    std::string node_1;
    std::string node_2;
};

struct Res {
    std::string name;
    double value;
    std::string node_1;
    std::string node_2;
};

struct Cap {
    std::string name;
    double value;
    std::string node_1;
    std::string node_2;
};

struct Ind {
    std::string name;
    double value;
    std::string node_1;
    std::string node_2;
};

struct scaledUnit {
    const QString unit;
    const double scaledValue;
};

class Parser {

public:
    Parser();
    ~Parser();
    void deviceParser(const QString line, const int lineNum);
    void commandParser(const QString line, const int lineNum);

private:
    std::vector<Vsrc> Vsrc_vec;
    std::vector<Res> Res_vec;
    std::vector<Cap> Cap_vec;
    std::vector<Ind> Ind_vec;

    double parseValue(const QString value_in_str);
};

#endif // PARSER_H
