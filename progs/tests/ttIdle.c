//-------------------------------------------------
//          TestSuite: Idle
//-------------------------------------------------
// As you can see, this testtask does nothing. Only
// sit back and relax. Expect your idle process to
// print dots on the LCD. If not, your scheduling
// strategy seems to ignore him or you idle task is
// lazy. Investigate then. That's not how it should
// work.
//-------------------------------------------------
#include "../progs.h"
#if defined(TESTTASK_ENABLED) && TESTTASK == TT_IDLE


#endif