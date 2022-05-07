#pragma once

#include <stdint.h>
#include <string>
#include <vector>

using u8 = uint8_t;
using u32 = uint32_t;
using u64 = uint64_t;

enum class Error
{
    None,
    Error,
    NoSuchFile,
    InvalidChunkSize,
};

struct FourCC
{
    FourCC() : value(0)
    {
    }

    FourCC(const FourCC& other) : value(other.value)
    {
    }

    FourCC& operator=(const FourCC& other)
    {
        value = other.value;
        return *this;
    }

    explicit FourCC(u32 val) : value(val)
    {
    }
    explicit FourCC(char val[4])
    {
        memcpy(values, val, 4 * sizeof(char));
    }

    bool operator==(const char* str) const
    {
        static char tmp[5] = {
            ' ', ' ', ' ', ' ', '\0',
        };

        memcpy(tmp, values, 4 * sizeof(char));

        return strcmp(tmp, str) == 0;
    }

    std::string ToStr() const
    {
        static char tmp[5] = {
            ' ', ' ', ' ', ' ', '\0',
        };

        memcpy(tmp, values, 4 * sizeof(char));

        return tmp;
    }

    union
    {
        u32 value;
        char values[4];
    };
};

struct RiffChunk
{
    FourCC id{FourCC(0u)};
    FourCC formType{FourCC(0u)};
    u32 length{0};

    u8* data{nullptr};

    std::vector<std::size_t> childrenIndices;
};

class Riff
{
public:
    Riff();
    ~Riff();

    Error LoadFrom(const std::string& path);

    RiffChunk& Get()
    {
        return chunks[0];
    }

    const RiffChunk& Get() const
    {
        return chunks[0];
    }

    void DumpToStdOut() const;

    std::vector<RiffChunk> chunks;
};