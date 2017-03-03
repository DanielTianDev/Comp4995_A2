//Game.cpp (for more detailed comment, go to Game.h)
#include "Game.h"


bool Game::isDraw = false;	//initializes the isDraw flag to false


/*---------------------------------------------------------------------
|  Function definition:  InitDirect3DDevice
|
|  Purpose:  Initializes our graphics device relative to our window created from
|			WinMain
|
|
|  Parameters:
|		hWndTarget			-- The x coordinate of the point
|		Width				-- The width of our device
|		Height				-- The height of our device
|		bWindowed			-- Window mode
|		FullScreenFormat	-- D3DFormat of device
|		pD3D				-- COM object
|		ppDevice			-- The device to initialize
|
|
|  returns:	int
*-------------------------------------------------------------------*/
int Game::InitDirect3DDevice(HWND hWndTarget, int Width, int Height, BOOL bWindowed, D3DFORMAT FullScreenFormat, LPDIRECT3D9 pD3D, LPDIRECT3DDEVICE9* ppDevice) {
	
	D3DPRESENT_PARAMETERS d3dpp;//rendering info
	D3DDISPLAYMODE d3ddm;//current display mode info
	HRESULT r = 0;

	if (*ppDevice)
		(*ppDevice)->Release();

	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	r = pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
	if (FAILED(r)) {
		SetError("Could not get display adapter information");
		return E_FAIL;
	}

	d3dpp.BackBufferWidth = Width;
	d3dpp.BackBufferHeight = Height;
	d3dpp.BackBufferFormat = bWindowed ? d3ddm.Format : FullScreenFormat;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	//D3DSWAPEFFECT_COPY;
	d3dpp.hDeviceWindow = hWndTarget;
	d3dpp.Windowed = bWindowed;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.FullScreen_RefreshRateInHz = 0;	//default refresh rate
	d3dpp.PresentationInterval = bWindowed ? 0 : D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;


	r = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWndTarget, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, ppDevice);
	if (FAILED(r)) {
		SetError("Could not create the render device");
		return E_FAIL;
	}

	// Turn on the zbuffer
	g_pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	// Turn on ambient lighting 
	g_pDevice->SetRenderState(D3DRS_AMBIENT, 0xffffffff);


	return S_OK;
}


/*---------------------------------------------------------------------
|  Function definition:  GameInit
|
|  Purpose:  Initializes the game by setting up the required data such
|				as font and loading in the background bitmap.
|
|
|  Parameters: None
|
|  returns:	int
*-------------------------------------------------------------------*/
int Game::GameInit() {
	HRESULT result = 0;	//return values


	//initialize Direct3D
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);//COM object
	if (g_pD3D == NULL) {
		SetError("Could not create IDirect3D9 object");
		return E_FAIL;
	}

	result = InitDirect3DDevice(g_hWndMain, 512, 512, TRUE, D3DFMT_X8R8G8B8, g_pD3D, &g_pDevice); //4th param = windowed
	if (FAILED(result)) {//FAILED is a macro that returns false if return value is a failure - safer than using value itself
		SetError("Initialization of the device failed");
		return E_FAIL;
	}


	//clear the backbuffer to black
	g_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	//create pointer to the back buffer
	g_pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);

	//create surface
	result = g_pDevice->CreateOffscreenPlainSurface(
		512,				//width of the surface
		512,				//height of the surface
		D3DFMT_X8R8G8B8,		//surface format
		D3DPOOL_DEFAULT,	//memory pool to use
		&surface,			//pointer to the surface
		NULL);				//reserved (always NULL)

	if (result != D3D_OK)	//make sure surface was successfully created
		return 1;

	result = LoadBitmapToSurface(surface);
	if (result != D3D_OK)	//check if the bitmap was successfully loaded to our surface
		return 1;

	result = D3DXCreateFont(g_pDevice, 20, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_font);
	if (result != D3D_OK)	//make sure font was created
		return 1;

	return 1;
}



