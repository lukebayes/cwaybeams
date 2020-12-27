#include "dom.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_NAME "default-name"

/**
 * Global id for all instantiated Elements.
 */
static ElementId lastId = 1;

/**
 * Simple pointer, used to determine pointer size.
 */
static int *ptr;

/**
 * Size of a pointer on this compilation target.
 */
static uint8_t POINTER_SIZE = sizeof(ptr);

/**
 * Get the next incremental identifier.
 */
static ElementId getNextId() {
  return lastId++;
}

/**
 * Free all malloc'd data from the provided attribute through any references it
 * may contain, including child Elements.
 */
void freeAttr(Attr *attr) {
  if (attr->name == ChildrenAttr) {
    struct Element **kids = childrenAttrData(attr);
    int count = attr->dataSize / POINTER_SIZE;
    for (int i = 0; i < count; i++) {
      freeElement(kids[i]);
    }
  }

  if (attr->name != GestureHandlerAttr) {
    free(attr->data);
  }

  free(attr);
}

/**
 * Recursively free all malloc'd data from the provided
 * element to it's leaves.
 */
void freeElement(Element *elem) {
  for (int i = 0; i < elem->attrCount; i++) {
    Attr *attr = elem->attrs[i];
    freeAttr(attr);
  }
  free(elem->attrs);
  free(elem);
}

/**
 * Create and returne a new, empty attribute struct.
 *
 * These entities must be sent to 'freeAttr' at some point in the future.
 */
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
 * Get the data from a Char Attr as (char *).
 */
char *charAttrData(Attr *attr) {
  return (char *)attr->data;
}

/**
 * Create a new Attr with the provided name and unsigned int data.
 */
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
 * Get the provided Attribute data as an unsigned integer.
 */
unsigned int uintAttrData(Attr *attr) {
  return (unsigned int)*attr->data;
}

/**
 * Create a new Gesture handler Attr.
 */
Attr *newHandlerAttr(AttrName name, GestureHandler *handler) {
  Attr *attr = newAttr();
  if (attr == NULL) {
    return NULL;
  }
  attr->name = name;
  attr->dataSize = POINTER_SIZE;
  attr->data = (unsigned char *)handler;
  return attr;
}

/**
 * Create a children Attr.
 */
Attr *newChildren(unsigned int count, ...) {
  Attr *attr = newAttr();
  if (attr == NULL) {
    return NULL;
  }

  attr->name = ChildrenAttr;
  attr->dataSize = count * POINTER_SIZE;

  struct Element *kids[attr->dataSize];
  if (kids == NULL) {
    return NULL;
  }

  va_list vargs;
  va_start(vargs, count);
  for (int i = 0; i < count; i++) {
    struct Element *kid = va_arg(vargs, struct Element *);
    kids[i] = kid;
  }
  va_end(vargs);

  attr->data = (unsigned char *)malloc(attr->dataSize);
  if (attr->data == NULL) {
    return NULL;
  }
  memcpy(attr->data, kids, attr->dataSize);
  return attr;
}

/**
 * Get an array of Element pointers as Children data from the provided Attr.
 */
struct Element **childrenAttrData(Attr *attr) {
  return (struct Element **)attr->data;
}

/**
 * Get the provided Layout value.
 */
Layout layoutAttrData(Attr *attr) {
  return uintAttrData(attr);
}

/**
 * Concrete Attr names bound to concrete types.
 */

/**
 * Create and return a name attribute.
 */
Attr *name(char *value) {
  return newCharAttr(Name, value);
}

/**
 * Create and return a width attribute.
 */
Attr *width(unsigned int value) {
  return newUintAttr(Width, value);
}

/**
 * Create and return a height attribute.
 */
Attr *height(unsigned int value) {
  return newUintAttr(Height, value);
}

/**
 * Create and return layout attribute.
 */
Attr *layout(Layout value) {
  return newUintAttr(LayoutAttr, value);
}

