/*
 * Sample program to illustrate VGA line drawing in 360x480
 * 256-color mode.
 *
 * Compiled with Borland C/C++.
 *
 * Must be linked with Listing 32.1 with a command line like:
 *
 *    bcc l32-2.c l32-1.asm
 *
 * By Michael Abrash
 */
#include <dos.h>                 /* contains geninterrupt */

#define TEXT_MODE       0x03
#define BIOS_VIDEO_INT  0x10
#define X_MAX           360      /* working screen width */
#define Y_MAX           480      /* working screen height */

extern void Draw360x480Dot();
extern void Set360x480Mode();

/*
 * Draws a line in octant 0 or 3 ( |DeltaX| >= DeltaY ).
 * |DeltaX|+1 points are drawn.
 */
void Octant0(X0, Y0, DeltaX, DeltaY, XDirection, Color)
unsigned int X0, Y0;          /* coordinates of start of the line */
unsigned int DeltaX, DeltaY;  /* length of the line */
int XDirection;               /* 1 if line is drawn left to right,
                                -1 if drawn right to left */
int Color;                    /* color in which to draw line */
{
	int DeltaYx2;
	int DeltaYx2MinusDeltaXx2;
	int ErrorTerm;

	/* Set up initial error term and values used inside drawing loop */
	DeltaYx2 = DeltaY * 2;
	DeltaYx2MinusDeltaXx2 = DeltaYx2 - (int) ( DeltaX * 2 );
	ErrorTerm = DeltaYx2 - (int) DeltaX;

	/* Draw the line */
	Draw360x480Dot(X0, Y0, Color);   /* draw the first pixel */
	while ( DeltaX-- ) {
		/* See if it's time to advance the Y coordinate */
		if ( ErrorTerm >= 0 ) {
			/* Advance the Y coordinate & adjust the error term
			back down */
			Y0++;
			ErrorTerm += DeltaYx2MinusDeltaXx2;
		} else {
			/* Add to the error term */
			ErrorTerm += DeltaYx2;
		}
		X0 += XDirection;          /* advance the X coordinate */
		Draw360x480Dot(X0, Y0, Color);    /* draw a pixel */
	}
}

/*
 * Draws a line in octant 1 or 2 ( |DeltaX| < DeltaY ).
 * |DeltaY|+1 points are drawn.
 */
void Octant1(X0, Y0, DeltaX, DeltaY, XDirection, Color)
unsigned int X0, Y0;          /* coordinates of start of the line */
unsigned int DeltaX, DeltaY;  /* length of the line */
int XDirection;               /* 1 if line is drawn left to right,
                                 -1 if drawn right to left */
int Color;                    /* color in which to draw line */
{
	int DeltaXx2;
	int DeltaXx2MinusDeltaYx2;
	int ErrorTerm;

	/* Set up initial error term and values used inside drawing loop */
	DeltaXx2 = DeltaX * 2;
	DeltaXx2MinusDeltaYx2 = DeltaXx2 - (int) ( DeltaY * 2 );
	ErrorTerm = DeltaXx2 - (int) DeltaY;

	Draw360x480Dot(X0, Y0, Color);		/* draw the first pixel */
	while ( DeltaY-- ) {
		/* See if it's time to advance the X coordinate */
		if ( ErrorTerm >= 0 ) {
			/* Advance the X coordinate & adjust the error term
			back down */
			X0 += XDirection;
			ErrorTerm += DeltaXx2MinusDeltaYx2;
		} else {
			/* Add to the error term */
			ErrorTerm += DeltaXx2;
		}
		Y0++;                   		/* advance the Y coordinate */
		Draw360x480Dot(X0, Y0,Color);  	/* draw a pixel */
	}
}

/*
 * Draws a line on the EGA or VGA.
 */