//-----------------------------------------------------------------------------
// Name: InitGeometry()
// Desc: Load the mesh and build the material and texture arrays
//-----------------------------------------------------------------------------
HRESULT Game::InitGeometry(std::string meshName)
{
	LPD3DXBUFFER pD3DXMtrlBuffer;

	// Load the mesh from the specified file
	if (FAILED(D3DXLoadMeshFromX("airplane 2.x", D3DXMESH_SYSTEMMEM, //"Tiger.x"
		g_pDevice, NULL,
		&pD3DXMtrlBuffer, NULL, &g_dwNumMaterials,
		&g_pMesh)))
	{
		// If model is not in current folder, try parent folder
		if (FAILED(D3DXLoadMeshFromX("..\\Tiger.x", D3DXMESH_SYSTEMMEM,
			g_pDevice, NULL,
			&pD3DXMtrlBuffer, NULL, &g_dwNumMaterials,
			&g_pMesh)))
		{
			MessageBox(NULL, "Could not find tiger.x", "Meshes.exe", MB_OK);
			return E_FAIL;
		}
	}

	// We need to extract the material properties and texture names from the 
	// pD3DXMtrlBuffer
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	g_pMeshMaterials = new D3DMATERIAL9[g_dwNumMaterials];
	g_pMeshTextures = new LPDIRECT3DTEXTURE9[g_dwNumMaterials];

	for (DWORD i = 0; i<g_dwNumMaterials; i++)
	{
		// Copy the material
		g_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;

		// Set the ambient color for the material (D3DX does not do this)
		g_pMeshMaterials[i].Ambient = g_pMeshMaterials[i].Diffuse;

		g_pMeshTextures[i] = NULL;
		if (d3dxMaterials[i].pTextureFilename != NULL &&
			lstrlen(d3dxMaterials[i].pTextureFilename) > 0)
		{
			// Create the texture
			if (FAILED(D3DXCreateTextureFromFile(g_pDevice,
				d3dxMaterials[i].pTextureFilename,
				&g_pMeshTextures[i])))
			{
				// If texture is not in current folder, try parent folder
				const TCHAR* strPrefix = TEXT("..\\");
				const int lenPrefix = lstrlen(strPrefix);
				TCHAR strTexture[MAX_PATH];
				lstrcpyn(strTexture, strPrefix, MAX_PATH);
				lstrcpyn(strTexture + lenPrefix, d3dxMaterials[i].pTextureFilename, MAX_PATH - lenPrefix);
				// If texture is not in current folder, try parent folder
				if (FAILED(D3DXCreateTextureFromFile(g_pDevice,
					strTexture,
					&g_pMeshTextures[i])))
				{
					MessageBox(NULL, "Could not find texture map", "Meshes.exe", MB_OK);
				}
			}
		}
	}

	// Done with the material buffer
	pD3DXMtrlBuffer->Release();

	return S_OK;
}



/*---------------------------------------------------------------------
|  Function definition:  LoadBitmapToSurface
|
|  Purpose:  Load in the background image used for our game
|
|
|  Parameters: surface - The surface to load the bitmap to
|
|  returns:	int
*-------------------------------------------------------------------*/
int Game::LoadBitmapToSurface(LPDIRECT3DSURFACE9 surface) {
	HRESULT r = 0;
	//load surface from file
	r = D3DXLoadSurfaceFromFile(
		surface,			//destination surface
		NULL,				//destination palette
		NULL,				//destination rectangle
		BITMAP_PATH,		//bmp src path
		NULL,				//source rectangle
		D3DX_DEFAULT,		//controls how image is filtered
		0,					//for transparency (0 for none)
		NULL);

	DWORD ress = GetLastError();

	//make sure file loaded okay
	if (r != D3D_OK)
		return 1;

	//draw surface to the backbuffer
	r = g_pDevice->StretchRect(surface, NULL, backbuffer, NULL, D3DTEXF_NONE);
	if (r != D3D_OK)
		return 1;

	return S_OK;
}


