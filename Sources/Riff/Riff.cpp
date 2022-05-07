#include "Riff/Riff.hpp"

#include <fstream>
#include <iostream>

#define CheckReturnCode(expr, Code) \
    if ((expr) != Error::None)      \
    {                               \
        __debugbreak();             \
        return Code;                \
    }

#define CheckBoolReturnCode(expr, Code) \
    if (!(expr))                        \
    {                                   \
        __debugbreak();                 \
        return Code;                    \
    }

#define CheckReturn(expr) CheckReturnCode(expr, Error::Error)
#define CheckBoolReturn(expr) CheckBoolReturnCode(expr, Error::Error)

struct FileReader
{
    FileReader()
    {
    }

    Error Open(std::string path)
    {
        in.open(path, std::ifstream::binary);

        if (!in.is_open())
            throw Error::NoSuchFile;

        in.seekg(0, in.end);
        fileLength = in.tellg();
        in.seekg(0, in.beg);

        return Error::None;
    }

    ~FileReader()
    {
        in.close();
    }

    bool EoF()
    {
        return fileLength == currentPtrIndex;
    }

    Error Read(void* data, u32 length)
    {
        in.read((char*) data, length);

        currentPtrIndex = in.tellg();

        return in.bad() ? Error::Error : Error::None;
    }

    std::ifstream in;
    u64 fileLength;
    u64 currentPtrIndex;
};

Error ReadChunk(FileReader& reader, Riff& riff, std::size_t chunkIndex)
{
    bool haveChildren = false;
    RiffChunk* chunk = &riff.chunks[chunkIndex];

    CheckReturn(reader.Read(&chunk->id.value, 4));
    CheckReturn(reader.Read(&chunk->length, 4));

    if (chunk->id == "RIFF" || chunk->id == "LIST")
    {
        haveChildren = true;
        CheckReturn(reader.Read(&chunk->formType.value, 4));
    }

    auto start = reader.currentPtrIndex;
    if (haveChildren)
    {
        auto actualLength = chunk->length - (haveChildren ? 4 : 0);  // minus chunk formType if there is one
        while ((reader.currentPtrIndex - start) < actualLength)
        {
            auto newChunkIndex = riff.chunks.size();
            chunk->childrenIndices.push_back(newChunkIndex);

            riff.chunks.resize(riff.chunks.size() + 1);
            CheckReturn(ReadChunk(reader, riff, newChunkIndex));
            chunk = &riff.chunks[chunkIndex];  // resize might have affected the ptr of chunk (new alloc) and resizes
                                               // inside ReadChunk as well

            // padding byte if the size is not even
            if ((chunk->length % 2) != 0)
            {
                char oneByte;
                reader.Read(&oneByte, 1);
            }
        }
    }
    else
    {
        // Copy data (for visualisation)
        chunk->data = (u8*) malloc(chunk->length * sizeof(u8));
        CheckReturn(reader.Read(chunk->data, chunk->length));
    }

    return Error::None;
}

Riff::Riff()
{
}

Riff::~Riff()
{
    for (auto chunk : chunks)
    {
        if (chunk.data != nullptr)
            free(chunk.data);
    }
}

Error Riff::LoadFrom(const std::string& path)
{
    FileReader reader;
    reader.Open(path);

    chunks.resize(1);

    CheckReturn(ReadChunk(reader, *this, 0));
    CheckBoolReturn(Get().id == "RIFF");

    return reader.EoF() ? Error::None : Error::InvalidChunkSize;
}

void DumpChunkToStr(const Riff& riff, const RiffChunk& chunk, int indent)
{
    std::string indentStr;
    for (int i = 0; i < indent; i++)
        indentStr += "\t";

    if (chunk.id == "LIST" || chunk.id == "RIFF")
    {
        std::cout << indentStr << chunk.id.ToStr() << " - " << chunk.formType.ToStr() << " - " << chunk.length << "\n";
    }
    else
    {
        std::cout << indentStr << chunk.id.ToStr() << " - " << chunk.length << "\n";
    }

    for (auto childIndex : chunk.childrenIndices)
    {
        DumpChunkToStr(riff, riff.chunks[childIndex], indent + 1);
    }
}

void Riff::DumpToStdOut() const
{
    DumpChunkToStr(*this, Get(), 0);
}