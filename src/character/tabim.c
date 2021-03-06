/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "tabim.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//tabim character structure
enum
{
	tabim_ArcMain_Idle0,
	tabim_ArcMain_Idle1,
	tabim_ArcMain_Idle2,
	tabim_ArcMain_Idle3,
	tabim_ArcMain_Left,
	tabim_ArcMain_Left1,
	tabim_ArcMain_Left2,
	tabim_ArcMain_Left3,
	tabim_ArcMain_Down,
	tabim_ArcMain_Down1,
	tabim_ArcMain_Down2,
	tabim_ArcMain_Down3,
	tabim_ArcMain_Up,
	tabim_ArcMain_Up1,
	tabim_ArcMain_Up2,
	tabim_ArcMain_Up3,
	tabim_ArcMain_Right,
	tabim_ArcMain_Right1,
	tabim_ArcMain_Right2,
	tabim_ArcMain_Right3,

	
	tabim_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[tabim_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_tabim;

//tabim character definitions
static const CharFrame char_tabim_frame[] = {
	{tabim_ArcMain_Idle0, {  0,   10, 127, 182}, { 51, 182}},
	{tabim_ArcMain_Idle0, {127,   15, 129, 178}, { 52, 178}},
	{tabim_ArcMain_Idle1, {  0,   15, 128, 177}, { 52, 177}},
	{tabim_ArcMain_Idle1, {128 ,  14, 128, 178}, { 51, 178}},
	{tabim_ArcMain_Idle2, {  0,   14, 128, 178}, { 51, 178}},
	{tabim_ArcMain_Idle2, {128,   13, 128, 179}, { 50, 179}},
	{tabim_ArcMain_Idle3, {  0,   12, 128, 180}, { 51, 180}},
	{tabim_ArcMain_Idle3, {128,   10, 128, 182}, { 51, 182}},
	
	{tabim_ArcMain_Left, {  18,   21,   172, 175}, { 64, 175}},
	{tabim_ArcMain_Left1,{  18,   16,   170, 176}, { 64, 176}}, 
	{tabim_ArcMain_Left2,{  23,   16,   169, 180}, { 63, 180}},
	{tabim_ArcMain_Left3,{  22,   16,   170, 180}, { 63, 180}},

	{tabim_ArcMain_Down,  {  0,   0, 131, 167}, { 42, 167}},
	{tabim_ArcMain_Down1, {  3,   3, 129, 166}, { 42, 166}}, 
	{tabim_ArcMain_Down2, {  3,   3, 129, 166}, { 42, 162}}, 
	{tabim_ArcMain_Down3, {  3,   1, 132, 167}, { 42, 167}}, 
	
	{tabim_ArcMain_Up,  {0,   19, 137, 205}, { 52, 205}},
	{tabim_ArcMain_Up1, {8,   0, 137, 205}, { 52, 205}}, 
	{tabim_ArcMain_Up2, {1,   2, 135, 203}, { 52,  203}}, 
	{tabim_ArcMain_Up3, {1,   3, 134, 203}, { 51, 203}}, 
	
	{tabim_ArcMain_Right,  {  35,   23, 197, 178}, { 51, 178}}, 
	{tabim_ArcMain_Right1, {  41,   23, 194, 178}, { 54, 178}},
	{tabim_ArcMain_Right2, {  40,   23, 191, 178}, { 53, 178}}, 
	{tabim_ArcMain_Right3, {  46,   22, 189, 178}, { 53, 178}},
};

static const Animation char_tabim_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4, 5, 6, 7, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
	{2, (const u8[]){ 8, 9, 10, 11, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 12,  13,  14, 15, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 16,  17, 18, 19, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 20, 21, 22, 23, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//tabim character functions
void Char_tabim_SetFrame(void *user, u8 frame)
{
	Char_tabim *this = (Char_tabim*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_tabim_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_tabim_Tick(Character *character)
{
	Char_tabim *this = (Char_tabim*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_tabim_SetFrame);
	Character_Draw(character, &this->tex, &char_tabim_frame[this->frame]);
}

void Char_tabim_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_tabim_Free(Character *character)
{
	Char_tabim *this = (Char_tabim*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_tabim_New(fixed_t x, fixed_t y)
{
	//Allocate tabim object
	Char_tabim *this = Mem_Alloc(sizeof(Char_tabim));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_tabim_New] Failed to allocate tabim object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_tabim_Tick;
	this->character.set_anim = Char_tabim_SetAnim;
	this->character.free = Char_tabim_Free;
	
	Animatable_Init(&this->character.animatable, char_tabim_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 8;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-90,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\TABIM.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //tabim_ArcMain_Idle0
		"idle1.tim", //tabim_ArcMain_Idle1
		"idle2.tim", //tabim_ArcMain_Idle1
		"idle3.tim", //tabim_ArcMain_Idle1
		"left.tim",  //tabim_ArcMain_Left
		"left1.tim",  //tabim_ArcMain_Left
		"left2.tim",  //tabim_ArcMain_Left
		"left3.tim",  //tabim_ArcMain_Left
		"down.tim",  //tabim_ArcMain_Down
		"down1.tim",  //tabim_ArcMain_Down
		"down2.tim",  //tabim_ArcMain_Down
		"down3.tim",  //tabim_ArcMain_Down
		"up.tim",    //tabim_ArcMain_Up
		"up1.tim",    //tabim_ArcMain_Up
		"up2.tim",    //tabim_ArcMain_Up
		"up3.tim",    //tabim_ArcMain_Up
		"right.tim", //tabim_ArcMain_Right
		"right1.tim", //tabim_ArcMain_Right
		"right2.tim", //tabim_ArcMain_Right
		"right3.tim", //tabim_ArcMain_Right
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
