#include "dom.h"
#include "debug.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_NAME "default-name"

static ElementId lastId = 1;


static int *ptr;
// NOTE(lbayes): This reports 8 on my modern laptop,
// I thought it would be 16, am I doing something dumb here?
static uint8_t POINTER_SIZE = sizeof(ptr);


void freeAttr(Attr *attr) {
  if (attr->name == Children) {
    printf("FREE KID size %d\n", attr->dataSize);
    struct Element **kids = childrenAttrData(attr);
    int count = attr->dataSize / POINTER_SIZE;
    printf("count: %d\n", count);
    for (int i = 0; i < count; i++) {
      printf("found a child attr\n");
      freeElement(kids[i]);
    }
  }
  free(attr->data);
  free(attr);
}

void freeElement(Element *elem) {
  printf("FREEING ELEM: %d\n", elem->id);
  // printf("FREEING ELEM: %s\n", elementName(elem));
  for (int i = 0; i < elem->attrCount; i++) {
    Attr *attr = elem->attrs[i];
    printf("ATTR NAME: %d\n", attr->name);
    freeAttr(attr);
  }
  free(elem->attrs);
  free(elem);
}

Attr *newAttr(void) {
  Attr *attr = malloc(sizeof(struct Attr));
  if (attr == NULL) {
    return NULL;
  }
  attr->dataSize = 0;
  return attr;
}

/**
 * Create a new Attr with the provided char value.
 */
Attr *newCharAttr(AttrName name, char *value) {
  Attr *attr = newAttr();
  if (attr == NULL) {
    return NULL;
  }
  attr->name = name;
  attr->dataSize = strlen(value) + 1;
  attr->data = (unsigned char *)malloc(attr->dataSize);
  memcpy(attr->data, value, attr->dataSize);
  return attr;
}

/**
 * Get the data from a Char Attr as char *
 */
char *charAttrData(Attr *attr) {
  return (char *)attr->data;
}

Attr *newUintAttr(AttrName name, unsigned int value) {
  Attr *attr = newAttr();
  if (attr == NULL) {
    return NULL;
  }
  attr->name = name;
  attr->dataSize = sizeof(unsigned int);
  attr->data = (unsigned char *)malloc(attr->dataSize);
  memcpy(attr->data, &value, attr->dataSize);
  return attr;
}

/**
 * Get the provided Attribute data as an
 * unsigned integer.
 */
unsigned int uintAttrData(Attr *attr) {
  return (unsigned int)*attr->data;
}

/**
 * Create a children Attr.
 */
Attr *newChildren(unsigned int count, ...) {
  Attr *attr = newAttr();
  if (attr == NULL) {
    return NULL;
  }
  attr->name = Children;
  attr->dataSize = count * POINTER_SIZE;

  struct Element *kids[attr->dataSize];
  if (kids == NULL) {
    return NULL;
  }

  va_list vargs;
  va_start(vargs, count);
  for (int i = 0; i < count; i++) {
    struct Element *kid = va_arg(vargs, struct Element *);

    printf("ASSOC NAME before: %s\n", elementName(kid));
    kids[i] = kid;
    printf("ASSOC NAME after: %s\n", elementName(kids[i]));
  }
  va_end(vargs);

  attr->data = (unsigned char *)malloc(attr->dataSize);
  if (attr->data == NULL) {
    return NULL;
  }
  memcpy(attr->data, kids, attr->dataSize);

  // NOTE(lbayes): LEFT OFF HERE, trying to deserialize child Elements, but not quite there yet.
  struct Element **after = childrenAttrData(attr);
  printf("ELEM ID AFTER: %d\n", after[0]->id);
  printf("ASSOC NAME AFTER: %s\n", elementName(after[0]));

  return attr;
}

struct Element **childrenAttrData(Attr *attr) {
  return (struct Element **)attr->data;
}

/**
 * Concrete Attr names bound to types.
 */

Attr *name(char *value) {
  return newCharAttr(Name, value);
}

Attr *width(unsigned int value) {
  return newUintAttr(Width, value);
}

Attr *height(unsigned int value) {
  return newUintAttr(Height, value);
}

Element *newBox(unsigned int attrCount, ...) {
  struct Attr **attrs = malloc(attrCount * POINTER_SIZE);
  if (attrs == NULL) {
    return NULL;
  }
  // Process Attrs
  va_list vargs;
  va_start(vargs, attrCount);
  for (int i = 0; i < attrCount; i++) {
    struct Attr *attr = va_arg(vargs, struct Attr *);
    attrs[i] = attr;
  }
  va_end(vargs);

  Element *elem = malloc(sizeof(struct Element));
  elem->id = lastId++;
  elem->parentId = 0;
  elem->attrCount = attrCount;
  elem->attrs = attrs;

  return elem;
}

void printElement(Element *elem) {
  printf("------------------------\n");
  printf("elem.id: %d\n", elem->id);
  printf("elem.parentId: %d\n", elem->parentId);
}

char *elementName(Element *elem) {
  for (int i = 0; i < elem->attrCount; i++) {
    if (elem->attrs[i]->name == Name) {
      return charAttrData(elem->attrs[i]);
    }
  }

  return DEFAULT_NAME;
}
