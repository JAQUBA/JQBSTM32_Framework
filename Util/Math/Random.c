#include "Random.h"

float random_float(float min, float max) {
    float r = (float)rand() / (float)RAND_MAX;
    return min + r * (max - min);
}
int random_num(int min, int max) {
    int r = rand() / RAND_MAX;
    return min + r * (max - min);
}