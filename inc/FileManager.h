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

    ~FileManager()
    {
        SignalBus::unsubscribe<FileCreatedEvent>(this);
        SignalBus::unsubscribe<FileRemovedEvent>(this);
    }

private:
    FileManager()
    {
        SignalBus::subscribe(&FileManager::FileCreatedEventHandler, this);
        SignalBus::subscribe<FileRemovedEvent>([&](auto t){
            opened_files_count--;
            std::cout << "File was removed: " << t.filename << ". Files opened: " << opened_files_count << "\n";
        }, this);
    }

    void FileCreatedEventHandler(FileCreatedEvent event){
        opened_files_count++;
        std::cout << "New file created: " << event.filename << ". Files opened: " << opened_files_count << "\n";
        last_created_file = event.filename;
    }

    std::string last_created_file;
    int opened_files_count = 0;
};