#include "./hashdict.h"

HashDict *hashdict_alloc(
  size_t head_entry_cap,
  size_t entry_cap_in_region
) {
  HashDict *hd = (HashDict *)malloc(sizeof(HashDict));
  assert(hd && "failed to alloc hashdict");

  entry_cap_in_region = (entry_cap_in_region <= 0)
    ? DEFAULT_ENTRY_CAP_IN_REGION
    : entry_cap_in_region;

  hd->entries_arena = arena_alloc(
    sizeof(HashDictEntry) * entry_cap_in_region
  );

  hd->head_entry_cap = (head_entry_cap <= 0)
    ? DEFAULT_HEAD_ENTRY_CAP
    : head_entry_cap;

  // calloc already sets nexts to NULL, hopefully.
  hd->heads = arena_malloc(
    hd->entries_arena,
    sizeof(HashDictEntry) * head_entry_cap
  );

  return hd;
}

void hashdict_dealloc(HashDict *hd) {
  arena_free(hd->entries_arena);
  free(hd);
}

HashDictEntry *hashdict_add_entry(
  HashDict *hd,
  const char *key,
  void *val,
  size_t key_size,
  size_t val_size
) {
  assert(hd);
  assert(key);
  assert(val);
  assert(key_size >= 1);
  assert(val_size >= 1);

  uint64_t key_hash = superhash(key, (int)key_size);
  uint64_t hashed_idx = key_hash % hd->head_entry_cap;
  HashDictEntry *head = (hd->heads + hashed_idx);
  // Heads are already allocated, key and val are our signals that it
  // is empty.
  if (!head->key && !head->val) {
    head->key = (char *)key;
    head->val = (void *)val;
    head->key_size = key_size;
    head->val_size = val_size;

    return head;
  }

  HashDictEntry *last_entry = head, *cur_entry = head->next;
  while (cur_entry) {
    last_entry = cur_entry;
    cur_entry = cur_entry->next;
  }

  HashDictEntry *new_entry = arena_malloc(
    hd->entries_arena,
    sizeof(HashDictEntry)
  );
  new_entry->key = (char *)key;
  new_entry->val = (void *)val;
  new_entry->key_size = key_size;
  new_entry->val_size = val_size;
  last_entry->next = new_entry;

  return new_entry;
}
