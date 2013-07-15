/******************** MODIFICATIONS to py1.h **************************
**
** Date       Who  What
**
** 2001       TLi  Original
**
**************************** End of List ******************************/

#if !defined(PY1_INCLUDED)
#define PY1_INCLUDED

#include "pyproc.h"
#include "pydata.h"
#include "conditions/haunted_chess.h"

void InitCheckDir(void);
void InitBoard(void);
void InitOpt(void);
void InitAlways(void);

#endif /*PY1_INCLUDED*/
