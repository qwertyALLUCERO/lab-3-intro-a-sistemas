#ifndef RANDOM
#define RANDOM
#include <time.h>
#include <stdlib.h>

int randomInt(int min, int max) 
{
    return (rand()%(max+1-min))+min;
}

char letraRandom()
{
    int min = 65; // 'A en ascii'
    int max = 122; // 'z en ascii'
    char letra;

    do
    {
        letra = (rand()%(max+1-min))+min;
    } while (letra>90 && letra<97); /*ENTRE 91 y 96 no son letras*/

    return letra;
}
#endif