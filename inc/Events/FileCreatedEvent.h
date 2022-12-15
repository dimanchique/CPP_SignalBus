#pragma once
#include "string"

struct FileCreatedEvent {
    FileCreatedEvent() : filename("new_file") {};
    explicit FileCreatedEvent(std::string &name) : filename(name) {};
    std::string filename;
};