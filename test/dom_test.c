#include "minunit.h"
#include <dom.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *testNewHeight(void) {
  Attr *one = height(23);
  unsigned int data = uintAttrData(one);
  muAssert(data == 23, "Expected data to be 23");
  freeAttr(one);
  return NULL;
}

char *testNewWidth(void) {
  Attr *one = width(20);
  unsigned int data = uintAttrData(one);
  muAssert(data == 20, "Expected data to be 20");
  freeAttr(one);
  return NULL;
}

char *testNewName(void) {
  Attr *attr = name("abcd");
  char *data = charAttrData(attr);
  muAssert(strcmp(data, "abcd") == 0, "Expected abcd");
  freeAttr(attr);
  return NULL;
}

char *testNewUintAttr(void) {
  Attr *attr = newUintAttr(Name, 1234);
  unsigned int data = uintAttrData(attr);
  muAssert(data == 1234, "Expected 1234");
  freeAttr(attr);
  return NULL;
}

char *testNewCharAttr(void) {
  Attr *attr = newCharAttr(Name, "abcd");
  char *data = charAttrData(attr);
  muAssert(strcmp(data, "abcd") == 0, "Expected abcd");
  freeAttr(attr);
  return NULL;
}

char *testNewBox(void) {
  Element *one = newBox(0);
  muAssert(one->parentId == 0, "Expected empty parentId");
  freeElement(one);
  return NULL;
}

char *testNewBoxWithName(void) {
  Element *one = newBox(1, name("abcd"));
  muAssert(one->parentId == 0, "Expected empty parentId");
  Attr *attr = one->attrs[0];
  char *name = charAttrData(attr);
  muAssert(strcmp(name, "abcd") == 0, "Expected name attr");
  freeElement(one);
  return NULL;
}

char *testNewChildren(void) {
  Element *root = newBox(1, name("root"));
  Attr *attr = newChildren(1, root);

  // printf("========================\n");
  // Element *kids = childrenAttrData(attr);
  // Element elem = kids[0];
  // printf("ID: %d\n", elem->id);
  // char *name = elementName(elem);
  // muAssert(strcmp(name, "root") == 0, "Expected name root");
  printf("----------------------------\n");
  freeAttr(attr);
  return NULL;

  /*
  Element *root = box(
    name("root"),
    children(
      box(name("child-1"))
    )
  );

  freeElement(root);
  */
}
