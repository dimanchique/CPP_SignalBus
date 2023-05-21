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
        SignalBus::fire(FileCreatedEvent(filename.data()));
    }

    ~File()
    {
        SignalBus::fire(FileRemovedEvent(filename.data()));
    }

    std::string filename;
};