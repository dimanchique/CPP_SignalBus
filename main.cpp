#include "SignalBus.h"
#include "ClassA.h"
#include "ClassB.h"

int main() {
    auto SignalBus = SignalBus::GetSignalBus();
    auto A = ClassA();
    auto A1 = ClassA();
    auto A2 = ClassA();
    auto A3 = ClassA();
    auto B = ClassB();
    SignalBus->FireWithData(SecondSignal{});
    SignalBus->Fire<FirstSignal>();

    SecondSignal a = {13};
    SignalBus->FireWithData(a);
    SignalBus->Fire<FirstSignal>();
    return 0;
}
