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

#include <exec/memory.h>
#include <libraries/gadtools.h>
#include <libraries/mui.h>
#include <proto/exec.h>

#include <pragma/muimaster_lib.h>
#include <pragma/commodities_lib.h>
#include <pragma/rexxsyslib_lib.h>

/******************************************************************************
* Prototypes
*******************************************************************************/
ULONG HotKeyFunc(register __a0 struct Hook *hook, register __a2 Object *obj, register __a1 CxMsg *cmsg);
ULONG arexxFName(register __a2 Object *obj, register __a1  char **msg);
ULONG arexxDelay(register __a2 Object *obj, register __a1 char **msg);
ULONG ButtonFunc(register __a2 Object *obj, register __a1 int *msg);
ULONG MenuFunc(register __a2 Object *obj, register __a1 int *msg);

void init(void);
void end(void);
struct ObjApp *CreateApp(void);
void DisposeApp(struct ObjApp *ObjectApp);

/******************************************************************************
* Definitions
*******************************************************************************/
#define MAKE_ID(a, b, c, d) ((ULONG)(a) << 24 | (ULONG)(b) << 16 | (ULONG)(c) << 8 | (ULONG)(d))

enum ID {ID_ARexx = 1};
enum BUTID {ID_BT0, ID_BT1};
enum MENUID {ID_MN_NEW, ID_MN_LOAD, ID_MN_CSON, ID_MN_CSOFF};

struct ObjApp
{
	APTR	App;
	APTR	WI_label_0;
	APTR	MN_label_0;
	APTR	MN_CS_ON, MN_CS_OFF;
	APTR	TX_label_0;
	APTR	BT_label_0;
	APTR	BT_label_1;
	char *	STR_TX_label_0;
};

#define CX_HOTKEY 1

/******************************************************************************
* Global Variables
*******************************************************************************/
struct IntuitionBase *IntuitionBase;
struct Library *MUIMasterBase;
struct Library *CxBase;

char buffer[40];
struct ObjApp *App = NULL;

CxObj *broker, *filter, *sender, *translate;
struct MsgPort *broker_mp;

struct Hook hotkey_hook = {{NULL, NULL}, (HOOKFUNC)HotKeyFunc, NULL, NULL};
struct Hook hook_fname = {{NULL, NULL}, (HOOKFUNC)arexxFName, NULL, NULL};
struct Hook hook_delay = {{NULL, NULL}, (HOOKFUNC)arexxDelay, NULL, NULL};
struct Hook hook_button = {{NULL, NULL}, (HOOKFUNC)ButtonFunc, NULL, NULL};
struct Hook hook_menu = {{NULL, NULL}, (HOOKFUNC)MenuFunc, NULL, NULL};

struct MUI_Command rexxCommands[] =
{
	{"test",	MC_TEMPLATE_ID,	ID_ARexx,	NULL},			// Simple ARexx-Command without Parameters
	{"fname",	"NAME/A",		1,			&hook_fname},
	{"delay",	"DELAY/A",		1,			&hook_delay},
	{NULL,		NULL, 			NULL,		NULL}
};

BOOL clearScreen = TRUE;
/******************************************************************************
* Main-Program
*******************************************************************************/

/*-----------------------------------------------------------------------------
- HotKeyFunc()
- Function for Hotkey-Commodity-Hook
------------------------------------------------------------------------------*/
ULONG HotKeyFunc(register __a0 struct Hook *hook, register __a2 Object *obj, register __a1 CxMsg *cmsg)
{
	ULONG cmsgid, cmsgtype;

	cmsgid = CxMsgID(cmsg);
	cmsgtype = CxMsgType(cmsg);

	switch (cmsgtype)
	{
		case CXM_IEVENT:
			switch (cmsgid)
			{
				case CX_HOTKEY:
					printf("Hotkey detected\n");
					break;
			}
			break;

		case CXM_COMMAND:
			switch (cmsgid)
			{
				case CXCMD_APPEAR:
					printf("Show Interface\n");
					break;
				case CXCMD_DISAPPEAR:
					printf("Hide Interface\n");
					break;
				case CXCMD_ENABLE:
					printf("Enable Program\n");
					break;
				case CXCMD_DISABLE:
					printf("Disable Program\n");
					break;
				case CXCMD_KILL:
					printf("Kill Program\n");
					break;
				case CXCMD_UNIQUE:
					printf("CXCMD_UNIQUE\n");
					break;
				default:
					break;
			}
			break;
	}

	return 0;
};

