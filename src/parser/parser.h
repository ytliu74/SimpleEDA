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

    auto GetCircuit() {
        UpdateNodeVec();
        return circuit;
    }

    auto GetAnalysisType() { return analysis_type; }
    auto GetDcAnalysis() { return dc_analysis; }
    auto GetAcAnalysis() { return ac_analysis; }
    auto GetTranAnalysis() { return tran_analysis; }
    auto GetPrintVariables() { return print_variable_vec; }

    bool ParserFinalCheck();

  private:
    QTextEdit* output;

    Circuit circuit;

    NodeName ReadNodeName(const QString qstrName);

    // std::string print
    bool command_op;
    bool command_end;

    AnalysisType analysis_type;
    DcAnalysis dc_analysis;
    AcAnalysis ac_analysis;
    TranAnalysis tran_analysis;

    std::vector<PrintVariable> print_variable_vec;
    PrintType print_type;

    double ParseValue(const QString value_in_str);
    void ParseError(const QString error_msg, const QString name, const int lineNum);

    void PrintCommandParser(const QStringList elements);

    void UpdateNodeVec();

    template <typename T>
    bool CheckNameRepetition(std::vector<T> struct_vec, DeviceName name);

    bool CheckGndNode();
};

#endif  // PARSER_H
