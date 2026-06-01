#pragma once

#include "clex.h"
#include "../cnodelist.h"
#include "cparserbase.h"

class CParserFile : public CParserBase {
private:
    CLex l;
    size_t loop_level{};
    CNodePtr last_switch;
    CVariablePtr current_function;
    std::map<std::string, int> pragma_once;

    CNodePtr ParseLine(bool *out_break, bool global);
    void ParseFunction(CNodePtr &node);
    void ParseAttributes(CNode &n);
    bool ParseTypeWoPointers(CType *out_type, bool can_break_inital = false);
    void ParseTypePointers(CType &out_type);
    void ParseTypeNameArray(CConstType base_type, CString &out_name, CType &out_type);
    void ParsePointerFlags(CPointer &addr);
    void ParseFunctionTypeArgs(CErrorPosition &e, CType &return_type, std::vector<CPointer> *function_pointer,
                               CType &out_type);
    CNodePtr ParseInitBlock(CType &type, bool can_change_size);
    CNodePtr ParseAsm(CErrorPosition &e);
    CNodePtr ParseFunctionBody();
    CNodePtr ParseFunctionBody2();
    CNodePtr ParseExpressionValue();
    CNodePtr ParseExpressionL();
    CNodePtr ParseExpressionM(CNodePtr result);
    CNodePtr ParseExpressionK();
    CNodePtr ParseExpressionJ();
    CNodePtr ParseExpressionI();
    CNodePtr ParseExpressionH();
    CNodePtr ParseExpressionG();
    CNodePtr ParseExpressionF();
    CNodePtr ParseExpressionE();
    CNodePtr ParseExpressionD();
    CNodePtr ParseExpressionC();
    CNodePtr ParseExpressionB();
    CNodePtr ParseExpressionA();
    CNodePtr ParseExpression();
    CNodePtr ParseExpressionComma();
    void ParseStruct(CStruct &struct_object);
    bool ParseType(CType *out_type, bool can_empty = false);
    uint64_t ParseUint64();
    int64_t ParseInt64();
    void ParseEnum();
    void ParseTypeWoPointersStruct(CType *out_type, bool is_union, CErrorPosition &e);
    CNodePtr ParseExpressionCall(CNodePtr &f, CErrorPosition &e);
    CBaseType ParseBaseType();
    CNodePtr ParseExpressionStructItem(CMonoOperatorCode mo, CNodePtr &a, CErrorPosition &e);
    CNodePtr ParseExpressionArrayElement(CNodePtr &a, CErrorPosition &e);
    void ParseGccAttributes(CAlignAttribute *a);
    void IgnoreInsideBrackets(size_t level);

    // Preprocessor

    void Preprocessor();
    void PreprocessorInclude();
    void PreprocessorPragmaCodepage();
    void PreprocessorPragmaOnce();
    void PreprocessorDefine();
    void PreprocessorIfdef();
    void PreprocessorIf();
    int64_t PreprocessorIf0();
    int64_t PreprocessorIfA();
    int64_t PreprocessorIfB();
    int64_t PreprocessorIfC();
    int64_t PreprocessorIfD();
    int64_t PreprocessorIfE();
    int64_t PreprocessorIfF();
    int64_t PreprocessorIfG();
    int64_t PreprocessorIfH();
    int64_t PreprocessorIfI();
    int64_t PreprocessorIfJ();
    int64_t PreprocessorIf2();
    void PreprocessorIfndef();
    void PreprocessorUndef();
    void PreprocessorEndif();
    void PreprocessorElse();
    bool PreprocessorIfdefCheck(CString id);

public:
    CParserFile(CParser &p) : CParserBase(p) {
    }
    void Parse(CNodeList &node_list, CString file_name);
};