/*---------------------------------------------------------------------
|  Function definition:  WndProc
|
|  Purpose:  The windows procedure associated with our program. In this
|			case, we are capturing the user click events and drawing lines
|			based on left button click and drag events.
|
|
|  Parameters: surface - The surface to load the bitmap to
|
|  returns:	int
*-------------------------------------------------------------------*/
LRESULT CALLBACK Game::WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {

	switch (uMessage) {
		case WM_CREATE:
		{
			return 0;
		}
		case WM_PAINT:
		{
			ValidateRect(hWnd, NULL);	//We took care of paint message without overhead
			return 0;
		}
		case WM_SIZE:
		{
			break;
		case WM_LBUTTONDOWN:
		{
		
			isDraw = true;
			break;
		}
		case WM_MOUSEMOVE:
		{
		
			break;
		}
		case WM_LBUTTONUP:
		{
		
			isDraw = false;
			break;
		}
		}
		case WM_DESTROY:
		{
			
			PostQuitMessage(0);
			return 0;
		}
		default:
		{
			return DefWindowProc(hWnd, uMessage, wParam, lParam);
		}
	}
}



/*---------------------------------------------------------------------
|  Function definition:  Render
|
|  Purpose:  Renders all game related graphics onto the screen
|			For every frame, this function does the following:
|				1. Draw background
|				2. Draws the frame rate over the background
|				3. Draw Line if user is holding down mouse
|
|
|  Parameters: none
|
|  returns:	int
*-------------------------------------------------------------------*/
int Game::Render() {

	HRESULT result;
	D3DLOCKED_RECT LockedRect;	//locked area of display memory(buffer really) we are drawing to
	LPDIRECT3DSURFACE9 pBackSurf = 0;

	if (!g_pDevice) {
		SetError("Cannot render because there is no device");
		return E_FAIL;
	}

	// Clear the backbuffer and the zbuffer
	g_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);


	// Begin the scene
	if (SUCCEEDED(g_pDevice->BeginScene())) {


		//gets pointer to backbuffer
		result = g_pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackSurf);
		if (FAILED(result))
			SetError("Couldn't get backbuffer");

		SimpleBitmapDraw(pBackSurf, result);

		// Setup the world, view, and projection matrices
		SetupMatrices();

		// Meshes are divided into subsets, one for each material. Render them in
		// a loop
		for (DWORD i = 0; i<g_dwNumMaterials; i++)
		{
			// Set the material and texture for this subset
			g_pDevice->SetMaterial(&g_pMeshMaterials[i]);
			g_pDevice->SetTexture(0, g_pMeshTextures[i]);

			// Draw the mesh subset
			g_pMesh->DrawSubset(i);
		}

		/*
		//get a lock on the surface
		result = pBackSurf->LockRect(&LockedRect, NULL, 0);
		if (FAILED(result))
			SetError("Could not lock the back buffer");

		DWORD* pData = (DWORD*)(LockedRect.pBits);
		pBackSurf->UnlockRect();
		pData = 0;
		pBackSurf->Release();//release lock	
		pBackSurf = 0; */
		
		g_pDevice->EndScene(); // End the scene

	}


	g_pDevice->Present(NULL, NULL, NULL, NULL);//swap over buffer to primary surface
	return S_OK;
}



/*---------------------------------------------------------------------
|  Function definition:  SimpleBitmapDraw
|
|  Purpose:  Displays the  background image and frame rate of the
|			 game loop onto the screen
|
|
|  Parameters: pBackSurf - the backsurface of our device
|			   r		 - The return values for our device functions
|
|  returns:	void
*-------------------------------------------------------------------*/
void Game::SimpleBitmapDraw(LPDIRECT3DSURFACE9 pBackSurf, HRESULT r) {

	if (g_pDevice->BeginScene()) {

		r = g_pDevice->StretchRect(surface, NULL, backbuffer, NULL, D3DTEXF_NONE);
		if (FAILED(r))
			SetError("Couldn't get surface");
		displayFramerate();
		g_pDevice->EndScene(); //stop rendering
	}
}

/*---------------------------------------------------------------------
|  Function definition:  DisplaySomeText
|
|  Purpose:  Draws text to the top left corner of the screen
|
|
|  Parameters: text - the text to be drawn
|
|  returns:	void
*-------------------------------------------------------------------*/
void Game::DisplaySomeText(std::string text)
{
	// Create a colour for the text - in this case blue
	D3DCOLOR fontColor = D3DCOLOR_ARGB(255, 255, 0, 10);
	// Create a rectangle to indicate where on the screen it should be drawn
	RECT rct;
	rct.left = 2;
	rct.right = 780;
	rct.top = 10;
	rct.bottom = rct.top + 20;
	m_font->DrawText(NULL, text.c_str(), -1, &rct, 0, fontColor);
}

