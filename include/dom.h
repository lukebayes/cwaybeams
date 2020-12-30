#ifndef __dom_h__
#define __dom_h__

#include <stdint.h>
#include <limits.h>
#include <stdbool.h>

#define MAX_ELEMENT_COUNT 65535

#define PP_ARG_N( \
          _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10, \
         _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
         _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
         _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
         _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
         _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, \
         _61, _62, _63, N, ...) N

/* Note 63 is removed */
#define PP_RSEQ_N()                                        \
         62, 61, 60,                                       \
         59, 58, 57, 56, 55, 54, 53, 52, 51, 50,           \
         49, 48, 47, 46, 45, 44, 43, 42, 41, 40,           \
         39, 38, 37, 36, 35, 34, 33, 32, 31, 30,           \
         29, 28, 27, 26, 25, 24, 23, 22, 21, 20,           \
         19, 18, 17, 16, 15, 14, 13, 12, 11, 10,           \
          9,  8,  7,  6,  5,  4,  3,  2,  1,  0

#define PP_NARG_(...)    PP_ARG_N(__VA_ARGS__)

/* Note dummy first argument _ and ##__VA_ARGS__ instead of __VA_ARGS__ */
#define PP_NARG(...)     PP_NARG_(_, ##__VA_ARGS__, PP_RSEQ_N())

/**
 * Wrap variadic macros with expected names.
 */
#define box(...)          newElement(PP_NARG(__VA_ARGS__), __VA_ARGS__)
#define vbox(...)         newElement(PP_NARG(__VA_ARGS__) + 1, \
    layout(LayoutVertical), __VA_ARGS__)
#define hbox(...)         newElement(PP_NARG(__VA_ARGS__) + 1, \
    layout(LayoutHorizontal), __VA_ARGS__)
#define children(...)     newChildren(PP_NARG(__VA_ARGS__), __VA_ARGS__)

typedef unsigned long ElementId;

typedef enum AttrName {
  None = 0,
  ChildrenAttr,
  Flex,
  GestureHandlerAttr,
  HFlex,
  HPercent,
  HeightAttr,
  LayoutAttr,
  NameAttr,
  VFlex,
  VPercent,
  WidthAttr,
  XOffset,
  XPos,
  YOffset,
  YPos
} AttrName;

typedef enum Layout {
  LayoutDefault = 0,
  LayoutStack,
  LayoutVertical,
  LayoutHorizontal,
} Layout;

/**
 * Container for arbitrary data
 * values.
 */
typedef struct Attr {
  AttrName name;
  unsigned int dataSize;
  unsigned char *data;
} Attr;

typedef struct Element {
  ElementId id;
  ElementId parentId;
  unsigned int width;
  unsigned int height;
  unsigned int x;
  unsigned int y;
  unsigned int z;
  unsigned int childCount;
  unsigned int attrCount;
  struct Attr **attrs;
} Element;

typedef void (GestureHandler)(void);

// Attribute creators
Attr *name(char*);
Attr *width(unsigned int value);
Attr *height(unsigned int value);
Attr *layout(Layout value);
Attr *handler(char *gestureName, GestureHandler *handler);
Attr *newChildren(unsigned int count, ...);
Attr *newCharAttr(AttrName name, char *value);
Attr *newUintAttr(AttrName name, unsigned value);
Attr *newHandlerAttr(AttrName name, GestureHandler *handler);

// Attribute getters
char *getName(Element *elem);
unsigned int getWidth(Element *elem);
unsigned int getHeight(Element *elem);
unsigned int getX(Element *elem);
unsigned int getY(Element *elem);
unsigned int getZ(Element *elem);
Layout getLayout(Element *elem);
struct Element **getChildren(Element *elem);

// Element helpers
unsigned int getUintAttr(Attr *attr);
char *getCharAttr(Attr *attr);
struct Element **getChildrenAttr(Attr *attr);
bool isRoot(Element *elem);

void printElement(Element *elem);
void freeElement(Element *elem);
void freeAttr(Attr *attr);
void freeRoot(Element *elem);

Element *newElement(unsigned int count, ...);
void emitEvent(Element *elem, char *gestureName);


#endif
