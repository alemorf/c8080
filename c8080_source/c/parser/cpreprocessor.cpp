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

#include "../../tools/parseutf8.h"
#include "cparserfile.h"

void CParserFile::Preprocessor() {
    if (l.IfToken("include"))
        return PreprocessorInclude();
    if (l.IfToken("pragma")) {
        if (l.IfToken("codepage"))
            return PreprocessorPragmaCodepage();
        if (l.IfToken("once"))
            return PreprocessorPragmaOnce();
        // TODO: #pragma pack ...
        l.Error("invalid preprocessing directive #pragma " + std::string(l.token_data, l.token_size));  // gcc
        l.PreprocessorLeave();
        return;
    }
    if (l.IfToken("define"))
        return PreprocessorDefine();
    if (l.IfToken("undef"))
        return PreprocessorUndef();
    if (l.IfToken("else"))
        return PreprocessorElse();
    if (l.IfToken("elif") || l.IfToken("elifdef") || l.IfToken("elifndef"))
        return PreprocessorElif();
    if (l.IfToken("endif"))
        return PreprocessorEndif();
    if (l.IfToken("ifdef"))
        return PreprocessorIfdef();
    if (l.IfToken("ifndef"))
        return PreprocessorIfndef();
    l.enable_macro_in_preprocessor = true;
    if (l.IfToken("if"))
        return PreprocessorIf();
    l.enable_macro_in_preprocessor = false;
    if (l.IfToken("error"))
        return l.Error(l.token_data);
    if (l.IfToken("warning"))
        return l.Warning(l.token_data);
    // TODO: #line #embed
    l.Error("invalid preprocessing directive #" + std::string(l.token_data, l.token_size));  // gcc
}

void CParserFile::PreprocessorInclude() {
    const char *arg = l.token_data;
    size_t arg_size = strlen(arg);

    bool current_dir;
    if (arg_size >= 2 && arg[0] == '"' && arg[arg_size - 1] == '"') {
        current_dir = true;
    } else if (arg_size >= 2 && arg[0] == '<' && arg[arg_size - 1] == '>') {
        current_dir = false;
    } else {
        l.Error("#include expects \"FILENAME\" or <FILENAME>");  // gcc
        return;
    }

    std::string file_name(arg + 1, arg_size - 2);
    l.PreprocessorLeave();

    std::string full_file_name;
    if (current_dir) {
        if (!cparser.FindAnyIncludeFile(file_name, l.file_name, full_file_name)) {
            l.Error("file \"" + file_name + "\" not found, local path \"" + l.file_name + "\"");
            return;
        }
    } else {
        if (!cparser.FindGlobalIncludeFile(file_name, full_file_name)) {
            l.Error("file \"" + file_name + "\" not found");
            return;
        }
    }

    const char *cached_file_name = nullptr;
    const char *contents = cparser.LoadFile(full_file_name, &cached_file_name);
    l.Include(contents, cached_file_name);
}

void CParserFile::PreprocessorPragmaCodepage() {
    if (l.IfToken("reset")) {
        if (!l.WantToken(CT_EOF))
            return;
        cparser.codepage.clear();
        return;
    }

    if (!l.WantToken("("))
        return;
    CErrorPosition p1(l);
    std::string from_str;
    if (!l.WantString1(from_str))
        return;
    uint32_t from = 0;
    const char *f = from_str.c_str();
    const char *f_end = f + from_str.size();
    if (!ParseUtf8(from, f, f_end) || f != f_end)
        return programm.Error(p1, "incorrect char");
    if (!l.WantToken(","))
        return;
    CErrorPosition p2(l);
    uint64_t to;
    CBaseType unused_type;
    if (!l.WantInteger(to, unused_type))
        return;
    if (to < 0 || to > UINT8_MAX)
        return programm.Error(p2, "incorrect number");
    if (!l.WantToken(")"))
        return;
    if (!l.WantToken(CT_EOF))
        return;
    cparser.codepage[from] = to;
}

void CParserFile::PreprocessorPragmaOnce() {
    if (!l.WantToken(CT_EOF))
        return;
    if (!pragma_once.try_emplace(l.file_name, 0).second)
        l.PreprocessorSkipFile();
}

void CParserFile::PreprocessorDefine() {
    bool args_e = (!isspace(l.cursor[-1]) && l.cursor[0] == '(');
    CString id;
    if (!l.WantIdent(id))
        return;

    std::vector<std::string> args;
    CMacroArgsMode args_mode = CMAM_NONE;
    if (args_e) {
        args_mode = CMAM_FIXED;
        if (!l.WantToken("("))
            return;
        if (!l.IfToken(")")) {
            do {
                if (l.IfToken("...")) {
                    args.push_back("__VA_ARGS__");
                    args_mode = CMAM_VA_OPT;
                    break;
                }
                std::string id;
                if (!l.WantIdent(id))
                    return;
                args.push_back(id);
                if (l.IfToken("...")) {
                    args_mode = CMAM_VAR_LAST;
                    break;
                }
            } while (l.IfToken(","));
            if (!l.WantToken(")"))
                return;
        }
    }

    l.PreprocessorLeave();
    l.AddMacro(programm.SaveString(id), l.token_data, strlen(l.token_data), &args, args_mode);
}

