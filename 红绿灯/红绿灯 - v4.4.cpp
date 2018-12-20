#include <windows.h>
#include "CIniFile.h"
using namespace std;

#define TIMER_SEC 1
static const int cLight = 11; // 灯数
static POINT cc[cLight]; // 圆心坐标
static int  radius = 15; // 圆的半径
static TCHAR szTime[3];  // 时间存储位置
static int allLightTime;   // 红绿灯周期，4*(greenTime+yellowTime)一个周期
static int timeCount;	// 红绿灯时间计数
int greenTime, yellowTime; // 绿灯时间、黄灯时间
static POINT arrow[cLight][8]; // 存储箭头的点,第一维：位置与cc[i]对应
							   // 第二维：0-5箭头，6-7箭杆
static RECT rect[cLight];	   // 框框数目，用于更新窗口指定区域
static POINT text[2];		   // 文本显示区域
static bool flag = false;      // 红绿灯周期改变的标志
static int xClient, yClient;   // 窗口的宽、高

void CreateArrowPoint();  // 创建箭头的点
void CreateRectPoint();   // 创建矩形的点，用于更新窗口指定区域
void CreateTextPoint();   // 创建文本显示区域位置
HFONT CreateCustomFont(); // 创建自定义字体
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszCmdLine, int iCmdShow)
{
	CIniFile *iniFile = new CIniFile();
	if ((*iniFile).SetFileName(TEXT("..\\Config.ini")))
	{
		greenTime = (*iniFile).GetValueInt(TEXT("LightTime"), TEXT("GreenTime"));
		yellowTime = (*iniFile).GetValueInt(TEXT("LightTime"), TEXT("YellowTime"));
	}
	if (greenTime < 5)
		greenTime = 5;
	if (yellowTime < 0)
		yellowTime = 0;
	allLightTime = 4 * (greenTime + yellowTime);

	static TCHAR szAppName[] = TEXT("Traffic_Light");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT or later!"), szAppName, MB_ICONERROR);
		return 0;
	}
	xClient = cLight * (5 + 2 * radius) + 5; //如果需要显示标题栏，则加15
	yClient = 10 + 2 * radius + 15; //如果需要显示标题栏，则加40
	hwnd = CreateWindowEx(WS_EX_TOPMOST, szAppName, TEXT("Traffic Light"),
		WS_POPUPWINDOW | WS_SIZEBOX | WS_MINIMIZEBOX,
		500, 200,
		xClient, yClient,
		NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0) > 0)	// 消息循环
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;		// 消息循环结束时将信息返回系统
}

