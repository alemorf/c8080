/*
 * c8080 compiler
 * Copyright (c) 2025 Aleksey Morozov
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

#include "index.h"
#include "common.h"

namespace I8080 {

typedef std::map<std::string, size_t> Saves;

class StateRegister {
public:
    AsmArgument value;
    AsmArgument variable;
};

class State {
public:
    StateRegister a, hl, de;
};

class StateItem {
public:
    size_t used = 0;
    State state;
};

static void CombineState(State &a, const State &b) {
    if (a.a.value != b.a.value)
        a.a.value.SetNone();
    if (a.a.variable != b.a.variable)
        a.a.variable.SetNone();
    if (a.hl.value != b.hl.value)
        a.hl.value.SetNone();
    if (a.hl.variable != b.hl.variable)
        a.hl.variable.SetNone();
    if (a.de.value != b.de.value)
        a.de.value.SetNone();
    if (a.de.variable != b.de.variable)
        a.de.variable.SetNone();
}

static inline void ResetState(State &s) {
    s.a.value.SetNone();
    s.a.variable.SetNone();
    s.hl.value.SetNone();
    s.hl.variable.SetNone();
    s.de.value.SetNone();
    s.de.variable.SetNone();
}

static StateRegister *ResetState(State &s, AsmRegister reg, bool saveValue = false) {
    switch (reg) {
        case R16_AF:
        case R8_A:
            if (!saveValue)
                s.a.value.SetNone();
            s.a.variable.SetNone();
            return &s.a;
        case R8_H:
        case R8_L:
            s.hl.value.SetNone();
            s.hl.variable.SetNone();
            return nullptr;
        case R16_HL:
            if (!saveValue)
                s.hl.value.SetNone();
            s.hl.variable.SetNone();
            return &s.hl;
        case R8_D:
        case R8_E:
            s.de.value.SetNone();
            s.de.variable.SetNone();
            return nullptr;
        case R16_DE:
            if (!saveValue)
                s.de.value.SetNone();
            s.de.variable.SetNone();
            return &s.de;
        case R8_B:
        case R8_C:
        case R16_BC:
            return nullptr;
        default:
            assert(false);
    }
    return nullptr;
}

static bool AddSave(AsmBase &a, Saves &saves, const AsmArgument &variable, uint64_t position) {
    if (variable.type != AAT_STRING)
        return false;
    Saves::iterator p = saves.find(variable.string);
    if (p != saves.end()) {
        a.lines[p->second].opcode = AC_REMOVED;
        p->second = position;
        return true;
    }
    saves[variable.string] = position;
    return false;
}

static void RemoveSave(Saves &saves, const AsmArgument &variable) {
    if (variable.type != AAT_STRING)
        return;
    if (variable.string.find('(') != std::string::npos) {
        // Это формула содержая один или несколько адресов
        // переменных. Значение формулы используется как адрес
        // переменной. Нам придется сохранить все переменные.

        // Например, эту строку нельзя удалять:
        //   ld  (var), a  <--
        //   ...
        //   ld  hl, var
        //   add (hl)

        saves.clear();
        return;
    }
    Saves::iterator p = saves.find(variable.string);
    if (p != saves.end())
        saves.erase(p);
}

static bool OptimizeMviA(State &s, AsmBase::Line& l) {
    // MOV M, A невозможно ускорить
    if (l.argument[0].reg == R8_M)
        return false;

    StateRegister *reg_state = ResetState(s, l.argument[0].reg, true);
    if (reg_state == nullptr)
        return false;

    // Удаление LD REG, CONST, если регистр уже содержит нужное значение
    if (reg_state->value == l.argument[1]) {
        l.opcode = AC_REMOVED;
        l.argument[0] = AsmArgument();
        l.argument[1] = AsmArgument();
        return true;
    }

    const AsmArgument prev_value = reg_state->value;
    reg_state->value = l.argument[1];

    if (l.argument[0].reg == R8_A) {
        // Замена LD A, 0 на XOR A
        if (l.argument[1].Is0()) {
            l.opcode = AC_ALU_REG;
            l.alu = ALU_XOR;
            l.argument[1] = AsmArgument();
            return true;
        }
        if (prev_value.type == AAT_NUMBER && l.argument[1].type == AAT_NUMBER) {
            // Замена LD A, CONST на INC A
            if (l.argument[1].number == uint8_t(prev_value.number + 1)) {
                l.opcode = AC_INC;
                l.argument[1] = AsmArgument();
                return true;
            }
            // Замена LD A, CONST на DEC A
            if (l.argument[1].number == uint8_t(prev_value.number - 1)) {
                l.opcode = AC_DEC;
                l.argument[1] = AsmArgument();
                return true;
            }
            // Замена LD A, CONST на ADD A
            if (l.argument[1].number == uint8_t(prev_value.number << 1)) {
                l.opcode = AC_ALU_REG;
                l.alu = ALU_ADD;
                l.argument[1] = AsmArgument();
                return true;
            }
            // Замена LD A, CONST на CPL
            if (l.argument[1].number == uint8_t(~prev_value.number)) {
                l.opcode = AC_CMA;
                l.argument[0] = AsmArgument();
                l.argument[1] = AsmArgument();
                return true;
            }
        }
    }

    // TODO: Загрузить значение из других регистров

    return false;
}

bool LoadSave(AsmBase &a, std::map<size_t, StateItem> &states, bool jb) {
    bool changed = false;
    State s;
    ResetState(s);
    size_t i = size_t(0) - size_t(1);
    bool noState = true;
    Saves saves;
    for (auto &l : a.lines) {
        i++;
        if (noState && l.opcode != AC_LABEL)
            continue;
        switch (l.opcode) {
            case AC_RET:
                ResetState(s);
                noState = true;
                saves.clear();  // Сохранить всё перед переходом
                break;
            case AC_JMP:
            case AC_JMP_CONDITION: {
                saves.clear();  // Сохранить всё перед переходом
                if (l.argument[0].type == AAT_LABEL) {
                    //                    assert(l.argument[0].label->destination - 1 > i);  // Только переход вперед
                    StateItem &ds = states[l.argument[0].label->destination - 1];
                    ds.used++;
                    if (ds.used == 1) {
                        ds.state = s;
                    } else {
                        CombineState(ds.state, s);
                    }
                }
                if (l.opcode == AC_JMP) {
                    ResetState(s);
                    noState = true;
                }
                break;
            }
            case AC_LABEL: {
                auto fs = states.find(i);
                if (fs != states.end() && fs->second.used >= 1 && (jb || !l.argument[0].label->jump_back)) {
                    if (noState) {
                        s = fs->second.state;
                    } else {
                        CombineState(s, fs->second.state);
                    }
                    noState = false;
                    break;
                }
                ResetState(s);
                noState = false;
                break;
            }
            case AC_STA:
                changed |= AddSave(a, saves, l.argument[0], i);
                s.a.variable = l.argument[0];  // TODO: Может изменить переменную. Можно удалить лишнее.

                // Замена LD (CONST), A на LD (HL), A
                if (s.hl.value == s.a.variable) {
                    l.opcode = AC_MOV;
                    l.argument[1].Set(R8_A);
                    l.argument[0].Set(R8_M);
                    changed = true;
                }
                break;
            case AC_SHLD:
                changed |= AddSave(a, saves, l.argument[0], i);
                s.hl.variable = l.argument[0];  // TODO: Может изменить переменную. Можно удалить лишнее.
                break;
            case AC_ALU_REG:
                if (l.alu == ALU_CMP)
                    break;  // CMP не изменяет регистры
                if ((l.alu == ALU_OR || l.alu == ALU_AND) && l.argument[0].reg == R8_A)
                    break;  // Команды OR A и AND A не изменяют регистры
                if (l.alu == ALU_XOR && l.argument[0].reg == R8_A) {
                    if (s.a.value.Is0()) {
                        l.opcode = AC_REMOVED;
                        changed = true;
                        break;
                    }
                    ResetState(s, R8_A);
                    s.a.value.Set(uint16_t(0));
                    break;
                }
                ResetState(s, R8_A);
                break;
            case AC_ALU_CONST:
                if (l.alu == ALU_CMP)
                    break;  // CMP не изменяет регистры
                if ((l.alu == ALU_OR || l.alu == ALU_XOR || l.alu == ALU_ADD || l.alu == ALU_SUB) &&
                    l.argument[0].Is0())
                    break;  // Команды OR 0, XOR 0, ADD 0, SUB 0 не изменяют регистры
                ResetState(s, R8_A);
                break;
            case AC_LHLD:
                if (s.hl.variable == l.argument[0]) {  // Если HL содержит адрес переменной
                    l.opcode = AC_REMOVED;
                    changed = true;
                    continue;
                }
                RemoveSave(saves, l.argument[0]);
                ResetState(s, R16_HL);
                s.hl.variable = l.argument[0];
                break;
            case AC_LDA:
                if (s.a.variable == l.argument[0]) {  // Если A содержит адрес переменной
                    l.opcode = AC_REMOVED;
                    changed = true;
                    continue;
                }
                RemoveSave(saves, l.argument[0]);
                ResetState(s, R8_A);
                s.a.variable = l.argument[0];
                break;
            case AC_LXI:
            case AC_MVI: {
                assert(l.argument[0].type == AAT_REG);
                RemoveSave(saves, l.argument[1]);  // Конструкция: ld hl, var / add (hl)
                if (OptimizeMviA(s, l))
                    changed = true;
                break;
            }
            case AC_XCHG:
                std::swap(s.hl, s.de);
                break;
            case AC_DAD:
                ResetState(s, R16_HL);  // TODO: Можно вычислить
                break;
            case AC_INC:
            case AC_DEC:
                assert(l.argument[0].type == AAT_REG);
                if (l.argument[0].reg != R16_SP)
                    ResetState(s, l.argument[0].reg);  // TODO: Можно вычислить
                break;
            case AC_MOV:
                RemoveSave(saves, l.argument[1]);  // Конструкция: ld hl, var / add (hl)
                if (l.argument[0].reg == R8_M)
                    break;
                ResetState(s, l.argument[0].reg);  // TODO: Можно вычислить. Обработать пару hl, de.
                break;
            case AC_POP:
                assert(l.argument[0].type == AAT_REG);
                ResetState(s, l.argument[0].reg);
                break;
            case AC_LXI_STACK_ADDR:
            case AC_LXI_ARG_STACK_ADDR:
                assert(l.argument[0].type == AAT_REG);
                ResetState(s, l.argument[0].reg);  // TODO: Можно вычислить
                break;
            case AC_LDAX:
                ResetState(s, R8_A);  // TODO: Можно вычислить
                break;
            case AC_STAX:
                // TODO: Может изменить переменную
                break;
            case AC_RAL:
            case AC_RAR:
            case AC_RRC:
            case AC_RLC:
            case AC_CMA:
                ResetState(s, R8_A);  // TODO: Можно вычислить
                break;
            // Can't change registers
            case AC_SPHL:
            case AC_LINE:
            case AC_REMARK:
            case AC_REMOVED:
            case AC_PUSH:
            case AC_STACK_CORRECTION:
            case AC_STACK_CORRECTION_RESET:
                break;
            case AC_RET_CONDITION:
                saves.clear();  // Must be saved before exit
                break;

            // Can change all registers
            case AC_ASSEMBLER:
            case AC_CALL:
            case AC_CALL_CONDITION:
                saves.clear();  // Must be saved before call
                ResetState(s);
                break;
            default:
                saves.clear();  // Unknown command
                ResetState(s);
        }
    }
    return changed;
}

bool AsmOptimizeLoadSave(AsmBase &a) {
    size_t i = 0;
    for (auto &l : a.lines) {
        if ((l.opcode == AC_JMP || l.opcode == AC_JMP_CONDITION) && l.argument[0].type == AAT_LABEL)
            if (l.argument[0].label->destination - 1 <= i)
                l.argument[0].label->jump_back = true;
        i++;
    }

    std::map<size_t, StateItem> states;

    bool result = LoadSave(a, states, false);
    result |= LoadSave(a, states, true);
    return result;
}

}  // namespace I8080
