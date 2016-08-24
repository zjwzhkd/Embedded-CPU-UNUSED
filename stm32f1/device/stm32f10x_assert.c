
#include "stm32f10x.h"
#include <stdio.h>

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{
	while (1)
	{
	}
}

#endif
