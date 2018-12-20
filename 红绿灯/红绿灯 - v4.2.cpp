#include <windows.h>
#define TIMER_SEC 1
static const int cLight = 10; // ����
static POINT cc[cLight]; // Բ������
static int  radius = 15; // Բ�İ뾶
static TCHAR szTime[3];  // ʱ��洢λ��
static int tLight = 0;   // ���̵����ڣ�120sһ������
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
		SetTimer(hwnd, TIMER_SEC, 500, NULL); // 0.5���Ӹ���һ�δ���
		return 0;

	case WM_TIMER:
		if (flag == false) //�ж��Ƿ����ʱ��
		{
			flag = true;
		}			
		else
		{
			flag = false;
			if (++tLight >= 120)
				tLight = 0;
		}

		if ((0 < tLight) && (tLight < 50) || (50 < tLight) && (tLight < 77) || 
			(80 < tLight) && (tLight < 85) || (85 < tLight) && (tLight < 112) || 
			(115 < tLight) && (tLight < 120))  // ֻ��Ҫ��������
		{			
			InvalidateRect(hwnd, &rect[3], TRUE);			
			InvalidateRect(hwnd, &rect[9], TRUE);
		}		
		else if ((77 <= tLight) && (tLight < 80)) // ��Ҫ�������ֺ��̵�
		{
			InvalidateRect(hwnd, &rect[8], TRUE);
			InvalidateRect(hwnd, &rect[3], TRUE);			
			InvalidateRect(hwnd, &rect[9], TRUE);
		}
		else if ((112 <= tLight) && (tLight < 115)) // ��Ҫ�������ֺ��̵�
		{			
			InvalidateRect(hwnd, &rect[2], TRUE); 
			InvalidateRect(hwnd, &rect[3], TRUE);			
			InvalidateRect(hwnd, &rect[9], TRUE);
		}
		else
			InvalidateRect(hwnd, NULL, TRUE); // �������ڸ���
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
		if ((0 <= tLight) && (tLight < 50)) // R R
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
				wsprintf(szTime, TEXT("%02d"), 85 - tLight)); // ��ƺ�95s
			TextOut(hdc, text[1].x, text[1].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 50 - tLight)); // �ҵƺ�55s
		}
		else if (tLight < 80) // R G
		{
			SelectObject(hdc, hPenRed);
			SelectObject(hdc, hBrushRed);
			Polygon(hdc, arrow[0], 6);
			Rectangle(hdc, arrow[0][6].x, arrow[0][6].y, arrow[0][7].x, arrow[0][7].y);
			if (76 < tLight && flag == true) // �̵���ǰ��˸3��
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
				wsprintf(szTime, TEXT("%02d"), 85 - tLight));
			SetTextColor(hdc, RGB(0, 255, 0));
			TextOut(hdc, text[1].x, text[1].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 80 - tLight)); // �ҵ���30s
		}
		else if (tLight < 85) // R Y
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
				wsprintf(szTime, TEXT("%02d"), 85 - tLight));
			SetTextColor(hdc, RGB(255, 255, 0));
			TextOut(hdc, text[1].x, text[1].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight)); // �ҵƻ�5s
		}
		else if (tLight < 115) // G R
		{
			if (111 < tLight && flag == true) // �̵���ǰ��˸3��
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
				wsprintf(szTime, TEXT("%02d"), 115 - tLight)); // �����30s
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, text[1].x, text[1].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 120 - tLight + 50)); // �ҵƺ�
		}
		else if(tLight < 120) // Y R
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
				wsprintf(szTime, TEXT("%02d"), 120 - tLight)); // ��ƻ�5s
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, text[1].x, text[1].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 120 - tLight + 50)); // �ҵƺ�
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
		arrow[i][0].x = cc[i].x - radius * 9 / 80;	arrow[i][0].y = cc[i].y - radius * 8 / 10;
		arrow[i][1].x = cc[i].x - radius * 18 / 20;	arrow[i][1].y = cc[i].y;
		arrow[i][2].x = cc[i].x - radius * 9 / 80;	arrow[i][2].y = cc[i].y + radius * 8 / 10;
		arrow[i][3].x = cc[i].x + radius * 27 / 80;	arrow[i][3].y = cc[i].y + radius * 8 / 10;
		arrow[i][4].x = cc[i].x - radius *  9 / 20;	arrow[i][4].y = cc[i].y;
		arrow[i][5].x = cc[i].x + radius * 27 / 80;	arrow[i][5].y = cc[i].y - radius * 8 / 10;
		arrow[i][6].x = cc[i].x - radius *  1 / 20;	arrow[i][6].y = cc[i].y - radius * 2 / 10;
		arrow[i][7].x = cc[i].x + radius * 18 / 20;	arrow[i][7].y = cc[i].y + radius * 2 / 10;
	}
	for (int i = 6; i < 9; i++)
	{
		arrow[i][0].y = cc[i].y - radius * 9 / 80;	arrow[i][0].x = cc[i].x - radius * 8 / 10;
		arrow[i][1].y = cc[i].y - radius * 18 / 20;	arrow[i][1].x = cc[i].x;
		arrow[i][2].y = cc[i].y - radius * 9 / 80;	arrow[i][2].x = cc[i].x + radius * 8 / 10;
		arrow[i][3].y = cc[i].y + radius * 27 / 80;	arrow[i][3].x = cc[i].x + radius * 8 / 10;
		arrow[i][4].y = cc[i].y - radius *  9 / 20;	arrow[i][4].x = cc[i].x;
		arrow[i][5].y = cc[i].y + radius * 27 / 80;	arrow[i][5].x = cc[i].x - radius * 8 / 10;
		arrow[i][6].y = cc[i].y - radius *  1 / 20;	arrow[i][6].x = cc[i].x - radius * 2 / 10;
		arrow[i][7].y = cc[i].y + radius * 18 / 20;	arrow[i][7].x = cc[i].x + radius * 2 / 10;
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
	text[0].x = rect[3].left; text[0].y = rect[3].top;
	text[1].x = rect[9].left; text[1].y = rect[9].top;
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