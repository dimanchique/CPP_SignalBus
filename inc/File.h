#pragma once
#include <utility>

#include "SignalBus.h"
#include "Events/FileCreatedEvent.h"
#include "Events/FileRemovedEvent.h"

class File
{

public:

    explicit File(std::string name)
    {
        filename = std::move(name);
        auto sb = SignalBus::GetSignalBus();
        sb->Fire(FileCreatedEvent(filename));
    }

    ~File()
    {
        auto sb = SignalBus::GetSignalBus();
        sb->Fire(FileRemovedEvent(filename));
    }

    std::string filename;
};