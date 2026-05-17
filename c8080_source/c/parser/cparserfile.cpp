/*
 * c8080 compiler
 * Copyright (c) 2025 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "cparserfile.h"
#include "cparseasmequs.h"
#include "../consts.h"
#include "../tools/ccalcconst.h"
#include "../tools/convert.h"
#include "../tools/cthrow.h"
#include "../tools/makeoperator.h"
#include "../tools/cprepareargs.h"

void CParserFile::Parse(CNodeList &node_list, CString file_name) {
    l.save_string = [this](const char *data, size_t size) { return programm.SaveString(data, size); };
    l.preprocessor = [this]() { Preprocessor(); };
    l.on_error = [this](CErrorPosition &l, CString text) { programm.Error(l, text); };

    const char *name = "";
    const char *contents = cparser.LoadFile(file_name, &name);
    l.Open(contents, name);
    l.AddMacro("__C8080_COMPILER", "", 0);
    for (auto &i : cparser.default_defines)
        l.Include(i.c_str(), "arguments");
    l.NextToken();

    while (!l.IfToken(CT_EOF))
        node_list.PushBack(ParseLine(nullptr, true));
}

uint64_t CParserFile::ParseUint64() {
    static const CType uint64_ctype{CBT_UNSIGNED_LONG_LONG};
    CNodePtr value = Convert(uint64_ctype, ParseExpression());
    CCalcConst(value, true);
    if (value->type != CNT_NUMBER) {
        programm.Error(value->e, "Is not number");
        return 0;
    }
    assert(!value->ctype.IsPointer() && value->ctype.base_type == CBT_UNSIGNED_LONG_LONG);
    return value->number.u;
}

int64_t CParserFile::ParseInt64() {
    static const CType int64_ctype{CBT_LONG_LONG};
    CNodePtr value = Convert(int64_ctype, ParseExpression());
    CCalcConst(value, true);
    if (value->type != CNT_NUMBER) {
        programm.Error(value->e, "Is not number");
        return 0;
    }
    assert(!value->ctype.IsPointer() && value->ctype.base_type == CBT_LONG_LONG);
    return value->number.i;
}

void CParserFile::ParsePointerFlags(CPointer &pointer) {
    for (;;) {
        if (!pointer.flag_restrict && l.IfToken("__restrict")) {
            pointer.flag_restrict = true;
            continue;
        }
        if (!pointer.flag_const && l.IfToken("const")) {
            pointer.flag_const = true;
            continue;
        }
        if (!pointer.flag_volatile && l.IfToken("volatile")) {
            pointer.flag_volatile = true;
            continue;
        }
        break;
    }
}

void CParserFile::IgnoreInsideBrackets(size_t level) {
    for (;;) {
        if (l.IfToken("(")) {
            level++;
            if (level == 0)  // Incredible overflow
                l.ThrowSyntaxError();
            continue;
        }
        if (l.IfToken(")")) {
            level--;
            if (level == 0)
                break;
            continue;
        }
        l.NextToken();
    }
}

void CParserFile::ParseGccAttributes(CAlignAttribute *a) {
    for (;;) {
        if (l.IfToken("__asm__")) {  // gcc compatibility
            if (l.WantToken("("))
                IgnoreInsideBrackets(1);
            continue;
        }

        if (l.IfToken("__attribute__")) {
            l.NeedToken("(");
            l.NeedToken("(");
            if (a && l.IfToken("aligned")) {
                if (a->exists)
                    l.Error(std::string("already defined"));
                l.NeedToken("(");
                a->exists = true;
                a->value = ParseUint64();
                l.NeedToken(")");
                l.NeedToken(")");
                l.NeedToken(")");
                continue;
            }
            IgnoreInsideBrackets(2);  // gcc compatibility
            continue;
        }

        break;
    }
}

CNodePtr CParserFile::ParseAsm(CErrorPosition &e) {
    std::string str;
    if (l.IfToken("(")) {
        l.NeedString2(str);
        l.NeedToken(")");
        l.NeedToken(";");
    } else if (l.token_data[0] == '{') {
        l.ReadRaw(str, '}', '}', '{');
        l.NextToken();
    } else {
        l.ThrowSyntaxError();
    }

    CParseAsmEqus(str, programm.asm_names);

    return CNODE({CNT_ASM, text : str, e : e});
}

void CParserFile::ParseEnum() {
    // TODO: enum automatically increases the size of the type
    int64_t value = 0;
    for (;;) {
        if (l.IfToken("}"))
            break;

        CErrorPosition e(l);
        std::string name;
        l.NeedIdent(name);
        if (FindVariableCurrentScope(name) != nullptr)
            programm.Error(e, std::string("redefinition of '") + name + "'");  // gcc
        if (l.IfToken("="))
            value = ParseInt64();

        if (value < C_INT_MIN || value > C_INT_MAX)
            programm.Error(e, "overflow in enumeration values");  // gcc

        CVariablePtr v = std::make_shared<CVariable>();
        v->type.base_type = CBT_INT;
        v->type.flag_const = true;
        v->name = name;
        v->e = e;
        v->body = CNODE({CNT_NUMBER, ctype : CType{CBT_INT}});
        v->body->number.i = value;

        scope_variables.push_back(v);

        if (l.IfToken("}"))
            break;
        l.NeedToken(",");
        value++;
    }
}

void CParserFile::ParseAttributes(CNode &n) {
    assert(n.variable);
    for (;;) {
        CErrorPosition e(l);
        if (l.IfToken("__address")) {
            CAddressAttribute a;

            if (l.WantToken("(")) {
                std::string str;
                if (l.IfString2(str)) {
                    if (str.empty())
                        l.Error("__address can't be empty", e);
                    else
                        a.value = str;
                } else {
                    a.value = ParseUint64();
                }
                l.CloseToken(")", ")");
            }

            if (!n.extern_flag)
                programm.Error(e, "__address() can only be used with 'extern'");

            if (n.variable->address_attribute.exists() && n.variable->address_attribute != a)
                programm.Error(e, "previous declaration is different");

            n.variable->address_attribute = a;
            continue;
        }
        if (l.IfToken("__link")) {
            CLinkAttribute a;

            if (l.WantToken("(")) {
                l.NeedString2(a.base_name);
                l.CloseToken(")", ")");
            }

            a.name_for_path = l.file_name;
            a.exists = true;

            if (!n.extern_flag)
                programm.Error(e, "__link() can only be used with 'extern'");

            if (n.variable->link_attribute.exists && n.variable->link_attribute != a)
                programm.Error(e, "previous declaration is different");

            n.variable->link_attribute = a;
            continue;
        }
        break;
    }
}

CNodePtr CParserFile::ParseLine(bool *out_break, bool global) {
    CErrorPosition e(l);

    if (l.IfToken(";"))
        return nullptr;

    if (l.IfToken("asm"))
        return ParseAsm(e);

    bool typedef_flag = l.IfToken("typedef");
    bool extern_flag = l.IfToken("extern");

    if (typedef_flag && extern_flag) {
        programm.Error(e, "multiple storage classes in declaration specifiers");  // gcc
        extern_flag = false;
    }

    CType base_type;
    if (!ParseTypeWoPointers(&base_type, out_break != nullptr)) {
        *out_break = true;
        if (typedef_flag || extern_flag)
            programm.Error(e, "useless storage class specifier in empty declaration");  // gcc
        return nullptr;
    }

    if (typedef_flag && base_type.flag_static) {
        programm.Error(e, "multiple storage classes in declaration specifiers");  // gcc
        base_type.flag_static = false;
    }

    if (l.IfToken(";")) {
        if (typedef_flag || extern_flag)
            programm.Error(e, "useless storage class specifier in empty declaration");  // gcc
        return nullptr;                                                                 // new struct, union, enum
    }

    CNodeList node_list;
    for (;;) {
        CType type;
        std::string name;
        ParseTypeNameArray(base_type, name, type);

        if (typedef_flag) {
            CNodePtr node = CNODE({CNT_TYPEDEF, ctype : type, e : e});
            RegisterTypedef(node, name);

            if (l.IfToken(";"))
                break;

            l.NeedToken(",");
            continue;
        }

        CNodePtr node =
            CNODE({typedef_flag ? CNT_TYPEDEF : CNT_DECLARE_VARIABLE, ctype : type, extern_flag : extern_flag, e : e});

        bool is_function = type.base_type == CBT_FUNCTION && !type.IsPointer();
        if (is_function)
            node->extern_flag = true;  // Function prototype is always "extern"

        CAlignAttribute align_attribute;
        ParseGccAttributes(&align_attribute);

        CNodePtr init;
        if (l.IfToken("=")) {
            if (typedef_flag || is_function)
                l.ThrowSyntaxError();
            init = ParseInitBlock(node->ctype, true);
            // TODO:            if (init)
            // TODO:                CCalcConst(init);
            // TODO:    All .next_mode
        }

        CVariablePtr v = RegisterVariable(node->extern_flag, node, global, name);
        v->align_attribute = align_attribute;

        node->variable = v;
        CPrepareArgs(node, programm);
        if (init)
            v->body = init;

        ParseAttributes(*node);

        node_list.PushBack(node);

        if (l.IfToken("{")) {
            if (!is_function || typedef_flag)
                l.ThrowSyntaxError();
            if (!global)
                l.Throw("nested functions are not supported");  // TODO
            ParseFunction(node);
            break;
        }

        if (l.IfToken(";"))
            break;

        if (!l.CloseToken(",", ";"))
            break;
    }

    return node_list.first;
}

void CParserFile::ParseTypePointers(CType &out_type) {
    while (l.IfToken("*")) {
        CPointer pointer;
        ParsePointerFlags(pointer);
        out_type.pointers.push_back(pointer);
    }
    for (;;) {
        if (out_type.variables_mode == CVM_NOT_SET && l.IfToken("__global")) {
            out_type.variables_mode = CVM_GLOBAL;
            continue;
        }
        if (out_type.variables_mode == CVM_NOT_SET && l.IfToken("__stack")) {
            out_type.variables_mode = CVM_STACK;
            continue;
        }
        break;
    }
}

void CParserFile::ParseTypeNameArray(CConstType base_type, std::string &out_name, CType &out_type) {
    CType type = base_type;
    ParseTypePointers(type);

    l.IfIdent(out_name);

    if (l.IfToken("(")) {
        CErrorPosition e(l);
        if (out_name.empty() && l.IfToken("*")) {
            std::vector<CPointer> pointers;
            do {
                CPointer pointer;
                ParsePointerFlags(pointer);
                pointers.push_back(pointer);
            } while (l.IfToken("*"));
            l.IfIdent(out_name);
            l.NeedToken(")");
            if (l.IfToken("(")) {
                ParseFunctionTypeArgs(e, type, &pointers, out_type);
                return;
            }
            type.pointers.insert(type.pointers.end(), pointers.begin(), pointers.end());
        } else {
            ParseFunctionTypeArgs(e, type, nullptr, out_type);
            return;
        }
    }

    std::vector<size_t> addrs;
    while (l.IfToken("[")) {
        uint64_t size = 0;
        if (!l.IfToken("]")) {
            size = ParseUint64();
            l.NeedToken("]");
        }
        addrs.push_back(size);
    }
    for (size_t i = addrs.size(); i != 0; i--)
        type.pointers.push_back(CPointer(addrs[i - 1]));
    out_type = type;
}

void CParserFile::ParseFunctionTypeArgs(CErrorPosition &e, CType &return_type, std::vector<CPointer> *fp,
                                        CType &out_type) {
    out_type.base_type = CBT_FUNCTION;
    out_type.flag_static = return_type.flag_static;
    return_type.flag_static = false;
    if (return_type.variables_mode != CVM_NOT_SET) {
        if (out_type.variables_mode != CVM_NOT_SET && out_type.variables_mode != return_type.variables_mode)
            programm.Error(e, "previous declaration is different");
        out_type.variables_mode = return_type.variables_mode;
    }
    return_type.variables_mode = CVM_NOT_SET;
    if (fp)
        for (auto &i : *fp)
            out_type.pointers.push_back(i);
    out_type.function_args.push_back(CStructItem{return_type});

    if (l.IfToken(")")) {
        out_type.many_function_args = true;
        return;
    }

    std::map<std::string, int> names;

    for (;;) {
        if (l.IfToken("...")) {
            l.NeedToken(")");
            out_type.many_function_args = true;
            break;
        }

        CErrorPosition e(l);
        CStructItem arg;
        CType pre_type;
        ParseType(&pre_type);

        if (pre_type.IsVoid()) {
            if (out_type.function_args.size() != 1)
                programm.Error(e, "'void' must be the only parameter");  // gcc
            l.NeedToken(")");
            out_type.many_function_args = false;
            break;
        }

        ParseTypeNameArray(pre_type, arg.name, arg.type);

        for (auto &i : arg.type.pointers)
            i.ResetArray();

        if (!arg.name.empty() && !names.try_emplace(arg.name, 0).second)
            programm.Error(e, std::string("redefinition of parameter '") + arg.name + "'");  // gcc

        out_type.function_args.push_back(arg);

        if (l.IfToken(")")) {
            out_type.many_function_args = false;
            break;
        }

        l.NeedToken(",");
    }
}

void CParserFile::ParseStruct(CStruct &struct_object) {
    CErrorPosition e(l);
    while (!l.IfToken("}")) {
        CType base_type;
        ParseTypeWoPointers(&base_type);
        do {
            CStructItemPtr struct_item = std::make_shared<CStructItem>();
            struct_item->type = base_type;
            ParseTypeNameArray(base_type, struct_item->name, struct_item->type);
            ParseGccAttributes(nullptr);
            struct_object.items.push_back(struct_item);
        } while (l.IfToken(","));
        l.NeedToken(";");
    }
    struct_object.CalcOffsets(e);
}

bool CParserFile::ParseType(CType *out_type, bool can_empty) {
    if (!ParseTypeWoPointers(out_type, can_empty))
        return false;
    ParseTypePointers(*out_type);
    return true;
}

void CParserFile::ParseFunction(CNodePtr &node) {
    if (!node->variable->only_extern) {
        programm.Error(node->e, "redefinition of '" + node->variable->name + "'");                  // gcc
        programm.Note(node->variable->e, "previous definition of '" + node->variable->name + "'");  // gcc
    }

    node->extern_flag = false;
    node->variable->only_extern = false;
    node->variable->body = node;

    // Init variables
    current_function = node->variable;
    scope_labels.clear();
    current_stack_size = 0;
    max_stack_size = 0;

    Enter();

    if (node->ctype.function_args.size() != current_function->function_arguments.size() + 1)
        throw std::runtime_error(std::string("Internal error in ") + __PRETTY_FUNCTION__);

    for (size_t i = 0; i < current_function->function_arguments.size(); i++) {
        CVariablePtr &a = current_function->function_arguments[i];
        a->name = node->ctype.function_args[i + 1u].name;
        scope_variables.push_back(a);
    }

    CNodeList list;
    while (!l.IfToken("}"))
        list.PushBack(ParseFunctionBody());
    node->a = list.first;

    Leave();

    current_function->function_stack_frame_size = max_stack_size;

    // Check labels
    for (auto &l : scope_labels)
        if (l.second->only_extern)
            programm.Error(l.second->e, "label '" + l.second->name + "'  used but not defined");  // gcc

    // Self check
    current_function = nullptr;
}

void CParserFile::ParseTypeWoPointersStruct(CType *out_type, bool is_union, CErrorPosition &e) {
    out_type->base_type = CBT_STRUCT;

    std::string name;
    if (l.IfIdent(name)) {
        out_type->struct_object = BindStructUnion(name, is_union, true, e);
        CStruct &s = *out_type->struct_object;

        if (l.IfToken("{")) {
            CStruct l;
            l.is_union = is_union;
            l.name = name;
            l.e = e;
            ParseStruct(l);

            if (s.inited) {
                if (s != l) {
                    programm.Error(l.e, "conflicting types for '" + name + "'; have '" + l.ToString() + "'");  // gcc
                    programm.Note(s.e,
                                  "previous declaration of '" + name + "' with type '" + s.ToString() + "'");  // gcc
                }
            } else {
                s = l;
            }
        }
    } else {
        l.NeedToken("{");
        out_type->struct_object = BindStructUnion("", is_union, true, e);
        ParseStruct(*out_type->struct_object);
    }
}

CNodePtr CParserFile::ParseExpressionComma() {
    CNodePtr result = ParseExpression();
    for (;;) {
        CErrorPosition e(l);
        if (!l.IfToken(","))
            return result;
        result = MakeOperator(COP_COMMA, result, ParseExpression(), e, programm.cmm);
    }
}

CNodePtr CParserFile::ParseExpression() {
    CNodePtr a = ParseExpressionA();

    CErrorPosition e(l);
    if (l.IfToken("?")) {
        CNodePtr b = ParseExpression();
        l.NeedToken(":");
        CNodePtr c = ParseExpression();
        return MakeOperatorIf(a, b, c, e, programm.cmm);
    }

    static const char *const operators[] = {"=", "+=", "-=", "*=", "/=", "%=", "<<=", ">>=", "&=", "^=", "|=", nullptr};
    static const COperatorCode operator_codes[] = {COP_SET,     COP_SET_ADD, COP_SET_SUB, COP_SET_MUL,
                                                   COP_SET_DIV, COP_SET_MOD, COP_SET_SHL, COP_SET_SHR,
                                                   COP_SET_AND, COP_SET_XOR, COP_SET_OR};
    size_t n = 0;
    if (l.IfToken(operators, n))
        return MakeOperator(operator_codes[n], a, ParseExpression(), e, programm.cmm);

    return a;
}

CNodePtr CParserFile::ParseExpressionA() {
    CNodePtr result = ParseExpressionB();
    for (;;) {
        CErrorPosition e(l);
        if (!l.IfToken("||"))
            return result;
        result = MakeOperator(COP_LOR, result, ParseExpressionB(), e, programm.cmm);
    }
}

CNodePtr CParserFile::ParseExpressionB() {
    CNodePtr result = ParseExpressionC();
    for (;;) {
        CErrorPosition e(l);
        if (!l.IfToken("&&"))
            return result;
        result = MakeOperator(COP_LAND, result, ParseExpressionC(), e, programm.cmm);
    }
}

CNodePtr CParserFile::ParseExpressionC() {
    CNodePtr result = ParseExpressionD();
    for (;;) {
        CErrorPosition e(l);
        if (!l.IfToken("|"))
            return result;
        result = MakeOperator(COP_OR, result, ParseExpressionD(), e, programm.cmm);
    }
}

CNodePtr CParserFile::ParseExpressionD() {
    CNodePtr result = ParseExpressionE();
    for (;;) {
        CErrorPosition e(l);
        if (!l.IfToken("^"))
            return result;
        result = MakeOperator(COP_XOR, result, ParseExpressionE(), e, programm.cmm);
    }
}

CNodePtr CParserFile::ParseExpressionE() {
    CNodePtr result = ParseExpressionF();
    for (;;) {
        CErrorPosition e(l);
        if (!l.IfToken("&"))
            return result;
        result = MakeOperator(COP_AND, result, ParseExpressionF(), e, programm.cmm);
    }
}

CNodePtr CParserFile::ParseExpressionF() {
    static const char *const operators[] = {"==", "!=", nullptr};
    static const COperatorCode operator_codes[] = {COP_CMP_E, COP_CMP_NE};
    CNodePtr result = ParseExpressionG();
    for (;;) {
        size_t n = 0;
        CErrorPosition e(l);
        if (!l.IfToken(operators, n))
            return result;
        result = MakeOperator(operator_codes[n], result, ParseExpressionG(), e, programm.cmm);
    }
}

CNodePtr CParserFile::ParseExpressionG() {
    static const char *const operators[] = {"<", "<=", ">", ">=", nullptr};
    static const COperatorCode operator_codes[] = {COP_CMP_L, COP_CMP_LE, COP_CMP_G, COP_CMP_GE};
    CNodePtr result = ParseExpressionH();
    for (;;) {
        size_t n = 0;
        CErrorPosition e(l);
        if (!l.IfToken(operators, n))
            return result;
        result = MakeOperator(operator_codes[n], result, ParseExpressionH(), e, programm.cmm);
    }
}

CNodePtr CParserFile::ParseExpressionH() {
    static const char *const operators[] = {">>", "<<", nullptr};
    static const COperatorCode operator_codes[] = {COP_SHR, COP_SHL};
    CNodePtr result = ParseExpressionI();
    for (;;) {
        size_t n = 0;
        CErrorPosition e(l);
        if (!l.IfToken(operators, n))
            return result;
        result = MakeOperator(operator_codes[n], result, ParseExpressionI(), e, programm.cmm);
    }
}

CNodePtr CParserFile::ParseExpressionI() {
    static const char *const operators[] = {"+", "-", nullptr};
    static const COperatorCode operator_codes[] = {COP_ADD, COP_SUB};
    CNodePtr result = ParseExpressionJ();
    for (;;) {
        size_t n = 0;
        CErrorPosition e(l);
        if (!l.IfToken(operators, n))
            return result;
        result = MakeOperator(operator_codes[n], result, ParseExpressionJ(), e, programm.cmm);
    }
}

CNodePtr CParserFile::ParseExpressionJ() {
    static const char *const operators[] = {"*", "/", "%", nullptr};
    static const COperatorCode operator_codes[] = {COP_MUL, COP_DIV, COP_MOD};
    CNodePtr result = ParseExpressionK();
    for (;;) {
        size_t n = 0;
        CErrorPosition e(l);
        if (!l.IfToken(operators, n))
            return result;
        result = MakeOperator(operator_codes[n], result, ParseExpressionK(), e, programm.cmm);
    }
}

CNodePtr CParserFile::ParseExpressionK() {
    CErrorPosition e(l);
    if (l.IfToken("(")) {
        CType new_type;
        if (ParseType(&new_type, true)) {
            l.NeedToken(")");
            return CNODE({CNT_CONVERT, a : ParseExpressionK(), ctype : new_type, e : e});
        }
        CNodePtr result = ParseExpressionComma();
        l.NeedToken(")");
        return ParseExpressionM(result);
    }

    static const char *const operators[] = {"++", "--", "+", "-", "!", "~", "*", "&", nullptr};
    static const CMonoOperatorCode operator_codes[] = {MOP_INC, MOP_DEC, MOP_PLUS,   MOP_MINUS,
                                                       MOP_NOT, MOP_NEG, MOP_DEADDR, MOP_ADDR};

    size_t n = 0;
    if (l.IfToken(operators, n)) {
        const CMonoOperatorCode mo = operator_codes[n];

        CNodePtr a = ParseExpressionK();

        CNodePtr result = CNODE({CNT_MONO_OPERATOR, a : a, ctype : a->ctype, mono_operator_code : mo, e : e});

        switch (mo) {
            case MOP_INC:
                if (a->ctype.IsConst())
                    programm.Error(e, "increment of read-only variable");  // gcc
                break;
            case MOP_DEC:
                if (a->ctype.IsConst())
                    programm.Error(e, "decrement of read-only variable");  // gcc
                break;
            case MOP_DEADDR:
                if (result->ctype.pointers.empty()) {
                    if (!programm.cmm)
                        programm.Error(
                            e, "invalid type argument of unary '*' (have '" + result->ctype.ToString() + "')");  // gcc
                } else {
                    result->ctype.pointers.pop_back();
                }
                break;
            case MOP_ADDR:
                result->ctype.pointers.push_back(CPointer());
                break;
            case MOP_NOT:
                result->ctype = CType{CBT_BOOL};
                break;
        }
        return result;
    }
    return ParseExpressionL();
}

CNodePtr CParserFile::ParseExpressionL() {
    CNodePtr result = ParseExpressionValue();
    return CParserFile::ParseExpressionM(result);
}

CNodePtr CParserFile::ParseExpressionCall(CNodePtr &f, CErrorPosition &e) {
    // Type check
    if (!f->ctype.IsFunction() && !f->ctype.IsFuncitonPointer() && !programm.cmm)
        programm.Error(e, "called type '" + f->ctype.ToString() + "' is not a function or function pointer");  // gcc

    // Parse arguments
    const std::vector<CStructItem> &fa = f->ctype.function_args;
    CNodeList args;
    size_t args_count = 1;
    if (!l.IfToken(")")) {
        do {
            CNodePtr arg = ParseExpression();
            if (args_count < fa.size() && !programm.cmm)
                arg = Convert(fa[args_count].type, arg);
            args.PushBack(arg);
            args_count++;
        } while (l.IfToken(","));
        l.NeedToken(")");
    }

    // Type check
    if (args_count < fa.size() && !programm.cmm)
        programm.Error(e, "too few arguments");  // gcc
    if (!f->ctype.many_function_args && args_count > fa.size() && !programm.cmm)
        programm.Error(e, "too many arguments");  // gcc

    // Make object
    CNodePtr call;
    if (f->type == CNT_MONO_OPERATOR && f->mono_operator_code == MOP_ADDR && f->a->type == CNT_LOAD_VARIABLE) {
        call = CNODE({CNT_FUNCTION_CALL, a : args.first, variable : f->a->variable, e : e});
    } else {
        call = CNODE({CNT_FUNCTION_CALL_ADDR, a : args.first, b : f, e : e});
    }

    if (fa.empty())
        call->ctype = CType{CBT_CHAR};
    else
        call->ctype = fa[0].type;

    return call;
}

CNodePtr CParserFile::ParseExpressionValue() {
    size_t n = 0;
    CErrorPosition e(l);
    if (l.IfTokenP(scope_variables, n)) {
        auto &v = scope_variables[n];
        CNodePtr result = CNODE({CNT_LOAD_VARIABLE, ctype : v->type, variable : v, e : e});

        if (v->link_attribute.exists && !v->link_attribute_processed) {
            std::string full_file_name;
            if (!cparser.FindAnyIncludeFile(v->link_attribute.base_name, v->link_attribute.name_for_path,
                                            full_file_name))
                l.Throw("file \"" + v->link_attribute.base_name + "\" not found, local path \"" +
                        v->link_attribute.name_for_path + "\"");
            cparser.AddSourceFile(full_file_name);
            v->link_attribute_processed = true;
        }

        if (result->ctype.IsFunction()) {
            CType type2 = v->type;
            type2.pointers.push_back(CPointer());
            return CNODE({CNT_MONO_OPERATOR, result, ctype : type2, mono_operator_code : MOP_ADDR, e : e});
        }
        return result;
    }

    if (l.IfToken("sizeof")) {
        l.NeedToken("(");
        CNodePtr result = CNODE({CNT_NUMBER, ctype : CType{CBT_SIZE}, e : e});

        CType type;
        if (ParseType(&type, true))
            result->a = CNODE({CNT_SIZEOF_TYPE, ctype : type, e : e});
        else
            result->a = ParseExpression();

        result->number.u = result->a->ctype.SizeOf(e);
        l.NeedToken(")");
        return result;
    }

    std::string str;
    if (l.IfString2(str, &cparser.codepage)) {
        CNodePtr result = CNODE({CNT_CONST_STRING, e : e});
        result->ctype.base_type = CBT_CHAR;
        result->ctype.flag_const = true;
        result->ctype.pointers.push_back(CPointer(str.size() + 1));
        result->const_string = programm.RegisterConstString(str);
        return result;
    }

    if (l.IfString1(str, &cparser.codepage)) {
        if (str.size() == 0)
            programm.Error(e, "empty character constant");  // gcc
        if (str.size() != 1)
            programm.Error(e, "empty-character character constant");  // gcc
        return CNODE({CNT_NUMBER, ctype : {CBT_CHAR}, number : {i : str[0]}, e : e});
    }

    uint64_t number = 0;
    if (l.IfInteger(number)) {
        CNodePtr node = CNODE({CNT_NUMBER, e : e});
        if (number <= INT16_MAX) {
            node->number.i = int16_t(number);
            node->ctype.base_type = CBT_SHORT;
        } else if (number <= INT32_MAX) {
            node->number.i = int32_t(number);
            node->ctype.base_type = CBT_LONG;
        } else if (number <= INT64_MAX) {
            node->number.i = int64_t(number);
            node->ctype.base_type = CBT_LONG_LONG;
        } else {
            node->number.u = number;
            node->ctype.base_type = CBT_UNSIGNED_LONG_LONG;
        }
        return node;
    }

    long double f = 0;
    if (l.IfFloat(f)) {
        CNodePtr node = CNODE({CNT_NUMBER, e : e});
        node->number.ld = f;
        node->ctype.base_type = CBT_LONG_DOUBLE;
        return node;
    }

    l.ThrowSyntaxError();
    return nullptr;
}

bool CParserFile::ParseTypeWoPointers(CType *out_type, bool can_empty_inital) {
    bool can_empty = can_empty_inital;

    for (;;) {
        if (l.IfToken("register"))  // gcc ignore
            continue;
        if (l.IfToken("__extension__"))  // gcc ignore
            continue;
        if (!out_type->flag_const && l.IfToken("const")) {
            out_type->flag_const = true;
            can_empty = false;
            continue;
        }
        if (!out_type->flag_volatile && l.IfToken("volatile")) {
            out_type->flag_volatile = true;
            can_empty = false;
            continue;
        }
        if (!out_type->flag_static && l.IfToken("static")) {
            out_type->flag_static = true;
            can_empty = false;
            continue;
        }
        break;
    }

    CErrorPosition e(l);

    if (l.IfToken("struct")) {
        ParseTypeWoPointersStruct(out_type, false, e);
        return true;
    }

    if (l.IfToken("union")) {
        ParseTypeWoPointersStruct(out_type, true, e);
        return true;
    }

    if (l.IfToken("enum")) {
        if (l.IfToken(CT_IDENT)) {  // TODO: enum type support
            if (l.IfToken("{"))
                ParseEnum();
        } else {
            l.NeedToken("{");
            ParseEnum();
        }
        out_type->base_type = CBT_INT;
        return true;
    }

    size_t n = 0;
    if (l.IfToken(scope_typedefs, n)) {
        CType flags = *out_type;
        *out_type = scope_typedefs[n].type;
        out_type->flag_const = flags.flag_const;
        out_type->flag_static = flags.flag_static;
        out_type->flag_volatile = flags.flag_volatile;
        out_type->variables_mode = flags.variables_mode;
        return true;
    }

    out_type->base_type = ParseBaseType();
    if (out_type->base_type != CBT_STRUCT)
        return true;

    if (!can_empty)
        l.ThrowSyntaxError();

    return false;
}

CBaseType CParserFile::ParseBaseType() {
    static const char *const strings0[] = {
        "void", "char", "short", "int", "long", "float", "double", "__builtin_va_list", "signed", "unsigned", nullptr};
    size_t n = 0;
    if (!l.IfToken(strings0, n))
        return CBT_STRUCT;

    static const char *const strings1[] = {"char", "short", "int", "long", nullptr};
    static const char *const strings2[] = {"double", "long", "unsigned", "signed", "int", nullptr};

    switch (n) {
        case 0:               // void
            return CBT_VOID;  // "void"
        case 1:               // char
            return CBT_CHAR;  // "char"
        case 2:               // short
            if (l.IfToken("unsigned")) {
                l.IfToken("int");           // "short unsigned int"
                return CBT_UNSIGNED_SHORT;  // "short unsigned"
            }
            l.IfToken("signed");  // "short signed"
            l.IfToken("int");     // "short int", "short signed int"
            return CBT_SHORT;     // "short"
        case 3:                   // int
            return CBT_INT;
        case 4:  // long
            if (!l.IfToken(strings2, n))
                return CBT_LONG;  // "long"
            switch (n) {
                case 0:                      // double
                    return CBT_LONG_DOUBLE;  // "long double"
                case 1:                      // long
                    if (l.IfToken("unsigned")) {
                        l.IfToken("int");               // "long long unsigned int"
                        return CBT_UNSIGNED_LONG_LONG;  // "long long unsigned"
                    }
                    l.IfToken("int");          // "long long int"
                    return CBT_LONG_LONG;      // "long long"
                case 2:                        // unsigned
                    l.IfToken("int");          // "long unsigned int"
                    return CBT_UNSIGNED_LONG;  // "long unsigned"
                case 3:                        // signed
                    l.IfToken("int");          // "long signed int"
                    return CBT_LONG;           // "long signed"
                case 4:                        // int
                    return CBT_LONG;           // "long int"
            }
            assert(false);
            break;
        case 5:
            return CBT_FLOAT;  // "float"
        case 6:
            return CBT_DOUBLE;  // "double"
        case 7:
            return CBT_VA_LIST;  // "__builtin_va_list"
        case 8:                  // signed
            if (!l.IfToken(strings1, n))
                return CBT_INT;  // "signed"
            switch (n) {
                case 0:                      // char
                    return CBT_SIGNED_CHAR;  // "signed char"
                case 1:                      // short
                    l.IfToken("int");        // "signed short int"
                    return CBT_SHORT;        // "signed short"
                case 2:                      // int
                    return CBT_INT;          // "signed int"
                case 3:                      // long
                    l.IfToken("int");        // "signed long int"
                    return CBT_LONG;         // "signed long"
            }
            assert(false);
            break;
        case 9:  // unsigned
            if (!l.IfToken(strings1, n))
                return CBT_UNSIGNED_INT;  // "unsigned"
            switch (n) {
                case 0:                         // char
                    return CBT_UNSIGNED_CHAR;   // "unsigned char"
                case 1:                         // short
                    l.IfToken("int");           // "unsigned short int"
                    return CBT_UNSIGNED_SHORT;  // "unsigned short"
                case 2:                         // int
                    return CBT_UNSIGNED_INT;    // "unsigned int"
                case 3:                         // long
                    if (l.IfToken("long")) {
                        l.IfToken("int");               // "unsigned long long int"
                        return CBT_UNSIGNED_LONG_LONG;  // "unsigned long long"
                    }
                    l.IfToken("int");          // "unsigned long int"
                    return CBT_UNSIGNED_LONG;  // "unsigned long"
            }
            assert(false);
            break;
    }
    assert(false);
    return CBT_STRUCT;
}

CNodePtr CParserFile::ParseExpressionStructItem(CMonoOperatorCode mo, CNodePtr &a, CErrorPosition &e) {
    std::string item_name;
    l.NeedIdent(item_name);

    if (a->ctype.pointers.size() != (mo == MOP_STRUCT_ITEM ? 0 : 1) || a->ctype.base_type != CBT_STRUCT) {
        programm.Error(e, std::string("invalid type argument of '") + ToString(a->mono_operator_code) + "' (have '" +
                              a->ctype.ToString() + "')");  // gcc
        return a;
    }

    if (a->ctype.struct_object == nullptr)
        l.Throw(std::string("Internal error in ") + __PRETTY_FUNCTION__);

    CStructItemPtr struct_item = a->ctype.struct_object->FindItem(item_name);
    if (struct_item == nullptr) {
        programm.Error(e, std::string("'struct ") + a->ctype.struct_object->name + "' has no member named '" +
                              item_name + "'");  // gcc
        return a;
    }

    CNodePtr result = CNODE({
        CNT_MONO_OPERATOR,
        a : a,
        ctype : struct_item->type,
        struct_item : struct_item,
        mono_operator_code : mo,
        e : e
    });

    if (result->a->ctype.flag_const)
        result->ctype.flag_const = true;

    return result;
}

CNodePtr CParserFile::ParseExpressionArrayElement(CNodePtr &a, CErrorPosition &e) {
    CNodePtr result =
        CNODE({CNT_MONO_OPERATOR, a : a, ctype : a->ctype, mono_operator_code : MOP_ARRAY_ELEMENT, e : e});
    result->b = Convert(CType{CBT_SIZE}, ParseExpressionComma(), programm.cmm);
    l.NeedToken("]");
    if (!result->ctype.pointers.empty()) {
        result->ctype.pointers.pop_back();
    } else if (!programm.cmm) {
        programm.Error(e, "subscripted value is neither array nor pointer nor vector");  // gcc
    }
    return result;
}

CNodePtr CParserFile::ParseExpressionM(CNodePtr result) {
    for (;;) {
        CErrorPosition e(l);
        if (l.IfToken("(")) {
            result = ParseExpressionCall(result, e);
            continue;
        }

        static const char *const operators[] = {"++", "--", ".", "->", "[", nullptr};
        size_t n = 0;
        if (!l.IfToken(operators, n))
            break;

        switch (n) {
            case 0:  // a++
                if (result->ctype.IsConst())
                    programm.Error(e, "increment of read-only variable");  // gcc
                result = CNODE(
                    {CNT_MONO_OPERATOR, a : result, ctype : result->ctype, mono_operator_code : MOP_POST_INC, e : e});
                break;
            case 1:  // a--
                if (result->ctype.IsConst())
                    programm.Error(e, "decrement of read-only variable");  // gcc
                result = CNODE(
                    {CNT_MONO_OPERATOR, a : result, ctype : result->ctype, mono_operator_code : MOP_POST_DEC, e : e});
                break;
            case 2:  // a.
                result = ParseExpressionStructItem(MOP_STRUCT_ITEM, result, e);
                break;
            case 3:  // a->
                result = ParseExpressionStructItem(MOP_STRUCT_ITEM_POINTER, result, e);
                break;
            case 4:  // a[
                result = ParseExpressionArrayElement(result, e);
                break;
            default:
                assert(false);
        }
    }
    return result;
}

CNodePtr CParserFile::ParseFunctionBody() {
    CErrorPosition e(l);
    if (l.token == CT_IDENT && l.cursor[0] == ':' && 0 != memcmp(l.token_data, "default", l.token_size)) {
        std::string label_name;
        l.NeedIdent(label_name);
        l.NeedToken(":");
        return CNODE({CNT_LABEL, variable : BindLabel(label_name, e, false), e : e});
    }
    if (l.IfToken("goto")) {
        std::string label_name;
        l.NeedIdent(label_name);
        l.CloseToken(";", ";");
        return CNODE({CNT_GOTO, variable : BindLabel(label_name, e, true), e : e});
    }
    if (l.IfToken("if")) {
        l.NeedToken("(");
        CNodePtr node = CNODE({CNT_IF, ParseExpressionComma(), e : e});
        l.NeedToken(")");
        node->b = ParseFunctionBody();
        if (l.IfToken("else"))
            node->c = ParseFunctionBody();
        return node;
    }
    if (l.IfToken("while")) {
        loop_level++;
        l.NeedToken("(");
        CNodePtr node = CNODE({CNT_WHILE, ParseExpressionComma(), e : e});
        l.NeedToken(")");
        node->b = ParseFunctionBody();
        loop_level--;
        return node;
    }
    if (l.IfToken("do")) {
        loop_level++;
        CNodePtr b = ParseFunctionBody();
        l.NeedToken("while");
        l.NeedToken("(");
        CNodePtr node = CNODE({CNT_DO, ParseExpressionComma(), b : b, e : e});
        l.NeedToken(")");
        l.CloseToken(";", ";");
        loop_level--;
        return node;
    }
    if (l.IfToken("break")) {
        l.CloseToken(";", ";");
        return CNODE({CNT_BREAK, e : e});
    }
    if (l.IfToken("continue")) {
        l.CloseToken(";", ";");
        return CNODE({CNT_CONTINUE, e : e});
    }
    if (l.IfToken("case")) {
        CNodePtr node = CNODE({CNT_CASE, ParseExpressionComma(), e : e});
        l.NeedToken(":");
        if (last_switch != nullptr) {
            node->case_link = last_switch->case_link;
            last_switch->case_link = node;
        } else {
            programm.Error(e, "case label not within a switch statement");  // gcc
        }
        return node;
    }
    if (l.IfToken("default")) {
        CNodePtr node = CNODE({CNT_DEFAULT, e : e});
        if (last_switch != nullptr) {
            CNodePtr default_link = last_switch->default_link.lock();
            if (default_link != nullptr)
                programm.Error(e, "multiple default labels in one switch");  // gcc
            last_switch->default_link = node;
        } else {
            programm.Error(e, "case label not within a switch statement");  // gcc
        }
        l.NeedToken(":");
        return node;
    }
    if (l.IfToken("switch")) {
        CNodePtr saved_switch = last_switch;
        l.NeedToken("(");
        auto value = ParseExpressionComma();
        auto node = CNODE({CNT_SWITCH, value});
        last_switch = node;
        node->e = e;
        l.NeedToken(")");
        Enter();
        l.NeedToken("{");
        CNodeList body;
        while (!l.IfToken("}"))
            body.PushBack(ParseFunctionBody());
        Leave();
        node->b = body.first;
        last_switch = saved_switch;
        return node;
    }
    if (l.IfToken("for")) {
        loop_level++;
        Enter();
        l.NeedToken("(");
        auto a = ParseFunctionBody2();
        // Always ends by ;
        CNodePtr b;
        if (!l.IfToken(";")) {
            b = ParseExpressionComma();
            l.NeedToken(";");
        }
        CNodePtr c;
        if (!l.IfToken(")")) {
            c = ParseExpressionComma();
            l.NeedToken(")");
        }
        auto d = ParseFunctionBody();
        Leave();
        loop_level--;
        return CNODE({CNT_FOR, a, b, c, d, e : e});
    }
    if (l.IfToken("{")) {
        Enter();
        CNodeList list;
        while (!l.IfToken("}"))
            list.PushBack(ParseFunctionBody());
        Leave();
        return CNODE({CNT_LEVEL, list.first, e : e});
    }
    if (l.IfToken("return")) {
        const std::vector<CStructItem> &fa = current_function->type.function_args;
        CNodePtr return_value;
        if (l.IfToken(";")) {
            if (fa.size() >= 1 && !fa[0].type.IsVoid() && !programm.cmm)
                programm.Error(e, "'return' with no value, in function returning non-void");  // gcc
        } else {
            return_value = ParseExpressionComma();
            l.CloseToken(";", ";");

            if (fa.size() >= 1 && !fa[0].type.IsVoid())
                return_value = Convert(fa[0].type, return_value, programm.cmm);
            else if (!programm.cmm)
                programm.Error(e, "'return' with a value, in function returning void");  // gcc
        }
        return CNODE({CNT_RETURN, return_value, e : e});
    }
    if (programm.cmm && l.IfToken("push_pop")) {
        l.NeedToken("(");
        CNodeList regs;
        if (!l.IfToken(")")) {
            for (;;) {
                regs.PushBack(ParseExpression());
                if (l.IfToken(")"))
                    break;
                l.NeedToken(",");
            }
        }
        l.NeedToken("{");
        CNodeList body;
        while (!l.IfToken("}"))
            body.PushBack(ParseFunctionBody());
        return CNODE({CNT_PUSH_POP, a : regs.first, b : body.first, e : e});
    }
    return ParseFunctionBody2();
}

CNodePtr CParserFile::ParseFunctionBody2() {
    if (l.IfToken(";"))
        return nullptr;
    bool empty = false;
    CNodePtr node = ParseLine(&empty, false);
    if (!empty)
        return node;
    node = ParseExpressionComma();
    l.CloseToken(";", ";");
    return node;
}

CNodePtr CParserFile::ParseInitBlock(CType &type, bool can_change_size) {
    if (!type.pointers.empty() && type.pointers.back().is_array) {
        // List
        if (l.IfToken("{")) {
            CType element_type = type;
            element_type.pointers.pop_back();
            CNodeList init;
            size_t items_count = 0;
            for (;;) {
                if (l.IfToken("}"))
                    break;
                CNodePtr i = ParseInitBlock(element_type, false);
                if (i) {
                    init.PushBack(i);
                    items_count++;
                }
                if (l.IfToken("}"))
                    break;
                if (!l.CloseToken(",", "}"))
                    break;
            }
            if (can_change_size && type.pointers.back().is_array && type.pointers.back().array_size == 0)
                type.pointers.back().array_size = items_count;

            if (items_count > type.pointers.back().array_size && init.last != nullptr)
                programm.Error(init.last->e, "excess elements in array initializer");  // gcc

            return init.first;
        }

        // String
        CErrorPosition e(l);
        if (type.pointers.size() == 1 && type.SizeOfBase(e) == 1 && type.pointers.back().is_array) {
            CNodePtr init = ParseExpression();
            if (init->type != CNT_CONST_STRING || init->ctype.pointers.size() != 1 ||
                !init->ctype.pointers[0].is_array) {
                programm.Error(init->e, "only const string or array");
                return nullptr;
            }
            const size_t items_count = init->ctype.pointers[0].array_size;
            if (can_change_size && type.pointers.back().is_array && type.pointers.back().array_size == 0)
                type.pointers.back().array_size = items_count;

            // Constant strings can be truncated

            init->type = CNT_IMMEDIATE_STRING;
            init->ctype = type;  // signed char <-> unsigned char
            return init;
        }

        l.SyntaxError();
        return nullptr;
    }

    // Struct
    if (type.pointers.empty() && type.base_type == CBT_STRUCT) {
        if (!l.WantToken("{"))
            return nullptr;

        CNodeList init;
        bool stop = false;
        for (auto &i : type.struct_object->items) {
            CErrorPosition e(l);
            if (stop) {
                init.PushBack(CNODE({CNT_IMMEDIATE_STRING, ctype : i->type, e : e}));
            } else {
                CNodePtr e = ParseInitBlock(i->type, false);
                if (e)
                    init.PushBack(e);
                stop = !l.IfToken(",");
            }
        }
        l.CloseToken("}", "}");
        return init.first;
    }

    // Value
    return Convert(type, ParseExpression(), programm.cmm);
}
