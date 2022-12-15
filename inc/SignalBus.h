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
    void Subscribe(std::function<void()> func, void *owner = nullptr)
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
    void SubscribeWithData(std::function<void(T)> func, void *owner = nullptr)
    {
        std::string SignalName = GetSignalName<T>();
        uintptr_t owner_key = owner ? (uintptr_t)owner : 0;

        if (SubscribedFunctionsWithData[SignalName].find(owner_key) != SubscribedFunctionsWithData[SignalName].end())
        {
            std::ostringstream err;
            err << owner_key << " is already subscribed to " << SignalName << ". ";
            throw std::logic_error(err.str() + std::string("Double subscription error"));
        }

        SubscribedFunctionsWithData[SignalName][owner_key] = func;
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
    }

    template<typename T>
    void Fire()
    {
        std::string SignalName = GetSignalName<T>();
        if (SubscribedFunctions.find(SignalName) == SubscribedFunctions.end())
        {
            std::cout << SignalName << " wasn't found\n";
            return;
        }

        for(auto &Subscriber : SubscribedFunctions[SignalName])
            Subscriber.second();
    }

    template<typename T>
    void FireWithData(T signal)
    {
        std::string SignalName = GetSignalName<T>();
        if (SubscribedFunctionsWithData.find(SignalName) == SubscribedFunctionsWithData.end())
        {
            std::cout << SignalName << " wasn't found\n";
            return;
        }

        for(auto &Subscriber : SubscribedFunctionsWithData[SignalName])
        {
            auto func = std::any_cast<std::function<void(T)>>(Subscriber.second);
            std::invoke(func, signal);
        }
    }

    template<typename T>
    static std::string GetSignalName()
    {
        int status;
        char * SignalName = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status);
        return SignalName;
    }

    static std::shared_ptr<SignalBus> GetSignalBus()
    {
        static std::shared_ptr<SignalBus> instance(new SignalBus);
        return instance;
    }

private:
    std::unordered_map<std::string, std::unordered_map<uintptr_t, std::function<void()>>> SubscribedFunctions;
    std::unordered_map<std::string, std::unordered_map<uintptr_t, std::any>> SubscribedFunctionsWithData;
};
