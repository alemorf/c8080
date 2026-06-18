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

#include "fs_tools.h"
#include <assert.h>
#include <sys/stat.h>
#include <string>
#include <iostream>

namespace FsTools {

class AutoCloseFile {
public:
    explicit AutoCloseFile(FILE *file);
    void Close();
    ~AutoCloseFile();

protected:
    FILE *file_ = nullptr;

    AutoCloseFile(const AutoCloseFile &) = delete;
    AutoCloseFile &operator=(const AutoCloseFile &) = delete;
};

AutoCloseFile::AutoCloseFile(FILE *file) {
    file_ = file;
}

void AutoCloseFile::Close() {
    if (file_ != nullptr) {
        if (fclose(file_) != 0) {
            std::cerr << "Can't close file, errno " << errno << std::endl;
        }
        file_ = nullptr;
    }
}

AutoCloseFile::~AutoCloseFile() {
    Close();
}

void LoadFile(CString file_name, std::function<void *(size_t)> allocate) {
    // Check parameters
    if (!allocate) {
        throw std::runtime_error(std::string(__func__) + std::string(": Incorrect parameter allocate = nullptr"));
    }

    FILE *const file = fopen(file_name.std().c_str(), "rb");
    if (file == nullptr) {
        throw std::runtime_error(std::string("Can't open file ") + std::string(file_name) + std::string(", errno ") +
                                 std::to_string(errno));
    }

    AutoCloseFile auto_close_file(file);

    struct stat buff = {};
    if (fstat(fileno(file), &buff) != 0) {
        throw std::runtime_error(std::string("Can't check file size ") + std::string(file_name) +
                                 std::string(", errno ") + std::to_string(errno));
    }

    if (buff.st_size < 0 || uint64_t(buff.st_size) > SIZE_MAX) {
        throw std::runtime_error(std::string("Too big file ") + std::string(file_name) +
                                 std::string(", current size ") + std::to_string(buff.st_size) +
                                 std::string(", max size ") + std::to_string(SIZE_MAX));
    }

    const size_t buffer_size = buff.st_size;
    void *const buffer = allocate(buffer_size);

    if ((buffer != nullptr) && (buffer_size > 0U)) {
        const auto result = fread(buffer, 1, buffer_size, file);
        if (result != buffer_size) {
            throw std::runtime_error(std::string("Can't read file ") + std::string(file_name) +
                                     std::string(", errno ") + std::to_string(errno) + std::string(", result ") +
                                     std::to_string(result) + std::string(", bufferSize ") +
                                     std::to_string(buffer_size));
        }
    }
}

void LoadFile(CString file_name, size_t max_file_size, std::vector<uint8_t> &out) {
    FsTools::LoadFile(file_name, [file_name, &out, max_file_size](size_t size) {
        if (size > max_file_size) {
            throw std::runtime_error(std::string("File ") + file_name + " longer then " +
                                     std::to_string(max_file_size) + " bytes");
        }
        out.resize(size);
        return out.data();
    });
}

void LoadFile(CString file_name, size_t max_file_size, std::string &out) {
    FsTools::LoadFile(file_name, [file_name, &out, max_file_size](size_t size) {
        if (size > max_file_size) {
            throw std::runtime_error(std::string("File ") + file_name + " longer then " +
                                     std::to_string(max_file_size) + " bytes");
        }
        out.resize(size);
        return out.data();
    });
}

void SaveFile(CString file_name, CString in) {
    SaveFile(file_name, in.data(), in.size());
}

void SaveFile(CString file_name, const std::vector<uint8_t> &in) {
    SaveFile(file_name, in.data(), in.size());
}

void SaveFile(CString file_name, const void *data, size_t size) {
    // Check parameters
    if (data == nullptr) {
        throw std::runtime_error(std::string(__func__) + std::string(": Incorrect parameter data = nullptr"));
    }

    FILE *const file = fopen(file_name.std().c_str(), "wb");

    if (file == nullptr) {
        throw std::runtime_error(std::string("Can't create file ") + std::string(file_name) + std::string(", errno ") +
                                 std::to_string(errno));
    }

    AutoCloseFile auto_close_file(file);

    if (size != 0U) {
        const auto result0 = fwrite(data, 1, size, file);
        if (result0 != size) {
            const int error_code = errno;

            auto_close_file.Close();

            if (remove(file_name.std().c_str()) != 0) {
                std::cerr << "Can't remove file " << file_name.std() << ", errno " << errno << std::endl;
            }

            throw std::runtime_error(std::string("Can't  write file ") + std::string(file_name) +
                                     std::string(", errno ") + std::to_string(error_code));
        }
    }
}

}  // namespace FsTools
