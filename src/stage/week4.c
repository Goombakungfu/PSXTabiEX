/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "week4.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Week 4 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	//Textures
	IO_Data arc_hench, arc_hench_ptr[2];
	
	Gfx_Tex tex_back1; //bg
	Gfx_Tex tex_back2; //tables n shit
	Gfx_Tex tex_back3; //shit without tables
	Gfx_Tex tex_sticks; //fg sticks
	Gfx_Tex tex_speaker; //speeeeeeeeeeeekr
	
	//Henchmen state
	Gfx_Tex tex_hench;
	u8 hench_frame, hench_tex_id;
	
	Animatable hench_animatable;
} Back_Week4;

//Henchmen animation and rects
static const CharFrame henchmen_frame[10] = {
	{0, {  0,  0,   164,  118}, { 20,  110}}, //1 left 2
	{0, {  0,  118, 164,  117}, { 8,   110}}, //2 left 3
	{1, {  0,  0,   140,  114}, { 26,  114}}, //3 left 4
	{1, {  0,  114, 140,  88},  { 18,  88}}, //4 left 5
	
	{2, {  0,  0,   164,  91},  { 14, 91}}, //5 right 1
	{2, {  0,  91,  164,  93},  { 31, 91}}, //6 right 2
	{3, {  0,  0,   174,  104}, { 31, 104}}, //7 right 3
	{3, {  0,  104, 176,  112}, { 19, 104}}, //8 right 4
};

static const Animation henchmen_anim[] = {
	{2, (const u8[]){0, 1, 2, 3, 4, 5, 6, 7, ASCR_BACK, 0}}, //Left
};

//Henchmen functions
void Week4_Henchmen_SetFrame(void *user, u8 frame)
{
	Back_Week4 *this = (Back_Week4*)user;
	
	//Check if this is a new frame
	if (frame != this->hench_frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &henchmen_frame[this->hench_frame = frame];
		if (cframe->tex != this->hench_tex_id)
			Gfx_LoadTex(&this->tex_hench, this->arc_hench_ptr[this->hench_tex_id = cframe->tex], 0);
	}
}

