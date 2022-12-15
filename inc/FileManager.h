#pragma once
#include "Events/FileCreatedEvent.h"
#include "Events/FileRemovedEvent.h"
#include "SignalBus.h"

class FileManager
{

public:
    static std::shared_ptr<FileManager> GetFileManager()
    {
        static std::shared_ptr<FileManager> instance(new FileManager);
        return instance;
    }

private:
    FileManager()
    {
        auto sb = SignalBus::GetSignalBus();
        sb->Subscribe<FileCreatedEvent>([&](auto event){
            std::cout << "New file created: " << event.filename << "\n";
            last_created_file = event.filename;
            opened_files_count++;
            }, this);

        sb->Subscribe<FileRemovedEvent>([&](auto event){
            std::cout << "File was removed: " << event.filename << "\n";
            opened_files_count--;
            }, this);
    }
    std::string last_created_file;
    int opened_files_count = 0;
};