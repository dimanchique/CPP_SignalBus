#include "FileManager.h"
#include "File.h"

int main() {
    auto Manager = FileManager::GetFileManager();
    auto f1 = File("File1");
    auto f2 = File("File2");
    auto f3 = File("File3");
    auto f4 = File("File4");
    auto f5 = File("File5");
    return 0;
}