void Week4_Henchmen_Draw(Back_Week4 *this, fixed_t x, fixed_t y)
{
	//Draw character
	const CharFrame *cframe = &henchmen_frame[this->hench_frame];
	
	fixed_t ox = x - ((fixed_t)cframe->off[0] << FIXED_SHIFT);
	fixed_t oy = y - ((fixed_t)cframe->off[1] << FIXED_SHIFT);
	
	RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
	RECT_FIXED dst = {ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
	Stage_DrawTex(&this->tex_hench, &src, &dst, stage.camera.bzoom);
}


void Back_Week4_DrawFG(StageBack *back)
{
	Back_Week4 *this = (Back_Week4*)back;
	fixed_t fx, fy;

	fx = stage.camera.x;
	fy = stage.camera.y;
	
	//draw fg sticks
	RECT sticks_src = {0, 0, 256, 256};
	RECT_FIXED sticks_dst = {
		FIXED_DEC(-165 - SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-140,1) - fy,
		FIXED_DEC(550 + SCREEN_WIDEOADD,1),
		FIXED_DEC(260,1)
	};
	
	Stage_DrawTex(&this->tex_sticks, &sticks_src, &sticks_dst, stage.camera.bzoom);
}

void Back_Week4_DrawMD(StageBack *back)
{
	Back_Week4 *this = (Back_Week4*)back;
	
	fixed_t fx, fy;
	
}

void Back_Week4_DrawBG(StageBack *back)
{
	Back_Week4 *this = (Back_Week4*)back;
	
	fixed_t fx, fy;

	//Draw tables n shit
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT fg_src = {0, 0, 256, 256};
	RECT_FIXED fg_dst = {
		FIXED_DEC(-165 - SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-140,1) - fy,
		FIXED_DEC(550 + SCREEN_WIDEOADD,1),
		FIXED_DEC(260,1)
	};
	
	Stage_DrawTex(&this->tex_back2, &fg_src, &fg_dst, stage.camera.bzoom);

	//draw speaker
	fx = stage.camera.x;
	fy = stage.camera.y;

	RECT speaker_src = {0, 0, 216, 88};
	RECT_FIXED speaker_dst = {
		FIXED_DEC(0 - SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-30,1) - fy,
		FIXED_DEC(216 + SCREEN_WIDEOADD,1),
		FIXED_DEC(88,1)
	};
	
	Stage_DrawTex(&this->tex_speaker, &speaker_src, &speaker_dst, stage.camera.bzoom);
	
	//Animate and draw henchmen
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		switch (stage.song_step & 3)
		{
			case 1:
				Animatable_SetAnim(&this->hench_animatable, 0);
				break;
		}
	}
	Animatable_Animate(&this->hench_animatable, (void*)this, Week4_Henchmen_SetFrame);
	Week4_Henchmen_Draw(this, FIXED_DEC(-170,1) - fx, FIXED_DEC(20,1) - fy);
	Week4_Henchmen_Draw(this,  FIXED_DEC(-70,1) - fx, FIXED_DEC(30,1) - fy);
	Week4_Henchmen_Draw(this, FIXED_DEC(190,1) - fx, FIXED_DEC(30,1) - fy);
	Week4_Henchmen_Draw(this, FIXED_DEC(310,1) - fx, FIXED_DEC(30,1) - fy);
	


	//Draw shit without tables
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT shid_src = {0, 0, 256, 256};
	RECT_FIXED shid_dst = {
		FIXED_DEC(-165 - SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-140,1) - fy,
		FIXED_DEC(550 + SCREEN_WIDEOADD,1),
		FIXED_DEC(260,1)
	};
	
	Stage_DrawTex(&this->tex_back3, &shid_src, &shid_dst, stage.camera.bzoom);
	
	//draw fire thing behind wall layer
	Week4_Henchmen_Draw(this,  FIXED_DEC(-20,1) - fx, FIXED_DEC(0,1) - fy);
	Week4_Henchmen_Draw(this,  FIXED_DEC(140,1) - fx, FIXED_DEC(0,1) - fy);
	
	//Draw bg
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT bg_src = {0, 0, 256, 256};
	RECT_FIXED bg_dst = {
		FIXED_DEC(-165 - SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-140,1) - fy,
		FIXED_DEC(550 + SCREEN_WIDEOADD,1),
		FIXED_DEC(260,1)
	};
	
	Stage_DrawTex(&this->tex_back1, &bg_src, &bg_dst, stage.camera.bzoom);
}

void Back_Week4_Free(StageBack *back)
{
	Back_Week4 *this = (Back_Week4*)back;
	
	//Free henchmen archive
	Mem_Free(this->arc_hench);
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Week4_New(void)
{
	//Allocate background structure
	Back_Week4 *this = (Back_Week4*)Mem_Alloc(sizeof(Back_Week4));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_Week4_DrawFG;
	this->back.draw_md = Back_Week4_DrawMD;
	this->back.draw_bg = Back_Week4_DrawBG;
	this->back.free = Back_Week4_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\WEEK4\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);
	Gfx_LoadTex(&this->tex_back3, Archive_Find(arc_back, "back3.tim"), 0);
	Gfx_LoadTex(&this->tex_sticks, Archive_Find(arc_back, "sticks.tim"), 0);
	Gfx_LoadTex(&this->tex_speaker, Archive_Find(arc_back, "speaker.tim"), 0);
	Mem_Free(arc_back);
	
	//Load henchmen textures
	this->arc_hench = IO_Read("\\WEEK4\\HENCH.ARC;1");
	this->arc_hench_ptr[0] = Archive_Find(this->arc_hench, "hench1.tim");
	this->arc_hench_ptr[1] = Archive_Find(this->arc_hench, "hench2.tim");
	this->arc_hench_ptr[2] = Archive_Find(this->arc_hench, "hench3.tim");
	this->arc_hench_ptr[3] = Archive_Find(this->arc_hench, "hench4.tim");

	
	//Initialize henchmen state
	Animatable_Init(&this->hench_animatable, henchmen_anim);
	Animatable_SetAnim(&this->hench_animatable, 0);
	this->hench_frame = this->hench_tex_id = 0xFF; //Force art load
	
	return (StageBack*)this;
}
