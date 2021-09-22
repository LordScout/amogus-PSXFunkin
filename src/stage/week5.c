/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "week5.h"

#include "../archive.h"
#include "../mem.h"

//Week 5 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	Gfx_Tex tex_back0; //Back wall
	Gfx_Tex tex_back2; //Lower bop
} Back_Week5;

//Week 5 background functions
void Back_Week5_DrawBG(StageBack *back)
{
	Back_Week5 *this = (Back_Week5*)back;
	
	fixed_t fx, fy;
	
	fixed_t beat_bop;
	if ((stage.song_step & 0x6) == 0)
		beat_bop = FIXED_UNIT - ((stage.note_scroll / 24) & FIXED_LAND);
	else
		beat_bop = 0;
	
	//Draw Santa
	
	//Draw snow
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT snow_src = {120, 155, 136, 101};
	RECT_FIXED snow_dst = {
		FIXED_DEC(-350,1) - fx,
		FIXED_DEC(900,1) - fy,
		FIXED_DEC(570,1),
		FIXED_DEC(27,1)
	};
	
	Stage_DrawTex(&this->tex_back2, &snow_src, &snow_dst, stage.camera.bzoom);
	snow_src.y = 255; snow_src.h = 0;
	snow_dst.y += snow_dst.h - FIXED_UNIT;
	snow_dst.h *= 3;
	Stage_DrawTex(&this->tex_back2, &snow_src, &snow_dst, stage.camera.bzoom);
	
	//Draw boppers
	static const struct Back_Week5_LowerBop
	{
		RECT src;
		RECT_FIXED dst;
	} lbop_piece[] = {
		{{72, 0, 56, 85}, {FIXED_DEC(-208,1), FIXED_DEC(15,1), FIXED_DEC(35,1), FIXED_DEC(50,1)}},
		{{0, 0, 72, 71}, {FIXED_DEC(-203,1), FIXED_DEC(-30,1), FIXED_DEC(50,1), FIXED_DEC(50,1)}},
		{{200, 0, 56, 69}, {FIXED_DEC(135,1), FIXED_DEC(15,1), FIXED_DEC(40,1), FIXED_DEC(50,1)}},
		{{130, 0, 69, 85}, {FIXED_DEC(130,1), FIXED_DEC(-30,1), FIXED_DEC(50,1), FIXED_DEC(55,1)}},
	};
	
	const struct Back_Week5_LowerBop *lbop_p = lbop_piece;
	for (size_t i = 0; i < COUNT_OF(lbop_piece); i++, lbop_p++)
	{
		RECT_FIXED lbop_dst = {
			lbop_p->dst.x - fx - (beat_bop << 1),
			lbop_p->dst.y - fy + (beat_bop << 3),
			lbop_p->dst.w + (beat_bop << 2),
			lbop_p->dst.h - (beat_bop << 3),
		};
		Stage_DrawTex(&this->tex_back2, &lbop_p->src, &lbop_dst, stage.camera.bzoom);
	}
	
	//Draw second floor
	fx = stage.camera.x >> 2;
	fy = stage.camera.y >> 2;
	
	static const struct Back_Week5_FloorPiece
	{
		RECT src;
		fixed_t scale;
	} floor_piece[] = {
		{{  0, 0, 161, 256}, FIXED_DEC(14,10)},
		{{161, 0,   9, 256}, FIXED_DEC(7,1)},
		{{171, 0,  85, 256}, FIXED_DEC(14,10)},
	};
	
	RECT_FIXED floor_dst = {
		FIXED_DEC(-220,1) - fx,
		FIXED_DEC(-115,1) - fy,
		0,
		FIXED_DEC(180,1)
	};
	
	//Draw boppers
	static const struct Back_Week5_UpperBop
	{
		RECT src;
		RECT_FIXED dst;
	} ubop_piece[] = {
		{{0, 0, 256, 76}, {FIXED_DEC(-200,1), FIXED_DEC(-132,1), FIXED_DEC(256,1) * 6 / 7, FIXED_DEC(76,1) * 6 / 7}},
		{{0, 76, 256, 76}, {FIXED_DEC(50,1), FIXED_DEC(-132,1), FIXED_DEC(256,1) * 6 / 7, FIXED_DEC(76,1) * 6 / 7}}
	};
	
	//Draw back wall
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	static const struct Back_Week5_WallPiece
	{
		RECT src;
		fixed_t scale;
	} wall_piece[] = {
		{{  0, 0, 0, 256}, FIXED_DEC(1,1)},
		{{113, 0,   6, 256}, FIXED_DEC(17,1)},
		{{119, 0, 137, 256}, FIXED_DEC(1,1)},
	};
	
	RECT_FIXED wall_dst = {
		FIXED_DEC(-900,1) - fx,
		FIXED_DEC(-130,1) - fy,
		0,
		FIXED_DEC(190,1)
	};
	
	RECT wall_src = {0, 0,256, 242};
	RECT_FIXED wall_fill;
	wall_fill.x = FIXED_DEC(-320, 1) - fx;
	wall_fill.y = FIXED_DEC(-240, 1) - fy;
	wall_fill.w = FIXED_DEC(600,1);
	wall_fill.h = FIXED_DEC(410,1);
	Stage_DrawTex(&this->tex_back0, &wall_src, &wall_fill, stage.camera.bzoom);
	
	const struct Back_Week5_WallPiece *wall_p = wall_piece;
	for (size_t i = 0; i < COUNT_OF(wall_piece); i++, wall_p++)
	{
		wall_dst.w = wall_p->src.w ? (wall_p->src.w * wall_p->scale) : wall_p->scale;
		Stage_DrawTex(&this->tex_back0, &wall_p->src, &wall_dst, stage.camera.bzoom);
		wall_dst.x += wall_dst.w;
	}
}

void Back_Week5_Free(StageBack *back)
{
	Back_Week5 *this = (Back_Week5*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Week5_New()
{
	//Allocate background structure
	Back_Week5 *this = (Back_Week5*)Mem_Alloc(sizeof(Back_Week5));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = Back_Week5_DrawBG;
	this->back.free = Back_Week5_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\WEEK5\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
