/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "heart.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Christmas Parents structure
enum
{
	heart_ArcMain_Idle0,
	heart_ArcMain_Idle1,
	heart_ArcMain_Idle2,
	heart_ArcMain_Idle3,
	heart_ArcMain_Idle4,
	heart_ArcMain_Idle5,
	heart_ArcMain_Idle6,
	heart_ArcMain_Idle7,
	
	heart_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[heart_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_heart;

//Christmas Parents definitions
static const CharFrame char_heart_frame[] = {
	{heart_ArcMain_Idle0, {0,    0,   256, 256}, {29, 106}},
	{heart_ArcMain_Idle1, {0,    0,   256, 256}, {29, 106}},
	{heart_ArcMain_Idle2, {0,    0,   256, 256}, {29, 106}},
	{heart_ArcMain_Idle3, {0,    0,   256, 256}, {29, 106}},
	{heart_ArcMain_Idle4, {0,    0,   256, 256}, {29, 106}},
	{heart_ArcMain_Idle5, {0,    0,   256, 256}, {29, 106}},
	{heart_ArcMain_Idle6, {0,    0,   256, 256}, {29, 106}},
	{heart_ArcMain_Idle7, {0,    0,   256, 256}, {29, 106}},
};

static const Animation char_heart_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  6,  7, ASCR_BACK, 1}}, //CharAnim_Idle
};

//Christmas Parents functions
void Char_heart_SetFrame(void *user, u8 frame)
{
	Char_heart *this = (Char_heart*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_heart_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_heart_Tick(Character *character)
{
	Char_heart *this = (Char_heart*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_heart_SetFrame);
	Character_Draw(character, &this->tex, &char_heart_frame[this->frame]);
}

void Char_heart_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_heart_Free(Character *character)
{
	Char_heart *this = (Char_heart*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_heart_New(fixed_t x, fixed_t y)
{
	//Allocate Christmas Parents object
	Char_heart *this = Mem_Alloc(sizeof(Char_heart));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_heart_New] Failed to allocate Christmas Parents object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_heart_Tick;
	this->character.set_anim = Char_heart_SetAnim;
	this->character.free = Char_heart_Free;
	
	Animatable_Init(&this->character.animatable, char_heart_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 5;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-100,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\HEART.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",   //heart_ArcMain_Idle0
		"idle1.tim",   //heart_ArcMain_Idle1
		"idle2.tim",   //heart_ArcMain_Idle1
		"idle3.tim",   //heart_ArcMain_Idle1
		"idle4.tim",   //heart_ArcMain_Idle1
		"idle5.tim",   //heart_ArcMain_Idle1
		"idle6.tim",   //heart_ArcMain_Idle1
		"idle7.tim",   //heart_ArcMain_Idle1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
