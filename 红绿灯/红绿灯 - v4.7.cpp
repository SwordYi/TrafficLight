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
static const int cLight = 21; // ����
static POINT cc[cLight]; // Բ������
static int  radius = 15; // Բ�İ뾶
static TCHAR szTime[3];  // ʱ��洢λ��
static int tLight = 0;   // ���̵����ڣ�120sһ������
static POINT arrow[cLight][8]; // �洢��ͷ�ĵ�,��һά��λ����cc[i]��Ӧ
							   // �ڶ�ά��0-5��ͷ��6-7����
static RECT rect[cLight];	   // �����Ŀ�����ڸ��´���ָ������
static POINT text[5];		   // �ı���ʾ����
static bool flag = false;      // ���̵����ڸı�ı�־
static int xClient, yClient;   // ���ڵĿ���

void CreateArrowPoint();  // ������ͷ�ĵ�
void CreateRectPoint();   // �������εĵ㣬���ڸ��´���ָ������
void CreateTextPoint();   // �����ı���ʾ����λ��
HFONT CreateCustomFont(); // �����Զ�������
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void SetColor(COLORREF& colorRed, COLORREF& colorGreen, COLORREF& colorYellow); //������ɫ

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
	xClient = cLight * (5 + 2 * radius) + 5 - 20; //�����Ҫ��ʾ�����������15����ȥ20������ΪcLight=21����λ��ɺ���ڱ�̫���ˣ�������ٵ�λ���Ͳ�Ҫ��20��
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
	static COLORREF colorRed;      // ��ɫ
	static COLORREF colorGreen;    // ��ɫ
	static COLORREF colorYellow;   // ��ɫ
	
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
			(115 < tLight) && (tLight < 120))  // ֻ��Ҫ��������
			;
		else if ((77 <= tLight) && (tLight < 80)) // ��Ҫ�������ֺ��̵�
		{
			InvalidateRect(hwnd, &rect[8], TRUE);	// ���µ�ɫֱ�е�
			InvalidateRect(hwnd, &rect[12], TRUE);	// ���¶�ɫԲ�ε�
			InvalidateRect(hwnd, &rect[16], TRUE);	// ���¶�ɫ��ת��
			InvalidateRect(hwnd, &rect[17], TRUE);	// ���ж�ɫֱ�е�
		}
		else if ((112 <= tLight) && (tLight < 115)) // ��Ҫ�������ֺ��̵�
		{			
			InvalidateRect(hwnd, &rect[2], TRUE);	// ���µ�ɫ��ת��
			InvalidateRect(hwnd, &rect[12], TRUE);	// ���¶�ɫԲ�ε�
			InvalidateRect(hwnd, &rect[16], TRUE);	// ���¶�ɫ��ת��
			InvalidateRect(hwnd, &rect[17], TRUE);	// ���ж�ɫֱ�е�
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
		RoundRect(hdc, cc[12].x - radius - 2, cc[12].y - radius - 2,
			cc[12].x + radius + 3, cc[12].y + radius + 3, 5, 5);  //��ɫ��Բ��
        RoundRect(hdc, cc[16].x - radius - 2, cc[16].y - radius - 2,
			cc[17].x + radius + 3, cc[17].y + radius + 3, 5, 5);  //��ɫ�Ƽ���   
		/* �����Ƶ�λ��*/
		SelectObject(hdc, hPen);
		for (int i = 0; i < 3; i++)  // ��ɫ��ת��λ��
			Arc(hdc, cc[i].x - radius, cc[i].y - radius, cc[i].x + radius, cc[i].y + radius,
				cc[i].x, cc[i].x - radius, cc[i].x, cc[i].x - radius);
		for (int i = 6; i < 9; i++) // ��ɫֱ�е�λ��
			Arc(hdc, cc[i].x - radius, cc[i].y - radius, cc[i].x + radius, cc[i].y + radius,
				cc[i].x, cc[i].x - radius, cc[i].x, cc[i].x - radius);
		Arc(hdc, cc[12].x - radius, cc[12].y - radius, cc[12].x + radius, cc[12].y + radius,
			cc[12].x, cc[12].x - radius, cc[12].x, cc[12].x - radius); // ��ɫ��Բ�ε�λ��
        for (int i = 16; i < 18; i++)   // ��ɫ�Ƽ��ε�λ��
			Arc(hdc, cc[i].x - radius, cc[i].y - radius, cc[i].x + radius, cc[i].y + radius,
				cc[i].x, cc[i].x - radius, cc[i].x, cc[i].x - radius);
		/* 
		���̵�һ������120s
		0-49:R R, 50-76:R G, 77-79:R G��, 80-84:R Y, 85-111:G R, 112-114:G�� R, 115-119:Y R
		 */
		if ((0 <= tLight) && (tLight < 50)) // R R R
		{
			//��ת���
			SelectObject(hdc, hPenRed);
			SelectObject(hdc, hBrushRed);
            // ��ת��ɫ���
			Polygon(hdc,arrow[0], 6);
			Rectangle(hdc, arrow[0][6].x, arrow[0][6].y, arrow[0][7].x, arrow[0][7].y);            
			// ֱ�е�ɫ���
			Polygon(hdc, arrow[6], 6);
			Rectangle(hdc, arrow[6][6].x, arrow[6][6].y, arrow[6][7].x, arrow[6][7].y);            
			// ��ɫ��Բ�κ��
			Ellipse(hdc, cc[12].x - radius, cc[12].y - radius, cc[12].x + radius, cc[12].y + radius);
            // ��ɫ��ת���
            Polygon(hdc,arrow[16], 6);
			Rectangle(hdc, arrow[16][6].x, arrow[16][6].y, arrow[16][7].x, arrow[16][7].y);
            // ��ɫֱ�к��
            Polygon(hdc,arrow[17], 6);
			Rectangle(hdc, arrow[17][6].x, arrow[17][6].y, arrow[17][7].x, arrow[17][7].y);

			/* ���ʣ��ʱ��	*/		
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			SetTextColor(hdc, colorRed);
			TextOut(hdc, text[0].x, text[0].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight)); // ��ɫ��ת�ƺ�95s
			TextOut(hdc, text[1].x, text[1].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 50 - tLight)); // ��ɫֱ�еƺ�55s
			TextOut(hdc, text[2].x, text[2].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 50 - tLight)); // ��ɫԲ�εƺ�55s
			TextOut(hdc, text[3].x, text[3].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight)); // ��ɫ��ת�ƺ�95s
			TextOut(hdc, text[4].x, text[4].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 50 - tLight)); // ��ɫֱ�еƺ�55s
		}
		else if (tLight < 80) // R G G
		{
			// ��ɫ��ת���
			SelectObject(hdc, hPenRed);
			SelectObject(hdc, hBrushRed);
			Polygon(hdc, arrow[0], 6);
			Rectangle(hdc, arrow[0][6].x, arrow[0][6].y, arrow[0][7].x, arrow[0][7].y);
			// ��ɫֱ���̵�
			if (76 < tLight && flag == true) // �̵���ǰ��˸3��,0.5s����1s��
				;
			else 
			{
				SelectObject(hdc, hPenGreen);
				SelectObject(hdc, hBrushGreen);
				Polygon(hdc, arrow[8], 6);
				Rectangle(hdc, arrow[8][6].x, arrow[8][6].y, arrow[8][7].x, arrow[8][7].y);				
			}
			// ��ɫԲ���̵�
			SelectObject(hdc, hPenGreen);
			SelectObject(hdc, hBrushGreen);
			Ellipse(hdc, cc[12].x - radius, cc[12].y - radius, cc[12].x + radius, cc[12].y + radius);
			// ��ɫ��ת���
			SelectObject(hdc, hPenRed);
			SelectObject(hdc, hBrushRed);
			Polygon(hdc, arrow[16], 6);
			Rectangle(hdc, arrow[16][6].x, arrow[16][6].y, arrow[16][7].x, arrow[16][7].y);
			// ��ɫֱ���̵�
			if (76 < tLight && flag == true) // �̵���ǰ��˸3��,0.5s����1s��
				;
			else
			{
				SelectObject(hdc, hPenGreen);
				SelectObject(hdc, hBrushGreen);
				Polygon(hdc, arrow[17], 6);
				Rectangle(hdc, arrow[17][6].x, arrow[17][6].y, arrow[17][7].x, arrow[17][7].y);
			}

			/* ���ʣ��ʱ��	*/
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			// ��ɫ��ת��ɫ
			SetTextColor(hdc, colorRed);
			TextOut(hdc, text[0].x, text[0].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight));
			// ��ɫֱ����ɫ
			SetTextColor(hdc, colorGreen);
			TextOut(hdc, text[1].x, text[1].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 80 - tLight)); // �ҵ���30s��
			// ��ɫԲ��
			TextOut(hdc, text[2].x, text[2].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 115 - tLight)); // ��ɫ����30s
			// ��ɫ��ת��ɫ
			SetTextColor(hdc, colorRed);
			TextOut(hdc, text[3].x, text[3].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight));
			// ��ɫֱ����ɫ
			SetTextColor(hdc, colorGreen);
			TextOut(hdc, text[4].x, text[4].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 80 - tLight)); // �ҵ���30s
		}
		else if (tLight < 85) // R Y G
		{
			// ��ɫ��ת���
			SelectObject(hdc, hPenRed);
			SelectObject(hdc, hBrushRed);
			Polygon(hdc, arrow[0], 6);
			Rectangle(hdc, arrow[0][6].x, arrow[0][6].y, arrow[0][7].x, arrow[0][7].y);
			// ��ɫֱ�лƵ�
			SelectObject(hdc, hPenYellow);
			SelectObject(hdc, hBrushYellow);
			Polygon(hdc, arrow[7], 6);
			Rectangle(hdc, arrow[7][6].x, arrow[7][6].y, arrow[7][7].x, arrow[7][7].y);
			// ��ɫԲ���̵�
			SelectObject(hdc, hPenGreen);
			SelectObject(hdc, hBrushGreen);
			Ellipse(hdc, cc[12].x - radius, cc[12].y - radius, cc[12].x + radius, cc[12].y + radius);
			// ��ɫ��ת���
			SelectObject(hdc, hPenRed);
			SelectObject(hdc, hBrushRed);
			Polygon(hdc, arrow[16], 6);
			Rectangle(hdc, arrow[16][6].x, arrow[16][6].y, arrow[16][7].x, arrow[16][7].y);
			// ��ɫֱ�лƵ�
			SelectObject(hdc, hPenYellow);
			SelectObject(hdc, hBrushYellow);
			Polygon(hdc, arrow[17], 6);
			Rectangle(hdc, arrow[17][6].x, arrow[17][6].y, arrow[17][7].x, arrow[17][7].y);

			/* ���ʣ��ʱ��	*/
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			// ��ɫ��ת��ɫ
			SetTextColor(hdc, colorRed);
			TextOut(hdc, text[0].x, text[0].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight));
			// ��ɫֱ����ɫ
			SetTextColor(hdc, colorYellow);
			TextOut(hdc, text[1].x, text[1].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight)); // �ҵƻ�5s
			// ��ɫԲ�κ�ɫ
			SetTextColor(hdc, colorGreen);
			TextOut(hdc, text[2].x, text[2].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 115 - tLight)); // ��ɫ���̵�
			// ��ɫ��ת��ɫ
			SetTextColor(hdc, colorRed);
			TextOut(hdc, text[3].x, text[3].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight));
			// ��ɫֱ����ɫ
			SetTextColor(hdc, colorYellow);
			TextOut(hdc, text[4].x, text[4].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 85 - tLight)); // �ҵƻ�5s
		}
		else if (tLight < 115) // G R G
		{
			// ��ɫ��ת�̵�
			if (111 < tLight && flag == true) // �̵���ǰ��˸3��
				;
			else
			{
				SelectObject(hdc, hPenGreen);
				SelectObject(hdc, hBrushGreen);
				Polygon(hdc, arrow[2], 6);
				Rectangle(hdc, arrow[2][6].x, arrow[2][6].y, arrow[2][7].x, arrow[2][7].y);
			}
			// ��ɫֱ�к��
			SelectObject(hdc, hPenRed);
			SelectObject(hdc, hBrushRed);
			Polygon(hdc, arrow[6], 6);
			Rectangle(hdc, arrow[6][6].x, arrow[6][6].y, arrow[6][7].x, arrow[6][7].y);
			// ��ɫ��ת�̵�
			if (111 < tLight && flag == true) // �̵���ǰ��˸3��
				;
			else
			{
				SelectObject(hdc, hPenGreen);
				SelectObject(hdc, hBrushGreen);	
				Ellipse(hdc, cc[12].x - radius, cc[12].y - radius, cc[12].x + radius, cc[12].y + radius);
			}
			// ��ɫ��ת�̵�
			if (111 < tLight && flag == true) // �̵���ǰ��˸3��
				;
			else
			{
				SelectObject(hdc, hPenGreen);
				SelectObject(hdc, hBrushGreen);
				Polygon(hdc, arrow[16], 6);
				Rectangle(hdc, arrow[16][6].x, arrow[16][6].y, arrow[16][7].x, arrow[16][7].y);
			}
			// ��ɫֱ�к��
			SelectObject(hdc, hPenRed);
			SelectObject(hdc, hBrushRed);
			Polygon(hdc, arrow[17], 6);
			Rectangle(hdc, arrow[17][6].x, arrow[17][6].y, arrow[17][7].x, arrow[17][7].y);

			/* ���ʣ��ʱ��	*/
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			// ��ɫ��ת��ɫ
			SetTextColor(hdc, colorGreen);
			TextOut(hdc, text[0].x, text[0].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 115 - tLight)); // �����30s
			// ��ɫֱ�к�ɫ
			SetTextColor(hdc, colorRed);
			TextOut(hdc, text[1].x, text[1].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 120 - tLight + 50)); // �ҵƺ�
			// ��ɫԲ����ɫ
			SetTextColor(hdc, colorGreen);
			TextOut(hdc, text[2].x, text[2].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 115 - tLight)); // ��ɫ���̵���˸5��
			// ��ɫ��ת��ɫ
			SetTextColor(hdc, colorGreen);
			TextOut(hdc, text[3].x, text[3].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 115 - tLight)); // �����30s
			// ��ɫֱ�к�ɫ
			SetTextColor(hdc, colorRed);
			TextOut(hdc, text[4].x, text[4].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 120 - tLight + 50)); // �ҵƺ�
		}
		else if(tLight < 120) // Y R Y
		{
			// ��ɫ��ת�Ƶ�
			SelectObject(hdc, hPenYellow);
			SelectObject(hdc, hBrushYellow);
			Polygon(hdc, arrow[1], 6);
			Rectangle(hdc, arrow[1][6].x, arrow[1][6].y, arrow[1][7].x, arrow[1][7].y);
			// ��ɫֱ�к��
			SelectObject(hdc, hPenRed);
			SelectObject(hdc, hBrushRed);
			Polygon(hdc, arrow[6], 6);
			Rectangle(hdc, arrow[6][6].x, arrow[6][6].y, arrow[6][7].x, arrow[6][7].y);
			// ��ɫ��ת�Ƶ�
			SelectObject(hdc, hPenYellow);
			SelectObject(hdc, hBrushYellow);
			Ellipse(hdc, cc[12].x - radius, cc[12].y - radius, cc[12].x + radius, cc[12].y + radius);
			// ��ɫ��ת�Ƶ�
			SelectObject(hdc, hPenYellow);
			SelectObject(hdc, hBrushYellow);
			Polygon(hdc, arrow[16], 6);
			Rectangle(hdc, arrow[16][6].x, arrow[16][6].y, arrow[16][7].x, arrow[16][7].y);
			// ��ɫֱ�к��
			SelectObject(hdc, hPenRed);
			SelectObject(hdc, hBrushRed);
			Polygon(hdc, arrow[17], 6);
			Rectangle(hdc, arrow[17][6].x, arrow[17][6].y, arrow[17][7].x, arrow[17][7].y);
			
			/* ���ʣ��ʱ��	*/
			SetBkColor(hdc, RGB(0, 0, 0));
			SelectObject(hdc, hFont);
			// ��ɫ��ת��ɫ
			SetTextColor(hdc, colorYellow);
			TextOut(hdc, text[0].x, text[0].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 120 - tLight)); // ��ƻ�5s
			// ��ɫֱ�к�ɫ
			SetTextColor(hdc, colorRed);
			TextOut(hdc, text[1].x, text[1].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 120 - tLight + 50)); // �ҵƺ�
			// ��ɫԲ�λ�ɫ
			SetTextColor(hdc, colorYellow);
			TextOut(hdc, text[2].x, text[2].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 120 - tLight)); // ��ɫ�ƻ�
			// ��ɫ��ת��ɫ
			SetTextColor(hdc, colorYellow);
			TextOut(hdc, text[3].x, text[3].y, szTime,
				wsprintf(szTime, TEXT("%02d"), 120 - tLight)); // ��ƻ�5s
			// ��ɫֱ�к�ɫ
			SetTextColor(hdc, colorRed);
			TextOut(hdc, text[4].x, text[4].y, szTime,
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
	// ��ת��
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
		if (i == 2) // ��������15��Ȼ��ص�ѭ��+1����16�ˣ�Ҳ���Ƕ�ɫ��ת�Ƶ�λ��
			i = 15;
	}
	// ֱ�е�
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
		if (i == 8) // ��������16��Ȼ��ص�ѭ��+1����17�ˣ�Ҳ���Ƕ�ɫֱ�еƵ�λ��
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

// �������ֺͻ�ˢ����ɫ
void SetColor(COLORREF& colorRed, COLORREF& colorGreen, COLORREF& colorYellow)
{
	CIniFile* ini = new CIniFile(L".\\Config.ini"); // ����ini�ļ�
	// ��������ɫֵ
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
	// �����̵���ɫֵ
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

	// ����Ƶ���ɫֵ
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
