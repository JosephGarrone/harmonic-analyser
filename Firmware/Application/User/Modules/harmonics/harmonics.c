#include "main.h"
#include "colors.h"
#include "harmonics.h"
#include "harmonic_fft.h"

/* External variables --------------------------------------------------------*/
extern float32_t fftOutput[FFT_SAMPLE_LENGTH / 2];
/* Private function prototypes -----------------------------------------------*/
static void Startup(WM_HWIN hWin, uint16_t xpos, uint16_t ypos);

/* Private typedef -----------------------------------------------------------*/
K_ModuleItem_Typedef harmonics_board =
{
	8,
	"harmonics",
	bmharmonics,
	0,
	Startup,
	NULL,
};

/* Private defines -----------------------------------------------------------*/
#define ID_WINDOW_0               (GUI_ID_USER + 0x00)
#define ID_TEXT_TITLE			  (GUI_ID_USER + 0x01)

#define ID_PROGRESS				  (GUI_ID_USER + 0x02)
#define ID_BUTTON_PAUSE			  (GUI_ID_USER + 0x03)

#define ID_BUTTON_EXIT            (GUI_ID_USER + 0x20)


#define UPDATE_PERIOD 1000
#define UPDATE_TIMER_PERIOD 30
#define UPDATE_COUNTER_MAX (UPDATE_PERIOD/UPDATE_TIMER_PERIOD)
/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static const GUI_WIDGET_CREATE_INFO dialogSettings[] =
{
  { WINDOW_CreateIndirect, "", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Harmonics", ID_TEXT_TITLE, 290, 30, 300, 40, 0, 0x0, 0 },
};

static WM_HTIMER hUpdateTimer;
uint8_t updateCounter = 0;
uint8_t pause = 0;

static void _OnPaint_exit(BUTTON_Handle hObj) {
	GUI_SetBkColor(GUI_BACKGROUND);
	GUI_Clear();

	GUI_SetColor(GUI_ACCENT_DARK);
	GUI_AA_FillCircle(110, 0, 100);

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

static void _OnPaint_pause(BUTTON_Handle hObj) {
	GUI_SetBkColor(GUI_BACKGROUND);
	GUI_Clear();

	GUI_SetColor(GUI_ACCENT_DARK);
	GUI_FillRect(0, 0, 120, 60);

	GUI_SetBkColor(GUI_ACCENT_DARK);
	GUI_SetColor(GUI_FOREGROUND);
	GUI_SetFont(&GUI_FontLubalGraph32);
	GUI_DispStringAt(pause ? "Resume" : "Pause", 8, 15);
}

static void _cbButton_pause(WM_MESSAGE *pMsg) {
	switch(pMsg->MsgId) {
		case WM_PAINT:
			_OnPaint_pause(pMsg->hWin);
			break;
		default:
			BUTTON_Callback(pMsg);
			break;
	}
}

static void drawHarmonics() {
	int xMin = 20;
	int yMin = 40;
	int yMax = 300;
	int width = 10;
	int x1, x2, y1, y2;
	char tmp[4];

	GUI_SetColor(GUI_FOREGROUND);

	for (int i = 0; i < 20; i++) {
		x1 = xMin + (i * (LCD_GetXSize() - xMin)) / 20;
		x2 = x1 + width;
		y1 = LCD_GetYSize() - yMin;
		y2 = y1 - (yMax * fftOutput[(i+1) * 2]) / (8388608);

		if (y2 < 0) {
			y2 = 0;
		}

		// y1/y2 inverted because drawing up from bottom
		GUI_FillRect(x1, y2, x2, y1);
		sprintf((char*)tmp, "%d", i + 1);
		GUI_DispStringAt(tmp, x1, y1 + 5);
	}
}

static void _cbDialog(WM_MESSAGE *pMsg) {
	WM_HWIN hItem;
	int id, ncode;

	switch (pMsg->MsgId) {
		case WM_INIT_DIALOG:
			// Create the exit button
			hItem = BUTTON_CreateEx(700, 0, 100, 100, pMsg->hWin, WM_CF_SHOW, 0, ID_BUTTON_EXIT);
			WM_SetCallback(hItem, _cbButton_exit);

			// Create the pause button
			hItem = BUTTON_CreateEx(16, 16, 120, 70, pMsg->hWin, WM_CF_SHOW, 0, ID_BUTTON_PAUSE);
			WM_SetCallback(hItem, _cbButton_pause);

			// Set the title
		    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TITLE);
		    TEXT_SetFont(hItem, &GUI_FontLubalGraph32B);
		    TEXT_SetTextColor(hItem, GUI_ACCENT);

		    // Start timer
			hUpdateTimer = WM_CreateTimer(pMsg->hWin, 0, UPDATE_TIMER_PERIOD, 0);

			// Start the progress bar
			GUI_SetColor(GUI_ACCENT_DARK);
			GUI_ClearRect(0, LCD_GetYSize() - 5, LCD_GetXSize(), LCD_GetYSize());
			GUI_FillRect(0, LCD_GetYSize() - 5, 0, LCD_GetYSize());
			break;
		case WM_PAINT:
			// Draw the progress bar
			if (pause == 1) {
				GUI_ClearRect(0, LCD_GetYSize() - 5, LCD_GetXSize(), LCD_GetYSize());
			} else {
				GUI_SetColor(GUI_ACCENT_DARK);
				GUI_FillRect(0, LCD_GetYSize() - 5, (LCD_GetXSize() * updateCounter) / UPDATE_COUNTER_MAX, LCD_GetYSize());
			}

			// Draw the harmonics
			drawHarmonics();

			GUI_SetColor(GUI_FOREGROUND);
			break;
		case WM_TIMER:
			if (updateCounter == UPDATE_COUNTER_MAX) {
				updateCounter = 0;
				fft_calculate();
			} else if (pause == 1) {
				updateCounter = 0;
			} else {
				updateCounter++;
			}
			WM_InvalidateWindow(pMsg->hWin);
			WM_RestartTimer(pMsg->Data.v, UPDATE_TIMER_PERIOD);
			break;
		case WM_NOTIFY_PARENT:
			id = WM_GetId(pMsg->hWinSrc);    	/* Id of widget */
			ncode = pMsg->Data.v;               /* Notification code */

			switch(id) {
				case ID_BUTTON_EXIT:
					switch(ncode) {
						case WM_NOTIFICATION_RELEASED:
							if (hUpdateTimer != 0) {
								WM_DeleteTimer(hUpdateTimer);
								hUpdateTimer = 0;
							}
							GUI_EndDialog(pMsg->hWin, 0);
							break;
					}
					break;
				case ID_BUTTON_PAUSE:
					switch (ncode) {
						case WM_NOTIFICATION_RELEASED:
							if (pause) {
								pause = 0;
							} else {
								pause = 1;
							}
							WM_InvalidateWindow(pMsg->hWin);
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
