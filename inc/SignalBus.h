#pragma once

#include <typeindex>
#include "any"
#include "memory"
#include "sstream"
#include "iostream"
#include "unordered_map"
#include "functional"
#include "typeinfo"

class SignalBus
{

private:
    SignalBus() = default;

public:
    ~SignalBus() = default;

    ///Access to static SignalBus instance
    static std::shared_ptr<SignalBus> GetSignalBus()
    {
        static std::shared_ptr<SignalBus> instance(new SignalBus);
        return instance;
    }

    ///Subscribe owner to event T using callback
    template<typename T>
    void Subscribe(std::function<void(T)> func, void *owner)
    {
        if (!owner)
            return;

        std::string SignalName;
        uintptr_t owner_key;

        GetEventNameAndOwnerId<T>(SignalName, owner_key, owner);

        if (IsEventExist(SignalName))
        {
            std::ostringstream err;
            err << owner_key << " is already subscribed to " << SignalName << ". ";
            throw std::logic_error(err.str() + std::string("Double subscription error"));
        }

        SubscribedFunctions[SignalName][owner_key] = std::move(func);
        std::cout << "Object " << owner_key << " subscribed to signal " << SignalName << "\n";
    }

    template <typename T, typename ObjectType>
    void Subscribe(void (ObjectType::*member_func)(T), ObjectType* owner)
    {
        Subscribe<T>([member_func, owner](T e) { (owner->*member_func)(e); }, owner);
    }

    ///Unsubscribe owner from event T
    template<typename T>
    void Unsubscribe(void *owner = nullptr)
    {
        std::string SignalName;
        uintptr_t owner_key;

        GetEventNameAndOwnerId<T>(&SignalName, &owner_key, owner);

        if (IsEventExistForOwner(SignalName, owner_key))
        {
            SubscribedFunctions[SignalName].erase(owner_key);
            std::cout << SignalName << " was removed for owner " << owner_key << "\n";
        }
    }

    ///Send empty signal T via SignalBus
    template<typename T>
    void Fire()
    {
        Fire(T());
    }

    ///Send loaded signal T via SignalBus
    template<typename T>
    void Fire(T signal)
    {
        std::string SignalName;
        GetEventName<T>(SignalName);

        if (!IsEventExist(SignalName))
        {
            std::cout << SignalName << " wasn't found\n";
            return;
        }

        for(auto &Subscriber : SubscribedFunctions[SignalName])
        {
            std::any& callable = Subscriber.second;
            // Check if the callable is a std::function object
            if (auto* lambda = std::any_cast<std::function<void(T)>>(&callable))
            {
                (*lambda)(signal);
            }
        }
    }

private:
    template<typename T>
    static void GetEventNameAndOwnerId(std::string &out_name, uintptr_t &out_uid, void* owner)
    {
        GetEventName<T>(out_name);
        GetOwnerId(out_uid, owner);
    }

    template<typename T>
    inline static void GetEventName(std::string &out_name)
    {
        out_name = typeid(T).name();
    }

    inline static void GetOwnerId(uintptr_t &out_uid, void* owner)
    {
        out_uid = (uintptr_t)owner;
    }

    inline bool IsEventExist(std::string &event_name)
    {
        return SubscribedFunctions.find(event_name) != SubscribedFunctions.end();
    }

    inline bool IsOwnerSubscribed(std::string &event_name, uintptr_t &owner_key)
    {
        return SubscribedFunctions[event_name].find(owner_key) != SubscribedFunctions[event_name].end();
    }

    inline bool IsEventExistForOwner(std::string &event_name, uintptr_t &owner_key)
    {
        return !IsEventExist(event_name) && IsOwnerSubscribed(event_name, owner_key);
    }

private:
    std::unordered_map<std::string, std::unordered_map<uintptr_t, std::any>> SubscribedFunctions;
};
