#include <windows.h>
#include "CIniFile.h"
using namespace std;

#define TIMER_SEC 1
static const int cLight = 11; // ����
static POINT cc[cLight]; // Բ������
static int  radius = 15; // Բ�İ뾶
static TCHAR szTime[3];  // ʱ��洢λ��
static int allLightTime;   // ���̵����ڣ�4*(greenTime+yellowTime)һ������
static int timeCount;	// ���̵�ʱ�����
int greenTime, yellowTime; // �̵�ʱ�䡢�Ƶ�ʱ��
static POINT arrow[cLight][8]; // �洢��ͷ�ĵ�,��һά��λ����cc[i]��Ӧ
							   // �ڶ�ά��0-5��ͷ��6-7����
static RECT rect[cLight];	   // �����Ŀ�����ڸ��´���ָ������
static POINT text[2];		   // �ı���ʾ����
static bool flag = false;      // ���̵����ڸı�ı�־
static int xClient, yClient;   // ���ڵĿ���

void CreateArrowPoint();  // ������ͷ�ĵ�
void CreateRectPoint();   // �������εĵ㣬���ڸ��´���ָ������
void CreateTextPoint();   // �����ı���ʾ����λ��
HFONT CreateCustomFont(); // �����Զ�������
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
	xClient = cLight * (5 + 2 * radius) + 5; //�����Ҫ��ʾ�����������15
	yClient = 10 + 2 * radius + 15; //�����Ҫ��ʾ�����������40
	hwnd = CreateWindowEx(WS_EX_TOPMOST, szAppName, TEXT("Traffic Light"),
		WS_POPUPWINDOW | WS_SIZEBOX | WS_MINIMIZEBOX,
		500, 200,
		xClient, yClient,
		NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0) > 0)	// ��Ϣѭ��
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;		// ��Ϣѭ������ʱ����Ϣ����ϵͳ
}

