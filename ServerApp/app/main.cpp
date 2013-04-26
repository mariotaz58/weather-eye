#include "controller.hpp"
#include <Windows.h>

int main ()
{
    controller ctrl;
    ctrl.start ();

    Sleep (INFINITE);
    return 0;
}
