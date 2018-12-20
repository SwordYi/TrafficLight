#include <windows.h>
#define TIMER_SEC 1
static const int cLight = 10; // ����
static POINT cc[cLight]; // Բ������
static int  radius = 15; // Բ�İ뾶
static TCHAR szTime[3];  // ʱ��洢λ��
static int tLight = 0;   // ���̵����ڣ�120sһ������
static POINT arrow[cLight][5]; // �洢��ͷ�ĵ�,��һά��λ����cc[i]��Ӧ
							   // �ڶ�ά��0-3��ͷ��4-5����
static bool flag = false;      // ���̵����ڸı�ı�־
static int xClient, yClient;   // ���ڵĿ���

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

//���ں���
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM  wParam, LPARAM  lParam)
{
	HDC hdc;
	PAINTSTRUCT ps; // ���������ͼ��Ϣ�Ľṹ�����	
	static HPEN hPen;       // ���ʣ����ƿ�
	static HPEN hPenRed;    // ���
	static HPEN hPenGreen;  // �̱�
	static HPEN hPenYellow; // �Ʊ�
	static HFONT hFont;     // �Զ�������
	switch (message)
	{
	case WM_CREATE:
		hPen = CreatePen(PS_SOLID, 1, RGB(50, 50, 50));
		SetTimer(hwnd, TIMER_SEC, 500, NULL); // 0.5���Ӹ���һ�δ���
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
		/* �ߴ�ı�ʱ���ؽ���ͷ��״�����ʡ������С */
		CreateArrow();
		DeleteObject(hPenRed); // �ؽ�����
		DeleteObject(hPenGreen);
		DeleteObject(hPenYellow);
		hPenRed = CreatePen(PS_SOLID, radius * 2 / 5, RGB(255, 0, 0));
		hPenGreen = CreatePen(PS_SOLID, radius * 2 / 5, RGB(0, 255, 0));
		hPenYellow = CreatePen(PS_SOLID, radius * 2 / 5, RGB(255, 255, 0));
		DeleteObject(hFont); // �ؽ�����
		hFont = CreateFont( 
			2 * radius, 0, 0, 0,
			FW_HEAVY,
			0, 0, 0, GB2312_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, L"abc"
			);
		return 0;

	case WM_LBUTTONDOWN: // ��ס���ڿ����ƶ�����
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
			Polyline(hdc, arrow[0], 3);
			MoveToEx(hdc, arrow[0][3].x, arrow[0][3].y, NULL);
			LineTo(hdc, arrow[0][4].x, arrow[0][4].y);
			Polyline(hdc, arrow[6], 3);
			MoveToEx(hdc, arrow[6][3].x, arrow[6][3].y, NULL);
			LineTo(hdc, arrow[6][4].x, arrow[6][4].y);
			// ���ʣ��ʱ��			
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, cc[3].x - radius, cc[3].y - radius, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight)); // ��ƺ�95s
			TextOut(hdc, cc[9].x - radius, cc[9].y - radius, szTime,
				wsprintf(szTime, TEXT("%02d"), 50 - tLight)); // �ҵƺ�55s
		}
		else if (tLight < 80) // R G
		{
			SelectObject(hdc, hPenRed);
			Polyline(hdc, arrow[0], 3);
			MoveToEx(hdc, arrow[0][3].x, arrow[0][3].y, NULL);
			LineTo(hdc, arrow[0][4].x, arrow[0][4].y);
			if (76 < tLight && flag == true) // �̵���ǰ��˸3��
				;
			else 
			{
				SelectObject(hdc, hPenGreen);
				Polyline(hdc, arrow[8], 3);
				MoveToEx(hdc, arrow[8][3].x, arrow[8][3].y, NULL);
				LineTo(hdc, arrow[8][4].x, arrow[8][4].y);
			}
			// ���ʣ��ʱ��
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, cc[3].x - radius, cc[3].y - radius, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight));
			SetTextColor(hdc, RGB(0, 255, 0));
			TextOut(hdc, cc[9].x - radius, cc[9].y - radius, szTime,
				wsprintf(szTime, TEXT("%02d"), 80 - tLight)); // �ҵ���30s
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
			// ���ʣ��ʱ��
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, cc[3].x - radius, cc[3].y - radius, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight));
			SetTextColor(hdc, RGB(255, 255, 0));
			TextOut(hdc, cc[9].x - radius, cc[9].y - radius, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight)); // �ҵƻ�5s
		}
		else if (tLight < 115) // G R
		{
			if (111 < tLight && flag == true) // �̵���ǰ��˸3��
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
			// ���ʣ��ʱ��
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(0, 255, 0));
			TextOut(hdc, cc[3].x - radius, cc[3].y - radius, szTime,
				wsprintf(szTime, TEXT("%02d"), 115 - tLight)); // �����30s
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, cc[9].x - radius, cc[9].y - radius, szTime,
				wsprintf(szTime, TEXT("%02d"), 120 - tLight + 50)); // �ҵƺ�
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
			// ���ʣ��ʱ��
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(255, 255, 0));
			TextOut(hdc, cc[3].x - radius, cc[3].y - radius, szTime,
				wsprintf(szTime, TEXT("%02d"), 120 - tLight)); // ��ƻ�5s
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, cc[9].x - radius, cc[9].y - radius, szTime,
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
		DeleteObject(hFont);
		PostQuitMessage(0);
		return 0;

	default:
		return  DefWindowProc(hwnd, message, wParam, lParam);
	}
}
/* 
�洢��ͷ�ĵ�,��һά��λ����cc[i]��Ӧ
�ڶ�ά��0-3��ͷ��4-5����
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