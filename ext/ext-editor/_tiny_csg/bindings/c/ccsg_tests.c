#include "ccsg.h"

//--------------------------------------------------------------------------------------------------
// HelloWorld
//--------------------------------------------------------------------------------------------------
uint32_t
CCSG_Test_HelloWorld(void)
{
    CCSG_World *world = CCSG_World_Create();

    CCSG_World_Destroy(world);

    return 0;
}