/* ���ں��� */
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM  wParam, LPARAM  lParam)
{
	HDC hdc;
	PAINTSTRUCT ps; // ���������ͼ��Ϣ�Ľṹ�����	
	static HPEN hPen;           // ���ʣ����ƿ�
	static HPEN hPenRed;        // ���
	static HPEN hPenGreen;      // �̱�
	static HPEN hPenYellow;     // �Ʊ�
	static HBRUSH hBrushRed;    // ��ˢ
	static HBRUSH hBrushGreen;  // ��ˢ
	static HBRUSH hBrushYellow; // ��ˢ
	static HFONT hFont;         // �Զ�������
	static HPEN hPenO;			// ����
	
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
		SetTimer(hwnd, TIMER_SEC, 500, NULL); // 0.5���Ӹ���һ�δ���
		return 0;

	case WM_TIMER:
		if (flag == false) //�ж��Ƿ����ʱ��
		{
			flag = true;
		}			
		else  // true��ʾ����ʱ��
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
			(4 * greenTime + 3 * yellowTime < timeCount && timeCount < 4 * (greenTime + yellowTime)))  // RR,ֻ��Ҫ��������
			;
		else if (3 * greenTime + 2 * yellowTime - 3 <= timeCount &&
			timeCount < 3 * greenTime + 2 * yellowTime) // ��Ҫ�������ֺ��̵�
		{
			InvalidateRect(hwnd, &rect[8], TRUE);
		}
		else if (4 * greenTime + 3 * yellowTime - 3 <= timeCount &&
			(timeCount < 4 * greenTime + 3 * yellowTime)) // ��Ҫ�������ֺ��̵�
		{			
			InvalidateRect(hwnd, &rect[2], TRUE); 
		}
		else if (flag == false)
			InvalidateRect(hwnd, NULL, TRUE); // �������ڸ���
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
		/* �ߴ�ı�ʱ���ؽ���ͷ��״�������С��ÿһ��rect����������ʼ���� */
		CreateArrowPoint();
		CreateRectPoint();
		CreateTextPoint();
		DeleteObject(hFont); // �ؽ�����ǰɾ��ԭ������
		hFont = CreateCustomFont();
		return 0;

	case WM_LBUTTONDOWN: // ��ס���ڿ��ƶ�����
		SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps); // ȡ���豸�������
		/* ��������λ��*/
		SelectObject(hdc, hPenO);
		SelectObject(hdc, (HBRUSH)GetStockObject(NULL_BRUSH));
		RoundRect(hdc, cc[0].x - radius - 2, cc[0].y - radius - 2,
			cc[2].x + radius + 3, cc[2].y + radius + 3, 5, 5);
		RoundRect(hdc, cc[6].x - radius - 2, cc[6].y - radius - 2,
			cc[8].x + radius + 3, cc[8].y + radius + 3, 5, 5);
		/* ����6���Ƶ�λ��*/
		SelectObject(hdc, hPen);
		for (int i = 0; i < 3; i++) 
			Arc(hdc, cc[i].x - radius, cc[i].y - radius, cc[i].x + radius, cc[i].y + radius,
				cc[i].x, cc[i].x - radius, cc[i].x, cc[i].x - radius);
		for (int i = 6; i < 9; i++) 
			Arc(hdc, cc[i].x - radius, cc[i].y - radius, cc[i].x + radius, cc[i].y + radius,
				cc[i].x, cc[i].x - radius, cc[i].x, cc[i].x - radius);
		/* 
		���̵�һ������120s
		0-49:R R, 50-76:R G, 77-79:R G��, 80-84:R Y, 85-111:G R, 112-114:G�� R, 115-119:Y R
		 */
		if (0 <= timeCount && timeCount < 2 * (greenTime + yellowTime)) // R R
		{
			SelectObject(hdc, hPenRed);
			SelectObject(hdc, hBrushRed);
			Polygon(hdc,arrow[0], 6);
			Rectangle(hdc, arrow[0][6].x, arrow[0][6].y, arrow[0][7].x, arrow[0][7].y);
			Polygon(hdc, arrow[6], 6);
			Rectangle(hdc, arrow[6][6].x, arrow[6][6].y, arrow[6][7].x, arrow[6][7].y);
			/* ���ʣ��ʱ��	*/		
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, text[0].x, text[0].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 3 * (greenTime + yellowTime) - timeCount)); // ��ƺ�95s
			TextOut(hdc, text[1].x, text[1].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 2 * (greenTime + yellowTime) - timeCount)); // �ҵƺ�55s
		}
		else if (timeCount < 3 * greenTime + 2 * yellowTime) // R G
		{
			SelectObject(hdc, hPenRed);
			SelectObject(hdc, hBrushRed);
			Polygon(hdc, arrow[0], 6);
			Rectangle(hdc, arrow[0][6].x, arrow[0][6].y, arrow[0][7].x, arrow[0][7].y);
			if (3 * greenTime + 2 * yellowTime - 3 < timeCount && flag == true) // �̵���ǰ��˸3��,0.5s����1s��
				;
			else 
			{
				SelectObject(hdc, hPenGreen);
				SelectObject(hdc, hBrushGreen);
				Polygon(hdc, arrow[8], 6);
				Rectangle(hdc, arrow[8][6].x, arrow[8][6].y, arrow[8][7].x, arrow[8][7].y);
			}
			/* ���ʣ��ʱ��	*/
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, text[0].x, text[0].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 3 * (greenTime + yellowTime) - timeCount));
			SetTextColor(hdc, RGB(0, 255, 0));
			TextOut(hdc, text[1].x, text[1].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 3 * greenTime + 2 * yellowTime - timeCount)); // �ҵ���30s
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
			/* ���ʣ��ʱ��	*/
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, text[0].x, text[0].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 3 * (greenTime + yellowTime) - timeCount));
			SetTextColor(hdc, RGB(255, 255, 0));
			TextOut(hdc, text[1].x, text[1].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 3 * (greenTime + yellowTime) - timeCount)); // �ҵƻ�5s
		}
		else if (timeCount < 4 * greenTime + 3 * yellowTime) // G R
		{
			if (4 * greenTime + 3 * yellowTime - 3 < timeCount && flag == true) // �̵���ǰ��˸3��
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
			/* ���ʣ��ʱ��	*/
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(0, 255, 0));
			TextOut(hdc, text[0].x, text[0].y, szTime,
				wsprintf(szTime, TEXT("%02d"), allLightTime - yellowTime - timeCount)); // �����30s
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, text[1].x, text[1].y, szTime,
				wsprintf(szTime, TEXT("%02d"), allLightTime - timeCount + 50)); // �ҵƺ�
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
			/* ���ʣ��ʱ��	*/
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(255, 255, 0));
			TextOut(hdc, text[0].x, text[0].y, szTime,
				wsprintf(szTime, TEXT("%02d"), allLightTime - timeCount)); // ��ƻ�5s
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, text[1].x, text[1].y, szTime,
				wsprintf(szTime, TEXT("%02d"), allLightTime - timeCount + 50)); // �ҵƺ�
		}
		EndPaint(hwnd, &ps);
		return 0;

	case WM_RBUTTONUP: // ���²�̧������Ҽ�ʱ�رմ���
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
�洢��ͷ�ĵ�,��һά��λ����cc[i]��Ӧ
�ڶ�ά��0-5��ͷ��6-7����
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
		DEFAULT_PITCH | FF_DONTCARE, L"����"
		);
}