//Includes
#include <stdio.h>
#include <math.h>
#include "common/String.hpp"
//main 
int main(int argc, char **argv)
{
//Start of user code
auto f = [](int a){ printf("%d\n", a);};
f(1);

//New line
f(2100);

//End of user code
return 0;
}
