#include <stdbool.h>
#include <stdint.h>
#include <string.h>

extern char memory[];
extern const char memory_init[];
extern const int32_t memory_init_size;
extern void dispatch();

int main()
{
    memcpy(memory, memory_init, memory_init_size);
    while (true)
        dispatch();
}
