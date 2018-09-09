#include "main.h"
#include "colors.h"
#include "settings.h"

/* External variables --------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void Startup(WM_HWIN hWin, uint16_t xpos, uint16_t ypos);

/* Private typedef -----------------------------------------------------------*/
K_ModuleItem_Typedef settings_board =
{
	8,
	"settings",
	bmsettings,
	0,
	Startup,
	NULL,
};

/* Private defines -----------------------------------------------------------*/
#define ID_WINDOW_0               (GUI_ID_USER + 0x00)
#define ID_TEXT_TITLE			  (GUI_ID_USER + 0x01)
#define ID_BUTTON_EXIT            (GUI_ID_USER + 0x20)

/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static const GUI_WIDGET_CREATE_INFO dialogSettings[] =
{
  { WINDOW_CreateIndirect, "", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Settings", ID_TEXT_TITLE, 290, 30, 300, 40, 0, 0x0, 0 },
};

static void _OnPaint_exit(BUTTON_Handle hObj) {
	GUI_SetBkColor(GUI_BACKGROUND);
	GUI_Clear();

	GUI_SetColor(GUI_ACCENT_DARK);
	GUI_AA_FillCircle(100, 0, 100);

	GUI_SetBkColor(GUI_ACCENT_DARK);
	GUI_SetColor(GUI_FOREGROUND);
	GUI_SetFont(&GUI_FontLubalGraph32);
	GUI_DispStringAt("Menu", 20, 20);
}

static void _cbButton_exit(WM_MESSAGE *pMsg) {
	switch (pMsg->MsgId) {
		case WM_PAINT:
			_OnPaint_exit(pMsg->hWin);
			break;
		default:
			BUTTON_Callback(pMsg);
			break;
	}
}

static void _cbDialog(WM_MESSAGE *pMsg) {
	WM_HWIN hItem;
	int id, ncode;

	switch (pMsg->MsgId) {
		case WM_INIT_DIALOG:
			hItem = BUTTON_CreateEx(700, 0, 100, 100, pMsg->hWin, WM_CF_SHOW, 0, ID_BUTTON_EXIT);
			WM_SetCallback(hItem, _cbButton_exit);

		    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TITLE);
		    TEXT_SetFont(hItem, &GUI_FontLubalGraph32B);
		    TEXT_SetTextColor(hItem, GUI_ACCENT);
			break;
		case WM_PAINT:
			GUI_SetColor(GUI_FOREGROUND);
			break;
		case WM_NOTIFY_PARENT:
			id = WM_GetId(pMsg->hWinSrc);    	/* Id of widget */
			ncode = pMsg->Data.v;               /* Notification code */

			switch(id) {
				case ID_BUTTON_EXIT:
					switch(ncode) {
						case WM_NOTIFICATION_RELEASED:
							GUI_EndDialog(pMsg->hWin, 0);
								break;
					}
					break;
			}
			break;
		default:
			WM_DefaultProc(pMsg);
			break;
	}
}

static void Startup(WM_HWIN hWin, uint16_t xpos, uint16_t ypos) {
	GUI_CreateDialogBox(dialogSettings, GUI_COUNTOF(dialogSettings), _cbDialog, hWin, xpos, ypos);
}
