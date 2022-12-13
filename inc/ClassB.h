#pragma once
#include "SignalBus.h"

class ClassB
{
public:
    ClassB()
    {
        auto sb = SignalBus::GetSignalBus();
        sb->Fire<FirstSignal>();
    }
};