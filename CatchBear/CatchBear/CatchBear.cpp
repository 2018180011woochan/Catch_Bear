// CatchBear.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "framework.h"
#include "CatchBear.h"
#include "Game.h"

#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "BufferReader.h"
#include "ServerPacketHandler.h"
#include "ServerSession.h"


#define MAX_LOADSTRING 100
wstring MyIPAddr;

// 전역 변수:
WindowInfo GWindowInfo;
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

unique_ptr<Game> game;
char strText[255];     // 텍스트 저장
char str[10];       // 조합중인 문자
int len = 0;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CATCHBEAR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CATCHBEAR));

    MSG msg;

    GWindowInfo.width = 1200;
    GWindowInfo.height = 800;
    GWindowInfo.windowed = true;

    game = make_unique<Game>();    // 아래서 delete할 필요x 알아서 삭제해줌
    game->Init(GWindowInfo);

    // 쓰레드 만들기

    ServerPacketHandler::Init();
    //this_thread::sleep_for(1s);
    ClientServiceRef service;
    // 기본 메시지 루프입니다:
    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            
            if (msg.message == WM_QUIT)
                break;

            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                
            }
        }
        
        if (!game->isIPAddrEnter())
            game->LoginSceneUpdate();
        else 
        {
            if (game->_isFirstEnter)    // 처음 접속했으면 클라이언트 서비스 설정해줌
            {
                MyIPAddr = game->GetFontString();
                service = MakeShared<ClientService>(
                    //NetAddress(L"127.0.0.1", 7777),
                    NetAddress(MyIPAddr, 7777),
                    MakeShared<IocpCore>(),
                    MakeShared<ServerSession>, // TODO : SessionManager 등
                    100);

                ASSERT_CRASH(service->Start());

                for (int32 i = 0; i < 5; i++)
                {
                    GThreadManager->Launch([=]()
                        {
                            while (true)
                            {
                                service->GetIocpCore()->Dispatch();
                            }
                        });
                }

                game->_isFirstEnter = false;
            }
            
            game->Update();
        }
        if (game->_isEnd)
            SendMessage(msg.hwnd, WM_CLOSE, 0, 0);
    }
    GThreadManager->Join();
    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CATCHBEAR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   GWindowInfo.hwnd = hWnd;

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HIMC    _hIMC = NULL;

    switch (message)
    {
    case WM_IME_COMPOSITION:    // 글씨 조합중
        _hIMC = ImmGetContext(hWnd);    // IME 핸들을 얻는다

        // 1. 조합이 완성되면
        if (lParam & GCS_RESULTSTR) 
        {
            // 현재 IME의 문자열 길이를 얻는다
            if ((len = ImmGetCompositionString(_hIMC, GCS_RESULTSTR, NULL, 0)) > 0)
            {
                // str에 조합중인 문자열을 받아낸다
                ImmGetCompositionString(_hIMC, GCS_RESULTSTR, str, len);

                // 제일 끝에 0을 붙여 깨끗하게 해준다
                str[len] = 0;

                // 전체 내용(실제 보일 텍스트) 뒤에 붙여준다
                strcpy(strText + strlen(strText), str);

                // 초기화
                memset(str, 0, 10);
            }
        }

        // 2. 조합중이면
        else if (lParam & GCS_COMPSTR)  
        {
            // 조합중인 길이를 얻는다
            len = ImmGetCompositionString(_hIMC, GCS_COMPSTR, NULL, 0);
            
            // str에 조합중인 문자를 얻는다
            ImmGetCompositionString(_hIMC, GCS_COMPSTR, str, len);

            str[len] = 0;
        }

        // 핸들 반환
        ImmReleaseContext(hWnd, _hIMC);
        break;

    case WM_CHAR:   // 문자 넘어오기
        if (wParam == VK_BACK)
        {
            len = strlen(strText);
            strText[len-1] = 0;
            game->setString(strText);
        }
        else
        {
            len = strlen(strText);
            strText[len] = wParam & 0xff;
            strText[len + 1] = 0;
            game->setString(strText);
        }
        break;


    case WM_KEYDOWN:    // 키다운
        break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
