#pragma once

struct FileCreatedEvent {
    explicit FileCreatedEvent(char* name) : filename(name) {};
    char* filename;
};