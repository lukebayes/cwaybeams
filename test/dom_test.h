#ifndef __dom_test_h__
#define __dom_test_h__

#include <dom.h>

char *testNewCharAttr(void);
char *testNewUintAttr(void);

char *testNewNameAttr(void);
char *testNewHeight(void);
char *testNewWidth(void);
char *testNewLargerWidth(void);

char *testNewBox(void);
char *testNewBoxWithName(void);

char *testNewChildren(void);
char *testElementWithChild(void);
char *testElementSize(void);

char *testGetName(void);
char *testIsRoot(void);
char *testLayout(void);
char *testDefaultLayout(void);
char *testHandler(void);

#endif // __dom_test_h__
