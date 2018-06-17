#ifdef __cplusplus
extern "C" {
#endif

struct CPUState *libtcg_init(char *path);
struct TCGContext *libtcg_gen(struct CPUState *cpu);
void libtcg_dump_ops(struct TCGContext *s);

#ifdef __cplusplus
}
#endif
