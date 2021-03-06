
#ifdef __amigaos4__
// Not real mac, we need some defines.
#include "libMacEmu.h"
#endif

#include "bullWindow.h"


WindowPtr	bullseyeWindow;
Rect		dragRect;
Rect		windowBounds = { 40, 40, 150, 150 };
Rect		circleStart = {10, 10, 100, 100};
int		width = 5;

void SetUpWindow(void)
{
	dragRect = screenBits.bounds;
	bullseyeWindow = NewWindow(0L, &windowBounds, "Bullseye", true, noGrowDocProc, (WindowPtr) -1L, true, 0);
	SetPort(bullseyeWindow);
}

void DrawBullseye(short active)
{
	Rect	myRect;
	int		color = 1;

	SetPort(bullseyeWindow);
	EraseRect(&circleStart);
	myRect = circleStart;

	while(myRect.left < myRect.right)
	{
		FillOval(&myRect, color ? (active ? black : gray) : white);
		InsetRect(&myRect, width, width);	  color = !color;	  
	}
}

