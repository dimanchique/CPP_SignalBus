#pragma once

struct FileRemovedEvent {
    explicit FileRemovedEvent(char* name) : filename(name) {};
    char* filename;
};