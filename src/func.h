#ifndef LF_FUNC_H
#define LF_FUNC_H

#include "./types.h"
#include "../libs/log.c/src/log.h"

ID *func_id_alloc(Module *mod, char *name, size_t name_size,
                  Value *(*native_impl)(HashDict *arg_by_name,
                                        Value *flowing_val),
                  size_t arg_count, Type return_type);

void func_id_free(ID *func_id);

HashDictEntry *func_id_add_arg(ID *func_id, char *name,
                               size_t name_size, Type type);

#endif // LF_FUNC_H