void CParserFile::PreprocessorIf() {
    assert(l.enable_macro_in_preprocessor);
    const int64_t result = PreprocessorIf0();
    l.enable_macro_in_preprocessor = false;
    if (!l.WantToken(CT_EOF))
        return;

    PreprocessorIfAny(result);
}

void CParserFile::PreprocessorIfAny(int64_t result) {
    while (l.PreprocessorIf(result)) {
        if (l.IfToken("elifdef")) {
            CString id;
            if (l.WantIdent(id)) {
                l.WantToken(CT_EOF);
                result = PreprocessorIfdefCheck(id);
            }
            continue;
        }
        if (l.IfToken("elifndef")) {
            CString id;
            if (l.WantIdent(id)) {
                l.WantToken(CT_EOF);
                result = !PreprocessorIfdefCheck(id);
            }
            continue;
        }
        l.enable_macro_in_preprocessor = true;
        if (l.WantToken("elif")) {
            result = PreprocessorIf0();
            l.enable_macro_in_preprocessor = false;
            l.WantToken(CT_EOF);
            continue;
        }
        l.enable_macro_in_preprocessor = false;
        l.SyntaxError();
        result = false;
    }
}

int64_t CParserFile::PreprocessorIf0() {
    int64_t result = PreprocessorIfA();

    if (l.IfToken("?")) {
        const int64_t true_value = PreprocessorIf0();
        l.NeedToken(":");
        const int64_t false_value = PreprocessorIf0();
        return result ? true_value : false_value;
    }

    return result;
}

int64_t CParserFile::PreprocessorIfA() {
    int64_t result = PreprocessorIfB();
    while (l.IfToken("||"))
        result = PreprocessorIfB() || result;
    return result;
}

int64_t CParserFile::PreprocessorIfB() {
    int64_t result = PreprocessorIfC();
    while (l.IfToken("&&"))
        result = PreprocessorIfC() && result;
    return result;
}

int64_t CParserFile::PreprocessorIfC() {
    int64_t result = PreprocessorIfD();
    while (l.IfToken("|"))
        result = PreprocessorIfD() | result;
    return result;
}

int64_t CParserFile::PreprocessorIfD() {
    int64_t result = PreprocessorIfE();
    while (l.IfToken("^"))
        result = PreprocessorIfE() ^ result;
    return result;
}

int64_t CParserFile::PreprocessorIfE() {
    int64_t result = PreprocessorIfF();
    while (l.IfToken("&"))
        result = PreprocessorIfF() & result;
    return result;
}

int64_t CParserFile::PreprocessorIfF() {
    int64_t result = PreprocessorIfG();
    static const char *const operators[] = {"==", "!=", nullptr};
    size_t n = 0;
    while (l.IfToken(operators, n)) {
        const int64_t y = PreprocessorIfG();
        switch (n) {
            case 0:
                result = (result == y);
                break;
            case 1:
                result = (result != y);
                break;
        }
    }
    return result;
}

int64_t CParserFile::PreprocessorIfG() {
    int64_t result = PreprocessorIfH();
    static const char *const operators[] = {"<", "<=", ">", ">=", nullptr};
    size_t n = 0;
    while (l.IfToken(operators, n)) {
        const int64_t y = PreprocessorIfH();
        switch (n) {
            case 0:
                result = (result < y);
                break;
            case 1:
                result = (result <= y);
                break;
            case 2:
                result = (result > y);
                break;
            case 3:
                result = (result >= y);
                break;
        }
    }
    return result;
}

int64_t CParserFile::PreprocessorIfH() {
    int64_t result = PreprocessorIfI();
    static const char *const operators[] = {">>", "<<", nullptr};
    size_t n = 0;
    while (l.IfToken(operators, n)) {
        const int64_t y = PreprocessorIfI();
        switch (n) {
            case 0:
                result = (result >> y);
                // TODO: Check overflow
                break;
            case 1:
                result = (result << y);
                // TODO: Check overflow
                break;
        }
    }
    return result;
}

