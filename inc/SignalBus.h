#pragma once
#include "any"
#include "memory"
#include "sstream"
#include "iostream"
#include "unordered_map"
#include "functional"
#include <cxxabi.h>
#include <utility>

struct FirstSignal {};
struct SecondSignal { public: int a = 16; };

class SignalBus
{

private:
    SignalBus() = default;

public:
    ~SignalBus() = default;

    template<typename T>
    void Subscribe(std::function<void(T)> func, void *owner = nullptr)
    {
        std::string SignalName = GetSignalName<T>();
        uintptr_t owner_key = owner ? (uintptr_t)owner : 0;

        if (SubscribedFunctions[SignalName].find(owner_key) != SubscribedFunctions[SignalName].end())
        {
            std::ostringstream err;
            err << owner_key << " is already subscribed to " << SignalName << ". ";
            throw std::logic_error(err.str() + std::string("Double subscription error"));
        }

        SubscribedFunctions[SignalName][owner_key] = std::move(func);
        std::cout << SignalName << " was added for owner " << owner_key << "\n";
    }

    template<typename T>
    void Unsubscribe(void *owner = nullptr)
    {
        std::string SignalName = GetSignalName<T>();
        uintptr_t owner_key = owner ? (uintptr_t)owner : 0;
        if (SubscribedFunctions.find(SignalName) != SubscribedFunctions.end() &&
            SubscribedFunctions[SignalName].find(owner_key) != SubscribedFunctions[SignalName].end())
        {
            SubscribedFunctions[SignalName].erase(owner_key);
            std::cout << SignalName << " was removed for owner " << owner_key << "\n";
        }
        if (SubscribedFunctions.find(SignalName) != SubscribedFunctions.end() &&
            SubscribedFunctions[SignalName].find(owner_key) != SubscribedFunctions[SignalName].end())
        {
            SubscribedFunctions[SignalName].erase(owner_key);
            std::cout << SignalName << " was removed for owner " << owner_key << "\n";
        }
    }

    template<typename T>
    void Fire()
    {
        Fire(T());
    }

    template<typename T>
    void Fire(T signal)
    {
        std::string SignalName = GetSignalName<T>();
        if (SubscribedFunctions.find(SignalName) == SubscribedFunctions.end())
        {
            std::cout << SignalName << " wasn't found\n";
            return;
        }

        for(auto &Subscriber : SubscribedFunctions[SignalName])
        {
            void* wrapper = Subscriber.second;
            auto callback = std::any_cast<std::function<void(T)>>(Subscriber.second);
            callback(signal);
        }
    }

    static std::shared_ptr<SignalBus> GetSignalBus()
    {
        static std::shared_ptr<SignalBus> instance(new SignalBus);
        return instance;
    }

private:
    template<typename T>
    static std::string GetSignalName()
    {
        int status;
        char * SignalName = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status);
        return SignalName;
    }

private:
    std::unordered_map<std::string, std::unordered_map<uintptr_t, std::any>> SubscribedFunctions;
};
