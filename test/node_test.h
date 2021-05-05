#ifndef __node_test_h__
#define __node_test_h__

#include "beam.h"

typedef int (FakeAddFunc)(int a, int b);

typedef enum FakeNodeKeys {
  FakeNodeKeysFirst = NodeAttrKeysLast,
  FakeNodeKeysName,
  FakeNodeKeysSize,
  FakeNodeKeysFunc,
} FakeNodeKeys;

#define get_name(node) get_char_attr_from_node(node, BeamAttrKeysName, DEFAULT_CHAR)

char *test_new_char_attr(void);
char *test_new_uint_attr(void);
char *test_new_children(void);
char *test_element_with_child(void);
char *test_is_root(void);
char *test_pointer_attr(void);
char *test_leaf_hash(void);
char *test_str_one(void);
char *test_str_kids(void);

#endif // __node_test_h__
