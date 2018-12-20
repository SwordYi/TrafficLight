#include <windows.h>
#define TIMER_SEC 1
static const int cLight = 10; // 灯数
static POINT cc[cLight]; // 圆心坐标
static int  radius = 15; // 圆的半径
static TCHAR szTime[3];  // 时间存储位置
static int tLight = 0;   // 红绿灯周期，120s一个周期
static POINT arrow[cLight][5]; // 存储箭头的点,第一维：位置与cc[i]对应
							   // 第二维：0-3箭头，4-5箭杆
static bool flag = false;      // 红绿灯周期改变的标志
static int xClient, yClient;   // 窗口的宽、高

void CreateArrow();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszCmdLine, int iCmdShow)
{
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

//窗口函数
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM  wParam, LPARAM  lParam)
{
	HDC hdc;
	PAINTSTRUCT ps; // 定义包含绘图信息的结构体变量	
	static HPEN hPen;       // 画笔，画灯框
	static HPEN hPenRed;    // 红笔
	static HPEN hPenGreen;  // 绿笔
	static HPEN hPenYellow; // 黄笔
	static HFONT hFont;     // 自定义字体
	switch (message)
	{
	case WM_CREATE:
		hPen = CreatePen(PS_SOLID, 1, RGB(50, 50, 50));
		SetTimer(hwnd, TIMER_SEC, 500, NULL); // 0.5秒钟更新一次窗口
		return 0;

	case WM_TIMER:
		if (flag == false)
		{
			flag = true;
		}			
		else
		{
			flag = false;
			if (++tLight >= 120)
				tLight = 0;
		}
		InvalidateRect(hwnd, NULL, TRUE);
		UpdateWindow(hwnd);
		return 0;

	case WM_SIZE:
		xClient = LOWORD(lParam);
		yClient = HIWORD(lParam);
		radius = (yClient - 10) / 2;
		for (int i = 0; i < cLight; i++)
		{
			cc[i].x = 5 + radius + (2 * radius + 5) * i;
			cc[i].y = 5 + radius;
		}
		/* 尺寸改变时，重建箭头形状、画笔、字体大小 */
		CreateArrow();
		DeleteObject(hPenRed); // 重建画笔
		DeleteObject(hPenGreen);
		DeleteObject(hPenYellow);
		hPenRed = CreatePen(PS_SOLID, radius * 2 / 5, RGB(255, 0, 0));
		hPenGreen = CreatePen(PS_SOLID, radius * 2 / 5, RGB(0, 255, 0));
		hPenYellow = CreatePen(PS_SOLID, radius * 2 / 5, RGB(255, 255, 0));
		DeleteObject(hFont); // 重建字体
		hFont = CreateFont( 
			2 * radius, 0, 0, 0,
			FW_HEAVY,
			0, 0, 0, GB2312_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, L"abc"
			);
		return 0;

	case WM_LBUTTONDOWN: // 按住窗口可以移动窗口
		SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps); // 取得设备环境句柄
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
		if ((0 <= tLight) && (tLight < 50)) // R R
		{
			SelectObject(hdc, hPenRed);
			Polyline(hdc, arrow[0], 3);
			MoveToEx(hdc, arrow[0][3].x, arrow[0][3].y, NULL);
			LineTo(hdc, arrow[0][4].x, arrow[0][4].y);
			Polyline(hdc, arrow[6], 3);
			MoveToEx(hdc, arrow[6][3].x, arrow[6][3].y, NULL);
			LineTo(hdc, arrow[6][4].x, arrow[6][4].y);
			// 输出剩余时间			
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, cc[3].x - radius, cc[3].y - radius, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight)); // 左灯红95s
			TextOut(hdc, cc[9].x - radius, cc[9].y - radius, szTime,
				wsprintf(szTime, TEXT("%02d"), 50 - tLight)); // 右灯红55s
		}
		else if (tLight < 80) // R G
		{
			SelectObject(hdc, hPenRed);
			Polyline(hdc, arrow[0], 3);
			MoveToEx(hdc, arrow[0][3].x, arrow[0][3].y, NULL);
			LineTo(hdc, arrow[0][4].x, arrow[0][4].y);
			if (76 < tLight && flag == true) // 绿灯灭前闪烁3秒
				;
			else 
			{
				SelectObject(hdc, hPenGreen);
				Polyline(hdc, arrow[8], 3);
				MoveToEx(hdc, arrow[8][3].x, arrow[8][3].y, NULL);
				LineTo(hdc, arrow[8][4].x, arrow[8][4].y);
			}
			// 输出剩余时间
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, cc[3].x - radius, cc[3].y - radius, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight));
			SetTextColor(hdc, RGB(0, 255, 0));
			TextOut(hdc, cc[9].x - radius, cc[9].y - radius, szTime,
				wsprintf(szTime, TEXT("%02d"), 80 - tLight)); // 右灯绿30s
		}
		else if (tLight < 85) // R Y
		{
			SelectObject(hdc, hPenRed);
			Polyline(hdc, arrow[0], 3);
			MoveToEx(hdc, arrow[0][3].x, arrow[0][3].y, NULL);
			LineTo(hdc, arrow[0][4].x, arrow[0][4].y);
			SelectObject(hdc, hPenYellow);
			Polyline(hdc, arrow[7], 3);
			MoveToEx(hdc, arrow[7][3].x, arrow[7][3].y, NULL);
			LineTo(hdc, arrow[7][4].x, arrow[7][4].y);
			// 输出剩余时间
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, cc[3].x - radius, cc[3].y - radius, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight));
			SetTextColor(hdc, RGB(255, 255, 0));
			TextOut(hdc, cc[9].x - radius, cc[9].y - radius, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight)); // 右灯黄5s
		}
		else if (tLight < 115) // G R
		{
			if (111 < tLight && flag == true) // 绿灯灭前闪烁3秒
				;
			else
			{
				SelectObject(hdc, hPenGreen);
				Polyline(hdc, arrow[2], 3);
				MoveToEx(hdc, arrow[2][3].x, arrow[2][3].y, NULL);
				LineTo(hdc, arrow[2][4].x, arrow[2][4].y);
			}
			SelectObject(hdc, hPenRed);
			Polyline(hdc, arrow[6], 3);
			MoveToEx(hdc, arrow[6][3].x, arrow[6][3].y, NULL);
			LineTo(hdc, arrow[6][4].x, arrow[6][4].y);
			// 输出剩余时间
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(0, 255, 0));
			TextOut(hdc, cc[3].x - radius, cc[3].y - radius, szTime,
				wsprintf(szTime, TEXT("%02d"), 115 - tLight)); // 左灯绿30s
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, cc[9].x - radius, cc[9].y - radius, szTime,
				wsprintf(szTime, TEXT("%02d"), 120 - tLight + 50)); // 右灯红
		}
		else if(tLight < 120) // Y R
		{
			SelectObject(hdc, hPenYellow);
			Polyline(hdc, arrow[1], 3);
			MoveToEx(hdc, arrow[1][3].x, arrow[1][3].y, NULL);
			LineTo(hdc, arrow[1][4].x, arrow[1][4].y);
			SelectObject(hdc, hPenRed);
			Polyline(hdc, arrow[6], 3);
			MoveToEx(hdc, arrow[6][3].x, arrow[6][3].y, NULL);
			LineTo(hdc, arrow[6][4].x, arrow[6][4].y);
			// 输出剩余时间
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(255, 255, 0));
			TextOut(hdc, cc[3].x - radius, cc[3].y - radius, szTime,
				wsprintf(szTime, TEXT("%02d"), 120 - tLight)); // 左灯黄5s
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, cc[9].x - radius, cc[9].y - radius, szTime,
				wsprintf(szTime, TEXT("%02d"), 120 - tLight + 50)); // 右灯红
		}
		EndPaint(hwnd, &ps);
		return 0;

	case WM_RBUTTONUP: // 按下并抬起鼠标右键时关闭窗口
	case WM_DESTROY:		
		DeleteObject(hPen);
		DeleteObject(hPenRed);
		DeleteObject(hPenGreen);
		DeleteObject(hPenYellow);
		DeleteObject(hFont);
		PostQuitMessage(0);
		return 0;

	default:
		return  DefWindowProc(hwnd, message, wParam, lParam);
	}
}
/* 
存储箭头的点,第一维：位置与cc[i]对应
第二维：0-3箭头，4-5箭杆
*/
void CreateArrow()
{
	for (int i = 0; i < 3; i++)
	{
		arrow[i][0].x = cc[i].x;					arrow[i][0].y = cc[i].y - radius * 7 / 10;
		arrow[i][1].x = cc[i].x - radius * 7 / 10;	arrow[i][1].y = cc[i].y;
		arrow[i][2].x = cc[i].x;					arrow[i][2].y = cc[i].y + radius * 7 / 10;
		arrow[i][3].x = cc[i].x;					arrow[i][3].y = cc[i].y;
		arrow[i][4].x = cc[i].x + radius * 7 / 10;	arrow[i][4].y = cc[i].y;
	}
	for (int i = 6; i < 9; i++)
	{
		arrow[i][0].x = cc[i].x - radius * 7 / 10;	arrow[i][0].y = cc[i].y;
		arrow[i][1].x = cc[i].x;					arrow[i][1].y = cc[i].y - radius * 7 / 10;
		arrow[i][2].x = cc[i].x + radius * 7 / 10;	arrow[i][2].y = cc[i].y;
		arrow[i][3].x = cc[i].x;					arrow[i][3].y = cc[i].y;
		arrow[i][4].x = cc[i].x;					arrow[i][4].y = cc[i].y + radius * 7 / 10;
	}
}