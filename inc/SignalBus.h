#pragma once

#include <memory>
#include <functional>
#include <string>
#include <stdexcept>
#include <iostream>
#include <any>

class SignalBus {

private:
    class FunctionWrapper{
    public:
        FunctionWrapper() = default;
        explicit FunctionWrapper(std::any callback): callback_(std::move(callback)){};

        template<typename T>
        void operator()(T signal){
            if (auto *lambda = std::any_cast<std::function<void(T)>>(&callback_)) {
                try { (*lambda)(signal); }
                catch (std::exception &e) { return; }
            }
        }

    private:
        std::any callback_;
    };


private:
    SignalBus() = default;

    ///Isolated access to Signal Bus Instance
    static SignalBus* getSignalBus()
    {
        static std::shared_ptr<SignalBus> instance_(new SignalBus);
        return instance_.get();
    }

public:
    ///subscribe to event T using lambda callback
    template<typename T>
    static void subscribe(std::function<void(T)> func, void* owner)
    {
        if (!owner)
            throw std::logic_error("Calling subscribe method with nullptr owner");

        const auto eventHash = getEventHash<T>();
        const auto ownerID = getOwnerID(owner);
        getSignalBus()->subscribe(func, eventHash, ownerID);
    }

    ///subscribe to event T using class member function callback
    template<typename T, typename ObjectType>
    static void subscribe(void (ObjectType::*func)(T), ObjectType* owner)
    {
        getSignalBus()->subscribe<T>(std::bind(func, owner, std::placeholders::_1), owner);
    }

    ///unsubscribe owner from event T
    template<typename T>
    static void unsubscribe(void *owner)
    {
        getSignalBus()->unsubscribe(getEventHash<T>(), owner);
    }

    ///send signal T using default constructor
    template<typename T>
    static void fire()
    {
        SignalBus::fire(T());
    }

    ///send signal T with payload
    template<typename T>
    static void fire(T signal)
    {
        const auto eventHash = getEventHash<T>();
        if (!getSignalBus()->isEventExist(eventHash)) return;
        for (auto& [subscriber_uid, callback] : getSignalBus()->subscribedFunctions_[eventHash])
            callback(signal);
    }

private:
    ///unsubscribe owner from event T
    void unsubscribe(size_t eventHash, void* owner)
    {
        if(!owner) return;
        const auto ownerID = getOwnerID(owner);
        if (!isEventExistForOwner(eventHash, ownerID)) return;

        subscribedFunctions_[eventHash].erase(ownerID);
        if (subscribedFunctions_[eventHash].empty())
            subscribedFunctions_.erase(eventHash);
    }

    ///subscribe owner to event T
    template<typename T>
    void subscribe(std::function<void(T)> func, size_t eventHash, uintptr_t ownerID)
    {
        if (isOwnerSubscribed(eventHash, ownerID))
            throw std::logic_error("Object " + std::to_string(ownerID) + " is already subscribed to event " +
                                   std::to_string(eventHash) + ". Double subscription error.");

        subscribedFunctions_[eventHash][ownerID] = std::move(FunctionWrapper(func));
    }

private:
    template<typename T>
    inline static size_t getEventHash()
    {
        return typeid(T).hash_code();
    }

    inline static uintptr_t getOwnerID(const void* owner)
    {
        return (uintptr_t)owner;
    }

    inline bool isEventExist(const size_t &event_name) const
    {
        return subscribedFunctions_.find(event_name) != subscribedFunctions_.end();
    }

    inline bool isOwnerSubscribed(const size_t &event_name, const uintptr_t &owner_key)
    {
        return subscribedFunctions_[event_name].find(owner_key) != subscribedFunctions_[event_name].end();
    }

    inline bool isEventExistForOwner(const size_t &event_name, const uintptr_t &owner_key)
    {
        return isEventExist(event_name) && isOwnerSubscribed(event_name, owner_key);
    }

private:
    std::unordered_map<size_t, std::unordered_map<uintptr_t, FunctionWrapper>> subscribedFunctions_;
};
