/**
 * @file ParserType.h
 * @author Yaotian Liu
 * @brief Header for type defines in Parser
 * @date 2022-10-13
 */

#if !defined(PARSERTYPE_H)
#define PARSERTYPE_H

#include <QString>
#include <iostream>

typedef std::string DeviceName;
typedef std::string NodeName;

struct Vsrc {
    DeviceName name;
    double value;
    NodeName node_1;
    NodeName node_2;
};

struct Res {
    DeviceName name;
    double value;
    NodeName node_1;
    NodeName node_2;
};

struct Cap {
    DeviceName name;
    double value;
    NodeName node_1;
    NodeName node_2;
};

struct Ind {
    DeviceName name;
    double value;
    NodeName node_1;
    NodeName node_2;
};

struct ScaledUnit {
    const QString unit;
    const double scaledValue;
};

struct AnalysisCommand {
    DeviceName Vsrc_name;
    double start;
    double end;
    double step;
};

enum AnalysisT { NONE, DC, AC, TRAN, NOISE, DISTO };
const std::string AnalysisT_lookup[] = {"NONE", "DC", "AC", "TRAN", "NOISE", "DISTO"};

enum AnalysisVariableT { MAG, REAL, IMAGINE, PHASE, DB };
enum PrintT { I, V };

struct PrintVariable {
    PrintT printType;
    AnalysisVariableT analysisVariableType;
    NodeName node_1;
    NodeName node_2;
};

#endif  // PARSERTYPE_H
