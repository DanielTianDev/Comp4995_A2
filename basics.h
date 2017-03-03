/*+----------------------------------------------------------------------
||
||		  basics.h
||
||        Author:  [Daniel Tian <danieltheman4000@gmail.com>]
||		  Date: January 30, 2017
||
||        Purpose:  [libraries necessary for the program are
||					included here all at once]
||
||
++-----------------------------------------------------------------------*/
#pragma once
#ifndef BASICS_H	//include guard
#define BASICS_H
#define BITMAP_PATH "kimi.png" //FILE PATH, bitmap should be in the same directory as cpp and header files
#define WIN32_LEAN_AND_MEAN

#include <windows.h>	/*all windows functions*/
#include <mmsystem.h>
#include <tchar.h>
#include <stdio.h>
#include <d3d9.h>		/*Directx9 libraries*/
#include <d3dx9.h>
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */
#include <cstdlib>		//line drawing
#include <iostream>		

#endif
