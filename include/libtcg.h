#ifndef LIBTCG_H
#define LIBTCG_H

#include <stddef.h>

struct CPUState;
struct TCGContext;

static const char *machine_type = "s390-ccw-virtio-3.0-machine";
#define RAM_SIZE (1024 * 1024 * 1024)

struct CPUState *libtcg_init(char *path);
struct TCGContext *libtcg_gen(struct CPUState *cpu, char *disasm, size_t disasm_len);

#endif
