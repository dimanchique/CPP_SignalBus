#pragma once
#include "any"
#include "memory"
#include "sstream"
#include "iostream"
#include "unordered_map"
#include "functional"
#include <cxxabi.h>
#include <utility>

class SignalBus
{

private:
    SignalBus() = default;

public:
    ~SignalBus() = default;

    static std::shared_ptr<SignalBus> GetSignalBus()
    {
        static std::shared_ptr<SignalBus> instance(new SignalBus);
        return instance;
    }

    template<typename T>
    void Subscribe(std::function<void(T)> func, void *owner = nullptr)
    {
        std::string SignalName;
        uintptr_t owner_key;

        GetEventNameAndUId<T>(SignalName, owner_key, owner);

        if (IsEventExists(SignalName))
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
        std::string SignalName;
        uintptr_t owner_key;

        GetEventNameAndUId<T>(&SignalName, &owner_key, owner);
        if (IsEventExistsForOwner(SignalName, owner_key))
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
        std::string SignalName;
        GetEventName<T>(SignalName);

        if (!IsEventExists(SignalName))
        {
            std::cout << SignalName << " wasn't found\n";
            return;
        }

        for(auto &Subscriber : SubscribedFunctions[SignalName])
        {
            auto callback = std::any_cast<std::function<void(T)>>(Subscriber.second);
            callback(signal);
        }
    }

private:
    template<typename T>
    static void GetEventNameAndUId(std::string &out_name, uintptr_t &out_uid, void* owner = nullptr)
    {
        GetEventName<T>(out_name);
        out_uid = owner ? (uintptr_t)owner : 0;
    }

    template<typename T>
    static void GetEventName(std::string &out_name)
    {
        int status;
        char * SignalName = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status);
        out_name = SignalName;
    }

    bool IsEventExists(std::string &event_name)
    {
        return SubscribedFunctions.find(event_name) != SubscribedFunctions.end();
    }

    bool IsOwnerSubscribed(std::string &event_name, uintptr_t &owner_key)
    {
        return SubscribedFunctions[event_name].find(owner_key) != SubscribedFunctions[event_name].end();
    }

    bool IsEventExistsForOwner(std::string &event_name, uintptr_t &owner_key)
    {
        return !IsEventExists(event_name) && IsOwnerSubscribed(event_name, owner_key);
    }

private:
    std::unordered_map<std::string, std::unordered_map<uintptr_t, std::any>> SubscribedFunctions;
};
