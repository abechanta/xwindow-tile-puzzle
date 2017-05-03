/*
// puzzle.h
*/

#ifndef PUZZLE_H
#define PUZZLE_H

void XInit();                                               /* Create Window */
void PixInit();                                   /* Draw Pixmap & Character */
void Redraw(Pixmap px);                        /* Copy from Pixmap to Window */
void Destroy();                                            /* Destroy Window */
void Flush(Pixmap px,                                          /* Flush Area */
	   int p_x, int p_y, int width, int height);
void Wait(int intr);                                           /* Empty Loop */
void Wait2();                                         /* Wait for mouse push */

void F_Help();                                           /* Open Help Window */
void F_Stage();                                             /* To Next Stage */
void F_Reset(int stg);                                         /* Retry Game */
void F_PlayBack();                                          /* Step PlayBack */
void F_Answer(int stg);                                       /* Show Answer */

void Draw_Field();                             /* Put Character to GameField */
void Rotate(int rotate_ID);                      /* Tile Rotate for ArrayBuf */
void R_Rotate(int rotate_ID, int mode);               /* Show Tile Rotate(H) */
void U_Rotate(int rotate_ID, int mode);               /* Show Tile Rotate(V) */
void SW_Add(int rotate_ID);                  /* Store Rotate_ID for PlayBack */
void Check();                                             /* Check for Clear */

#endif
