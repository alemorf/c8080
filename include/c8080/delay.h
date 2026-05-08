/*
 * c8080 stdlib
 * Copyright (c) 2025 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <stdint.h>
#include <c8080/c8080.h>
#ifndef __C8080_CPU_MHZ
#include <c8080/cpumhz.h>
#endif

void __global Delay(uint16_t n) __link("c8080/delay.c");

#define DELAY_MS(MS) Delay(((unsigned long)(MS)*__C8080_CPU_MHZ + 20000) / 40000)