void EVGALine(X0, Y0, X1, Y1, Color)
int X0, Y0;    			/* coordinates of one end of the line */
int X1, Y1;    			/* coordinates of the other end of the line */
unsigned char Color;    /* color in which to draw line */
{
	int DeltaX, DeltaY;
	int Temp;

	/* Save half the line-drawing cases by swapping Y0 with Y1
	   and X0 with X1 if Y0 is greater than Y1. As a result, DeltaY
	   is always > 0, and only the octant 0-3 cases need to be
	   handled. */
	if ( Y0 > Y1 ) {
		Temp = Y0;
		Y0 = Y1;
		Y1 = Temp;
		Temp = X0;
		X0 = X1;
		X1 = Temp;
	}

	/* Handle as four separate cases, for the four octants in which
	   Y1 is greater than Y0 */
	DeltaX = X1 - X0;    /* calculate the length of the line
						    in each coordinate */
	DeltaY = Y1 - Y0;
	if ( DeltaX > 0 ) {
		if ( DeltaX > DeltaY ) {
			Octant0(X0, Y0, DeltaX, DeltaY, 1, Color);
		} else {
			Octant1(X0, Y0, DeltaX, DeltaY, 1, Color);
		}
	} else {
		DeltaX = -DeltaX;             /* absolute value of DeltaX */
		if ( DeltaX > DeltaY ) {
			Octant0(X0, Y0, DeltaX, DeltaY, -1, Color);
		} else {
			Octant1(X0, Y0, DeltaX, DeltaY, -1, Color);
		}
	}
}

/*
 * Subroutine to draw a rectangle full of vectors, of the
 * specified length and in varying colors, around the
 * specified rectangle center.
 */
void VectorsUp(XCenter, YCenter, XLength, YLength)
int XCenter, YCenter;   /* center of rectangle to fill */
int XLength, YLength;   /* distance from center to edge
                           of rectangle */
{
	int WorkingX, WorkingY, Color = 1;

	/* Lines from center to top of rectangle */
	WorkingX = XCenter - XLength;
	WorkingY = YCenter - YLength;
	for ( ; WorkingX < ( XCenter + XLength ); WorkingX++ )
		EVGALine(XCenter, YCenter, WorkingX, WorkingY, Color++);

	/* Lines from center to right of rectangle */
	WorkingX = XCenter + XLength - 1;
	WorkingY = YCenter - YLength;
	for ( ; WorkingY < ( YCenter + YLength ); WorkingY++ )
		EVGALine(XCenter, YCenter, WorkingX, WorkingY, Color++);

	/* Lines from center to bottom of rectangle */
	WorkingX = XCenter + XLength - 1;
	WorkingY = YCenter + YLength - 1;
	for ( ; WorkingX >= ( XCenter - XLength ); WorkingX-- )
		EVGALine(XCenter, YCenter, WorkingX, WorkingY, Color++);

	/* Lines from center to left of rectangle */
	WorkingX = XCenter - XLength;
	WorkingY = YCenter + YLength - 1;
	for ( ; WorkingY >= ( YCenter - YLength ); WorkingY-- )
		EVGALine(XCenter, YCenter, WorkingX, WorkingY, Color++);
}

/*
 * Sample program to draw four rectangles full of lines.
 */
void main()
{
	char temp;

	Set360x480Mode();

	/* Draw each of four rectangles full of vectors */
	VectorsUp(X_MAX / 4, Y_MAX / 4, X_MAX / 4, Y_MAX / 4, 1);
	VectorsUp(X_MAX * 3 / 4, Y_MAX / 4, X_MAX / 4, Y_MAX / 4, 2);
	VectorsUp(X_MAX / 4, Y_MAX * 3 / 4, X_MAX / 4, Y_MAX / 4, 3);
	VectorsUp(X_MAX * 3 / 4, Y_MAX * 3 / 4, X_MAX / 4, Y_MAX / 4, 4);

	/* Wait for the enter key to be pressed */
	scanf("%c", &temp);

	/* Back to text mode */
	_AX = TEXT_MODE;
	geninterrupt(BIOS_VIDEO_INT);
}
