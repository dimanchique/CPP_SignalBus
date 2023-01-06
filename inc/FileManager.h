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
        auto sb = SignalBus::GetSignalBus();
        sb->Unsubscribe<FileCreatedEvent>(this);
        sb->Unsubscribe<FileRemovedEvent>(this);
    }

private:
    FileManager()
    {
        auto sb = SignalBus::GetSignalBus();
        sb->Subscribe(&FileManager::FileCreatedEventHandler, this);
        sb->Subscribe(&FileManager::FileRemovedEventHandler, this);
    }

    void FileRemovedEventHandler(FileRemovedEvent event){
        opened_files_count--;
        std::cout << "File was removed: " << event.filename << ". Files opened: " << opened_files_count << "\n";
    }

    void FileCreatedEventHandler(FileCreatedEvent event){
        opened_files_count++;
        std::cout << "New file created: " << event.filename << ". Files opened: " << opened_files_count << "\n";
        last_created_file = event.filename;
    }

    std::string last_created_file;
    int opened_files_count = 0;
};