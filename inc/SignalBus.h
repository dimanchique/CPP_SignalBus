#pragma once

#include <memory>
#include <functional>
#include <string>
#include <stdexcept>
#include <iostream>
#include <any>

#define SignalBusDebug

class FunctionWrapper{
public:
    FunctionWrapper() = default;
    explicit FunctionWrapper(std::any callback): callback(std::move(callback)){};

    template<typename T>
    void operator()(T signal){
        if (auto *lambda = std::any_cast<std::function<void(T)>>(&callback)) {
            try { (*lambda)(signal); }
            catch (std::exception &e){
                shouldBeDeleted = true;
            }
        }
    }
    bool shouldBeDeleted = false;

private:
    std::any callback;
};

class SignalBus {

private:
    SignalBus() = default;

public:
    ///Access to static SignalBus instance
    static std::shared_ptr<SignalBus> GetSignalBus()
    {
        static std::shared_ptr<SignalBus> instance(new SignalBus);
        return instance;
    }

    ///Subscribe owner to event T using lambda callback
    template<typename T>
    void Subscribe(std::function<void(T)> func, void* owner)
    {
        if (!owner)
            throw std::logic_error("Calling Subscribe method with nullptr owner");

        const auto EventHash = GetEventHash<T>();
        const auto OwnerID = GetOwnerId(owner);

        Subscribe(func, EventHash, OwnerID);
    }

    ///Subscribe owner to event T using class member function callback
    template<typename T, typename ObjectType>
    void Subscribe(void (ObjectType::*func)(T), ObjectType* owner)
    {
        Subscribe<T>(std::bind(func, owner, std::placeholders::_1), owner);
    }

    ///Unsubscribe owner from event T
    template<typename T>
    void Unsubscribe(void *owner)
    {
        const auto EventHash = GetEventHash<T>();
        const auto OwnerID = GetOwnerId(owner);
        Unsubscribe(EventHash, OwnerID);
    }

    ///Send signal T using default constructor
    template<typename T>
    void Fire()
    {
        Fire(T());
    }

    ///Send signal T
    template<typename T>
    void Fire(T signal)
    {
        const auto EventHash = GetEventHash<T>();

        if (!IsEventExist(EventHash))
            return;

        for (auto& [subscriber_uid, callback] : SubscribedFunctions[EventHash])
        {
            callback(signal);
            if (callback.shouldBeDeleted)
                Unsubscribe(EventHash, subscriber_uid, true);
        }
    }

private:
    ///Unsubscribe owner from event T
    void Unsubscribe(size_t EventHash, uintptr_t OwnerID, bool isDeadOwner = false)
    {
        if (!IsEventExistForOwner(EventHash, OwnerID))
            return;

        SubscribedFunctions[EventHash].erase(OwnerID);
#ifdef SignalBusDebug
        std::cout << "Object " << OwnerID << " unsubscribed from event " << EventHash << " ";
        if (isDeadOwner)
            std::cout << "Object was deleted.\n";
        else
            std::cout<<"\n";
#endif
        if (SubscribedFunctions[EventHash].empty())
        {
#ifdef SignalBusDebug
            std::cout << "No more subscribers for event " << EventHash << "\n";
#endif
            SubscribedFunctions.erase(EventHash);
        }
    }

    ///Subscribe owner to event T
    template<typename T>
    void Subscribe(std::function<void(T)> func, size_t EventHash, uintptr_t OwnerID)
    {
        if (IsOwnerSubscribed(EventHash, OwnerID))
            throw std::logic_error("Object " + std::to_string(OwnerID) + " is already subscribed to event " +
                                   std::to_string(EventHash) + ". Double subscription error.");

        SubscribedFunctions[EventHash][OwnerID] = std::move(FunctionWrapper(func));
#ifdef SignalBusDebug
        std::cout << "Object " << OwnerID << " subscribed to event " << EventHash << "\n";
#endif
    }

private:
    template<typename T>
    inline static size_t GetEventHash()
    {
        return typeid(T).hash_code();
    }

    inline static uintptr_t GetOwnerId(const void* owner)
    {
        return (uintptr_t)owner;
    }

    inline bool IsEventExist(const size_t &event_name) const
    {
        return SubscribedFunctions.find(event_name) != SubscribedFunctions.end();
    }

    inline bool IsOwnerSubscribed(const size_t &event_name, const uintptr_t &owner_key)
    {
        return SubscribedFunctions[event_name].find(owner_key) != SubscribedFunctions[event_name].end();
    }

    inline bool IsEventExistForOwner(const size_t &event_name, const uintptr_t &owner_key)
    {
        return IsEventExist(event_name) && IsOwnerSubscribed(event_name, owner_key);
    }

private:
    std::unordered_map<size_t, std::unordered_map<uintptr_t, FunctionWrapper>> SubscribedFunctions;
};