/*-----------------------------------------------------------------------------
- arexxFName()
- Function for "fname"-ARexx-Hook
------------------------------------------------------------------------------*/
ULONG arexxFName(register __a2 Object *obj, register __a1 char **msg)
{
	char *data = (char *)msg[0];

	DisplayBeep(NULL);
	printf("We got an incoming fname-command, value = %s\n", data);

	return 0;
}

/*-----------------------------------------------------------------------------
- arexxDelay()
- Function for "delay"-ARexx-Hook
------------------------------------------------------------------------------*/
ULONG arexxDelay(register __a2 Object *obj, register __a1 char **msg)
{
	unsigned int ndata = (unsigned int)atoi(msg[0]);

	DisplayBeep(NULL);
	printf("We got an incoming delay-command, value = %ld\n", ndata);

	return 0;
}

/*-----------------------------------------------------------------------------
- ButtonFunc()
- Function for Button-Hook
------------------------------------------------------------------------------*/
ULONG ButtonFunc(register __a2 Object *obj, register __a1 int *msg)
{
	switch (*msg)
	{
		case ID_BT0:
			printf("Button 0\n");
			break;
		case ID_BT1:
			printf("Button 1\n");
			break;
	}

	return 0;
}

/*-----------------------------------------------------------------------------
- MenuFunc()
- Function for Menu-Hook
------------------------------------------------------------------------------*/
ULONG MenuFunc(register __a2 Object *obj, register __a1 int *msg)
{
	switch (*msg)
	{
		case ID_MN_NEW:
			printf("Menu: New\n");
			break;
		case ID_MN_LOAD:
			printf("Menu: Load\n");
			break;
		case ID_MN_CSON:
			clearScreen = TRUE;
			set(App->MN_CS_ON, MUIA_Menuitem_Checked, TRUE);
			set(App->MN_CS_OFF, MUIA_Menuitem_Checked, FALSE);
			printf("ClearScreen On\n");
			break;
		case ID_MN_CSOFF:
			clearScreen = FALSE;
			set(App->MN_CS_ON, MUIA_Menuitem_Checked, FALSE);
			set(App->MN_CS_OFF, MUIA_Menuitem_Checked, TRUE);
			printf("ClearScreen Off\n");
			break;
	}

	return 0;
}

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

	// Get Broker and Broker-MsgPort from MUI-App
	get(App->App, MUIA_Application_Broker, (ULONG *)&broker);
	get(App->App, MUIA_Application_BrokerPort, (ULONG *)&broker_mp);

	// Initialize Hotkey
	filter = CxFilter("rawkey rcommand l");
	AttachCxObj(broker, filter);
	sender = CxSender(broker_mp, CX_HOTKEY);
	AttachCxObj(filter, sender);
	translate = CxTranslate(NULL);
	AttachCxObj(filter, translate);

	// Activate Broker
	ActivateCxObj(broker, 1L);

	while (running)
	{
		switch (DoMethod(App->App, MUIM_Application_Input, &signal))
		{
			// Window close
			case MUIV_Application_ReturnID_Quit:
				if ((MUI_RequestA(App->App, 0, 0, "Quit?", "_Yes|_No", "\33cAre you sure?", 0)) == 1)
					running = FALSE;
				break;

			// Simple Arexx-Message (without Parameters)
			case ID_ARexx:
				DisplayBeep(NULL);
				printf("A simple ARexx-Command \"test\" received\n");
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

	if (!(CxBase = OpenLibrary("commodities.library", 37)))
	{
		printf("Can't Open Commodities Library\n");
		CloseLibrary((struct Library *)MUIMasterBase);
		CloseLibrary((struct Library *)IntuitionBase);
		exit(20);
	}
}

/*-----------------------------------------------------------------------------
- end()
------------------------------------------------------------------------------*/
void end(void)
{
	CloseLibrary((struct Library *)CxBase);
	CloseLibrary((struct Library *)MUIMasterBase);
	CloseLibrary((struct Library *)IntuitionBase);
	exit(20);
}

/*-----------------------------------------------------------------------------
- CreateApp()
------------------------------------------------------------------------------*/
struct ObjApp *CreateApp(void)
{
	struct ObjApp * ObjectApp;

	APTR MNlabel0Project, MNlabel0New, MNlabel0BarLabel0, MNlabel0Load, MNlabel0Save;
	APTR MNlabel0SaveAa, MNlabel0Transfer, MNlabel0BarLabel1, MNlabel0About;
	APTR MNlabel0BarLabel2, MNlabel0Hide, MNlabel0Quit, MNlabel0Item, MNlabel0Init;
	APTR MNlabel0Copy, MNlabel0Paste, MNlabel0BarLabel3, MNlabel0ClearScreen;
	APTR MNlabel0On, MNlabel0Off;

	APTR GROUP_ROOT_0;
	
	if (!(ObjectApp = AllocVec(sizeof(struct ObjApp),MEMF_CLEAR)))
		return(NULL);

	ObjectApp->STR_TX_label_0 = "This is just a Test for the MUI-ARexx/Commodity-Functionality";

	ObjectApp->TX_label_0 = TextObject,
		MUIA_Background,	MUII_TextBack,
		MUIA_Frame, 		MUIV_Frame_Text,
		MUIA_Text_Contents, ObjectApp->STR_TX_label_0,
		MUIA_Text_SetMin, 	TRUE,
	End;

	ObjectApp->BT_label_0 = SimpleButton("Button 0");
	ObjectApp->BT_label_1 = SimpleButton("Button 1");

	GROUP_ROOT_0 = GroupObject,
		Child, ObjectApp->TX_label_0,
		Child, ObjectApp->BT_label_0,
		Child, ObjectApp->BT_label_1,
	End;

	ObjectApp->WI_label_0 = WindowObject,
		MUIA_Window_Title, 		"MUI-ACT",
		MUIA_Window_ID, 		MAKE_ID('0', 'W', 'I', 'N'),
		MUIA_Window_SizeGadget, FALSE,
		WindowContents, 		GROUP_ROOT_0,
	End;

	MNlabel0New = MenuitemObject,
		MUIA_Menuitem_Title, "New",
		MUIA_Menuitem_Shortcut, "N",
	End;

	MNlabel0BarLabel0 = MUI_MakeObject(MUIO_Menuitem, NM_BARLABEL, 0, 0, 0);

	MNlabel0Load = MenuitemObject,
		MUIA_Menuitem_Title, "Load...",
		MUIA_Menuitem_Shortcut, "O",
	End;

	MNlabel0Save = MenuitemObject,
		MUIA_Menuitem_Title, "Save",
		MUIA_Menuitem_Shortcut, "S",
	End;

	MNlabel0SaveAa = MenuitemObject,
		MUIA_Menuitem_Title, "Save Aa...",
		MUIA_Menuitem_Shortcut, "A",
	End;

	MNlabel0Transfer = MenuitemObject,
		MUIA_Menuitem_Title, "Transfer",
	End;

	MNlabel0BarLabel1 = MUI_MakeObject(MUIO_Menuitem, NM_BARLABEL, 0, 0, 0);

	MNlabel0About = MenuitemObject,
		MUIA_Menuitem_Title, "About...",
	End;

	MNlabel0BarLabel2 = MUI_MakeObject(MUIO_Menuitem, NM_BARLABEL, 0, 0, 0);

	MNlabel0Hide = MenuitemObject,
		MUIA_Menuitem_Title, "Hide",
		MUIA_Menuitem_Shortcut, "L",
	End;

	MNlabel0Quit = MenuitemObject,
		MUIA_Menuitem_Title, "Quit",
		MUIA_Menuitem_Shortcut, "Q",
	End;

	MNlabel0Project = MenuitemObject,
		MUIA_Menuitem_Title, "Project",
		MUIA_Family_Child, MNlabel0New,
		MUIA_Family_Child, MNlabel0BarLabel0,
		MUIA_Family_Child, MNlabel0Load,
		MUIA_Family_Child, MNlabel0Save,
		MUIA_Family_Child, MNlabel0SaveAa,
		MUIA_Family_Child, MNlabel0Transfer,
		MUIA_Family_Child, MNlabel0BarLabel1,
		MUIA_Family_Child, MNlabel0About,
		MUIA_Family_Child, MNlabel0BarLabel2,
		MUIA_Family_Child, MNlabel0Hide,
		MUIA_Family_Child, MNlabel0Quit,
	End;

	MNlabel0Init = MenuitemObject,
		MUIA_Menuitem_Title, "Init",
	End;

	MNlabel0Copy = MenuitemObject,
		MUIA_Menuitem_Title, "Copy",
		MUIA_Menuitem_Shortcut, "C",
	End;

	MNlabel0Paste = MenuitemObject,
		MUIA_Menuitem_Title, "Paste",
		MUIA_Menuitem_Shortcut, "V",
	End;

	MNlabel0BarLabel3 = MUI_MakeObject(MUIO_Menuitem, NM_BARLABEL, 0, 0, 0);

	MNlabel0On = MenuitemObject,
		MUIA_Menuitem_Title, "On",
		MUIA_Menuitem_Checkit, TRUE,
		MUIA_Menuitem_Checked, TRUE,
	End;

	ObjectApp->MN_CS_ON = MNlabel0On;

	MNlabel0Off = MenuitemObject,
		MUIA_Menuitem_Title, "Off",
		MUIA_Menuitem_Checkit, TRUE,
	End;

	ObjectApp->MN_CS_OFF = MNlabel0Off;

	MNlabel0ClearScreen = MenuitemObject,
		MUIA_Menuitem_Title, "Clear Screen",
		MUIA_Family_Child, MNlabel0On,
		MUIA_Family_Child, MNlabel0Off,
	End;

	MNlabel0Item = MenuitemObject,
		MUIA_Menuitem_Title, "Item",
		MUIA_Family_Child, MNlabel0Init,
		MUIA_Family_Child, MNlabel0Copy,
		MUIA_Family_Child, MNlabel0Paste,
		MUIA_Family_Child, MNlabel0BarLabel3,
		MUIA_Family_Child, MNlabel0ClearScreen,
	End;

	ObjectApp->MN_label_0 = MenustripObject,
		MUIA_Family_Child, MNlabel0Project,
		MUIA_Family_Child, MNlabel0Item,
	End;

	ObjectApp->App = ApplicationObject,
		MUIA_Application_Author,		"M.Volkel",
		MUIA_Application_Base,			"MUIACT",	// Arexx-Port: "MUIACT.1"
		MUIA_Application_Title,			"MUI-ACT",
		MUIA_Application_Version,		"$VER: MUI-ACT V0.1",
		MUIA_Application_Copyright,		"(C)2022 M.Volkel",
		MUIA_Application_Description,	"Testing MUI-ARexx/Commodity-Functions",
		MUIA_Application_Menustrip,		ObjectApp->MN_label_0,
		MUIA_Application_BrokerHook,	&hotkey_hook,
		MUIA_Application_Commands,		&rexxCommands,
		SubWindow,						ObjectApp->WI_label_0,
	End;

	if (!ObjectApp->App)
	{
		FreeVec(ObjectApp);
		return(NULL);
	}

	// Window-Close-Method
	DoMethod(ObjectApp->WI_label_0, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, ObjectApp->App, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

	// Hook-Methods for Buttons
	DoMethod(ObjectApp->BT_label_0, MUIM_Notify, MUIA_Pressed, FALSE, MUIV_Notify_Self, 3, MUIM_CallHook, &hook_button, ID_BT0);
	DoMethod(ObjectApp->BT_label_1, MUIM_Notify, MUIA_Pressed, FALSE, MUIV_Notify_Self, 3, MUIM_CallHook, &hook_button, ID_BT1);

	// Hook-Methods for Menu
	DoMethod(MNlabel0New, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, ObjectApp->App, 3, MUIM_CallHook, &hook_menu, ID_MN_NEW);
	DoMethod(MNlabel0Load, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, ObjectApp->App, 3, MUIM_CallHook, &hook_menu, ID_MN_LOAD);
	DoMethod(MNlabel0On, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, ObjectApp->App, 3, MUIM_CallHook, &hook_menu, ID_MN_CSON);
	DoMethod(MNlabel0Off, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, ObjectApp->App, 3, MUIM_CallHook, &hook_menu, ID_MN_CSOFF);

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
