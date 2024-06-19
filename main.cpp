#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GLUT/glut.h>

#include "Board.h"
#include "globals.h"
#include "Engine.h"

int main(int argc, char **argv)
{
    Eng().run(argc, argv);
    return 1;
}
