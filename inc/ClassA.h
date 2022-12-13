#pragma once
#include "SignalBus.h"

class ClassA
{
public:
    ClassA()
    {
        static int ID = 0;
        EntityID = ID++;
        auto sb = SignalBus::GetSignalBus();
        sb->Subscribe<FirstSignal>([&] { MethodA(); }, this);

        sb->SubscribeWithData<SecondSignal>([&](void* signal) {
            auto data = (SecondSignal*) signal;
            MethodB(*data);
        }, this);
    }

    void MethodA() const
    {
        std::cout<< "MethodA called for object ClassA " << EntityID << "\n";
    }

    static void MethodB(SecondSignal signal)
    {
        std::cout << signal.a << "\n";
    }

    int EntityID;
};
