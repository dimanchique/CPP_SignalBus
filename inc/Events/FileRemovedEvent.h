#pragma once
#include "string"

struct FileRemovedEvent {
    FileRemovedEvent() : filename("new_file") {};
    explicit FileRemovedEvent(std::string &name) : filename(name) {};
    std::string filename;
};