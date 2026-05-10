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

#include "staticstack.h"
#include "../c/cnodelist.h"
#include "../c/tools/makeoperator.h"
#include "../c/tools/cthrow.h"

// Calculate stack position at compile time
// Replace stack variables with global variables

static bool LastAgumentInCpuRegister(Prepare &p, CVariable &f) {
    if (p.programm.GetVariableMode(f.type) == CVM_GLOBAL && f.type.function_args.size() >= 2u &&
        !f.type.many_function_args) {  // Min 1 argument
        switch (f.type.function_args.back().type.GetAsmType()) {
            case CBT_CHAR:
            case CBT_UNSIGNED_CHAR:  // 8 bit
            case CBT_SHORT:
            case CBT_UNSIGNED_SHORT:  // 16 bit
            case CBT_LONG:
            case CBT_UNSIGNED_LONG:  // 32 bit
                return true;
        }
    }
    return false;
}

static void PrepareFunctionStaticStack(Prepare &p, CVariable &f) {
    if (p.programm.GetVariableMode(f.type) != CVM_GLOBAL)
        return;  // Standard stack is used

    if (f.c.static_stack != nullptr)
        return;  // Already executed

    // Allocate static stack
    if (p.programm.c.static_stack == nullptr) {
        auto v = std::make_shared<CVariable>();
        v->output_name = "__static_stack";
        v->name = "?__static_stack";
        v->c.use_counter++;  // Always used
        v->type.base_type = CBT_UNSIGNED_CHAR;
        v->type.pointers.push_back(CPointer(1));  // .count will be filled in the last step
        p.programm.AddVariable(v);
        p.programm.c.static_stack = v;
    }

    // Allocate function static stack
    auto s = std::make_shared<CVariable>();
    s->name = "?__s_" + f.name;
    s->output_name = "__s_" + f.name;
    s->c.use_counter++;  // Always used
    s->type.base_type = CBT_UNSIGNED_CHAR;
    s->type.pointers.push_back(CPointer());
    p.programm.AddVariable(s);
    f.c.static_stack = s;

    f.function_stack_frame_size_0 = f.function_stack_frame_size;

    // Allocate space for function arguments in the static stack
    for (size_t i = 0; i < f.function_arguments.size(); i++) {
        auto &arg = f.function_arguments[i];
        assert(arg != nullptr && arg->is_stack_variable && arg->is_function_argument && !arg->only_extern);
        arg->output_name = "__a_" + std::to_string(i + 1u) + "_" + f.output_name;
        arg->c.use_counter++;            // Always used
        arg->is_stack_variable = false;  // Now it is a global variable / equ
        arg->c.equ_enabled = true;       // TODO: Replace to OPERATOR.ADD()
        arg->c.equ_text = s->output_name + " + " + std::to_string(f.function_stack_frame_size);
        p.programm.AddVariable(arg);

        f.function_stack_frame_size += arg->type.SizeOf(arg->e);
    }
}

bool PrepareStaticArgumentsCall(Prepare &p, CNodePtr &node) {
    if (!p.programm.cmm && node->type == CNT_FUNCTION_CALL && node->a != nullptr &&
        p.programm.GetVariableMode(node->variable->type) == CVM_GLOBAL) {
        PrepareFunctionStaticStack(p, *node->variable);

        std::vector<CStructItem> &arg_list = node->variable->type.function_args;
        std::vector<CVariablePtr> &arg_variables = node->variable->function_arguments;

        CNodePtr arg = node->a;
        node->a = nullptr;

        CNodeList body;
        size_t arg_n = 1;
        size_t offset = node->variable->function_stack_frame_size_0;
        while (arg) {
            if (!node->variable->type.many_function_args && arg_n == arg_list.size())
                p.programm.Error(node->e, "too many arguments");

            CNodePtr next_arg = arg->next_node;
            arg->next_node = nullptr;

            CNodePtr command = nullptr;
            if (arg_n + 1 == arg_list.size() && LastAgumentInCpuRegister(p, *node->variable) &&
                !node->variable->type.many_function_args) {
                command = CNODE({CNT_SAVE_TO_REGISTER, a : arg, ctype : arg_list[arg_n].type, e : arg->e});
            } else {
                std::string arg_text;
                CType arg_type;
                if (arg_n - 1 < arg_variables.size() && arg_n < arg_list.size()) {
                    CVariablePtr &av = arg_variables[arg_n - 1];
                    assert(av != nullptr);
                    arg_text = av->output_name;
                    arg_type = arg_list[arg_n].type;
                    offset += arg_type.SizeOf(arg->e);
                } else {
                    arg_text = node->variable->c.static_stack->output_name + " + " + std::to_string(offset);
                    arg_type = arg->ctype;
                    if (arg_type.IsArray())
                        arg_type.pointers.back().ResetArray();
                    if (arg_type.pointers.empty())
                        arg_type.flag_const = false;
                    else
                        arg_type.pointers.back().flag_const = false;
                    size_t s = arg_type.SizeOf(arg->e);
                    /* Для работы printf. В Си все аргументы выравниваются на int, в этом компиляторе только vararg. */
                    if (s < C_SIZEOF_INT)
                        s = C_SIZEOF_INT;
                    offset += s;
                }

                assert(!arg_text.empty());

                CNodePtr aa = CNODE({
                    CNT_CONST,
                    ctype : arg_type,
                    text : arg_text,
                    e : arg->e
                });  // TODO: Replace with CNT_LOAD_VARIABLE

                aa->ctype.pointers.push_back(CPointer());

                CNodePtr a =
                    CNODE({CNT_MONO_OPERATOR, a : aa, ctype : arg_type, mono_operator_code : MOP_DEADDR, e : arg->e});

                command = MakeOperator(COP_SET, a, arg, arg->e, p.programm.cmm);
            }
            body.PushBack(command);

            arg = next_arg;
            arg_n++;
        }

        if (arg_n < arg_list.size())
            C_ERROR_INTERNAL(node, "not enough arguments");

        node->c = body.first;

        if (node->variable->function_stack_frame_size < offset)
            node->variable->function_stack_frame_size = offset;

        return true;
    }
    return false;
}

