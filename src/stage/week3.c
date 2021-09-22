/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "week3.h"

#include "../mem.h"
#include "../archive.h"
#include "../random.h"
#include "../timer.h"

//Week 3 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back4; //Train
	Gfx_Tex tex_back5; //Sky
	
	//Window state
	u8 win_r, win_g, win_b;
	fixed_t win_time;
	
	//Train state
	fixed_t building_y;
} Back_Week3;

//Week 3 background functions
static const u8 win_cols[][3] = {
	{ 49, 162, 253},
	{ 49, 253, 140},
	{251,  51, 245},
	{253,  69,  49},
	{251, 166,  51},
};

void Back_Week3_Window(Back_Week3 *this)
{
	const u8 *win_col = win_cols[RandomRange(0, COUNT_OF(win_cols) - 1)];
	this->win_r = win_col[0];
	this->win_g = win_col[1];
	this->win_b = win_col[2];
	this->win_time = FIXED_DEC(3,1);
}

void Back_Week3_DrawBG(StageBack *back)
{
	Back_Week3 *this = (Back_Week3*)back;
	
	fixed_t fx, fy;
	
	//Move building

	fx = stage.camera.x;
	fy = stage.camera.y;

	RECT building_src = { 15, 0, 91, 250,};
	RECT_FIXED building_dst = {
		FIXED_DEC(-430,1) - fx, 
		FIXED_DEC(-320,1) - this->building_y,
		FIXED_DEC(775,1),
		FIXED_DEC(1600,1)
	};
	while (building_dst.y < FIXED_DEC(320, 1))
	{
		Stage_DrawTex(&this->tex_back4, &building_src, &building_dst, stage.camera.bzoom);
		building_dst.y += building_dst.h;
	}
	this->building_y = (this->building_y + (timer_dt << 11)) % building_dst.h;
	
	//Draw sky
	fx = stage.camera.x >> 1;
	fy = stage.camera.y >> 1;
	
	RECT sky_src = {0, 0, 256, 256};
	RECT_FIXED sky_dst = {
		FIXED_DEC(-400 - SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-425,1) - fy,
		FIXED_DEC(815 + SCREEN_WIDEOADD,1),
		FIXED_DEC(825,1)
	};
	
	Stage_DrawTex(&this->tex_back5, &sky_src, &sky_dst, stage.camera.bzoom);
	sky_dst.x += sky_dst.w;
	sky_src.y += sky_src.h;
	Stage_DrawTex(&this->tex_back5, &sky_src, &sky_dst, stage.camera.bzoom);
}

void Back_Week3_Free(StageBack *back)
{
	Back_Week3 *this = (Back_Week3*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Week3_New(void)
{
	//Allocate background structure
	Back_Week3 *this = (Back_Week3*)Mem_Alloc(sizeof(Back_Week3));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Week3_DrawBG;
	this->back.free = Back_Week3_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\WEEK3\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back4, Archive_Find(arc_back, "back4.tim"), 0);
	Gfx_LoadTex(&this->tex_back5, Archive_Find(arc_back, "back5.tim"), 0);
	Mem_Free(arc_back);
	
	//Initialize window state
	this->win_time = -1;
	
	//Initialize train state
	this->building_y = 0;
	
	return (StageBack*)this;
}
