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

    auto getResistor() { return Res_vec; }
    auto getCapacitor() { return Cap_vec; }
    auto getInductor() { return Ind_vec; }
    auto getVsrc() { return Vsrc_vec; }
    auto getNode() {
        updateNodeVec();
        return Node_vec;
    }

    bool parserFinalCheck();

  private:
    std::vector<Vsrc> Vsrc_vec;
    std::vector<Res> Res_vec;
    std::vector<Cap> Cap_vec;
    std::vector<Ind> Ind_vec;
    std::vector<NodeName> Node_vec;

    NodeName readNodeName(const QString qstrName);

    // std::string print
    bool command_OP;
    bool command_END;

    analysis_T analysisType;
    AnalysisCommand analysisCommand;

    double parseValue(const QString value_in_str);
    void parseError(const std::string error_msg, const int lineNum);

    void printParser(const QStringList elements);

    void updateNodeVec();

    template <typename T>
    bool checkNameRepetition(std::vector<T> struct_vec, DeviceName name);

    bool checkGNDNode();
};

#endif  // PARSER_H
