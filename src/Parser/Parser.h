/**
 * @file Parser.h
 * @author Yaotian Liu
 * @brief Header of Parser
 * @date 2022-10-11
 */

#if !defined(PARSER_H)
#define PARSER_H

#include <QRegularExpression>
#include <QString>
#include <QStringList>
#include <iostream>
#include <set>
#include <vector>

#include "ParserType.h"

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
    int getNodeNum() {
        updateNodeVec();
        return Node_vec.size();
    }

  private:
    std::vector<Vsrc> Vsrc_vec;
    std::vector<Res> Res_vec;
    std::vector<Cap> Cap_vec;
    std::vector<Ind> Ind_vec;
    std::vector<NodeName> Node_vec;

    // std::string print
    bool command_OP;
    bool command_END;

    analysis_T analysisType;
    AnalysisCommand analysisCommand;

    double parseValue(const QString value_in_str);
    void parseError(const std::string error_msg, const int lineNum);

    void printParser(const QStringList elements);
    std::vector<printVariable> printVariable_vec;

    void updateNodeVec();

    template <typename T>
    bool checkNameRepetition(std::vector<T> struct_vec, DeviceName name);
};

#endif  // PARSER_H
