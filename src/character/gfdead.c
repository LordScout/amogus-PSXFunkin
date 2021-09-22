/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "gfdead.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

#include "speaker.h"

#include "../stage/week7.h"

//GF character structure
enum
{
	GFDead_ArcMain_BopLeft,
	GFDead_ArcMain_BopRight,
	GFDead_ArcMain_Cry,

	GFDead_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;

	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[GFDead_Arc_Max];

	Gfx_Tex tex;
	u8 frame, tex_id;

	//Speaker
	Speaker speaker;

	//Pico test
	u16* pico_p;
} Char_GF;

//GF character definitions
static const CharFrame char_gf_frame[] = {
	{GFDead_ArcMain_BopLeft, {  0,   0,  74, 103}, { 40,  93}}, //0 bop left 1
	{GFDead_ArcMain_BopLeft, { 74,   0,  73, 102}, { 39,  93}}, //1 bop left 2
	{GFDead_ArcMain_BopLeft, {147,   0,  73, 102}, { 39,  93}}, //2 bop left 3
	{GFDead_ArcMain_BopLeft, {  0, 103,  73, 103}, { 39,  94}}, //3 bop left 4
	{GFDead_ArcMain_BopLeft, { 73, 102,  82, 105}, { 43,  96}}, //4 bop left 5
	{GFDead_ArcMain_BopLeft, {155, 102,  81, 105}, { 43,  96}}, //5 bop left 6

	{GFDead_ArcMain_BopRight, {  0,   0,  81, 103}, { 43,  94}}, //6 bop right 1
	{GFDead_ArcMain_BopRight, { 81,   0,  81, 103}, { 43,  94}}, //7 bop right 2
	{GFDead_ArcMain_BopRight, {162,   0,  80, 103}, { 42,  94}}, //8 bop right 3
	{GFDead_ArcMain_BopRight, {  0, 103,  79, 103}, { 41,  94}}, //9 bop right 4
	{GFDead_ArcMain_BopRight, { 79, 103,  73, 105}, { 35,  96}}, //10 bop right 5
	{GFDead_ArcMain_BopRight, {152, 103,  74, 104}, { 35,  95}}, //11 bop right 6

	{GFDead_ArcMain_Cry, {  0,   0,  73, 101}, { 37,  93}}, //12 cry
	{GFDead_ArcMain_Cry, { 73,   0,  73, 101}, { 37,  93}}, //13 cry
};

static const Animation char_gf_anim[CharAnim_Max] = {
	{0, (const u8[]) { ASCR_CHGANI, CharAnim_Left }},                           //CharAnim_Idle
	{1, (const u8[]) { 0,  0,  1,  1,  2,  2,  3,  4,  4,  5, ASCR_BACK, 1 }}, //CharAnim_Left
	{0, (const u8[]) { ASCR_CHGANI, CharAnim_Left }},                           //CharAnim_LeftAlt
	{2, (const u8[]) { 12, 13, ASCR_REPEAT }},                                  //CharAnim_Down
	{0, (const u8[]) { ASCR_CHGANI, CharAnim_Left }},                           //CharAnim_DownAlt
	{0, (const u8[]) { ASCR_CHGANI, CharAnim_Left }},                           //CharAnim_Up
	{0, (const u8[]) { ASCR_CHGANI, CharAnim_Left }},                           //CharAnim_UpAlt
	{1, (const u8[]) { 6,  6,  7,  7,  8,  8,  9, 10, 10, 11, ASCR_BACK, 1 }}, //CharAnim_Right
	{0, (const u8[]) { ASCR_CHGANI, CharAnim_Left }},                           //CharAnim_RightAlt
};

//GF character functions
void Char_GFDead_SetFrame(void* user, u8 frame)
{
	Char_GF* this = (Char_GF*)user;

	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame* cframe = &char_gf_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_GFDead_Tick(Character* character)
{
	Char_GF* this = (Char_GF*)character;

	//Initialize Pico test
	if (stage.stage_id == StageId_7_3 && stage.back != NULL && this->pico_p == NULL)
		this->pico_p = ((Back_Week7*)stage.back)->pico_chart;

	if (this->pico_p != NULL)
	{
		if (stage.note_scroll >= 0)
		{
			//Scroll through Pico chart
			u16 substep = stage.note_scroll >> FIXED_SHIFT;
			while (substep >= ((*this->pico_p) & 0x7FFF))
			{
				//Play animation and bump speakers
				character->set_anim(character, ((*this->pico_p) & 0x8000) ? CharAnim_Right : CharAnim_Left);
				Speaker_Bump(&this->speaker);
				this->pico_p++;
			}
		}
	}
	else
	{
		if (stage.flag & STAGE_FLAG_JUST_STEP)
		{
			//Perform dance
			if ((stage.song_step % stage.gf_speed) == 0)
			{
				//Switch animation
				if (character->animatable.anim == CharAnim_Left)
					character->set_anim(character, CharAnim_Right);
				else
					character->set_anim(character, CharAnim_Left);

				//Bump speakers
				Speaker_Bump(&this->speaker);
			}
		}
	}

	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_GFDead_SetFrame);
	Character_Draw(character, &this->tex, &char_gf_frame[this->frame]);

	//Tick speakers
	Speaker_Tick(&this->speaker, character->x, character->y);
}

void Char_GFDead_SetAnim(Character* character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
}

void Char_GFDead_Free(Character* character)
{
	Char_GF* this = (Char_GF*)character;

	//Free art
	Mem_Free(this->arc_main);
}

Character* Char_GFDead_New(fixed_t x, fixed_t y)
{
	//Allocate gf object
	Char_GF* this = Mem_Alloc(sizeof(Char_GF));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_GFDead_New] Failed to allocate gf object");
		ErrorLock();
		return NULL;
	}

	//Initialize character
	this->character.tick = Char_GFDead_Tick;
	this->character.set_anim = Char_GFDead_SetAnim;
	this->character.free = Char_GFDead_Free;

	Animatable_Init(&this->character.animatable, char_gf_anim);
	Character_Init((Character*)this, x, y);

	//Set character stage information
	this->character.health_i = 1;

	this->character.focus_x = FIXED_DEC(16, 1);
	this->character.focus_y = FIXED_DEC(-50, 1);
	this->character.focus_zoom = FIXED_DEC(13, 10);

	//Load art
	this->arc_main = IO_Read("\\CHAR\\GFDEAD.ARC;1");

	const char** pathp = (const char* []){
		"bopleftdead.tim",  //GF_ArcMain_BopLeft
		"boprightdead.tim", //GF_ArcMain_BopRight
		"crydead.tim",      //GF_ArcMain_Cry
		NULL
	};
	IO_Data* arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);

	//Initialize render state
	this->tex_id = this->frame = 0xFF;

	//Initialize speaker
	Speaker_Init(&this->speaker);

	//Initialize Pico test
	if (stage.stage_id == StageId_7_3 && stage.back != NULL)
		this->pico_p = ((Back_Week7*)stage.back)->pico_chart;
	else
		this->pico_p = NULL;

	return (Character*)this;
}
