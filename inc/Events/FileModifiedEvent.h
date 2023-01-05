#pragma once

struct FileModifiedEvent {
    explicit FileModifiedEvent(char* name) : filename(name) {};
    char* filename;
};