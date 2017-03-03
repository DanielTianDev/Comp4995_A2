//-----------------------------------------------------------------------------
// File: Meshes.cpp
//
// Desc: For advanced geometry, most apps will prefer to load pre-authored
//       meshes from a file. Fortunately, when using meshes, D3DX does most of
//       the work for this, parsing a geometry file and creating vertx buffers
//       (and index buffers) for us. This tutorial shows how to use a D3DXMESH
//       object, including loading it from a file and rendering it. One thing
//       D3DX does not handle for us is the materials and textures for a mesh,
//       so note that we have to handle those manually.
//
//       Note: one advanced (but nice) feature that we don't show here is that
//       when cloning a mesh we can specify the FVF. So, regardless of how the
//       mesh was authored, we can add/remove normals, add more texture
//       coordinate sets (for multi-texturing), etc.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "../Game.h"
//CHARCTER SET -> NOTHING



//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	
	Game game; //game object
	HWND hWnd;	//handle to the window
	MSG msg;	//WndProc messages
	WNDCLASSEX wc;
	static char strAppName[] = "Meshes by Daniel Tian";

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.lpfnWndProc = game.WndProc;
	wc.hInstance = hInst;
	wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_HAND);
	wc.hCursor = LoadCursor(NULL, IDC_CROSS);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = strAppName;
	RegisterClassEx(&wc);


	hWnd = CreateWindowEx(NULL,
		strAppName,
		strAppName,
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, //no resizing
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		512, 512,
		NULL,
		NULL,
		hInst,
		NULL);

	//DWORD creationError = GetLastError();

	game.g_hWndMain = hWnd;	//set our global window handle

	if (game.GameInit()) {
		// Create the scene geometry
		if (SUCCEEDED(game.InitGeometry("Tiger.x"))) {
			ShowWindow(hWnd, SW_SHOWDEFAULT);
			UpdateWindow(hWnd);

		

			// Enter the message loop
			ZeroMemory(&msg, sizeof(msg));
			while (msg.message != WM_QUIT)
			{
				if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
					game.GameLoop();
				if (GetAsyncKeyState(VK_ESCAPE))
					PostQuitMessage(0);
			}
		}
	}
	else {
		game.SetError("Initialization Failed");
		game.Cleanup();
		return E_FAIL;
	}

	game.Cleanup();// clean up the game
	UnregisterClass("A2X", wc.hInstance);
	return msg.wParam;

}



