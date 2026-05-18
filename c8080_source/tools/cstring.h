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

#pragma once

#include <string>
#include <assert.h>
#include <string.h>
#include <limits>

class CString {
private:
    const char *string_start;
    mutable size_t string_size;

public:
    static constexpr size_t npos = std::numeric_limits<size_t>::max();

    CString() = delete;

    CString(const std::string &a) {
        string_start = a.c_str();
        string_size = a.size();
        assert(string_start != nullptr);
    }

    CString(const char a[]) {
        string_start = a;
        string_size = std::numeric_limits<size_t>::max();
        assert(string_start != nullptr);
    }

    CString(const char a[], size_t s) {
        string_start = a;
        string_size = s;
        assert(string_start != nullptr);
    }

    CString(const CString &a) {
        string_start = a.string_start;
        string_size = a.string_size;
        assert(string_start != nullptr);
    }

    const char operator[](size_t n) const {
        return string_start[n];
    }

    size_t size() const {
        if (string_size == std::numeric_limits<size_t>::max())
            string_size = strlen(string_start);
        return string_size;
    }

    bool empty() const {
        return size() == 0;
    }

    const char *c_str() const {
        return string_start;
    }

    const char *data() const {
        return string_start;
    }

    operator std::string() const {
        return std::string(string_start, size());
    }

    std::string std() const {
        return std::string(string_start, size());
    }

    size_t rfind(char c) const {
#if defined(__MINGW32__)
        const char *p = string_start + size();
        while (p != string_start) {
            p--;
            if (*p == c)
                return p - string_start;
        }
        return npos;
#elif defined(__APPLE__)
        const char *p = string_start + size();
        while (p != string_start) {
            p--;
            if (*p == c)
                return p - string_start;
        }
        return npos;
#else
        const char *const p = static_cast<const char *>(memrchr(string_start, c, size()));
        return p != nullptr ? p - string_start : npos;
#endif
    }

    size_t find(char c, size_t start = 0) const {
        const char *const p = static_cast<const char *>(memchr(string_start + start, c, size() - start));
        return p != nullptr ? p - string_start : npos;
    }

    std::string substr(size_t start, size_t size) {
        return std::string(string_start + start, size);
    }

    // +

    friend std::string operator+(const char a[], const CString &b) {
        return a + b.std();
    }

    friend std::string operator+(const CString &a, const char b[]) {
        return a.std() + b;
    }

    friend std::string operator+(const CString &a, const CString &b) {
        return a.std() + b.std();
    }

    friend std::string operator+(const std::string &a, const CString &b) {
        return a + b.std();
    }

    friend std::string operator+(const CString &a, const std::string &b) {
        return a.std() + b;
    }

    // ==

    friend bool operator==(const char a[], const CString &b) {
        assert(a != nullptr);
        const size_t s = strlen(a);
        return s == b.size() && 0 == memcmp(a, b.string_start, s);
    }

    friend bool operator==(const CString &a, const char b[]) {
        assert(b != nullptr);
        const size_t s = strlen(b);
        return s == a.size() && 0 == memcmp(a.string_start, b, s);
    }

    friend bool operator==(const CString &a, const CString &b) {
        const size_t s = a.size();
        return s == b.size() && 0 == memcmp(a.string_start, b.string_start, s);
    }

    friend bool operator==(const std::string &a, const CString &b) {
        const size_t s = a.size();
        return s == b.size() && 0 == memcmp(a.c_str(), b.string_start, s);
    }

    friend bool operator==(const CString &a, const std::string &b) {
        const size_t s = a.size();
        return s == b.size() && 0 == memcmp(a.string_start, b.c_str(), s);
    }

    // !=

    friend bool operator!=(const char a[], const CString &b) {
        return !(a == b);
    }

    friend bool operator!=(const CString &a, const char b[]) {
        return !(a == b);
    }

    friend bool operator!=(const CString &a, const CString &b) {
        return !(a == b);
    }

    friend bool operator!=(const std::string &a, const CString &b) {
        return !(a == b);
    }

    friend bool operator!=(const CString &a, const std::string &b) {
        return !(a == b);
    }

    // <

    friend bool operator<(const char a[], const CString &b) {
        assert(a != nullptr);
        const size_t s = strlen(a), bs = b.size();
        if (s == bs)
            return 0 < memcmp(a, b.string_start, s);
        return s < bs;
    }

    friend bool operator<(const CString &a, const char b[]) {
        assert(b != nullptr);
        const size_t s = a.size(), bs = strlen(b);
        if (s == bs)
            return 0 < memcmp(a.string_start, b, s);
        return s < bs;
    }

    friend bool operator<(const CString &a, const CString &b) {
        const size_t s = a.size(), bs = b.size();
        if (s == bs)
            return 0 < memcmp(a.string_start, b.string_start, s);
        return s < bs;
    }

    friend bool operator<(const std::string &a, const CString &b) {
        const size_t s = a.size(), bs = b.size();
        if (s == bs)
            return 0 < memcmp(a.c_str(), b.string_start, s);
        return s < bs;
    }

    friend bool operator<(const CString &a, const std::string &b) {
        const size_t s = a.size(), bs = b.size();
        if (s == bs)
            return 0 < memcmp(a.string_start, b.c_str(), s);
        return s < bs;
    }

    int CaseCmp(CString b) {
        return strcasecmp(c_str(), b.c_str());
    }
};
