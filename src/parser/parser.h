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
#include <QTextEdit>
#include <iostream>
#include <set>
#include <vector>

#include "parser_type.h"

class Parser {
  public:
    Parser();
    Parser(QTextEdit* output);
    ~Parser();
    void DeviceParser(const QString line, const int lineNum);
    void CommandParser(const QString line, const int lineNum);

    auto GetResistor() { return res_vec; }
    auto GetCapacitor() { return cap_vec; }
    auto GetInductor() { return ind_vec; }
    auto GetVsrc() { return vsrc_vec; }
    auto GetIsrc() { return isrc_vec; }
    auto GetVCCS() { return vccs_vec; }
    auto GetVCVS() { return vcvs_vec; }
    auto GetNode() {
        UpdateNodeVec();
        return node_vec;
    }

    bool ParserFinalCheck();

  private:
    QTextEdit* output;

    std::vector<Vsrc> vsrc_vec;
    std::vector<Isrc> isrc_vec;
    std::vector<VCCS> vccs_vec;
    std::vector<VCVS> vcvs_vec;
    std::vector<Res> res_vec;
    std::vector<Cap> cap_vec;
    std::vector<Ind> ind_vec;
    std::vector<NodeName> node_vec;

    NodeName ReadNodeName(const QString qstrName);

    // std::string print
    bool command_op;
    bool command_end;

    AnalysisT analysis_type;
    AnalysisCommand analysis_command;

    double ParseValue(const QString value_in_str);
    void ParseError(const QString error_msg, const int lineNum);

    void PrintCommandParser(const QStringList elements);

    void UpdateNodeVec();

    template <typename T>
    bool CheckNameRepetition(std::vector<T> struct_vec, DeviceName name);

    bool CheckGndNode();
};

#endif  // PARSER_H
