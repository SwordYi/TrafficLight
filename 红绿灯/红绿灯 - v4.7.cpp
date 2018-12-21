/**
	ProductName:Traffic Light
	Author:SwordYi
	CreateTime:2015-10-05
	ModifyTime:2018-12-20
*/

#include <Windows.h>
#include "CIniFile.h"
#include "resource.h"

#define TIMER_SEC 1
static const int cLight = 21; // 灯数
static POINT cc[cLight]; // 圆心坐标
static int  radius = 15; // 圆的半径
static TCHAR szTime[3];  // 时间存储位置
static int tLight = 0;   // 红绿灯周期，120s一个周期
static POINT arrow[cLight][8]; // 存储箭头的点,第一维：位置与cc[i]对应
							   // 第二维：0-5箭头，6-7箭杆
static RECT rect[cLight];	   // 框框数目，用于更新窗口指定区域
static POINT text[5];		   // 文本显示区域
static bool flag = false;      // 红绿灯周期改变的标志
static int xClient, yClient;   // 窗口的宽、高

void CreateArrowPoint();  // 创建箭头的点
void CreateRectPoint();   // 创建矩形的点，用于更新窗口指定区域
void CreateTextPoint();   // 创建文本显示区域位置
HFONT CreateCustomFont(); // 创建自定义字体
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void SetColor(COLORREF& colorRed, COLORREF& colorGreen, COLORREF& colorYellow); //设置颜色

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
	wndclass.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_ICON2);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT or later!"), szAppName, MB_ICONERROR);
		return 0;
	}
	xClient = cLight * (5 + 2 * radius) + 5 - 20; //如果需要显示标题栏，则加15。减去20，是因为cLight=21个灯位造成后面黑边太多了，如果减少灯位，就不要减20啦
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
	static COLORREF colorRed;      // 红色
	static COLORREF colorGreen;    // 绿色
	static COLORREF colorYellow;   // 黄色
	
	switch (message)
	{
	case WM_CREATE:
		SetColor(colorRed, colorGreen, colorYellow);
		
		hPen = CreatePen(PS_SOLID, 1, RGB(50, 50, 50));
		hPenRed = CreatePen(PS_SOLID, 1, colorRed);
		hPenGreen = CreatePen(PS_SOLID, 1, colorGreen);
		hPenYellow = CreatePen(PS_SOLID, 1, colorYellow);
		hBrushRed = CreateSolidBrush(colorRed);
		hBrushGreen = CreateSolidBrush(colorGreen);
		hBrushYellow = CreateSolidBrush(colorYellow);
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
			if (++tLight >= 120)
				tLight = 0;
			InvalidateRect(hwnd, &rect[4], TRUE);
			InvalidateRect(hwnd, &rect[10], TRUE);
			InvalidateRect(hwnd, &rect[14], TRUE);
            InvalidateRect(hwnd, &rect[19], TRUE);
            InvalidateRect(hwnd, &rect[20], TRUE);
		}

		if ((0 < tLight) && (tLight < 50) || (50 < tLight) && (tLight < 77) ||
			(80 < tLight) && (tLight < 85) || (85 < tLight) && (tLight < 112) ||
			(115 < tLight) && (tLight < 120))  // 只需要更新文字
			;
		else if ((77 <= tLight) && (tLight < 80)) // 需要更新文字和绿灯
		{
			InvalidateRect(hwnd, &rect[8], TRUE);	// 更新单色直行灯
			InvalidateRect(hwnd, &rect[12], TRUE);	// 更新多色圆形灯
			InvalidateRect(hwnd, &rect[16], TRUE);	// 更新多色左转灯
			InvalidateRect(hwnd, &rect[17], TRUE);	// 更行多色直行灯
		}
		else if ((112 <= tLight) && (tLight < 115)) // 需要更新文字和绿灯
		{			
			InvalidateRect(hwnd, &rect[2], TRUE);	// 更新单色左转灯
			InvalidateRect(hwnd, &rect[12], TRUE);	// 更新多色圆形灯
			InvalidateRect(hwnd, &rect[16], TRUE);	// 更新多色左转灯
			InvalidateRect(hwnd, &rect[17], TRUE);	// 更行多色直行灯
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
		RoundRect(hdc, cc[12].x - radius - 2, cc[12].y - radius - 2,
			cc[12].x + radius + 3, cc[12].y + radius + 3, 5, 5);  //多色灯圆形
        RoundRect(hdc, cc[16].x - radius - 2, cc[16].y - radius - 2,
			cc[17].x + radius + 3, cc[17].y + radius + 3, 5, 5);  //多色灯箭形   
		/* 画出灯的位置*/
		SelectObject(hdc, hPen);
		for (int i = 0; i < 3; i++)  // 单色左转灯位置
			Arc(hdc, cc[i].x - radius, cc[i].y - radius, cc[i].x + radius, cc[i].y + radius,
				cc[i].x, cc[i].x - radius, cc[i].x, cc[i].x - radius);
		for (int i = 6; i < 9; i++) // 单色直行灯位置
			Arc(hdc, cc[i].x - radius, cc[i].y - radius, cc[i].x + radius, cc[i].y + radius,
				cc[i].x, cc[i].x - radius, cc[i].x, cc[i].x - radius);
		Arc(hdc, cc[12].x - radius, cc[12].y - radius, cc[12].x + radius, cc[12].y + radius,
			cc[12].x, cc[12].x - radius, cc[12].x, cc[12].x - radius); // 多色灯圆形的位置
        for (int i = 16; i < 18; i++)   // 多色灯箭形的位置
			Arc(hdc, cc[i].x - radius, cc[i].y - radius, cc[i].x + radius, cc[i].y + radius,
				cc[i].x, cc[i].x - radius, cc[i].x, cc[i].x - radius);
		/* 
		红绿灯一个周期120s
		0-49:R R, 50-76:R G, 77-79:R G闪, 80-84:R Y, 85-111:G R, 112-114:G闪 R, 115-119:Y R
		 */
		if ((0 <= tLight) && (tLight < 50)) // R R R
		{
			//左转红灯
			SelectObject(hdc, hPenRed);
			SelectObject(hdc, hBrushRed);
            // 左转单色红灯
			Polygon(hdc,arrow[0], 6);
			Rectangle(hdc, arrow[0][6].x, arrow[0][6].y, arrow[0][7].x, arrow[0][7].y);            
			// 直行单色红灯
			Polygon(hdc, arrow[6], 6);
			Rectangle(hdc, arrow[6][6].x, arrow[6][6].y, arrow[6][7].x, arrow[6][7].y);            
			// 多色灯圆形红灯
			Ellipse(hdc, cc[12].x - radius, cc[12].y - radius, cc[12].x + radius, cc[12].y + radius);
            // 多色左转红灯
            Polygon(hdc,arrow[16], 6);
			Rectangle(hdc, arrow[16][6].x, arrow[16][6].y, arrow[16][7].x, arrow[16][7].y);
            // 多色直行红灯
            Polygon(hdc,arrow[17], 6);
			Rectangle(hdc, arrow[17][6].x, arrow[17][6].y, arrow[17][7].x, arrow[17][7].y);

			/* 输出剩余时间	*/		
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, colorRed);
			TextOut(hdc, text[0].x, text[0].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight)); // 单色左转灯红95s
			TextOut(hdc, text[1].x, text[1].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 50 - tLight)); // 单色直行灯红55s
			TextOut(hdc, text[2].x, text[2].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 50 - tLight)); // 多色圆形灯红55s
			TextOut(hdc, text[3].x, text[3].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight)); // 多色左转灯红95s
			TextOut(hdc, text[4].x, text[4].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 50 - tLight)); // 多色直行灯红55s
		}
		else if (tLight < 80) // R G G
		{
			// 单色左转红灯
			SelectObject(hdc, hPenRed);
			SelectObject(hdc, hBrushRed);
			Polygon(hdc, arrow[0], 6);
			Rectangle(hdc, arrow[0][6].x, arrow[0][6].y, arrow[0][7].x, arrow[0][7].y);
			// 单色直行绿灯
			if (76 < tLight && flag == true) // 绿灯灭前闪烁3秒,0.5s亮，1s灭
				;
			else 
			{
				SelectObject(hdc, hPenGreen);
				SelectObject(hdc, hBrushGreen);
				Polygon(hdc, arrow[8], 6);
				Rectangle(hdc, arrow[8][6].x, arrow[8][6].y, arrow[8][7].x, arrow[8][7].y);				
			}
			// 多色圆形绿灯
			SelectObject(hdc, hPenGreen);
			SelectObject(hdc, hBrushGreen);
			Ellipse(hdc, cc[12].x - radius, cc[12].y - radius, cc[12].x + radius, cc[12].y + radius);
			// 多色左转红灯
			SelectObject(hdc, hPenRed);
			SelectObject(hdc, hBrushRed);
			Polygon(hdc, arrow[16], 6);
			Rectangle(hdc, arrow[16][6].x, arrow[16][6].y, arrow[16][7].x, arrow[16][7].y);
			// 多色直行绿灯
			if (76 < tLight && flag == true) // 绿灯灭前闪烁3秒,0.5s亮，1s灭
				;
			else
			{
				SelectObject(hdc, hPenGreen);
				SelectObject(hdc, hBrushGreen);
				Polygon(hdc, arrow[17], 6);
				Rectangle(hdc, arrow[17][6].x, arrow[17][6].y, arrow[17][7].x, arrow[17][7].y);
			}

			/* 输出剩余时间	*/
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			// 单色左转红色
			SetTextColor(hdc, colorRed);
			TextOut(hdc, text[0].x, text[0].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight));
			// 单色直行绿色
			SetTextColor(hdc, colorGreen);
			TextOut(hdc, text[1].x, text[1].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 80 - tLight)); // 右灯绿30s。
			// 多色圆形
			TextOut(hdc, text[2].x, text[2].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 115 - tLight)); // 多色灯绿30s
			// 多色左转红色
			SetTextColor(hdc, colorRed);
			TextOut(hdc, text[3].x, text[3].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight));
			// 多色直行绿色
			SetTextColor(hdc, colorGreen);
			TextOut(hdc, text[4].x, text[4].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 80 - tLight)); // 右灯绿30s
		}
		else if (tLight < 85) // R Y G
		{
			// 单色左转红灯
			SelectObject(hdc, hPenRed);
			SelectObject(hdc, hBrushRed);
			Polygon(hdc, arrow[0], 6);
			Rectangle(hdc, arrow[0][6].x, arrow[0][6].y, arrow[0][7].x, arrow[0][7].y);
			// 单色直行黄灯
			SelectObject(hdc, hPenYellow);
			SelectObject(hdc, hBrushYellow);
			Polygon(hdc, arrow[7], 6);
			Rectangle(hdc, arrow[7][6].x, arrow[7][6].y, arrow[7][7].x, arrow[7][7].y);
			// 多色圆形绿灯
			SelectObject(hdc, hPenGreen);
			SelectObject(hdc, hBrushGreen);
			Ellipse(hdc, cc[12].x - radius, cc[12].y - radius, cc[12].x + radius, cc[12].y + radius);
			// 多色左转红灯
			SelectObject(hdc, hPenRed);
			SelectObject(hdc, hBrushRed);
			Polygon(hdc, arrow[16], 6);
			Rectangle(hdc, arrow[16][6].x, arrow[16][6].y, arrow[16][7].x, arrow[16][7].y);
			// 多色直行黄灯
			SelectObject(hdc, hPenYellow);
			SelectObject(hdc, hBrushYellow);
			Polygon(hdc, arrow[17], 6);
			Rectangle(hdc, arrow[17][6].x, arrow[17][6].y, arrow[17][7].x, arrow[17][7].y);

			/* 输出剩余时间	*/
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			// 单色左转红色
			SetTextColor(hdc, colorRed);
			TextOut(hdc, text[0].x, text[0].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight));
			// 单色直行绿色
			SetTextColor(hdc, colorYellow);
			TextOut(hdc, text[1].x, text[1].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight)); // 右灯黄5s
			// 多色圆形红色
			SetTextColor(hdc, colorGreen);
			TextOut(hdc, text[2].x, text[2].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 115 - tLight)); // 多色灯绿灯
			// 多色左转红色
			SetTextColor(hdc, colorRed);
			TextOut(hdc, text[3].x, text[3].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight));
			// 多色直行绿色
			SetTextColor(hdc, colorYellow);
			TextOut(hdc, text[4].x, text[4].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight)); // 右灯黄5s
		}
		else if (tLight < 115) // G R G
		{
			// 单色左转绿灯
			if (111 < tLight && flag == true) // 绿灯灭前闪烁3秒
				;
			else
			{
				SelectObject(hdc, hPenGreen);
				SelectObject(hdc, hBrushGreen);
				Polygon(hdc, arrow[2], 6);
				Rectangle(hdc, arrow[2][6].x, arrow[2][6].y, arrow[2][7].x, arrow[2][7].y);
			}
			// 单色直行红灯
			SelectObject(hdc, hPenRed);
			SelectObject(hdc, hBrushRed);
			Polygon(hdc, arrow[6], 6);
			Rectangle(hdc, arrow[6][6].x, arrow[6][6].y, arrow[6][7].x, arrow[6][7].y);
			// 多色左转绿灯
			if (111 < tLight && flag == true) // 绿灯灭前闪烁3秒
				;
			else
			{
				SelectObject(hdc, hPenGreen);
				SelectObject(hdc, hBrushGreen);	
				Ellipse(hdc, cc[12].x - radius, cc[12].y - radius, cc[12].x + radius, cc[12].y + radius);
			}
			// 多色左转绿灯
			if (111 < tLight && flag == true) // 绿灯灭前闪烁3秒
				;
			else
			{
				SelectObject(hdc, hPenGreen);
				SelectObject(hdc, hBrushGreen);
				Polygon(hdc, arrow[16], 6);
				Rectangle(hdc, arrow[16][6].x, arrow[16][6].y, arrow[16][7].x, arrow[16][7].y);
			}
			// 多色直行红灯
			SelectObject(hdc, hPenRed);
			SelectObject(hdc, hBrushRed);
			Polygon(hdc, arrow[17], 6);
			Rectangle(hdc, arrow[17][6].x, arrow[17][6].y, arrow[17][7].x, arrow[17][7].y);

			/* 输出剩余时间	*/
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			// 单色左转绿色
			SetTextColor(hdc, colorGreen);
			TextOut(hdc, text[0].x, text[0].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 115 - tLight)); // 左灯绿30s
			// 单色直行红色
			SetTextColor(hdc, colorRed);
			TextOut(hdc, text[1].x, text[1].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 120 - tLight + 50)); // 右灯红
			// 多色圆形绿色
			SetTextColor(hdc, colorGreen);
			TextOut(hdc, text[2].x, text[2].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 115 - tLight)); // 多色灯绿灯闪烁5秒
			// 多色左转绿色
			SetTextColor(hdc, colorGreen);
			TextOut(hdc, text[3].x, text[3].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 115 - tLight)); // 左灯绿30s
			// 多色直行红色
			SetTextColor(hdc, colorRed);
			TextOut(hdc, text[4].x, text[4].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 120 - tLight + 50)); // 右灯红
		}
		else if(tLight < 120) // Y R Y
		{
			// 单色左转黄灯
			SelectObject(hdc, hPenYellow);
			SelectObject(hdc, hBrushYellow);
			Polygon(hdc, arrow[1], 6);
			Rectangle(hdc, arrow[1][6].x, arrow[1][6].y, arrow[1][7].x, arrow[1][7].y);
			// 单色直行红灯
			SelectObject(hdc, hPenRed);
			SelectObject(hdc, hBrushRed);
			Polygon(hdc, arrow[6], 6);
			Rectangle(hdc, arrow[6][6].x, arrow[6][6].y, arrow[6][7].x, arrow[6][7].y);
			// 多色左转黄灯
			SelectObject(hdc, hPenYellow);
			SelectObject(hdc, hBrushYellow);
			Ellipse(hdc, cc[12].x - radius, cc[12].y - radius, cc[12].x + radius, cc[12].y + radius);
			// 多色左转黄灯
			SelectObject(hdc, hPenYellow);
			SelectObject(hdc, hBrushYellow);
			Polygon(hdc, arrow[16], 6);
			Rectangle(hdc, arrow[16][6].x, arrow[16][6].y, arrow[16][7].x, arrow[16][7].y);
			// 多色直行红灯
			SelectObject(hdc, hPenRed);
			SelectObject(hdc, hBrushRed);
			Polygon(hdc, arrow[17], 6);
			Rectangle(hdc, arrow[17][6].x, arrow[17][6].y, arrow[17][7].x, arrow[17][7].y);
			
			/* 输出剩余时间	*/
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			// 单色左转黄色
			SetTextColor(hdc, colorYellow);
			TextOut(hdc, text[0].x, text[0].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 120 - tLight)); // 左灯黄5s
			// 单色直行红色
			SetTextColor(hdc, colorRed);
			TextOut(hdc, text[1].x, text[1].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 120 - tLight + 50)); // 右灯红
			// 多色圆形黄色
			SetTextColor(hdc, colorYellow);
			TextOut(hdc, text[2].x, text[2].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 120 - tLight)); // 多色灯黄
			// 多色左转黄色
			SetTextColor(hdc, colorYellow);
			TextOut(hdc, text[3].x, text[3].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 120 - tLight)); // 左灯黄5s
			// 多色直行红色
			SetTextColor(hdc, colorRed);
			TextOut(hdc, text[4].x, text[4].y, szTime,
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
	// 左转灯
	for (int i = 0; (i < cLight) && (i < 3 || i ==16); i++)
	{
		arrow[i][0].x = cc[i].x - radius * 9 / 80;			arrow[i][0].y = cc[i].y - radius * 8 / 10;
		arrow[i][1].x = cc[i].x - radius * 18 / 20;			arrow[i][1].y = cc[i].y;
		arrow[i][2].x = cc[i].x - radius * 9 / 80;			arrow[i][2].y = cc[i].y + radius * 8 / 10;
		arrow[i][3].x = arrow[i][2].x + radius * 2 / 5;		arrow[i][3].y = cc[i].y + radius * 8 / 10;
		arrow[i][4].x = arrow[i][1].x + radius * 2 / 5;		arrow[i][4].y = cc[i].y;
		arrow[i][5].x = arrow[i][0].x + radius * 2 / 5;		arrow[i][5].y = cc[i].y - radius * 8 / 10;
		arrow[i][6].x = cc[i].x - radius *  4 / 20;			arrow[i][6].y = cc[i].y - radius * 2 / 10;
		arrow[i][7].x = cc[i].x + radius * 18 / 20;			arrow[i][7].y = arrow[i][6].y + radius * 2 / 5;
		if (i == 2) // 让它跳到15，然后回到循环+1就是16了，也就是多色左转灯的位置
			i = 15;
	}
	// 直行灯
	for (int i = 6; (i < cLight) && (i < 9 || i == 17); i++)
	{
		arrow[i][0].y = cc[i].y - radius * 9 / 80;			arrow[i][0].x = cc[i].x - radius * 8 / 10;
		arrow[i][1].y = cc[i].y - radius * 18 / 20;			arrow[i][1].x = cc[i].x;
		arrow[i][2].y = cc[i].y - radius * 9 / 80;			arrow[i][2].x = cc[i].x + radius * 8 / 10;
		arrow[i][3].y = arrow[i][2].y + radius * 2 / 5;		arrow[i][3].x = cc[i].x + radius * 8 / 10;
		arrow[i][4].y = arrow[i][1].y + radius * 2 / 5;		arrow[i][4].x = cc[i].x;
		arrow[i][5].y = arrow[i][0].y + radius * 2 / 5;		arrow[i][5].x = cc[i].x - radius * 8 / 10;
		arrow[i][6].y = cc[i].y - radius *  4 / 20;			arrow[i][6].x = cc[i].x - radius * 2 / 10;
		arrow[i][7].y = cc[i].y + radius * 18 / 20;			arrow[i][7].x = arrow[i][6].x + radius * 2 / 5;
		if (i == 8) // 让它跳到16，然后回到循环+1就是17了，也就是多色直行灯的位置
			i = 16;
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
	text[2].x = rect[14].left; text[2].y = rect[14].top;
    text[3].x = rect[19].left; text[3].y = rect[19].top;
    text[4].x = rect[20].left; text[4].y = rect[20].top;
}

HFONT CreateCustomFont()
{
	return CreateFont(
		2 * radius, 0, 0, 0,
		FW_HEAVY,
		0, 0, 0, GB2312_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, L"abc"
		);
}

// 设置文字和画刷的颜色
void SetColor(COLORREF& colorRed, COLORREF& colorGreen, COLORREF& colorYellow)
{
	CIniFile* ini = new CIniFile(L".\\Config.ini"); // 读入ini文件
	// 读入红灯颜色值
	int r = ini->GetValueInt(L"ColorRed", L"R");
	int g = ini->GetValueInt(L"ColorRed", L"G");
	int b = ini->GetValueInt(L"ColorRed", L"B");
	if ((0 <= r && r <= 255) && (0 <= g && g <= 255) && (0 <= b && b <= 255))
	{
		colorRed = RGB(r, g, b);
	}
	else 
	{
		colorRed = RGB(255, 65, 56);
	}
	// 读入绿灯颜色值
	r = ini->GetValueInt(L"ColorGreen", L"R");
	g = ini->GetValueInt(L"ColorGreen", L"G");
	b = ini->GetValueInt(L"ColorGreen", L"B");
	if ((0 <= r && r <= 255) && (0 <= g && g <= 255) && (0 <= b && b <= 255))
	{
		colorGreen = RGB(r, g, b);
	}
	else
	{
		colorGreen = RGB(65, 255, 184);
	}

	// 读入黄灯颜色值
	r = ini->GetValueInt(L"ColorYellow", L"R");
	g = ini->GetValueInt(L"ColorYellow", L"G");
	b = ini->GetValueInt(L"ColorYellow", L"B");
	if ((0 <= r && r <= 255) && (0 <= g && g <= 255) && (0 <= b && b <= 255))
	{
		colorYellow = RGB(r, g, b);
	}
	else
	{		
		colorYellow = RGB(255, 232, 93);
	}
}
