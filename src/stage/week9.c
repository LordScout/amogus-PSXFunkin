/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "week9.h"

#include "../mem.h"
#include "../archive.h"

//Week 2 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //Background
	Gfx_Tex tex_back1; //Window
	Gfx_Tex tex_back2; //Lightning window
} Back_Week8;

//Week 2 background functions
void Back_Week9_DrawBG(StageBack *back)
{
	Back_Week8 *this = (Back_Week8*)back;
	
	fixed_t fx, fy;
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	//Draw window
	RECT window_src = {0, 0, 256, 161};
	RECT_FIXED window_dst = {
		FIXED_DEC(-140,1) - fx,
		FIXED_DEC(-115,1) - fy,
		FIXED_DEC(256,1),
		FIXED_DEC(183,1)
	};
	
	Stage_DrawTex(&this->tex_back1, &window_src, &window_dst, stage.camera.bzoom);
	
	//Draw window light
	RECT windowl_src = {0, 128, 0, 0};
	RECT_FIXED windowl_dst = {
		FIXED_DEC(-900,1) - fx,
		FIXED_DEC(44,1) - fy,
		FIXED_DEC(350,1),
		FIXED_DEC(148,1)
	};
	
	Stage_DrawTex(&this->tex_back1, &windowl_src, &windowl_dst, stage.camera.bzoom);
	
	fx = stage.camera.x >> 3;
	fy = stage.camera.y >> 3;

	//Draw background
	RECT back_src = {0, 0, 256, 161};
	RECT_FIXED back_dst = {
		FIXED_DEC(-130,1) - fx,
		FIXED_DEC(-70,1) - fy,
		FIXED_DEC(256,1),
		FIXED_DEC(161,1)
	};
	
	Stage_DrawTex(&this->tex_back0, &back_src, &back_dst, stage.camera.bzoom);
	
	#if SCREEN_WIDTH > 320
		RECT backl_src = {0, 0, 1, 256};
		RECT_FIXED backl_dst = {
			FIXED_DEC(-185,1) - FIXED_DEC(SCREEN_WIDEADD,2) - fx,
			FIXED_DEC(-125,1) - fy,
			FIXED_DEC(SCREEN_WIDEADD,2),
			FIXED_DEC(267,1)
		};
		RECT backr_src = {255, 0, 0, 256};
		RECT_FIXED backr_dst = {
			FIXED_DEC(168,1) - fx,
			FIXED_DEC(-125,1) - fy,
			FIXED_DEC(SCREEN_WIDEADD,2),
			FIXED_DEC(267,1)
		};
		
		Stage_DrawTex(&this->tex_back0, &backl_src, &backl_dst, stage.camera.bzoom);
		Stage_DrawTex(&this->tex_back0, &backr_src, &backr_dst, stage.camera.bzoom);
	#endif
}

void Back_Week9_Free(StageBack *back)
{
	Back_Week8 *this = (Back_Week8*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Week9_New(void)
{
	//Allocate background structure
	Back_Week8 *this = (Back_Week8*)Mem_Alloc(sizeof(Back_Week8));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Week9_DrawBG;
	this->back.free = Back_Week9_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\WEEK9\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
