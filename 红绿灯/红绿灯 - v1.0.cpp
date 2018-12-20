#include <windows.h>
#define TIMER_SEC 1
static const int cLight = 10; // ����
static int cc[cLight], radius = 15; // Բ�ĺͰ뾶
static TCHAR szTime[3];   // ������ʱ��
static int tLight = 0; // ���̵�����
static int xClient, yClient; // ���ڵĿ���
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
	//wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
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
	//	WS_THICKFRAME | ,
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
	static HBRUSH hBrush[4]; // ��ˢ 
	static HPEN hPen; // ������ˢ
	static HFONT hFont; // �����Զ�������
	switch (message)
	{
	case WM_CREATE:
		hBrush[0] = CreateSolidBrush(RGB(255, 0, 0)); // ������ˢ
		hBrush[1] = CreateSolidBrush(RGB(0, 255, 0));
		hBrush[2] = CreateSolidBrush(RGB(255, 255, 0));
		hBrush[3] = (HBRUSH)GetStockObject(BLACK_BRUSH);
		hPen = CreatePen(PS_SOLID, 1, RGB(80, 80, 80));
		for (int i = 0; i < cLight; i++) // ����Բ��
		{
			cc[i] = 5 + radius + (2 * radius + 5) * i;
		}
		hFont = CreateFont( // �Խ�����
			2 * radius, 0, 0, 0, 
			FW_HEAVY,
			0, 0, 0, GB2312_CHARSET,
			OUT_DEFAULT_PRECIS,	CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
			DEFAULT_PITCH | FF_DONTCARE, L"abc"
			);
		SetTimer(hwnd, TIMER_SEC, 1000, NULL); // 1���Ӹ���һ�δ���
//		SetWindowOrgEx(hdc, 0, 0, NULL);  	// ���ô���ԭ������
		return 0;
	case WM_TIMER:
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;
	case WM_SIZE:
		xClient = LOWORD(lParam);
		yClient = HIWORD(lParam);
		radius = (yClient - 10) / 2;
		for (int i = 0; i < cLight; i++)
		{
			cc[i] = 5 + radius + (2 * radius + 5) * i;
		}
		DeleteObject(hFont);
		hFont = CreateFont( // �Խ�����
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
		hdc = BeginPaint(hwnd, &ps);		// ȡ���豸�������
		// ���̵�һ������120s
		// 0-49:R R, 50-79:R G, 80-84:R Y, 85-114:G R, 115-119:Y R
		if ((tLight >= 0) && (tLight < 50)) // R R
		{
			SelectObject(hdc, hBrush[0]);
			Ellipse(hdc, cc[0] - radius, 5, cc[0] + radius, 5 + 2 * radius);
			Ellipse(hdc, cc[6] - radius, 5, cc[6] + radius, 5 + 2 * radius);
			SelectObject(hdc, hPen);
			Arc(hdc, cc[1] - radius, 5, cc[1] + radius, 5 + 2 * radius,
				cc[1], cc[1] - radius, cc[1], cc[1] - radius);
			Arc(hdc, cc[2] - radius, 5, cc[2] + radius, 5 + 2 * radius,
				cc[2], cc[2] - radius, cc[2], cc[2] - radius);
			Arc(hdc, cc[7] - radius, 5, cc[7] + radius, 5 + 2 * radius,
				cc[7], cc[7] - radius, cc[7], cc[7] - radius);
			Arc(hdc, cc[8] - radius, 5, cc[8] + radius, 5 + 2 * radius,
				cc[8], cc[8] - radius, cc[8], cc[8] - radius);
			// ���ʣ��ʱ��			
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, cc[3] - radius, 5, szTime, 
				wsprintf(szTime, TEXT("%02d"), 85 - tLight)); // ��ƺ�95s
			TextOut(hdc, cc[9] - radius, 5, szTime, 
				wsprintf(szTime, TEXT("%02d"), 50 - tLight)); // �ҵƺ�55s
		}
		else if (tLight < 80) // R G
		{
			SelectObject(hdc, hBrush[0]);
			Ellipse(hdc, cc[0] - radius, 5, cc[0] + radius, 5 + 2 * radius);
			SelectObject(hdc, hBrush[1]);
			Ellipse(hdc, cc[8] - radius, 5, cc[8] + radius, 5 + 2 * radius);
			SelectObject(hdc, hPen);
			Arc(hdc, cc[1] - radius, 5, cc[1] + radius, 5 + 2 * radius,
				cc[1], cc[1] - radius, cc[1], cc[1] - radius);
			Arc(hdc, cc[2] - radius, 5, cc[2] + radius, 5 + 2 * radius,
				cc[2], cc[2] - radius, cc[2], cc[2] - radius);
			Arc(hdc, cc[6] - radius, 5, cc[6] + radius, 5 + 2 * radius,
				cc[6], cc[6] - radius, cc[6], cc[6] - radius);
			Arc(hdc, cc[7] - radius, 5, cc[7] + radius, 5 + 2 * radius,
				cc[7], cc[7] - radius, cc[7], cc[7] - radius);
			// ���ʣ��ʱ��
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, cc[3] - radius, 5, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight));
			SetTextColor(hdc, RGB(0, 255, 0));
			TextOut(hdc, cc[9] - radius, 5, szTime,
				wsprintf(szTime, TEXT("%02d"), 80 - tLight)); // �ҵ���30s
		}
		else if (tLight < 85) // R Y
		{
			SelectObject(hdc, hBrush[0]);
			Ellipse(hdc, cc[0] - radius, 5, cc[0] + radius, 5 + 2 * radius);
			SelectObject(hdc, hBrush[2]);
			Ellipse(hdc, cc[7] - radius, 5, cc[7] + radius, 5 + 2 * radius);
			SelectObject(hdc, hPen);
			Arc(hdc, cc[1] - radius, 5, cc[1] + radius, 5 + 2 * radius,
				cc[1], cc[1] - radius, cc[1], cc[1] - radius);
			Arc(hdc, cc[2] - radius, 5, cc[2] + radius, 5 + 2 * radius,
				cc[2], cc[2] - radius, cc[2], cc[2] - radius);
			Arc(hdc, cc[6] - radius, 5, cc[6] + radius, 5 + 2 * radius,
				cc[6], cc[6] - radius, cc[6], cc[6] - radius);
			Arc(hdc, cc[8] - radius, 5, cc[8] + radius, 5 + 2 * radius,
				cc[8], cc[8] - radius, cc[8], cc[8] - radius);
			// ���ʣ��ʱ��
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, cc[3] - radius, 5, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight));
			SetTextColor(hdc, RGB(255, 255, 0));
			TextOut(hdc, cc[9] - radius, 5, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight)); // �ҵƻ�5s
		}
		else if (tLight < 115) // G R
		{
			SelectObject(hdc, hBrush[1]);
			Ellipse(hdc, cc[2] - radius, 5, cc[2] + radius, 5 + 2 * radius);
			SelectObject(hdc, hBrush[0]);
			Ellipse(hdc, cc[6] - radius, 5, cc[6] + radius, 5 + 2 * radius);
			SelectObject(hdc, hPen);
			Arc(hdc, cc[0] - radius, 5, cc[0] + radius, 5 + 2 * radius,
				cc[0], cc[0] - radius, cc[0], cc[0] - radius);
			Arc(hdc, cc[1] - radius, 5, cc[1] + radius, 5 + 2 * radius,
				cc[1], cc[1] - radius, cc[1], cc[1] - radius);
			Arc(hdc, cc[7] - radius, 5, cc[7] + radius, 5 + 2 * radius,
				cc[7], cc[7] - radius, cc[7], cc[7] - radius);
			Arc(hdc, cc[8] - radius, 5, cc[8] + radius, 5 + 2 * radius,
				cc[8], cc[8] - radius, cc[8], cc[8] - radius);
			// ���ʣ��ʱ��
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(0, 255, 0));
			TextOut(hdc, cc[3] - radius, 5, szTime,
				wsprintf(szTime, TEXT("%02d"), 115 - tLight)); // �����30s
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, cc[9] - radius, 5, szTime,
				wsprintf(szTime, TEXT("%02d"), 120 - tLight + 50)); // �ҵƺ�
		}
		else if(tLight < 120) // Y R
		{
			SelectObject(hdc, hBrush[2]);
			Ellipse(hdc, cc[1] - radius, 5, cc[1] + radius, 5 + 2 * radius);
			SelectObject(hdc, hBrush[0]);
			Ellipse(hdc, cc[6] - radius, 5, cc[6] + radius, 5 + 2 * radius);
			SelectObject(hdc, hPen);
			Arc(hdc, cc[0] - radius, 5, cc[0] + radius, 5 + 2 * radius,
				cc[0], cc[0] - radius, cc[0], cc[0] - radius);
			Arc(hdc, cc[2] - radius, 5, cc[2] + radius, 5 + 2 * radius,
				cc[2], cc[2] - radius, cc[2], cc[2] - radius);
			Arc(hdc, cc[7] - radius, 5, cc[7] + radius, 5 + 2 * radius,
				cc[7], cc[7] - radius, cc[7], cc[7] - radius);
			Arc(hdc, cc[8] - radius, 5, cc[8] + radius, 5 + 2 * radius,
				cc[8], cc[8] - radius, cc[8], cc[8] - radius);
			// ���ʣ��ʱ��
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, RGB(255, 255, 0));
			TextOut(hdc, cc[3] - radius, 5, szTime,
				wsprintf(szTime, TEXT("%02d"), 120 - tLight)); // ��ƻ�5s
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, cc[9] - radius, 5, szTime,
				wsprintf(szTime, TEXT("%02d"), 120 - tLight + 50)); // �ҵƺ�
		}
	//	DeleteObject(hBrush);
	//	DeleteObject(hPen);
		EndPaint(hwnd, &ps);
		if (++tLight >= 120)
		{
			tLight = 0;
		}
		return 0;
	case WM_RBUTTONUP: // ���²�̧������Ҽ�ʱ�رմ���
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return  DefWindowProc(hwnd, message, wParam, lParam);
	}
}