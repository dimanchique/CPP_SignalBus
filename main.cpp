#include "FileManager.h"
#include "File.h"

int main() {
    auto Manager = FileManager::GetFileManager();
    auto f1 = File::CreateNewFile("File1");
    auto f2 = File::CreateNewFile("File2");
    auto f3 = File::CreateNewFile("File3");
    auto f4 = File::CreateNewFile("File4");
    auto f5 = File::CreateNewFile("File5");
    return 0;
}