Attr *handler(char *gestureName, GestureHandler *handler) {
  return newHandlerAttr(GestureHandlerAttr, handler);
}

/**
 * Create a new Element with the provided attributes.
 */
Element *newElement(unsigned int attrCount, ...) {
  struct Attr **attrs = malloc(attrCount * POINTER_SIZE);
  if (attrs == NULL) {
    return NULL;
  }
  Element *elem = malloc(sizeof(struct Element));
  elem->id = getNextId();
  elem->parentId = 0;
  elem->childCount = 0;

  // Process Attrs
  va_list vargs;
  va_start(vargs, attrCount);
  for (int i = 0; i < attrCount; i++) {
    struct Attr *attr = va_arg(vargs, struct Attr *);
    if (attr->name == ChildrenAttr) {
      elem->childCount += (attr->dataSize / POINTER_SIZE);
      struct Element **kids = childrenAttrData(attr);
      for (int k = 0; k < elem->childCount; k++) {
        kids[k]->parentId = elem->id;
      }
    }
    attrs[i] = attr;
  }
  va_end(vargs);

  elem->attrCount = attrCount;
  elem->attrs = attrs;

  return elem;
}

/**
 * Print each char code in the provided string.
 */
void printChars(char *chars) {
  int i = 0;
  while (chars[i++] != '\0') {
    printf("Char: %d %c\n", chars[i], chars[i]);
  }
}

void printElementIndented(Element *elem, char *indent) {
  printf("------------------------\n");
  printf("%selem.id: %ld\n", indent, elem->id);
  printf("%selem.parentId: %ld\n", indent, elem->parentId);
  printf("%selem.name: %s\n", indent, elementName(elem));
  struct Element **kids = elementChildren(elem);
  if (kids != NULL) {
    char *nextIndent = malloc(strlen(indent) + 2);
    nextIndent = strcpy(nextIndent, indent);
    nextIndent = strcat(nextIndent, "\t");
    for (int i = 0; i < elem->childCount; i++) {
      printElementIndented(kids[i], nextIndent);
    }
    free(nextIndent);
  }
}

/**
 * Print the provided element and attributes.
 */
void printElement(Element *elem) {
  char *empty = malloc(1);
  printElementIndented(elem, "");
  free(empty);
}

/**
 * Get the provided Element name attribute, or DEFAULT_NAME if one was not
 * provided.
 */
char *elementName(Element *elem) {
  for (int i = 0; i < elem->attrCount; i++) {
    if (elem->attrs[i]->name == Name) {
      return charAttrData(elem->attrs[i]);
    }
  }

  return DEFAULT_NAME;
}

/**
 * Get the index of the provided attribute for the provided Element, or return
 * -1 if the AttrName is not found.
 */
int elementAttrIndex(Element *elem, AttrName name) {
  for (int i = 0; i < elem->attrCount; i++) {
    if (elem->attrs[i]->name == name) {
      return i;
    }
  }
  return -1;
}

/**
 * Get the provided Element children collection.
 */
struct Element **elementChildren(Element *elem) {
  int index = elementAttrIndex(elem, ChildrenAttr);
  if (index > -1) {
    return childrenAttrData(elem->attrs[index]);
  }

  return NULL;
}

/**
 * Get the layout attribute (or default value) from the provided element.
 */
Layout elementLayout(Element *elem) {
  int index = elementAttrIndex(elem, LayoutAttr);
  if (index > -1) {
    return layoutAttrData(elem->attrs[index]);
  }

  return LayoutDefault;
}

/**
 * Call any handlers found for the provided gesture name.
 */
void emitEvent(Element *elem, char *gestureName) {
  int index = elementAttrIndex(elem, GestureHandlerAttr);
  if (index > -1) {
    Attr *attr = elem->attrs[index];
    GestureHandler *handler = (GestureHandler *)attr->data;
    handler();
  }
}

/**
 * Return true if the provided Element does not have an assigned parentId.
 */
bool isRoot(Element *elem) {
  return elem->parentId == 0;
}
