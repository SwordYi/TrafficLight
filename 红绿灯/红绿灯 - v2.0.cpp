#include <windows.h>
#define TIMER_SEC 1
static const int cLight = 10; // 灯数
static POINT cc[cLight]; // 圆心坐标
static int  radius = 15; // 圆的半径
static TCHAR szTime[3];  // 时间存储位置
static int tLight = 0; // 红绿灯周期，120s一个周期
static bool flag = false; // 红绿灯周期改变的标志,false:0.5s,true:1s
static int xClient, yClient; // 窗口的宽、高

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
	static HPEN hPen;           // 画笔，画灯框
	static HBRUSH hBrushRed;    // 红刷 
	static HBRUSH hBrushGreen;  // 绿刷 
	static HBRUSH hBrushYellow; // 黄刷 
	static HFONT hFont;         // 自定义字体

	switch (message)
	{
	case WM_CREATE:
		hPen = CreatePen(PS_SOLID, 1, RGB(50, 50, 50));
		hBrushRed = CreateSolidBrush(RGB(255, 0, 0)); 
		hBrushGreen = CreateSolidBrush(RGB(0, 255, 0));
		hBrushYellow = CreateSolidBrush(RGB(255, 255, 0));
		SetTimer(hwnd, TIMER_SEC, 500, NULL); // 1秒钟更新一次窗口
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
		for (int i = 0; i < cLight; i++) // 计算圆心
		{
			cc[i].x = 5 + radius + (2 * radius + 5) * i;
			cc[i].y = 5 + radius;
		}
		/* 尺寸改变时，重建字体大小 */
		DeleteObject(hFont);
		hFont = CreateFont( // 自建字体
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
		hdc = BeginPaint(hwnd, &ps);		// 取得设备环境句柄
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
			SelectObject(hdc, hBrushRed);
			Ellipse(hdc, cc[0].x - radius, cc[0].y - radius, cc[0].x + radius, cc[0].y + radius);
			Ellipse(hdc, cc[6].x - radius, cc[0].y - radius, cc[6].x + radius, cc[0].y + radius);
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
			SelectObject(hdc, hBrushRed);
			Ellipse(hdc, cc[0].x - radius, cc[0].y - radius, cc[0].x + radius, cc[0].y + radius);
			if (76 < tLight && flag == true) // 绿灯灭前闪烁3秒
				;
			else 
			{
				SelectObject(hdc, hBrushGreen);
				Ellipse(hdc, cc[8].x - radius, cc[8].y - radius, cc[8].x + radius, cc[8].y + radius);
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
			SelectObject(hdc, hBrushRed);
			Ellipse(hdc, cc[0].x - radius, cc[0].y - radius, cc[0].x + radius, cc[0].y + radius);
			SelectObject(hdc, hBrushYellow);
			Ellipse(hdc, cc[7].x - radius, cc[7].y - radius, cc[7].x + radius, cc[7].y + radius);
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
				SelectObject(hdc, hBrushGreen);
				Ellipse(hdc, cc[2].x - radius, cc[2].y - radius, cc[2].x + radius, cc[2].y + radius);
			}
			SelectObject(hdc, hBrushRed);
			Ellipse(hdc, cc[6].x - radius, cc[6].y - radius, cc[6].x + radius, cc[6].y + radius);
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
			SelectObject(hdc, hBrushYellow);
			Ellipse(hdc, cc[1].x - radius, cc[1].y - radius, cc[1].x + radius, cc[1].y + radius);
			SelectObject(hdc, hBrushRed);
			Ellipse(hdc, cc[6].x - radius, cc[6].y - radius, cc[6].x + radius, cc[6].y + radius);
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
		DeleteObject(hBrushRed);
		DeleteObject(hBrushGreen);
		DeleteObject(hBrushYellow);
		DeleteObject(hFont);
		PostQuitMessage(0);
		return 0;

	default:
		return  DefWindowProc(hwnd, message, wParam, lParam);
	}
}