/* 窗口函数 */
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM  wParam, LPARAM  lParam)
{
	HDC hdc;
	PAINTSTRUCT ps; // 定义包含绘图信息的结构体变量	
	static HPEN hPen;           // 画笔，画灯框
	static HPEN hPenRed;        // 红笔
	static HPEN hPenGreen;      // 绿笔
	static HPEN hPenYellow;     // 黄笔
	static HBRUSH hBrushRed;    // 红刷
	static HBRUSH hBrushGreen;  // 绿刷
	static HBRUSH hBrushYellow; // 黄刷
	static HFONT hFont;         // 自定义字体
	static HPEN hPenO;			// 外框笔
	
	switch (message)
	{
	case WM_CREATE:
		hPen = CreatePen(PS_SOLID, 1, RGB(50, 50, 50));
		hPenRed = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		hPenGreen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
		hPenYellow = CreatePen(PS_SOLID, 1, RGB(255, 255, 0));
		hBrushRed = CreateSolidBrush(RGB(255, 0, 0));
		hBrushGreen = CreateSolidBrush(RGB(0, 255, 0));
		hBrushYellow = CreateSolidBrush(RGB(255, 255, 0));
		hPenO = CreatePen(PS_SOLID, 2, RGB(200, 200, 200));
		SetTimer(hwnd, TIMER_SEC, 500, NULL); // 0.5秒钟更新一次窗口
		return 0;

	case WM_TIMER:
		if (flag == false) //判断是否更新时间
		{
			flag = true;
		}			
		else  // true表示更新时间
		{
			flag = false;
			if (++timeCount >= allLightTime)
				timeCount = 0;
			InvalidateRect(hwnd, &rect[4], TRUE);
			InvalidateRect(hwnd, &rect[10], TRUE);
		}

		if (
			(0 < timeCount) && (timeCount < 2 * (greenTime + yellowTime)) || // RR
			(2 * (greenTime + yellowTime) < timeCount && timeCount < 3 * greenTime + 2 * yellowTime - 3) || // RG
			(3 * greenTime + 2 * yellowTime < timeCount && timeCount < 3 * (greenTime + yellowTime)) || // RY
			(3 * (greenTime + yellowTime) < timeCount && timeCount < 4 * greenTime + 3 * yellowTime - 3) || // GR
			(4 * greenTime + 3 * yellowTime < timeCount && timeCount < 4 * (greenTime + yellowTime)))  // RR,只需要更新文字
			;
		else if (3 * greenTime + 2 * yellowTime - 3 <= timeCount &&
			timeCount < 3 * greenTime + 2 * yellowTime) // 需要更新文字和绿灯
		{
			InvalidateRect(hwnd, &rect[8], TRUE);
		}
		else if (4 * greenTime + 3 * yellowTime - 3 <= timeCount &&
			(timeCount < 4 * greenTime + 3 * yellowTime)) // 需要更新文字和绿灯
		{			
			InvalidateRect(hwnd, &rect[2], TRUE); 
		}
		else if (flag == false)
			InvalidateRect(hwnd, NULL, TRUE); // 整个窗口更新
		return 0;

	case WM_SIZE:
		yClient = HIWORD(lParam);
		radius = (yClient - 10) / 2;
		//xClient = cLight * (5 + 2 * radius) + 5;
		for (int i = 0; i < cLight; i++)
		{
			cc[i].x = 5 + radius + (2 * radius + 5) * i;
			cc[i].y = 5 + radius;
		}
		/* 尺寸改变时，重建箭头形状、字体大小、每一个rect区域、文字起始区域 */
		CreateArrowPoint();
		CreateRectPoint();
		CreateTextPoint();
		DeleteObject(hFont); // 重建字体前删除原有字体
		hFont = CreateCustomFont();
		return 0;

	case WM_LBUTTONDOWN: // 按住窗口可移动窗口
		SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps); // 取得设备环境句柄
		/* 画出外框的位置*/
		SelectObject(hdc, hPenO);
		SelectObject(hdc, (HBRUSH)GetStockObject(NULL_BRUSH));
		RoundRect(hdc, cc[0].x - radius - 2, cc[0].y - radius - 2,
			cc[2].x + radius + 3, cc[2].y + radius + 3, 5, 5);
		RoundRect(hdc, cc[6].x - radius - 2, cc[6].y - radius - 2,
			cc[8].x + radius + 3, cc[8].y + radius + 3, 5, 5);
		/* 画出6个灯的位置*/
		SelectObject(hdc, hPen);
		for (int i = 0; i < 3; i++) 
			Arc(hdc, cc[i].x - radius, cc[i].y - radius, cc[i].x + radius, cc[i].y + radius,
				cc[i].x, cc[i].x - radius, cc[i].x, cc[i].x - radius);
		for (int i = 6; i < 9; i++) 
			Arc(hdc, cc[i].x - radius, cc[i].y - radius, cc[i].x + radius, cc[i].y + radius,
				cc[i].x, cc[i].x - radius, cc[i].x, cc[i].x - radius);
		/* 
		红绿灯一个周期120s
		0-49:R R, 50-76:R G, 77-79:R G闪, 80-84:R Y, 85-111:G R, 112-114:G闪 R, 115-119:Y R
		 */
		if (0 <= timeCount && timeCount < 2 * (greenTime + yellowTime)) // R R
		{
			SelectObject(hdc, hPenRed);
			SelectObject(hdc, hBrushRed);
			Polygon(hdc,arrow[0], 6);
			Rectangle(hdc, arrow[0][6].x, arrow[0][6].y, arrow[0][7].x, arrow[0][7].y);
			Polygon(hdc, arrow[6], 6);
			Rectangle(hdc, arrow[6][6].x, arrow[6][6].y, arrow[6][7].x, arrow[6][7].y);
			/* 输出剩余时间	*/		
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, text[0].x, text[0].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 3 * (greenTime + yellowTime) - timeCount)); // 左灯红95s
			TextOut(hdc, text[1].x, text[1].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 2 * (greenTime + yellowTime) - timeCount)); // 右灯红55s
		}
		else if (timeCount < 3 * greenTime + 2 * yellowTime) // R G
		{
			SelectObject(hdc, hPenRed);
			SelectObject(hdc, hBrushRed);
			Polygon(hdc, arrow[0], 6);
			Rectangle(hdc, arrow[0][6].x, arrow[0][6].y, arrow[0][7].x, arrow[0][7].y);
			if (3 * greenTime + 2 * yellowTime - 3 < timeCount && flag == true) // 绿灯灭前闪烁3秒,0.5s亮，1s灭
				;
			else 
			{
				SelectObject(hdc, hPenGreen);
				SelectObject(hdc, hBrushGreen);
				Polygon(hdc, arrow[8], 6);
				Rectangle(hdc, arrow[8][6].x, arrow[8][6].y, arrow[8][7].x, arrow[8][7].y);
			}
			/* 输出剩余时间	*/
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, text[0].x, text[0].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 3 * (greenTime + yellowTime) - timeCount));
			SetTextColor(hdc, RGB(0, 255, 0));
			TextOut(hdc, text[1].x, text[1].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 3 * greenTime + 2 * yellowTime - timeCount)); // 右灯绿30s
		}
		else if (timeCount < 3 * (greenTime + yellowTime)) // R Y
		{
			SelectObject(hdc, hPenRed);
			SelectObject(hdc, hBrushRed);
			Polygon(hdc, arrow[0], 6);
			Rectangle(hdc, arrow[0][6].x, arrow[0][6].y, arrow[0][7].x, arrow[0][7].y);
			SelectObject(hdc, hPenYellow);
			SelectObject(hdc, hBrushYellow);
			Polygon(hdc, arrow[7], 6);
			Rectangle(hdc, arrow[7][6].x, arrow[7][6].y, arrow[7][7].x, arrow[7][7].y);
			/* 输出剩余时间	*/
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, text[0].x, text[0].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 3 * (greenTime + yellowTime) - timeCount));
			SetTextColor(hdc, RGB(255, 255, 0));
			TextOut(hdc, text[1].x, text[1].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 3 * (greenTime + yellowTime) - timeCount)); // 右灯黄5s
		}
		else if (timeCount < 4 * greenTime + 3 * yellowTime) // G R
		{
			if (4 * greenTime + 3 * yellowTime - 3 < timeCount && flag == true) // 绿灯灭前闪烁3秒
				;
			else
			{
				SelectObject(hdc, hPenGreen);
				SelectObject(hdc, hBrushGreen);
				Polygon(hdc, arrow[2], 6);
				Rectangle(hdc, arrow[2][6].x, arrow[2][6].y, arrow[2][7].x, arrow[2][7].y);
			}
			SelectObject(hdc, hPenRed);
			SelectObject(hdc, hBrushRed);
			Polygon(hdc, arrow[6], 6);
			Rectangle(hdc, arrow[6][6].x, arrow[6][6].y, arrow[6][7].x, arrow[6][7].y);
			/* 输出剩余时间	*/
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(0, 255, 0));
			TextOut(hdc, text[0].x, text[0].y, szTime,
				wsprintf(szTime, TEXT("%02d"), allLightTime - yellowTime - timeCount)); // 左灯绿30s
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, text[1].x, text[1].y, szTime,
				wsprintf(szTime, TEXT("%02d"), allLightTime - timeCount + 50)); // 右灯红
		}
		else if(timeCount < 4 * (greenTime + yellowTime)) // Y R
		{
			SelectObject(hdc, hPenYellow);
			SelectObject(hdc, hBrushYellow);
			Polygon(hdc, arrow[1], 6);
			Rectangle(hdc, arrow[1][6].x, arrow[1][6].y, arrow[1][7].x, arrow[1][7].y);
			SelectObject(hdc, hPenRed);
			SelectObject(hdc, hBrushRed);
			Polygon(hdc, arrow[6], 6);
			Rectangle(hdc, arrow[6][6].x, arrow[6][6].y, arrow[6][7].x, arrow[6][7].y);
			/* 输出剩余时间	*/
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(255, 255, 0));
			TextOut(hdc, text[0].x, text[0].y, szTime,
				wsprintf(szTime, TEXT("%02d"), allLightTime - timeCount)); // 左灯黄5s
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, text[1].x, text[1].y, szTime,
				wsprintf(szTime, TEXT("%02d"), allLightTime - timeCount + 50)); // 右灯红
		}
		EndPaint(hwnd, &ps);
		return 0;

	case WM_RBUTTONUP: // 按下并抬起鼠标右键时关闭窗口
	case WM_DESTROY:		
		DeleteObject(hPen);
		DeleteObject(hPenRed);
		DeleteObject(hPenGreen);
		DeleteObject(hPenYellow);
		DeleteObject(hBrushRed);
		DeleteObject(hBrushGreen);
		DeleteObject(hBrushYellow);
		DeleteObject(hFont);
		DeleteObject(hPenO);
		PostQuitMessage(0);
		return 0;

	default:
		return  DefWindowProc(hwnd, message, wParam, lParam);
	}
}
/* 
存储箭头的点,第一维：位置与cc[i]对应
第二维：0-5箭头，6-7箭杆
*/
void CreateArrowPoint()
{
	for (int i = 0; i < 3; i++)
	{
		arrow[i][0].x = cc[i].x - radius * 9 / 80;			arrow[i][0].y = cc[i].y - radius * 8 / 10;
		arrow[i][1].x = cc[i].x - radius * 18 / 20;			arrow[i][1].y = cc[i].y;
		arrow[i][2].x = cc[i].x - radius * 9 / 80;			arrow[i][2].y = cc[i].y + radius * 8 / 10;
		arrow[i][3].x = arrow[i][2].x + radius * 2 / 5;		arrow[i][3].y = cc[i].y + radius * 8 / 10;
		arrow[i][4].x = arrow[i][1].x + radius * 2 / 5;		arrow[i][4].y = cc[i].y;
		arrow[i][5].x = arrow[i][0].x + radius * 2 / 5;		arrow[i][5].y = cc[i].y - radius * 8 / 10;
		arrow[i][6].x = cc[i].x - radius *  4 / 20;			arrow[i][6].y = cc[i].y - radius * 2 / 10;
		arrow[i][7].x = cc[i].x + radius * 18 / 20;			arrow[i][7].y = arrow[i][6].y + radius * 2 / 5;
	}
	for (int i = 6; i < 9; i++)
	{
		arrow[i][0].y = cc[i].y - radius * 9 / 80;			arrow[i][0].x = cc[i].x - radius * 8 / 10;
		arrow[i][1].y = cc[i].y - radius * 18 / 20;			arrow[i][1].x = cc[i].x;
		arrow[i][2].y = cc[i].y - radius * 9 / 80;			arrow[i][2].x = cc[i].x + radius * 8 / 10;
		arrow[i][3].y = arrow[i][2].y + radius * 2 / 5;		arrow[i][3].x = cc[i].x + radius * 8 / 10;
		arrow[i][4].y = arrow[i][1].y + radius * 2 / 5;		arrow[i][4].x = cc[i].x;
		arrow[i][5].y = arrow[i][0].y + radius * 2 / 5;		arrow[i][5].x = cc[i].x - radius * 8 / 10;
		arrow[i][6].y = cc[i].y - radius *  4 / 20;			arrow[i][6].x = cc[i].x - radius * 2 / 10;
		arrow[i][7].y = cc[i].y + radius * 18 / 20;			arrow[i][7].x = arrow[i][6].x + radius * 2 / 5;
	}
}

void CreateRectPoint()
{
	for (int i = 0; i < cLight; i++)
	{
		rect[i].left = cc[i].x - radius;
		rect[i].right = cc[i].x + radius + 5;
		rect[i].top = cc[i].y - radius;
		rect[i].bottom = cc[i].y + radius;
	}
}

void CreateTextPoint()
{
	text[0].x = rect[4].left; text[0].y = rect[4].top;
	text[1].x = rect[10].left; text[1].y = rect[10].top;
}

HFONT CreateCustomFont()
{
	return CreateFont(
		2 * radius, 0, 0, 0,
		FW_HEAVY,
		0, 0, 0, GB2312_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, L"黑体"
		);
}