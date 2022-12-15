#pragma once
#include <utility>

#include "SignalBus.h"
#include "Events/FileCreatedEvent.h"
#include "Events/FileRemovedEvent.h"

class File
{

public:

    static std::shared_ptr<File> CreateNewFile(std::string name)
    {
        return std::shared_ptr<File>(new File(std::move(name)));
    }

    ~File()
    {
        auto sb = SignalBus::GetSignalBus();
        sb->Fire(FileRemovedEvent(filename));
    }

private:

    explicit File(std::string name)
    {
        filename = std::move(name);
        auto sb = SignalBus::GetSignalBus();
        sb->Fire(FileCreatedEvent(filename));
    }

    std::string filename;
};