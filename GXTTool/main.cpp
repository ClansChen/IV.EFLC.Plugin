#include "../common/stdinc.h"
#include "IVText.h"

int main(int argc, char **argv)
{
    IVText instance;

    if (argc == 1)
    {
        instance.Process0Arg();
    }
    else if (argc == 3)
    {
        instance.Process2Args(argv[1], argv[2]);
    }
    else
    {
        std::cout << "打开方式不对。" << std::endl;
    }

    return 0;
}