bool PrepareStaticLoadVariable(Prepare &p, CNodePtr &node) {
    // Must be called before PrepareLoadVariable()
    if (node->type == CNT_LOAD_VARIABLE) {
        CVariablePtr &v = node->variable;
        assert(v != nullptr);

        // Replace stack variables with global variables if a static stack is used
        if (v->is_stack_variable && p.function != nullptr &&
            p.programm.GetVariableMode(p.function->type) == CVM_GLOBAL) {
            assert(!v->is_function_argument);  // Replaced before in PrepareFunctionStaticStack()
            v->c.equ_enabled = true;
            v->c.equ_text = p.function->c.static_stack->output_name + " + " + std::to_string(v->stack_offset);

            v->is_stack_variable = false;
            v->output_name = p.function->name + "_" + v->name;
            p.programm.AddVariable(v);
            return true;
        }
    }
    return false;
}

static void PrepareLastFunctionArgCode(Prepare &p) {
    if (p.programm.cmm || !p.function)
        return;

    CVariable &f = *p.function;
    if (p.programm.GetVariableMode(f.type) == CVM_GLOBAL && LastAgumentInCpuRegister(p, f) &&
        !f.type.many_function_args) {
        CVariablePtr &register_argument = p.function->function_arguments.back();
        if (p.programm.asm_names.find(register_argument->output_name) == p.programm.asm_names.end()) {
            CNodePtr a = CNODE({
                CNT_OPERATOR,
                next_node : p.function->body->a,
                operator_code : COP_SET,
                e : p.function->e,

            });
            a->a = CNODE({CNT_LOAD_VARIABLE});
            a->a->variable = register_argument;
            a->a->ctype = register_argument->type;
            a->b = CNODE({CNT_LOAD_FROM_REGISTER});
            a->b->ctype = register_argument->type;
            a->ctype = register_argument->type;
            p.function->body->a = a;
        }
    }
}

void CalculateStaticStack(CProgramm &p) {
    if (p.c.static_stack == nullptr)
        return;  // There are no functions using a static stack

    // Calculate stack frame position for each function (by call tree)
    for (;;) {
        bool done = true;
        for (auto &i : p.all_top_variables) {
            if (i->c.use_counter > 0 && i->type.IsFunction() && i->c.call_count == 0 && i->c.called_by.size() > 0 &&
                !i->c.called_by_processed) {
                const size_t stack_end = i->c.function_stack_frame_offset + i->function_stack_frame_size;
                for (const auto &j : i->c.called_by) {
                    auto c = j.function.lock();

                    // Calculate stack frame offset
                    if (c->c.function_stack_frame_offset < stack_end)
                        c->c.function_stack_frame_offset = stack_end;

                    assert(c->c.call_count > 0);
                    if (c->c.call_count > 0) {
                        c->c.call_count--;
                        if (c->c.call_count == 0)
                            done = false;
                    }
                }
                i->c.called_by_processed = true;
            }
        }
        if (done)
            break;
    }

    size_t stack_size = 0;
    for (auto &x : p.all_top_variables) {
        if (x->type.IsFunction() && x->c.static_stack != nullptr) {
            // Check recursion
            if (!x->c.called_by_processed && x->c.called_by.size() > 0) {
                CVariablePtr v = x->c.called_by[0].function.lock();
                CThrow(x->e, "Recursion is not supported (" + x->name + " called by " + (v ? v->name : nullptr) + ")");
            }

            // Update function static_stack varaible
            x->c.static_stack->c.equ_enabled = true;  // TODO: Replace with CNT_LOAD_VARIABLE
            x->c.static_stack->c.equ_text =
                p.c.static_stack->output_name + " + " + std::to_string(x->c.function_stack_frame_offset);

            // Calculate stack size
            const size_t stack_end = x->c.function_stack_frame_offset + x->function_stack_frame_size;
            if (stack_size < stack_end)
                stack_size = stack_end;
        }
    }

    p.c.static_stack->type.pointers[0].SetArray(stack_size);
}

void PrepareFunctionStaticStack(Prepare &p) {
    PrepareLastFunctionArgCode(p);
    PrepareFunctionStaticStack(p, *p.function);
}
