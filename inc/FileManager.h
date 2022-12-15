#pragma once
#include "Events/FileCreatedEvent.h"
#include "Events/FileRemovedEvent.h"
#include "iostream"
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
            current_file = event.filename;
            }, this);

        sb->Subscribe<FileRemovedEvent>([&](auto event){
            std::cout << "File was removed: " << event.filename << "\n";
            if (event.filename == current_file)
                std::cout << "This was a working file!" << "\n";
            }, this);
    }
    std::string current_file;
};