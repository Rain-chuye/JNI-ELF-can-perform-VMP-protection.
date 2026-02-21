#ifndef VMP_ENGINE_H
#define VMP_ENGINE_H

#include "elf_parser.h"
#include <functional>
#include <string>

class VmpEngine {
public:
    VmpEngine(ElfParser* parser);
    void setLogCallback(std::function<void(const std::string&)> cb) { mLogCallback = cb; }
    void protect();

private:
    ElfParser* mParser;
    std::function<void(const std::string&)> mLogCallback;
    void log(const std::string& msg);
};

#endif // VMP_ENGINE_H
