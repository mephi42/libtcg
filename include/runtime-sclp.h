#ifndef RUNTIME_SCLP_H
#define RUNTIME_SCLP_H

#include <stddef.h>
#include <stdint.h>

struct CPUS390XState;
struct SCCB;

struct sclp_handler {
    uint32_t value;
    uint32_t mask;
    int (*f)(struct CPUS390XState *env, struct SCCB *sccb, uint32_t code);
};

#define MAX_SCLP_HANDLERS 128
extern struct sclp_handler sclp_handlers[MAX_SCLP_HANDLERS];
extern size_t n_sclp_handlers;

#endif
