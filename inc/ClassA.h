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

        sb->SubscribeWithData<SecondSignal>([&](SecondSignal signal) {
            MethodB(signal);
        }, this);
    }

    void MethodA() const
    {
        std::cout<< "MethodA called for object ClassA " << EntityID << ". Flag: " << flag << "\n";
    }

    void MethodB(SecondSignal signal)
    {
        flag = signal.a;
    }

    int EntityID;
    int flag = 0;
};