int64_t CParserFile::PreprocessorIfI() {
    int64_t result = PreprocessorIfJ();
    static const char *const operators[] = {"+", "-", nullptr};
    size_t n = 0;
    while (l.IfToken(operators, n)) {
        const int64_t y = PreprocessorIfJ();
        switch (n) {
            case 0:
                if (__builtin_add_overflow(result, y, &result))
                    l.Error("integer overflow in preprocessor expression");  // gcc
                break;
            case 1:
                if (__builtin_sub_overflow(result, y, &result))
                    l.Error("integer overflow in preprocessor expression");  // gcc
                break;
        }
    }
    return result;
}

int64_t CParserFile::PreprocessorIfJ() {
    int64_t result = PreprocessorIf2();
    static const char *const operators[] = {"*", "/", "%", nullptr};
    size_t n = 0;
    while (l.IfToken(operators, n)) {
        const int64_t y = PreprocessorIf2();
        switch (n) {
            case 0:
                if (__builtin_mul_overflow(result, y, &result))
                    l.Error("integer overflow in preprocessor expression");  // gcc
                break;
            case 1:
                if (y == 0)
                    l.Error("division by zero in preprocessor expression");  // gcc
                else
                    result = (result / y);
                break;
            case 2:
                if (y == 0)
                    l.Error("division by zero in preprocessor expression");  // gcc
                else
                    result = (result % y);
                break;
        }
    }
    return result;
}

int64_t CParserFile::PreprocessorIf2() {
    static const char *const operators[] = {"+", "-", "!", "~", nullptr};
    size_t n = 0;
    while (l.IfToken(operators, n)) {
        const int64_t result = PreprocessorIf2();
        switch (n) {
            case 0:
                return +result;
            case 1:
                return -result;
            case 2:
                return !result;
            case 3:
                return ~result;
        }
    }

    if (l.IfToken("(")) {
        const int64_t result = PreprocessorIf0();
        l.WantToken(")");
        return result;
    }

    uint64_t value;
    CBaseType unused_type;
    if (l.IfInteger(value, unused_type)) {
        if (value > INT64_MAX)
            l.Error("integer constant is too large for its type");
        return int64_t(value);
    }

    l.enable_macro_in_preprocessor = false;
    if (l.IfToken("defined")) {
        const bool need_close = l.IfToken("(");
        std::string id;
        l.enable_macro_in_preprocessor = true;
        l.WantIdent(id);
        if (need_close)
            if (!l.CloseToken(")", ")"))
                return false;
        return PreprocessorIfdefCheck(id);
    }
    l.enable_macro_in_preprocessor = true;

    if (l.IfToken("__has_include")) {
        std::string name;
        if (l.token_data[0] == '(') {
            l.ReadRaw(name, ')', ')', '(');
            l.NextToken();
        }
        bool current_dir;
        if (name.size() >= 2 && name[0] == '"' && name[name.size() - 1] == '"') {
            current_dir = true;
        } else if (name.size() >= 2 && name[0] == '<' && name[name.size() - 1] == '>') {
            current_dir = false;
        } else {
            l.Error("__has_include expects \"FILENAME\" or <FILENAME>");
            return false;
        }
        std::string file_name = name.substr(1, name.size() - 2);

        std::string full_file_name;
        if (current_dir)
            return cparser.FindAnyIncludeFile(file_name, l.file_name, full_file_name);
        return cparser.FindGlobalIncludeFile(file_name, full_file_name);
    }

    l.WantToken(CT_IDENT);

    return 0;
}

bool CParserFile::PreprocessorIfdefCheck(CString id) {
    if (id == "__has_include")
        return true;
    return l.FindMacro(id);
}

void CParserFile::PreprocessorIfdef() {
    std::string id;
    if (!l.WantIdent(id))
        return;
    if (!l.WantToken(CT_EOF))
        return;

    PreprocessorIfAny(PreprocessorIfdefCheck(id));
}

void CParserFile::PreprocessorIfndef() {
    std::string id;
    if (!l.WantIdent(id))
        return;
    if (!l.WantToken(CT_EOF))
        return;

    PreprocessorIfAny(!PreprocessorIfdefCheck(id));
}

void CParserFile::PreprocessorUndef() {
    std::string id;
    if (!l.WantIdent(id))
        return;
    if (!l.WantToken(CT_EOF))
        return;

    l.PreprocessorLeave();
    l.DeleteMacro(id);
}

void CParserFile::PreprocessorElse() {
    if (!l.WantToken(CT_EOF))
        return;
    if (!l.PreprocessorElse(false))
        l.Error("#else without #if");  // gcc
}

void CParserFile::PreprocessorElif() {
    if (!l.PreprocessorElse(true))
        l.Error("#elif without #if");  // gcc
}

void CParserFile::PreprocessorEndif() {
    if (!l.WantToken(CT_EOF))
        return;
    if (!l.PreprocessorEndIf())
        l.Error("#endif without #if");  // gcc
}