/*---------------------------------------------------------------------
|  Function definition:  displayFramerate
|
|  Purpose:  Displays the frame rate of the game loop onto the screen
|
|
|  Parameters: none
|
|  returns:	void
*-------------------------------------------------------------------*/
void Game::displayFramerate() {
	char buff[100];
	snprintf(buff, sizeof(buff), "%s: %f", "FPS", frameRate);
	std::string buffAsStdStr = buff;
	DisplaySomeText(buffAsStdStr);
}

/*---------------------------------------------------------------------
|  Function definition:  GameLoop
|
|  Purpose:  The main game loop. Every frame this function does
|			 the following:
|				1. Get initial time
|				2. Render (draws background, framerate and line)
|				3. Get time after render() is done executing
|				4. Calculate the total time passed, and if the time
|				  is over 1000 milliseconds, then update our framerate counter
|
|
|  Parameters: none
|
|  returns:	int
*-------------------------------------------------------------------*/
int Game::GameLoop() {

	clock_t beginFrame = clock(); // 5555

	Render();

	clock_t endFrame = clock();
	deltaTime += endFrame - beginFrame;
	if (deltaTime > 1000) {
		frameRate = frames;
		frames = 0;
		deltaTime = 0;
	}
	frames++;

	if (GetAsyncKeyState(VK_ESCAPE))
		PostQuitMessage(0);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: SetupMatrices()
// Desc: Sets up the world, view, and projection transform matrices.
//-----------------------------------------------------------------------------
VOID Game::SetupMatrices()
{
	// For our world matrix, we will just leave it as the identity
	D3DXMATRIXA16 matWorld;
	D3DXMatrixRotationY(&matWorld, timeGetTime() / 1000.0f);
	g_pDevice->SetTransform(D3DTS_WORLD, &matWorld);

	// Set up our view matrix. A view matrix can be defined given an eye point,
	// a point to lookat, and a direction for which way is up. Here, we set the
	// eye five units back along the z-axis and up three units, look at the 
	// origin, and define "up" to be in the y-direction.
	D3DXVECTOR3 vEyePt(0.0f, 3.0f, -5.0f);
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
	g_pDevice->SetTransform(D3DTS_VIEW, &matView);

	// For the projection matrix, we set up a perspective transform (which
	// transforms geometry from 3D view space to 2D viewport space, with
	// a perspective divide making objects smaller in the distance). To build
	// a perpsective transform, we need the field of view (1/4 pi is common),
	// the aspect ratio, and the near and far clipping planes (which define at
	// what distances geometry should be no longer be rendered).
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f);
	g_pDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}

/*---------------------------------------------------------------------
|  Function definition:  GameShutdown
|
|  Purpose:  release resources. First display adapter because COM object
|			created it, then COM object
|
|
|  Parameters: none
|
|  returns:	int
*-------------------------------------------------------------------*/
int Game::Cleanup() {

	if (g_pMeshMaterials != NULL)
		delete[] g_pMeshMaterials;

	if (g_pMeshTextures)
	{
		for (DWORD i = 0; i < g_dwNumMaterials; i++)
		{
			if (g_pMeshTextures[i])
				g_pMeshTextures[i]->Release();
		}
		delete[] g_pMeshTextures;
	}

	if (g_pMesh != NULL)
		g_pMesh->Release();

	if (g_pDevice)
		g_pDevice->Release();

	if (g_pD3D)
		g_pD3D->Release();

	return S_OK;
}


/*---------------------------------------------------------------------
|  Function definition:  SetError
|
|  Purpose:  Sets the error when they are encountered
|
|
|  Parameters: szFormat - format of the error
|
|  returns:	void
*-------------------------------------------------------------------*/
void Game::SetError(char* szFormat, ...) {
	char szBuffer[1024];
	va_list pArgList;
	va_start(pArgList, szFormat);
	_vsntprintf(szBuffer, sizeof(szBuffer) / sizeof(char), szFormat, pArgList);
	va_end(pArgList);
	OutputDebugString(szBuffer);
	OutputDebugString("\n");
}