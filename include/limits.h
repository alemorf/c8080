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

#define CHAR_BIT 8
#define CHAR_WIDTH 8
#define SCHAR_MIN (-128)
#define SCHAR_MAX 127
#define SCHAR_WIDTH 8
#define UCHAR_MAX 255
#define UCHAR_WIDTH 8
#define CHAR_MIN SCHAR_MIN
#define CHAR_MAX SCHAR_MAX
#define SHRT_MIN (-32768)
#define SHRT_MAX 32767
#define SHRT_WIDTH 16
#define USHRT_MAX 65535
#define USHRT_WIDTH 16
#define INT_MIN SHRT_MIN
#define INT_MAX SHRT_MAX
#define INT_WIDTH SHRT_WIDTH
#define UINT_MAX 65535
#define UINT_WIDTH USHRT_WIDTH
#define LONG_MIN (-2147483648)
#define LONG_MAX 2147483647
#define LONG_WIDTH 32
#define ULONG_MAX 4294967295
#define ULONG_WIDTH 32
#define LLONG_MIN (-9223372036854775808)
#define LLONG_MAX 9223372036854775807
#define LLONG_WIDTH 64
#define ULLONG_MAX 18446744073709551615
#define ULLONG_WIDTH 64
#define BOOL_MAX 1
#define BOOL_WIDTH 8
