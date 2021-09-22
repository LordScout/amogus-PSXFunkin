/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "mom.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"
#include "../timer.h"

//Mom character structure
enum
{
	Mom_ArcMain_Idle0,
	Mom_ArcMain_Idle1,
	Mom_ArcMain_Idle2,
	Mom_ArcMain_Idle3,
	Mom_ArcMain_LeftA0,
	Mom_ArcMain_LeftA1,
	Mom_ArcMain_DownA0,
	Mom_ArcMain_DownA1,
	Mom_ArcMain_UpA0,
	Mom_ArcMain_UpA1,
	Mom_ArcMain_RightA0,
	Mom_ArcMain_RightA1,
	
	Mom_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Mom_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
} Char_Mom;

//Mom character definitions
static const CharFrame char_mom_frame[] = {
	{Mom_ArcMain_Idle0, {0,   0, 255, 208}, {128, 210}}, //0 idle 1
	{Mom_ArcMain_Idle1, {0,   0, 211, 199}, {97, 201}}, //1 idle 2
	{Mom_ArcMain_Idle2, {0,   0, 239, 201}, {114, 202}}, //2 idle 3
	{Mom_ArcMain_Idle3, {0,   0, 254, 207}, {127, 208}}, //3 idle 4

	{Mom_ArcMain_LeftA0, {0,   0, 253, 210}, {148, 211}}, //4 left a 1
	{Mom_ArcMain_LeftA1, {0,   0, 255, 204}, {165, 206}}, //5 left a 2

	{Mom_ArcMain_DownA0, {0,   0, 255, 202}, {122, 204}}, //8 down a 1
	{Mom_ArcMain_DownA1, {0,   0, 255, 190}, {126, 192}}, //9 down a 2

	{Mom_ArcMain_UpA0, {0,   0, 250, 240}, {122, 241}}, //12 up a 1
	{Mom_ArcMain_UpA1, {0,   0, 255, 186}, {131, 187}}, //13 up a 2

	{Mom_ArcMain_RightA0, {0,   0, 196, 193}, {59, 194}}, //16 right a 1
	{Mom_ArcMain_RightA1, {0,   0, 185, 203}, {50, 204}}, //17 right a 2

};

static const Animation char_mom_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 4,  5, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 6,  7, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 8,  9, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){10, 11, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Mom character functions
void Char_Mom_SetFrame(void *user, u8 frame)
{
	Char_Mom *this = (Char_Mom*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_mom_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Mom_Tick(Character *character)
{
	Char_Mom *this = (Char_Mom*)character;
	
	//Perform idle dance
	Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Mom_SetFrame);
	Character_Draw(character, &this->tex, &char_mom_frame[this->frame]);
}

void Char_Mom_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Mom_Free(Character *character)
{
	Char_Mom *this = (Char_Mom*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Mom_New(fixed_t x, fixed_t y)
{
	//Allocate mom object
	Char_Mom *this = Mem_Alloc(sizeof(Char_Mom));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Mom_New] Failed to allocate mom object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Mom_Tick;
	this->character.set_anim = Char_Mom_SetAnim;
	this->character.free = Char_Mom_Free;
	
	Animatable_Init(&this->character.animatable, char_mom_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 4;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-85,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\MOM.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Mom_ArcMain_Idle0
		"idle1.tim", //Mom_ArcMain_Idle1
		"idle2.tim", //Mom_ArcMain_Idle1
		"idle3.tim", //Mom_ArcMain_Idle1
		"lefta0.tim",  //Mom_ArcMain_Left
		"lefta1.tim",  //Mom_ArcMain_Left
		"downa0.tim",  //Mom_ArcMain_Down
		"downa1.tim",  //Mom_ArcMain_Down
		"upa0.tim",    //Mom_ArcMain_Up
		"upa1.tim",    //Mom_ArcMain_Up
		"righta0.tim", //Mom_ArcMain_Right
		"righta1.tim", //Mom_ArcMain_Right
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
