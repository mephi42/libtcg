#ifndef LIBTCG_H
#define LIBTCG_H

struct CPUState;
struct TCGContext;

#define RAM_SIZE (1024 * 1024 * 1024)

struct CPUState *libtcg_init(char *path);
struct TCGContext *libtcg_gen(struct CPUState *cpu);

#endif
