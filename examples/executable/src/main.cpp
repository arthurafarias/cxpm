#include <iostream>
#include <source0.h>

extern int s1;
extern int s2;
extern int s3;

int main(int argc, char* argv[])
{
    std::cout << "Hello World" << s3 << std::endl;
    return 0;
}