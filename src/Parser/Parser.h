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
#include <QRegularExpression>


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

enum analysis_T { NONE, DC, AC, TRAN, NOISE, DISTO };
const std::string analysis_T_lookup[] = { "NONE", "DC", "AC", "TRAN", "NOISE", "DISTO" };

enum analysisVariable_T { MAG, REAL, IMAGINE, PHASE, DB };
enum print_T { I, V };

struct printVariable {
    print_T printType;
    analysisVariable_T analysisVariableType;
    std::string node_1;
    std::string node_2;
};


class Parser {

public:
    Parser();
    ~Parser();
    void deviceParser(const QString line, const int lineNum);
    void commandParser(const QString line, const int lineNum);

    int getDeviceNum() { return (Res_vec.size() + Ind_vec.size() + Cap_vec.size()); }
    int getResistorNum() { return Res_vec.size(); }
    int getCapacitorNum() { return Ind_vec.size(); }
    int getInductorNum() { return Cap_vec.size(); }
    int getVsrcNum() { return Vsrc_vec.size(); }
    int getNodeNum();

private:
    std::vector<Vsrc> Vsrc_vec;
    std::vector<Res> Res_vec;
    std::vector<Cap> Cap_vec;
    std::vector<Ind> Ind_vec;

    std::vector<std::string> Vsrc_name_vec;
    std::vector<std::string> Res_name_vec;
    std::vector<std::string> Cap_name_vec;
    std::vector<std::string> Ind_name_vec;

    // std::string print
    bool command_OP;
    bool command_END;

    analysis_T analysisType;

    double parseValue(const QString value_in_str);
    void parseError(const std::string error_msg, const int lineNum);

    void printParser(const QStringList elements);
    std::vector<printVariable> printVariable_vec;
};

#endif // PARSER_H
