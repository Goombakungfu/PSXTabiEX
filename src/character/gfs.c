/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "gfs.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Christmas Parents structure
enum
{
	gfs_ArcMain_Idle0,
	gfs_ArcMain_Idle1,
	gfs_ArcMain_Idle2,
	gfs_ArcMain_Idle3,
	gfs_ArcMain_LeftA0,
	gfs_ArcMain_LeftA1,
	gfs_ArcMain_LeftB0,
	gfs_ArcMain_LeftB1,
	gfs_ArcMain_DownA0,
	gfs_ArcMain_DownA1,
	gfs_ArcMain_DownB0,
	gfs_ArcMain_DownB1,
	gfs_ArcMain_UpA0,
	gfs_ArcMain_UpA1,
	gfs_ArcMain_UpB0,
	gfs_ArcMain_UpB1,
	gfs_ArcMain_RightA0,
	gfs_ArcMain_RightA1,
	gfs_ArcMain_RightB0,
	gfs_ArcMain_RightB1,
	
	gfs_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[gfs_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_gfs;

//Christmas Parents definitions
static const CharFrame char_gfs_frame[] = {
	{gfs_ArcMain_Idle0, {0,    0,   83, 106}, {29, 106}},
	{gfs_ArcMain_Idle0, {83,   0,   79, 107}, {28, 107}},
	{gfs_ArcMain_Idle0, {162,  0,   77, 107}, {28, 107}}, 
	{gfs_ArcMain_Idle0, {0,    106, 79, 108}, {29, 108}}, 
	{gfs_ArcMain_Idle0, {79,   107, 81, 110}, {28, 110}},
	{gfs_ArcMain_Idle0, {160,  107, 79, 111}, {28, 111}},
	{gfs_ArcMain_Idle1, {0,    0,   80, 111}, {29, 111}}, 
	{gfs_ArcMain_Idle1, {0,    0,   79, 111}, {28, 111}}, 
};

static const Animation char_gfs_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  6,  7, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
};

//Christmas Parents functions
void Char_gfs_SetFrame(void *user, u8 frame)
{
	Char_gfs *this = (Char_gfs*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_gfs_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_gfs_Tick(Character *character)
{
	Char_gfs *this = (Char_gfs*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_gfs_SetFrame);
	Character_Draw(character, &this->tex, &char_gfs_frame[this->frame]);
}

void Char_gfs_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_gfs_Free(Character *character)
{
	Char_gfs *this = (Char_gfs*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_gfs_New(fixed_t x, fixed_t y)
{
	//Allocate Christmas Parents object
	Char_gfs *this = Mem_Alloc(sizeof(Char_gfs));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_gfs_New] Failed to allocate Christmas Parents object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_gfs_Tick;
	this->character.set_anim = Char_gfs_SetAnim;
	this->character.free = Char_gfs_Free;
	
	Animatable_Init(&this->character.animatable, char_gfs_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 5;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-100,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\GFS.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",   //gfs_ArcMain_Idle0
		"idle1.tim",   //gfs_ArcMain_Idle1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
