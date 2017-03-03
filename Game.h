/*+----------------------------------------------------------------------
||
||		  Game.h
||
||        Author:  [Daniel Tian <danieltheman4000@gmail.com>]
||		  Date: January 30, 2017
||
||        Purpose:  [ Create the background and directx9 devices/surfaces
||					for our program]
||
||		  Inherits From:  [None]
||
|+-----------------------------------------------------------------------
||
||      Attributes:  [private:
||						clock_t deltaTime - how much time has passed
||						unsigned int frames - holds the current number of
||							frames
||						double  frameRate  - current frame rate per second
||
||					  public:
||						LPDIRECT3D9 g_pD3D - our COM object
||						LPDIRECT3DDEVICE9 g_pDevice - the graphics device
||						HWND g_hWndMain - handle to main window
||						LPDIRECT3DSURFACE9 backbuffer - the back buffer
||						LPDIRECT3DSURFACE9 surface - surface for our bitmap
||						ID3DXFont *m_font - font used to display frame counter
||						static Line line  - line object used to draw lines in the game
||						static bool isDraw - true if the user holds down the mouse]
||
||
|+-----------------------------------------------------------------------
||
||
||		Methods:  [ 	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM)	 									return LRESULT
||						int InitDirect3DDevice(HWND, int, int, BOOL, D3DFORMAT, LPDIRECT3D9, LPDIRECT3DDEVICE9*);		return int
||						int GameInit();																					return int
||						int GameLoop();																					return int
||						int GameShutdown();																				return int
||						int Render();																					return int
||						void SetError(char*, ...);																		return void
||						void displayFramerate();																		return void
||						void DisplaySomeText(std::string);																return void
||						void SimpleBitmapDraw(LPDIRECT3DSURFACE9, HRESULT);												return void
||						int LoadBitmapToSurface(LPDIRECT3DSURFACE9);													return int
||					]
||
||
++-----------------------------------------------------------------------*/

#pragma once
#ifndef GAME_H	
#define GAME_H

#include "basics.h"

class Game {
public:
	//-----------------------------------------------------------------------------
	// Global variables
	//-----------------------------------------------------------------------------
	LPDIRECT3D9 g_pD3D = NULL;//COM object Used to create the D3DDevice
	LPDIRECT3DDEVICE9 g_pDevice = 0;//Our graphics rendering device

	DWORD g_dwNumMaterials = 0L;   // Number of mesh materials
	LPD3DXMESH              g_pMesh = NULL; // Our mesh object in sysmem
	D3DMATERIAL9*           g_pMeshMaterials = NULL; // Materials for our mesh
	LPDIRECT3DTEXTURE9*     g_pMeshTextures = NULL; // Textures for our mesh

	HWND g_hWndMain;//handle to main window
	LPDIRECT3DSURFACE9 backbuffer = NULL;	//backbuffer surface
	LPDIRECT3DSURFACE9 surface = NULL;	//surface for bitmap
	ID3DXFont *m_font;	//font of the text drawn to screen
	static bool isDraw;
	size_t IMG_WIDTH;
	size_t IMG_HEIGHT;

	//WndProc to handle user input such as clicking in order to draw lines
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	//initialize graphics device
	int InitDirect3DDevice(HWND, int, int, BOOL, D3DFORMAT, LPDIRECT3D9, LPDIRECT3DDEVICE9*);
	//initialize game by creating and initializing up any necessary information/data
	int GameInit();
	//The main game loop that executes until the user quits the game
	int GameLoop();
	//Load the mesh and build the material and texture arrays
	HRESULT InitGeometry(std::string);
	//Releases resources after game ends
	int Cleanup();
	//Sets up the world, view, and projection transform matrices
	VOID SetupMatrices();
	//Renders the back surface to the front
	int Render();
	//sets errors for when the program encounters errors
	void SetError(char*, ...);
	//draws the framerate onto the top left hand corner
	void displayFramerate();
	//Draws text onto the screen
	void DisplaySomeText(std::string);
	//Draws the background image
	void SimpleBitmapDraw(LPDIRECT3DSURFACE9, HRESULT);
	//Loads the background image to LPDIRECT3DSURFACE9 surface
	int LoadBitmapToSurface(LPDIRECT3DSURFACE9);

private:
	clock_t deltaTime = 0;	//difference between start and end time
	unsigned int frames = 0; //temp variable to hold number of frames
	double  frameRate = 0; //framerate variable
};
#endif