/******************************************************************************
* MUI_ACT - MUI-ARexx/Commodity-Test
* (C)2022 M.Volkel (mario.volkel@outlook.com)
*******************************************************************************/

// Comment templates

/******************************************************************************
*
*******************************************************************************/

/*-----------------------------------------------------------------------------
-
------------------------------------------------------------------------------*/

/******************************************************************************
* Header-Files
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <exec/memory.h>
#include <libraries/gadtools.h>
#include <libraries/mui.h>

#include <pragma/diskfont_lib.h>
#include <proto/exec.h>
#include <pragma/graphics_lib.h>
#include <pragma/muimaster_lib.h>

/******************************************************************************
* Prototypes
*******************************************************************************/
void init(void);
void end(void);
struct ObjApp *CreateApp(void);
void DisposeApp(struct ObjApp *ObjectApp);

/******************************************************************************
* Definitions
*******************************************************************************/
#define MAKE_ID(a, b, c, d) ((ULONG)(a) << 24 | (ULONG)(b) << 16 | (ULONG)(c) << 8 | (ULONG)(d))

struct ObjApp
{
	APTR	App;
	APTR	WI_label_0;
	APTR	TX_label_0;
	char *	STR_TX_label_0;
};

/******************************************************************************
* Global Variables
*******************************************************************************/
struct IntuitionBase *IntuitionBase;
struct Library *MUIMasterBase;

char buffer[40];
struct ObjApp *App = NULL; /* Object */

/******************************************************************************
* Main-Program
*******************************************************************************/

/*-----------------------------------------------------------------------------
- main()
------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	BOOL running = TRUE;
	ULONG signal;

	/* Program initialisation : generated by GenCodeC */
	init();

	/* Create Object : generated by GenCodeC */
	if (!(App = CreateApp()))
	{
		printf("Can't Create App\n");
		end();
	}

	while (running)
	{
		switch (DoMethod(App->App, MUIM_Application_Input, &signal))
		{
			case MUIV_Application_ReturnID_Quit:
				if ((MUI_RequestA(App->App, 0, 0, "Quit?", "_Yes|_No", "\33cAre you sure?", 0)) == 1)
					running = FALSE;
				break;

			default:
				break;
		}

		if (running && signal)
			Wait(signal);
	}

	DisposeApp(App);
	end();
}

/*-----------------------------------------------------------------------------
- init()
------------------------------------------------------------------------------*/
void init(void)
{
	if (!(IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 37)))
	{
		printf("Can't Open Intuition Library\n");
		exit(20);
	}
	if (!(MUIMasterBase = OpenLibrary(MUIMASTER_NAME, MUIMASTER_VMIN)))
	{
		printf("Can't Open MUIMaster Library\n");
		CloseLibrary((struct Library *)IntuitionBase);
		exit(20);
	}
}

/*-----------------------------------------------------------------------------
- end()
------------------------------------------------------------------------------*/
void end(void)
{
	CloseLibrary(MUIMasterBase);
	CloseLibrary((struct Library *)IntuitionBase);
	exit(20);
}

/*-----------------------------------------------------------------------------
- CreateApp()
------------------------------------------------------------------------------*/
struct ObjApp *CreateApp(void)
{
	struct ObjApp * ObjectApp;
	APTR GROUP_ROOT_0;

	if (!(ObjectApp = AllocVec(sizeof(struct ObjApp),MEMF_CLEAR)))
		return(NULL);

	ObjectApp->STR_TX_label_0 = "This is just a Test for the MUI-ARexx/Commodity-Functionality";

	ObjectApp->TX_label_0 = TextObject,
		MUIA_Background, MUII_TextBack,
		MUIA_Frame, MUIV_Frame_Text,
		MUIA_Text_Contents, ObjectApp->STR_TX_label_0,
		MUIA_Text_SetMin, TRUE,
	End;

	GROUP_ROOT_0 = GroupObject,
		Child, ObjectApp->TX_label_0,
	End;

	ObjectApp->WI_label_0 = WindowObject,
		MUIA_Window_Title, "MUI-ACT",
		MUIA_Window_ID, MAKE_ID('0', 'W', 'I', 'N'),
		MUIA_Window_SizeGadget, FALSE,
		WindowContents, GROUP_ROOT_0,
	End;

	ObjectApp->App = ApplicationObject,
		MUIA_Application_Author, "M.Volkel",
		MUIA_Application_Base, "AppBase",
		MUIA_Application_Title, "MUI-ACT",
		MUIA_Application_Version, "$VER: MUI-ACT V0.1",
		MUIA_Application_Copyright, "(C)2022 M.Volkel)",
		MUIA_Application_Description, "Testing MUI-ARexx/Commodity-Functions",
		SubWindow, ObjectApp->WI_label_0,
	End;

	if (!ObjectApp->App)
	{
		FreeVec(ObjectApp);
		return(NULL);
	}

	// Window-Close
	DoMethod(ObjectApp->WI_label_0, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, ObjectApp->App, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

	// Window open
	set(ObjectApp->WI_label_0, MUIA_Window_Open, TRUE);

	return(ObjectApp);
}

/*-----------------------------------------------------------------------------
- DisposeApp()
------------------------------------------------------------------------------*/
void DisposeApp(struct ObjApp *ObjectApp)
{
	MUI_DisposeObject(ObjectApp->App);
	FreeVec(ObjectApp);
}
