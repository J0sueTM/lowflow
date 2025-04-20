/*
 * Copyright (C) Josué Teodoro Moreira
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "./id.h"

Value *id_eval(ID *id, Flow *flow)
{
  assert(id);
  assert(flow);

  if (id->spec.type == FUNC) {
    Func *func = id->val.func;
    if (!func) {
      log_fatal("nil func_id(%p)", id);
      return NULL;
    }

    if (func->native_impl) {
      assert(flow->arg_by_name);
      return func->native_impl(flow->arg_by_name, &flow->val);
    }

    return func->child;
  }

  return &flow->val;
}

void id_free(ID *id)
{
  assert(id);
  switch (id->spec.type) {
  case FUNC:
    func_id_free(id);
    break;
  default:
    log_error("invalid id(%p) [type=%d]", id, (int)id->spec.type);
    break;
  }
}
