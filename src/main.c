//////////////////////////////////////////
// SHADOW DANCER PROJECT by GameDevBoss //
// Thanks to:                           //
// Edmo Caldas (music)                  //
// Ray Castello (gfx, art)              //
// Vubidugil (Ninja original sprites)   //
// Gabriel Pyron (Gfx, art)             //
//////////////////////////////////////////

#include "genesis.h"
#include "gfx.h"
#include "sprite.h"
#include "sound.h"

//--- VARIABLES ---//

#define HOW_FAR_TO_LEFT_BEFORE_CAMERA_MOVES 152
#define HOW_FAR_TO_RIGHT_BEFORE_CAMERA_MOVES 153 
#define HOW_FAR_TO_TOP_BEFORE_CAMERA_MOVES 115 
#define HOW_FAR_TO_BOTTOM_BEFORE_CAMERA_MOVES 116
#define HORIZONTAL_RESOLUTION 320
#define VERTICAL_RESOLUTION 224
#define P1_SFX 65
#define P2_SFX 66
#define CPU_SFX 67
#define SHURIKEN_SPEED 4
#define MAX_SHURIKENS 4
#define WALK_SPEED 2
#define RELEASE_ANIM_TIMER 10
#define MAX_SMOKE_FX 3
#define MAX_BOMBS 10
#define TOTAL_MAP_BOXES 72
#define MAX_ENEMYS 20

//Global variables
u8   i;                //General purpose integer variable
u32  gFrames = 0;      //Frame Counter
u32  gScore = 0;
u8   gLives = 2;
u16  gRoom = 3;        //Game 'Room' (Menu, In game, etc)
u16  gDescompressionExit=0; //Used in the decompression room
u16  gInd_tileset;     //Variable used to load background data
Map* level_map;        //BGA map
Map* level_mapb;       //BGB map
u16 gBG_Width;         //Map Size X in pixels
u16 gBG_Height;        //Map Size Y in pixels
int current_camera_x;  //current camera position X
int current_camera_y;  //current camera position Y
int new_camera_x; 	   //new camera position X
int new_camera_y; 	   //new camera position Y
bool gPauseSystem = 0; //Pause System
bool gEnableMove = 1;  //Disables player inputs at specific times
u8 gPing2  = 1;        //Variavel que alterna entre 0 e 1 constantemente
u8 gPing10 = 9;        //Variavel que alterna entre 0 ; 1 ; 2 ... 7 ; 8 ; 9 constantemente
u8 gPing20 = 19;
u8 gPing60 = 59;
s16 gBGScrollingY;
u8 gGravity = 1;
u8 gMapID = 1;  //global MapID
u8 gSubMapID = 1;  //global subMapID used in dynamic loading
u16 gItemID;
s16 collisionMatrix[TOTAL_MAP_BOXES][5]; //MAP collision (id retangle), (x1,y1,x2,y2,shinobi plane)
s16 collisionMatrixB[TOTAL_MAP_BOXES/4][5]; //MAP collision (id retangle), (x1,y1,x2,y2,type movement 1=down, 2-up or down, 3-up)
s16 totalMapCollisionBoxes; //total MAP boxes that you can collide
s16 totalMapCollisionBoxesB; //total MAP boxes that you can collide
Sprite* hudElement[20];
bool gReloadPal = FALSE;
u8 gRelogioTimer = 60;
u8 gRelMinutos = 2;
u8 gRelSegundos = 59;

//Player Struct
struct PlayerDEF {
	Sprite* sprite;     //Player Sprite
	u16 state;          //Controls the state (animation) of the Player
	u8 id;              //select your character
	u8 totalShurikens;
	s8  dir;            //Direction
	s16 impulseX;       //self explanatory
	s16 impulseY;       //self explanatory
	s16 velocityX;      //self explanatory
	s16 velocityY;      //self explanatory
	s8 extraImpulse;
	u8 jumpTimer;       //Controls the height of the jump by applying acceleration at predefined time intervals
	bool groundSensor;    //Used to check if the character is standing on a platform or not
	s16 x;              //X
	s16 y;              //Y
	s32 x_prev;         //Previous X Position
	s32 y_prev;         //Previous Y Position
	u8  DM;             //Directional Movement
	u8  axisX;          //Pivot Point X
	u8  axisY;          //Pivot Point Y
	u16 frameTimeAtual; //Current time of current animation frame
	u16 frameTimeTotal; //Total time of current animation frame
	u16 animFrame;      //Current animation frame
	u16 totalAnimationFrames; //Total number of frames in this state (animation)
	u32 animTimeAtual;  //Current animation time
	u32 animTimeTotal;  //Total time of current animation
	u16 animRow;        //Line on which given animation is
	u16 dataAnim[60];   //Total frames available for each state (animation)
	u8 releaseAnim;
	bool planeChangerSensor;
	bool planeChangerUPReady;
	bool planeChangerDOWNReady;
	u8 playerLayer;
	//JOYSTICK
	u8 key_JOY_status[12];
	u8 key_JOY_UP_status; u8 key_JOY_DOWN_status; u8 key_JOY_LEFT_status; u8 key_JOY_RIGHT_status;
	u8 key_JOY_A_status; u8 key_JOY_B_status; u8 key_JOY_C_status;
	u8 key_JOY_X_status; u8 key_JOY_Y_status; u8 key_JOY_Z_status;
	u8 key_JOY_START_status; u8 key_JOY_MODE_status;
}; struct PlayerDEF P[3];

//Anim Object
struct AnimObjDEF {
	Sprite* sprite;     //Sprite
	s16 x;              //X
	s16 y;              //Y
	u8  axisX;          //Pivot Point X
	u8  axisY;          //Pivot Point Y
}; struct AnimObjDEF AnimObj[5];

struct fxobjsDEF {
	Sprite* sprite;
    u16 x;
    u16 y;
	u8 dir;
	u8 life;
	bool active;
}; struct fxobjsDEF fxobjs[MAX_SMOKE_FX];

struct shurikenDeadDEF {
	Sprite* sprite;
    u16 x;
    u16 y;
	u8 dir;
	u8 life;
	bool active;
}; struct shurikenDeadDEF obj_shurikendead[MAX_SHURIKENS];

struct bombsDEF {
	Sprite* sprite;
    u16 x;
    u16 y;
	u8 dir;
	u8 life;
	bool active;
}; struct bombsDEF bombs[MAX_BOMBS];

struct objshurikensDEF {
	Sprite* sprite;
    u16 x;
    u16 y;
	u8 dir;
	u8 life;
	u8 myLayer;
	bool active;
}; struct objshurikensDEF obj_shuriken[MAX_SHURIKENS];

struct enemysDEF {
	Sprite* sprite;
    u16 x;
    u16 y;
	s8 dir;
	u8 life;
	u8 layer;
	u8 state;
	u8 type;
	u8 myTimer;
	bool active;
}; struct enemysDEF enemys[MAX_ENEMYS];

struct seedDEF {
    u16 x;
    u16 y;
	u8 type;
	u8 layer;
	bool active;
}; struct seedDEF seed[25];


//--- FUNCTIONS ---//

void PLAYER_STATE(u8 Player, u16 State);
void INPUT_SYSTEM();
void FSM();
void ANIMATION();
void PHYSICS();
void JUMP_INIT(u8 player);
void COLLISION_HANDLING(u8 player, s32 RectX1, s32 RectY1, s32 RectX2, s32 RectY2);
void PLAY_SND(u16 sndNumber);
void CAMERA();
void CLEAR_VDP();
bool CHECK_COLLISION(s32 R1x1, s32 R1y1, s32 R1x2, s32 R1y2, s32 R2x1, s32 R2y1, s32 R2x2, s32 R2y2);
void CREATE_STAGE(s16 mapID);
void DYNAMIC_LOADING_TILESET();
void FXOBJECT(s32 x, s32 y, u8 dir);
void UPDATE_FXOBJS();
void DRAW_FXOBJS();
void BOMB(s32 x, s32 y, u8 dir);
void UPDATE_BOMBS();
void DRAW_BOMBS();
void OBJSHURIKENS(s32 x, s32 y, u8 dir);
void UPDATE_OBJSHURIKENS();
void DRAW_OBJSHURIKENS();
void OBJSHURIKENSDEAD(s32 x, s32 y, u8 dir);
void UPDATE_OBJSHURIKENSDEAD();
void DRAW_OBJSHURIKENSDEAD();
void ENEMYS(s32 x, s32 y, u8 type, s8 dir, u8 layer);
void UPDATE_ENEMYS();
void DRAW_ENEMYS();
void OBJ_SEED(s32 x, s32 y, u8 type, u8 layer);
void UPDATE_SEED();
void UPDATE_SCORE(u16 score);

int main(bool hardReset) ///MAIN///
{
    //Init VDP (Video Display Processor)
	SYS_disableInts();
	 VDP_init();                   //Initializes the VDP (Video Display Processor)
	 VDP_setScreenWidth320();      //Default resolution of 320x224 (Width)
	 VDP_setScreenHeight224();     //Default resolution of 320x224 (Height)
	 VDP_setPlaneSize(64,32,TRUE); //Recommended for large BGs //old: SGDK 1.65
	 VDP_setTextPlane(BG_A);       //Texts will be drawn on BG_A
	 VDP_setTextPalette(PAL0);     //Texts will be drawn with the latest PAL0 color
	 SPR_initEx(330);             //420 is the default value of SGDK 1.80
	 VDP_setBackgroundColor(0);    //Range 0-63 //4 16 color palettes = 64 colors
	SYS_enableInts();
	
	if(!hardReset){ SYS_hardReset(); } //Prevent reset bug

	//--- MAIN LOOP ---//
    while(TRUE)
    {
		gFrames++; 
		if(gPauseSystem==0)
		{ 
			if(gPing2  ==  1){ gPing2 = -1; } gPing2++;  //var 'gPing2'  (50%) variacao: 0 ; 1
			if(gPing10 ==  9){ gPing10= -1; } gPing10++; //var 'gPing10' (10%) variacao: 0 ; 1 ; 2 ; 3 ; 4 ; 5 ; 6 ; 7 ; 8 ; 9
			if(gPing20 == 19){ gPing20= -1; } gPing20++; //var 'gPing20' 
			if(gPing60 == 59){ gPing60= -1; } gPing60++; //var 'gPing60' 
		}
		
		INPUT_SYSTEM();
		
		if(gRoom==10) //ALL SYSTENS; GAMEPLAY
		{
			FSM();
			ANIMATION();
			PHYSICS();
			if(gFrames>1){CAMERA();}
			UPDATE_FXOBJS();
			DRAW_FXOBJS();
			UPDATE_BOMBS();
			DRAW_BOMBS();
			UPDATE_OBJSHURIKENS();
			DRAW_OBJSHURIKENS();
			UPDATE_OBJSHURIKENSDEAD();
			DRAW_OBJSHURIKENSDEAD();
			UPDATE_ENEMYS();
			DRAW_ENEMYS();
			DYNAMIC_LOADING_TILESET();
			UPDATE_SEED();
			UPDATE_SCORE(1);
			if(hudElement[1]){ SPR_setAnimAndFrame(hudElement[1], 0, gLives); }
			
			//---relogio----------------------------------------------------------------------------------------
			bool atualizaSegundos = FALSE;
			bool atualizaMinutos = FALSE;
			if(gRelogioTimer>0)
			{
				gRelogioTimer--;
			}else{
				gRelogioTimer=60;
				if(gRelSegundos>0)
				{
					gRelSegundos--;
					atualizaSegundos = TRUE;
				}else{
					if(gRelMinutos>0)
					{
						gRelMinutos--;
						atualizaMinutos = TRUE;
						gRelSegundos = 59;
						atualizaSegundos = TRUE;
					}else{
						//time over 0:00
					}
				}
			}
			if(atualizaMinutos==TRUE)
			{
				if(hudElement[ 7]){ SPR_setAnimAndFrame(hudElement[ 7], 0, gRelMinutos); }
			}
			if(atualizaSegundos==TRUE)
			{
				u8 a = 0;
				u8 b = 0;
				     if(gRelSegundos== 0){ a=0; b=0; }
				     if(gRelSegundos>  0 && gRelSegundos<10){ a=0; }
				else if(gRelSegundos>=10 && gRelSegundos<20){ a=1; }
				else if(gRelSegundos>=20 && gRelSegundos<30){ a=2; }
				else if(gRelSegundos>=30 && gRelSegundos<40){ a=3; }
				else if(gRelSegundos>=40 && gRelSegundos<50){ a=4; }
				else if(gRelSegundos>=50 && gRelSegundos<60){ a=5; }
				if(gRelSegundos>0){ b = gRelSegundos-(a*10); }
				if(hudElement[ 9]){ SPR_setAnimAndFrame(hudElement[ 9], 0, a); } //decimal segundos
				if(hudElement[10]){ SPR_setAnimAndFrame(hudElement[10], 0, b); } //segundos
			}
			//---relogio end ----------------------------------------------------------------------------------------
			
		}
		
		//---fake reset; return to level select--------------------------------------------------------------
		if(P[1].key_JOY_START_status==1 && gRoom==10 && gFrames>15)
		{
			gFrames=0;
			if(level_map){MAP_scrollTo(level_map , 0, 0);}
			if(level_mapb){MAP_scrollTo(level_mapb, 0, 0);}
			//CLEAR_VDP();
			gRoom=9;
			if(level_map ){ MEM_free(level_map) ; level_map  = NULL; }
			if(level_mapb){ MEM_free(level_mapb); level_mapb = NULL; }
			if(P[0].sprite){ SPR_releaseSprite(P[0].sprite); P[0].sprite = NULL; } //clear memory
			if(P[1].sprite){ SPR_releaseSprite(P[1].sprite); P[1].sprite = NULL; } //clear memory
			if(P[2].sprite){ SPR_releaseSprite(P[2].sprite); P[2].sprite = NULL; } //clear memory
			for (int i = 0; i < MAX_SMOKE_FX ; i++){ fxobjs[i].active           = FALSE; }
			for (int i = 0; i < MAX_BOMBS    ; i++){ bombs[i].active            = FALSE; }
			for (int i = 0; i < MAX_SHURIKENS; i++){ obj_shuriken[i].active     = FALSE; }
			for (int i = 0; i < MAX_SHURIKENS; i++){ obj_shurikendead[i].active = FALSE; }
			for (int i = 0; i < MAX_ENEMYS   ; i++){ enemys[i].active           = FALSE; }
			for (int i = 0; i <= 16; i++){ if(hudElement[i]){ SPR_releaseSprite(hudElement[i]); hudElement[i] = NULL; } }
		}
		//---fake reset; return to level select--------------------------------------------------------------
		
		P[1].x_prev=P[1].x; //update Previous X Position
		P[1].y_prev=P[1].y; //update Previous Y Position 
		
		/*DESCOMPRESSION*/
		if(gRoom==0)
		{
			if(gFrames==15)
			{
				CLEAR_VDP();
			}
			if(gFrames==20)
			{
				if (P[1].sprite){ SPR_releaseSprite(P[1].sprite); P[1].sprite = NULL; }
				gRoom=gDescompressionExit;
				gFrames=0;
			}
		}
		
		/*DOG SCREEN*/
		if(gRoom==3)
		{
			if(gFrames==1)
			{
				//if(level_map ){ MEM_free(level_map) ; level_map  = NULL; }
				if(level_mapb){ MEM_free(level_mapb); level_mapb = NULL; }
				
				gInd_tileset=1;
				//BGB
				VDP_loadTileSet(&intro1_bga_tileset, gInd_tileset, DMA);
				level_mapb = MAP_create(&intro1_bga_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset)); 
				//PAL_setPalette(PAL0, intro1_bga_pal.palette->data,DMA);
				gInd_tileset += intro1_bga_tileset.numTile;
				MAP_scrollTo(level_mapb, 0, 0);
				
			}
			
			if(gFrames==2){ PAL_setPalette(PAL0, intro1_bga_pal.palette->data,DMA); }
			if(gFrames==10){ XGM_setPCM(P1_SFX, snd_rosnado, sizeof(snd_rosnado)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(gFrames==10+6*1){MAP_scrollTo(level_mapb,     0, 0);}
			if(gFrames==10+6*2){MAP_scrollTo(level_mapb, 320*1, 0);}
			if(gFrames==10+6*3){MAP_scrollTo(level_mapb, 320*2, 0);}
			if(gFrames==10+6*4){MAP_scrollTo(level_mapb, 320*3, 0);}
			
			if(gFrames== 99){ PAL_setColors(0, palette_black, 64, DMA); }
			if(gFrames==100)
			{
				if(level_map ){ MEM_free(level_map) ; level_map  = NULL; }
				if(level_mapb){ MEM_free(level_mapb); level_mapb = NULL; }
				
				gInd_tileset=1;
				//BGB
				VDP_loadTileSet(&intro1b_bga_tileset, gInd_tileset, DMA);
				level_mapb = MAP_create(&intro1b_bga_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset)); 
				gInd_tileset += intro1b_bga_tileset.numTile;
				MAP_scrollTo(level_mapb, 0, 0);
			}
			
			if(gFrames==102){ PAL_setPalette(PAL0, intro1_bga_pal.palette->data,DMA); }
			if(gFrames==145){MAP_scrollTo(level_mapb,     0, 0);}
			
			if(gFrames==159){ XGM_setPCM(P1_SFX, snd_latido, sizeof(snd_latido)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(gFrames==160){MAP_scrollTo(level_mapb, 320*1, 0);}
			if(gFrames==175){MAP_scrollTo(level_mapb,     0, 0);}
			
			if(gFrames==189){ XGM_setPCM(P1_SFX, snd_latido, sizeof(snd_latido)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(gFrames==190){MAP_scrollTo(level_mapb, 320*1, 0);}
			if(gFrames==210){MAP_scrollTo(level_mapb,     0, 0);}
			
			if(gFrames==249){ XGM_setPCM(P1_SFX, snd_latido, sizeof(snd_latido)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			if(gFrames==250){MAP_scrollTo(level_mapb, 320*1, 0);}
			if(gFrames==260){MAP_scrollTo(level_mapb,     0, 0);}
			
			if(gFrames==300)
			{
				PAL_setColors(0, palette_black, 64, DMA);
			}
			
			if((gFrames>=30 && P[1].key_JOY_START_status==1) || (gFrames>=302))
			{
				MAP_scrollTo(level_mapb, 0, 0);
				CLEAR_VDP();
				
				XGM_setPCM(P1_SFX, snd_latido, sizeof(snd_latido)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); 
				
				//gRoom=0;
				//gDescompressionExit=5;
				
				gRoom=4;
				if(P[1].key_JOY_START_status==1){ gRoom=5; }
				gFrames=1;
			}
			
		}
		
		/*NINJA INTRO SCREEN*/
		if(gRoom==4)
		{
			if(gFrames==1)
			{
				gInd_tileset=1;
				gBGScrollingY=192;
				//BGB
				VDP_loadTileSet(&intro2_bgb_tileset, gInd_tileset, DMA);
				level_mapb = MAP_create(&intro2_bgb_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset)); 
				//PAL_setPalette(PAL0, intro2_bgb_pal.palette->data,DMA);
				gInd_tileset += intro2_bgb_tileset.numTile;
				MAP_scrollTo(level_mapb, 0, 0);
				
				AnimObj[0].sprite = SPR_addSpriteExSafe(&spr_txt_tgnr, -99, -99, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				AnimObj[0].x = -152;
				AnimObj[0].y = 16;
				SPR_setVRAMTileIndex(AnimObj[0].sprite, 1441); //define uma posicao especifica para o GFX na VRAM
				PAL_setPalette(PAL1, spr_txt_tgnr.palette->data,DMA);
				
				XGM_stopPlay();
				XGM_startPlay(title_ninja); 
				XGM_isPlaying();
				
			}
			
			if(gFrames>=210){ /*if(gPing2==0){ */if(AnimObj[0].x<16){ AnimObj[0].x++; } } //}
			
			if(gFrames==2){ PAL_setPalette(PAL0, intro2_bgb_pal.palette->data,DMA); }
			
			if(gFrames>=60){ if(gPing2==0){ if(gBGScrollingY>32){ gBGScrollingY--; } } }
			MAP_scrollTo(level_mapb, 0, gBGScrollingY);
			
			if((gFrames>=15 && P[1].key_JOY_START_status==1) || (gFrames>=60*8))
			{
				gBGScrollingY=0;
				MAP_scrollTo(level_mapb, 0, 0);
				
				if(AnimObj[0].sprite){ SPR_releaseSprite(AnimObj[0].sprite); AnimObj[0].sprite = NULL; } //clear memory
				
				//CLEAR_VDP();
				
				//gRoom=0;
				//gDescompressionExit=5;
				
				PAL_setColors(0, palette_black, 64, DMA);
				gRoom=5;
				gFrames=0;
			}
			
		}
		
		/*LOGO SCREEN*/
		if(gRoom==5)
		{
			if(gFrames==1)
			{
				gInd_tileset=1;
				gScore = 0;
				gLives = 2;
				//BGB
				VDP_loadTileSet(&bg_logo_bgb_tileset, gInd_tileset, DMA);
				level_mapb = MAP_create(&bg_logo_bgb_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset)); 
				//PAL_setPalette(PAL0, bg_logo_bgb_pal.palette->data,DMA);
				gInd_tileset += bg_logo_bgb_tileset.numTile;
				MAP_scrollTo(level_mapb, 0, 0);
				
				P[0].sprite = SPR_addSpriteExSafe(&spr_ninja_logo, 18, 72, TILE_ATTR(PAL2, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				if(P[0].sprite){ SPR_setVRAMTileIndex(P[0].sprite, 1441); } //define uma posicao especifica para o GFX na VRAM
				
				P[1].sprite = SPR_addSpriteExSafe(&spr_press_start, 88, 20, TILE_ATTR(PAL2, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				if(P[1].sprite){ SPR_setVRAMTileIndex(P[1].sprite, 1984); } //define uma posicao especifica para o GFX na VRAM
			}
			
			if(gFrames==4)
			{
				//BGA
				VDP_loadTileSet(&bg_logo_bga_tileset, gInd_tileset, DMA);
				level_map = MAP_create(&bg_logo_bga_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, gInd_tileset)); 
				//PAL_setPalette(PAL1, bg_logo_bga_pal.palette->data,DMA);
				gInd_tileset += bg_logo_bga_tileset.numTile;
				MAP_scrollTo(level_map, 0, 0);
			}
			
			if(P[0].sprite){ SPR_setPosition(P[0].sprite, 18, 72); }
			if(gPing60<=50)
			{
				if(P[1].sprite){ SPR_setPosition(P[1].sprite, 88, 20); }
			}else{
				if(P[1].sprite){ SPR_setPosition(P[1].sprite, 88, -16); }
			}
			
			if(gFrames==15)
			{
				PAL_setPalette(PAL0, bg_logo_bgb_pal.palette->data,DMA);
				PAL_setPalette(PAL1, bg_logo_bga_pal.palette->data,DMA);
				PAL_setPalette(PAL2, spr_ninja_logo .palette->data,DMA);
			}
			
			if(gFrames==20){ XGM_setPCM(P1_SFX, snd_credito, sizeof(snd_credito)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			
			//if(gPing10== 0){MAP_scrollTo(level_mapb, 0, 0);}
			//if(gPing10== 3){MAP_scrollTo(level_mapb, 320*1, 0);}
			//if(gPing10== 6){MAP_scrollTo(level_mapb, 320*2, 0);}
			//if(gPing10== 8){MAP_scrollTo(level_mapb, 320*3, 0);}
			
			if(gFrames>=6)
			{
				if(gPing20== 0){MAP_scrollTo(level_mapb,     0, 0);}
				if(gPing20== 5){MAP_scrollTo(level_mapb, 320*1, 0);}
				if(gPing20==10){MAP_scrollTo(level_mapb, 320*2, 0);}
				if(gPing20==15){MAP_scrollTo(level_mapb, 320*3, 0);}
			}
			
			if(gFrames>=60 && P[1].key_JOY_START_status==1)
			{
				MAP_scrollTo(level_mapb, 0, 0);
				if(P[0].sprite){ SPR_setPosition(P[0].sprite, 0, 0); }
				if(P[1].sprite){ SPR_setPosition(P[1].sprite, 0, 0); }
				
				if(P[0].sprite){ SPR_releaseSprite(P[0].sprite); P[0].sprite = NULL; } //clear memory
				if(P[1].sprite){ SPR_releaseSprite(P[1].sprite); P[1].sprite = NULL; } //clear memory
				//if(P[1].key_JOY_A_status>0){ P[1].id=2; }else{P[1].id=1;}
				CLEAR_VDP();
				//gRoom=0;
				//gDescompressionExit=10;
				gRoom=8;
				gFrames=1;
				gMapID=1;
				gSubMapID=1;
			}
			
		}
		
		/*CHAR SELECT*/
		if(gRoom==8)
		{
			if(gFrames==1)
			{
				PAL_setColors(0, palette_black, 64, DMA);
				
				gInd_tileset=1;
				//BGB
				VDP_loadTileSet(&bg_charsel_bgb_tileset, gInd_tileset, DMA);
				level_mapb = MAP_create(&bg_charsel_bgb_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset)); 
				//PAL_setPalette(PAL0, bg_charselbgb_pal.palette->data,DMA);
				gInd_tileset += bg_charsel_bgb_tileset.numTile;
				MAP_scrollTo(level_mapb, 0, 0);
				
				P[1].sprite = SPR_addSpriteExSafe(&spr_charsel_shinobi2,  64, 56, TILE_ATTR(PAL2, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				P[2].sprite = SPR_addSpriteExSafe(&spr_charsel_shinobi1, 184, 56, TILE_ATTR(PAL3, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				
				P[1].id=1;
			}
			
			if(gFrames==4)
			{
				P[0].sprite = SPR_addSpriteExSafe(&spr_char_sel_cursor_new_fx,  91, 197, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				SPR_setVRAMTileIndex(P[0].sprite, 1441); //define uma posicao especifica para o GFX na VRAM
				
				AnimObj[0].sprite = SPR_addSpriteExSafe(&spr_char_sel_titleA, 320, 224, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				AnimObj[1].sprite = SPR_addSpriteExSafe(&spr_char_sel_titleB, 320, 224, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				
				
			}
			
			if(gFrames==6)
			{
				XGM_stopPlay();
				XGM_startPlay(music_charsel); 
				XGM_isPlaying();
				AnimObj[2].sprite = SPR_addSpriteExSafe(&spr_char_sel_cursor_new_fx, 91-43, 197, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				SPR_setVRAMTileIndex(AnimObj[2].sprite, 1984); //define uma posicao especifica para o GFX na VRAM
			}
			
			SPR_setPosition(P[1].sprite,  64, 56);
			SPR_setPosition(P[2].sprite, 184, 56);
			
			SPR_setPosition(AnimObj[0].sprite,  60+4,  8);
			SPR_setPosition(AnimObj[1].sprite, 156+4,  8);
			
			//altera player
			if(P[1].key_JOY_LEFT_status >0){P[1].id=1;}
			if(P[1].key_JOY_RIGHT_status>0){P[1].id=2;}
			
			//posiciona o cursor
			if(P[1].id==1){ SPR_setPosition(P[0].sprite,  91-43,  197); if(AnimObj[2].sprite){SPR_setPosition(AnimObj[2].sprite, 91-43, 197);} }
			if(P[1].id==2){ SPR_setPosition(P[0].sprite, 211-43,  197); if(AnimObj[2].sprite){SPR_setPosition(AnimObj[2].sprite, 211-43, 197); } }
			
			//anima o bg
			if(gFrames>=6)
			{
				if(gPing20== 0){MAP_scrollTo(level_mapb,   0, 0);}
				if(gPing20== 5){MAP_scrollTo(level_mapb, 320, 0);}
				if(gPing20==10){MAP_scrollTo(level_mapb, 640, 0);}
				if(gPing20==15){MAP_scrollTo(level_mapb, 960, 0);}
			}
			
			if(gFrames==15)
			{
				PAL_setPalette(PAL0, bg_charsel_bgb_pal.palette->data,DMA);
				PAL_setPalette(PAL1, spr_char_sel_titleA.palette->data,DMA);
				PAL_setPalette(PAL2, spr_charsel_shinobi2 .palette->data,DMA);
				PAL_setPalette(PAL3, spr_charsel_shinobi1 .palette->data,DMA);
			}
			
			if(gFrames>=60 && P[1].key_JOY_START_status==1)
			{
				MAP_scrollTo(level_mapb, 0, 0);
				
				if(P[0].sprite){ SPR_releaseSprite(P[0].sprite); P[0].sprite = NULL; } //clear memory
				if(P[1].sprite){ SPR_releaseSprite(P[1].sprite); P[1].sprite = NULL; } //clear memory
				if(P[2].sprite){ SPR_releaseSprite(P[2].sprite); P[2].sprite = NULL; } //clear memory
				
				if(AnimObj[0].sprite){ SPR_releaseSprite(AnimObj[0].sprite); AnimObj[0].sprite = NULL; } //clear memory
				if(AnimObj[1].sprite){ SPR_releaseSprite(AnimObj[1].sprite); AnimObj[1].sprite = NULL; } //clear memory
				if(AnimObj[2].sprite){ SPR_releaseSprite(AnimObj[2].sprite); AnimObj[2].sprite = NULL; } //clear memory
				
				CLEAR_VDP();
				//gRoom=0;
				//gDescompressionExit=9;
				gRoom=9;
				gFrames=0;
			}
			
		}
		
		/*STAGE SELECT*/
		if(gRoom==9)
		{
			if(gFrames==1)
			{
				PAL_setColors(0, palette_black, 64, DMA);
				
				gInd_tileset=1;
				//BGB
				VDP_loadTileSet(&bg_bgb_lvl44_tileset, gInd_tileset, DMA);
				level_mapb = MAP_create(&bg_bgb_lvl44_map, BG_B, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, gInd_tileset)); 
				gInd_tileset += bg_bgb_lvl44_tileset.numTile;
				MAP_scrollTo(level_mapb, 0, 0);
				//BGA
				VDP_loadTileSet(&bg_bgb_stage_sel_tileset, gInd_tileset, DMA);
				level_map = MAP_create(&bg_bgb_stage_sel_map, BG_A, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset)); 
				gInd_tileset += bg_bgb_stage_sel_tileset.numTile;
				MAP_scrollTo(level_map, 0, 0);
				
				if(P[0].sprite){ SPR_releaseSprite(P[0].sprite); P[0].sprite = NULL; } //clear memory
				P[0].sprite = SPR_addSpriteExSafe(&spr_stage_sel_cursor, 136, 40, TILE_ATTR(PAL0, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
			}
			
			if(gFrames==6){ PAL_setPalette(PAL0, bg_bgb_stage_sel_pal.palette->data,DMA); }
			
			bool playSndCursor=FALSE;
			if(P[1].key_JOY_UP_status  ==1){ if(gMapID>  1){gMapID--;}else{gMapID=15;} playSndCursor=TRUE; }
			if(P[1].key_JOY_DOWN_status==1){ if(gMapID< 15){gMapID++;}else{gMapID= 1;} playSndCursor=TRUE; }
			if(playSndCursor==TRUE){ XGM_setPCM(P1_SFX, snd_cursor, sizeof(snd_cursor)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
			
			if(gMapID== 1){ SPR_setPosition(P[0].sprite, 136,  40); }
			if(gMapID== 2){ SPR_setPosition(P[0].sprite, 136,  48); }
			if(gMapID== 3){ SPR_setPosition(P[0].sprite, 136,  56); }
			if(gMapID== 4){ SPR_setPosition(P[0].sprite, 136,  72); }
			if(gMapID== 5){ SPR_setPosition(P[0].sprite, 136,  80); }
			if(gMapID== 6){ SPR_setPosition(P[0].sprite, 136,  88); }
			if(gMapID== 7){ SPR_setPosition(P[0].sprite, 136,  96); }
			if(gMapID== 8){ SPR_setPosition(P[0].sprite, 136, 112); }
			if(gMapID== 9){ SPR_setPosition(P[0].sprite, 136, 120); }
			if(gMapID==10){ SPR_setPosition(P[0].sprite, 136, 128); }
			if(gMapID==11){ SPR_setPosition(P[0].sprite, 136, 136); }
			if(gMapID==12){ SPR_setPosition(P[0].sprite, 136, 152); }
			if(gMapID==13){ SPR_setPosition(P[0].sprite, 136, 160); }
			if(gMapID==14){ SPR_setPosition(P[0].sprite, 136, 168); }
			if(gMapID==15){ SPR_setPosition(P[0].sprite, 136, 176); }
			
			if(gFrames>=60 && P[1].key_JOY_START_status==1)
			{
				MAP_scrollTo(level_map,  0, 0);
				MAP_scrollTo(level_mapb, 0, 0);
				
				if(P[0].sprite){ SPR_releaseSprite(P[0].sprite); P[0].sprite = NULL; } //clear memory
				
				//if(gMapID>2){gMapID=1;} //LIMITADOR DE DESENVOLVIMENTO
				
				CLEAR_VDP();
				//gRoom=0;
				//gDescompressionExit=9;
				gRoom=11;
				gFrames=0;
			}
			
		}
		
		/*IN GAME ROOM*/
		if(gRoom==10)
		{
			if(gFrames==1)
			{
				CREATE_STAGE(gMapID);
				
				if(P[1].sprite){ SPR_releaseSprite(P[1].sprite); P[1].sprite = NULL; } //clear memory
				//player config sprite
				if(P[1].id==2)
				{
					P[1].sprite = SPR_addSpriteSafe(&spr_player2, -99, -99, TILE_ATTR(PAL3, TRUE, FALSE, FALSE));
				}else{
					P[1].sprite = SPR_addSpriteSafe(&spr_player, -99, -99, TILE_ATTR(PAL3, TRUE, FALSE, FALSE));
				}
				if(P[1].sprite){ SPR_setDepth(P[1].sprite, 5); }
				
				//P[1].x e P[1].y sao definidos pela funcao "CREATE_STAGE(gMapID)"
				P[1].axisX=48;
				P[1].axisY=96;
				P[1].releaseAnim=0;
				P[1].totalShurikens=0;
				P[1].dir=1;
				P[1].impulseX=0;
				P[1].impulseY=0;
				P[1].velocityX=0;
				P[1].velocityY=0;
				P[1].planeChangerSensor=FALSE;
				P[1].playerLayer=1; if(gMapID==14){P[1].playerLayer=2;}
				P[1].extraImpulse=0;
				PLAYER_STATE(1, 100);
			}
			
			if(gFrames==2) //HUD
			{
				hudElement[0] = SPR_addSpriteExSafe(&spr_HUD_face,   8,  0, TILE_ATTR(PAL3, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				hudElement[1] = SPR_addSpriteExSafe(&spr_HUD_lives_numbers,  56, 16, TILE_ATTR(PAL3, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				
				hudElement[4] = SPR_addSpriteExSafe(&spr_HUD_bomb, 232,  0, TILE_ATTR(PAL3, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				hudElement[5] = SPR_addSpriteExSafe(&spr_HUD_numbers, 296,  8, TILE_ATTR(PAL3, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				hudElement[6] = SPR_addSpriteExSafe(&spr_HUD_magic, 0, 195, TILE_ATTR(PAL3, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				
				//2:59 clock
				hudElement[ 7] = SPR_addSpriteExSafe(&spr_HUD_numbers, 168,  8, TILE_ATTR(PAL3, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				if(hudElement[ 7]){ SPR_setAnimAndFrame(hudElement[ 7], 0, 2); }
				hudElement[ 8] = SPR_addSpriteExSafe(&spr_HUD_numbers, 176,  8, TILE_ATTR(PAL3, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				if(hudElement[ 8]){ SPR_setAnimAndFrame(hudElement[ 8], 0, 10); } //: << 2:59
				hudElement[ 9] = SPR_addSpriteExSafe(&spr_HUD_numbers, 184,  8, TILE_ATTR(PAL3, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				if(hudElement[ 9]){ SPR_setAnimAndFrame(hudElement[ 9], 0, 5); }
				hudElement[10] = SPR_addSpriteExSafe(&spr_HUD_numbers, 192,  8, TILE_ATTR(PAL3, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				if(hudElement[10]){ SPR_setAnimAndFrame(hudElement[10], 0, 9); }
				
				//points
				hudElement[11] = SPR_addSpriteExSafe(&spr_HUD_points,  40,  8, TILE_ATTR(PAL3, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				hudElement[12] = SPR_addSpriteExSafe(&spr_HUD_points,  48,  8, TILE_ATTR(PAL3, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				hudElement[13] = SPR_addSpriteExSafe(&spr_HUD_points,  56,  8, TILE_ATTR(PAL3, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				hudElement[14] = SPR_addSpriteExSafe(&spr_HUD_points,  64,  8, TILE_ATTR(PAL3, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				hudElement[15] = SPR_addSpriteExSafe(&spr_HUD_points,  72,  8, TILE_ATTR(PAL3, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				hudElement[16] = SPR_addSpriteExSafe(&spr_HUD_points,  80,  8, TILE_ATTR(PAL3, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				
				//VRAM MEM PLAN
				SPR_setVRAMTileIndex(hudElement[11], 1987); //define uma posicao especifica para o GFX na VRAM //POINTS x00000
				SPR_setVRAMTileIndex(hudElement[12], 1988); //define uma posicao especifica para o GFX na VRAM //POINTS 0x0000
				SPR_setVRAMTileIndex(hudElement[13], 1989); //define uma posicao especifica para o GFX na VRAM //POINTS 00x000
				SPR_setVRAMTileIndex(hudElement[14], 1990); //define uma posicao especifica para o GFX na VRAM //POINTS 000x00
				SPR_setVRAMTileIndex(hudElement[15], 1991); //define uma posicao especifica para o GFX na VRAM //POINTS 0000x0
				SPR_setVRAMTileIndex(hudElement[16], 1992); //define uma posicao especifica para o GFX na VRAM //POINTS 00000x
				SPR_setVRAMTileIndex(hudElement[ 6], 1993); //define uma posicao especifica para o GFX na VRAM //HUD MARCADOR MAGIC
				SPR_setVRAMTileIndex(hudElement[ 4], 2013); //define uma posicao especifica para o GFX na VRAM //HUD MARCADOR DE BOMBA
				SPR_setVRAMTileIndex(hudElement[ 0], 2026); //define uma posicao especifica para o GFX na VRAM //HUD FACE
				SPR_setVRAMTileIndex(hudElement[ 1], 2036); //define uma posicao especifica para o GFX na VRAM //LIVES
				SPR_setVRAMTileIndex(hudElement[ 5], 2038); //define uma posicao especifica para o GFX na VRAM //BOMBS
				SPR_setVRAMTileIndex(hudElement[10], 2040); //define uma posicao especifica para o GFX na VRAM //MINUTES
				SPR_setVRAMTileIndex(hudElement[ 9], 2042); //define uma posicao especifica para o GFX na VRAM //:
				SPR_setVRAMTileIndex(hudElement[ 8], 2044); //define uma posicao especifica para o GFX na VRAM //DEC SEGUNDOS
				SPR_setVRAMTileIndex(hudElement[ 7], 2046); //define uma posicao especifica para o GFX na VRAM //SEGUNDOS
			
				if(hudElement[ 0]){ SPR_setDepth(hudElement[ 0], 1); }
				if(hudElement[ 1]){ SPR_setDepth(hudElement[ 1], 1); }
				if(hudElement[ 2]){ SPR_setDepth(hudElement[ 2], 1); }
				if(hudElement[ 3]){ SPR_setDepth(hudElement[ 3], 1); }
				if(hudElement[ 4]){ SPR_setDepth(hudElement[ 4], 1); }
				if(hudElement[ 5]){ SPR_setDepth(hudElement[ 5], 1); }
				if(hudElement[ 6]){ SPR_setDepth(hudElement[ 6], 1); }
				if(hudElement[ 7]){ SPR_setDepth(hudElement[ 7], 1); }
				if(hudElement[ 8]){ SPR_setDepth(hudElement[ 8], 1); }
				if(hudElement[ 9]){ SPR_setDepth(hudElement[ 9], 1); }
				if(hudElement[10]){ SPR_setDepth(hudElement[10], 1); }
			}
			
			if(gFrames==5) //start music
			{
				XGM_stopPlay();
				XGM_startPlay(music_11); 
				XGM_isPlaying();
			}
			
			if(gFrames==10) //force camera update
			{
				MAP_scrollTo(level_map, new_camera_x, new_camera_y);
				MAP_scrollTo(level_mapb, new_camera_x/2, new_camera_y/2);
			}
			
			if(gFrames==12) //stage pal
			{
				PAL_setPalette(PAL2, spr_enemy02.palette->data,DMA); //general palete data
				
				if(gMapID== 1) //LVL1-1
				{
					PAL_setPalette(PAL0, bg_bgb_lvl11_pal.palette->data,DMA);
					PAL_setPalette(PAL1, bg_bga_lvl11_pal.palette->data,DMA);
				}
				if(gMapID== 2) //LVL1-2
				{
					PAL_setPalette(PAL0, bg_bgb_lvl12_pal.palette->data,DMA);
					PAL_setPalette(PAL1, bg_bga_lvl12A_pal.palette->data,DMA);
				}
				if(gMapID== 3) //LVL1-3
				{
					PAL_setPalette(PAL0, bg_bgb_lvl13_pal.palette->data,DMA);
					PAL_setPalette(PAL1, bg_bga_lvl13_pal.palette->data,DMA);
				}
				if(gMapID== 4) //LVL2-1
				{
					PAL_setPalette(PAL0, bg_bgb_lvl21_pal.palette->data,DMA);
					PAL_setPalette(PAL1, bg_bga_lvl21_pal.palette->data,DMA);
				}
				if(gMapID== 5) //LVL2-2
				{
					PAL_setPalette(PAL0, bg_bgb_lvl22_pal.palette->data,DMA);
					PAL_setPalette(PAL1, bg_bga_lvl22A_pal.palette->data,DMA);
				}
				if(gMapID== 6) //LVL2-3
				{
					PAL_setPalette(PAL0, bg_bgb_lvl23_pal.palette->data,DMA);
					PAL_setPalette(PAL1, bg_bga_lvl23_pal.palette->data,DMA);
				}
				if(gMapID== 7) //LVL2-4
				{
					PAL_setPalette(PAL0, bg_bgb_lvl24_pal.palette->data,DMA);
					PAL_setPalette(PAL1, bg_bga_lvl24_pal.palette->data,DMA);
				}
				if(gMapID== 8) //LVL3-1
				{
					PAL_setPalette(PAL0, bg_bgb_lvl31_pal.palette->data,DMA);
					PAL_setPalette(PAL1, bg_bga_lvl31_pal.palette->data,DMA);
				}
				if(gMapID== 9) //LVL3-2
				{
					PAL_setPalette(PAL0, bg_bgb_lvl32_pal.palette->data,DMA);
					PAL_setPalette(PAL1, bg_bga_lvl32_pal.palette->data,DMA);
				}
				if(gMapID==10) //LVL3-3
				{
					PAL_setPalette(PAL0, bg_bgb_lvl33_pal.palette->data,DMA);
					PAL_setPalette(PAL1, bg_bga_lvl33_pal.palette->data,DMA);
				}
				if(gMapID==11) //LVL3-4
				{
					PAL_setPalette(PAL0, bg_bgb_lvl34_pal.palette->data,DMA);
					PAL_setPalette(PAL1, bg_bga_lvl34_pal.palette->data,DMA);
				}
				if(gMapID==12) //LVL4-1
				{
					PAL_setPalette(PAL0, bg_bgb_lvl41_pal.palette->data,DMA);
					PAL_setPalette(PAL1, bg_bga_lvl41_pal.palette->data,DMA);
				}
				if(gMapID==13) //LVL4-2
				{
					PAL_setPalette(PAL0, bg_bgb_lvl42_pal.palette->data,DMA);
					PAL_setPalette(PAL1, bg_bga_lvl42_pal.palette->data,DMA);
				}
				if(gMapID==14) //LVL4-3
				{
					PAL_setPalette(PAL0, bg_bgb_lvl43_pal.palette->data,DMA);
					PAL_setPalette(PAL1, bg_bga_lvl43_pal.palette->data,DMA);
				}
				if(gMapID==15) //LVL4-4
				{
					PAL_setPalette(PAL0, bg_bgb_lvl44_pal.palette->data,DMA);
					PAL_setPalette(PAL1, bg_bga_lvl44_pal.palette->data,DMA);
				}
				
				//player pal
				if(P[1].id==2)
				{
					PAL_setPalette(PAL3, spr_player2.palette->data,DMA);
				}else{
					PAL_setPalette(PAL3, spr_player.palette->data,DMA);
				}
				
			}
			
			if(gFrames==13)
			{
				if(gMapID==1)
				{
					//OBJ_SEED(s32 x, s32 y, u8 type, u8 layer)
					OBJ_SEED(352+40*1, 224, 1, 1);
					OBJ_SEED(352+40*2, 224, 2, 1);
					OBJ_SEED(352+40*3, 224, 3, 1);
					OBJ_SEED(352+40*4, 224, 4, 1);
				}
			}
			
			//FIX THE HUD
			if(hudElement[ 0]){ SPR_setPosition(hudElement[ 0],   8,   0); }
			if(hudElement[ 1]){ SPR_setPosition(hudElement[ 1],  56,  16); }
			if(hudElement[ 4]){ SPR_setPosition(hudElement[ 4], 232,   0); }
			if(hudElement[ 5]){ SPR_setPosition(hudElement[ 5], 296,   8); }
			if(hudElement[ 6]){ SPR_setPosition(hudElement[ 6],   0, 195); }
			if(hudElement[ 7]){ SPR_setPosition(hudElement[ 7], 160,   8); }
			if(hudElement[ 8]){ SPR_setPosition(hudElement[ 8], 176,   8); }
			if(hudElement[ 9]){ SPR_setPosition(hudElement[ 9], 184,   8); }
			if(hudElement[10]){ SPR_setPosition(hudElement[10], 192,   8); }
			if(hudElement[11]){ SPR_setPosition(hudElement[11],  40,   8); }
			if(hudElement[12]){ SPR_setPosition(hudElement[12],  48,   8); }
			if(hudElement[13]){ SPR_setPosition(hudElement[13],  56,   8); }
			if(hudElement[14]){ SPR_setPosition(hudElement[14],  64,   8); }
			if(hudElement[15]){ SPR_setPosition(hudElement[15],  72,   8); }
			if(hudElement[16]){ SPR_setPosition(hudElement[16],  80,   8); }
			
		}
		
		/*LOG MISSION ... SCREEN*/
		if(gRoom==11)
		{
			if(gFrames==1)
			{
				PAL_setColors(0, palette_black, 64, DMA); 
				gRelogioTimer = 60;
				gRelMinutos = 2;
				gRelSegundos = 59;
				
				gInd_tileset=1;
				
				//BGB
				VDP_loadTileSet(&bg_bgb_lvl44_tileset, gInd_tileset, DMA);
				level_mapb = MAP_create(&bg_bgb_lvl44_map, BG_B, TILE_ATTR_FULL(PAL3, FALSE, FALSE, FALSE, gInd_tileset)); 
				gInd_tileset += bg_bgb_lvl44_tileset.numTile;
				MAP_scrollTo(level_mapb, 0, 0);
				if(gMapID>=1 && gMapID<=3)
				{
					//BGA
					VDP_loadTileSet(bg_bgb_log_m1.tileset,gInd_tileset,DMA); 
					VDP_setTileMapEx(BG_A,bg_bgb_log_m1.tilemap,TILE_ATTR_FULL(PAL0,FALSE,FALSE,FALSE,gInd_tileset),0,0,0,0,40,32,DMA_QUEUE);
					gInd_tileset += bg_bgb_log_m1.tileset->numTile;
					P[0].sprite = SPR_addSpriteExSafe(&spr_log_mission1     ,  48,   8, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
					P[1].sprite = SPR_addSpriteExSafe(&spr_log_mission1_txtA,   0, 176, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
					P[2].sprite = SPR_addSpriteExSafe(&spr_log_mission1_txtB, 160, 176, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				}
				if(gMapID>=4 && gMapID<=7)
				{
					//BGA
					VDP_loadTileSet(bg_bgb_log_m2.tileset,gInd_tileset,DMA); 
					VDP_setTileMapEx(BG_A,bg_bgb_log_m2.tilemap,TILE_ATTR_FULL(PAL0,FALSE,FALSE,FALSE,gInd_tileset),0,0,0,0,40,32,DMA_QUEUE);
					gInd_tileset += bg_bgb_log_m2.tileset->numTile;
					P[0].sprite = SPR_addSpriteExSafe(&spr_log_mission2     ,  48,   8, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
					P[1].sprite = SPR_addSpriteExSafe(&spr_log_mission2_txtA,   0, 176, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
					P[2].sprite = SPR_addSpriteExSafe(&spr_log_mission2_txtB, 160, 176, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				}
				if(gMapID>=8 && gMapID<=11)
				{
					//BGA
					VDP_loadTileSet(bg_bgb_log_m3.tileset,gInd_tileset,DMA); 
					VDP_setTileMapEx(BG_A,bg_bgb_log_m3.tilemap,TILE_ATTR_FULL(PAL0,FALSE,FALSE,FALSE,gInd_tileset),0,0,0,0,40,32,DMA_QUEUE);
					gInd_tileset += bg_bgb_log_m3.tileset->numTile;
					P[0].sprite = SPR_addSpriteExSafe(&spr_log_mission3     ,  48,   8, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
					P[1].sprite = SPR_addSpriteExSafe(&spr_log_mission3_txtA,   0, 176, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
					P[2].sprite = SPR_addSpriteExSafe(&spr_log_mission3_txtB, 160, 176, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				}
				if(gMapID>=12 && gMapID<=15)
				{
					//BGA
					VDP_loadTileSet(bg_bgb_log_m4.tileset,gInd_tileset,DMA); 
					VDP_setTileMapEx(BG_A,bg_bgb_log_m4.tilemap,TILE_ATTR_FULL(PAL0,FALSE,FALSE,FALSE,gInd_tileset),0,0,0,0,40,32,DMA_QUEUE);
					gInd_tileset += bg_bgb_log_m4.tileset->numTile;
					P[0].sprite = SPR_addSpriteExSafe(&spr_log_mission4     ,  48,   8, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
					P[1].sprite = SPR_addSpriteExSafe(&spr_log_mission4_txtA,   0, 176, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
					P[2].sprite = SPR_addSpriteExSafe(&spr_log_mission4_txtB, 160, 176, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				}
				
				//if(level_map){ MAP_scrollTo(level_map, 0, 0); }
				if(level_mapb){ MAP_scrollTo(level_mapb, 0, 0); }
				
				
				//PAL_setPalette(PAL1, spr_log_pal.palette->data,DMA);
				
				if(gMapID==1){AnimObj[0].sprite = SPR_addSpriteExSafe(&spr_log_indicator11, 320, 224, TILE_ATTR(PAL2, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );}
				if(gMapID==2){AnimObj[0].sprite = SPR_addSpriteExSafe(&spr_log_indicator12, 320, 224, TILE_ATTR(PAL2, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );}
				if(gMapID==3){AnimObj[0].sprite = SPR_addSpriteExSafe(&spr_log_indicator13, 320, 224, TILE_ATTR(PAL2, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );}
				//PAL_setPalette(PAL2, spr_log_indicator11.palette->data,DMA);
				
				// nao funciona -> PAL_setPaletteColors(17,RGB24_TO_VDPCOLOR(0x0098e5), palette->data,DMA);
			}
			
			if(gFrames==5)
			{
				if(gMapID>= 1 && gMapID<= 3){ PAL_setPalette(PAL0, bg_bgb_log_m1.palette->data,DMA); }
				if(gMapID>= 4 && gMapID<= 7){ PAL_setPalette(PAL0, bg_bgb_log_m2.palette->data,DMA); }
				if(gMapID>= 8 && gMapID<=11){ PAL_setPalette(PAL0, bg_bgb_log_m3.palette->data,DMA); }
				if(gMapID>=12 && gMapID<=15){ PAL_setPalette(PAL0, bg_bgb_log_m4.palette->data,DMA); }
				PAL_setPalette(PAL1, spr_log_pal.palette->data,DMA);
				PAL_setPalette(PAL2, spr_log_indicator11.palette->data,DMA);
				
				XGM_stopPlay();
				XGM_startPlay(music_log_mission); 
				XGM_isPlaying();
			}
			
			if(gMapID==1){if(AnimObj[0].sprite){ SPR_setPosition(AnimObj[0].sprite, 130, 122); }}
			if(gMapID==2){if(AnimObj[0].sprite){ SPR_setPosition(AnimObj[0].sprite, 181,  93); }}
			if(gMapID==3){if(AnimObj[0].sprite){ SPR_setPosition(AnimObj[0].sprite, 158,  80); }}
			
			if(P[0].sprite){ SPR_setPosition(P[0].sprite,  48,   8); }
			if(P[1].sprite){ SPR_setPosition(P[1].sprite,   0, 176); }
			if(P[2].sprite){ SPR_setPosition(P[2].sprite, 160, 176); }
			
			if((gFrames>=10 && P[1].key_JOY_START_status==1) || (gFrames>=260))
			{
				if(P[0].sprite){ SPR_releaseSprite(P[0].sprite); P[0].sprite = NULL; } //clear memory
				if(P[1].sprite){ SPR_releaseSprite(P[1].sprite); P[1].sprite = NULL; } //clear memory
				if(P[2].sprite){ SPR_releaseSprite(P[2].sprite); P[2].sprite = NULL; } //clear memory
				if(AnimObj[0].sprite){ SPR_releaseSprite(AnimObj[0].sprite); AnimObj[0].sprite = NULL; } //clear memory
				//CLEAR_VDP();
				//gRoom=0;
				//gDescompressionExit=10;
				gRoom=10;
				gFrames=0;
			}
			
		}
		
		//--- FINALIZATIONS ---//
		//VDP_showFPS(1); //Shows FPS rate
		
		if(gPauseSystem==0){ SPR_update(); } //Updates (draws) the sprites
        SYS_doVBlankProcess(); //Wait for screen refresh and do all SGDK VBlank tasks
    }

    return 0;
}

void PLAYER_STATE(u8 Player, u16 State)
{
	P[Player].animFrame = 1;
	P[Player].totalAnimationFrames = 1;
	P[Player].frameTimeAtual = 1;
	P[Player].frameTimeTotal = 1;
	P[Player].animTimeAtual  = 1;
	P[Player].animTimeTotal  = 1;
	P[Player].dataAnim[1]    = 1;
	P[Player].state = State;
	P[Player].axisX = 48;
	P[Player].axisY = 96;
	
	//if(State==101){ FXOBJECT(P[1].x, P[1].y, 1); } //efeito de fumaca no chao
	
	//SOUNDs
	if(State==100){ PLAY_SND(0); } //snd_null
	if(State==400){ PLAY_SND(1); } 
	
	//the states is ordered as they appear in the sprite atlas
	//the number indicates the direction, like a numerical keyboard
	//example: "109" the sprite points to the upper right corner
	//there are some exceptions but this is the basic idea
	
	if(State==100){ //static
	P[Player].dataAnim[1]  = 250;
	P[Player].totalAnimationFrames = 1;
	P[Player].animRow = 0;
	}
	else if(State==101){ //shoot shuriken
	P[Player].dataAnim[1]  = 2;
	P[Player].dataAnim[2]  = 2;
	P[Player].dataAnim[3]  = 3;
	P[Player].dataAnim[4]  = 2;
	P[Player].dataAnim[5]  = 180;
	P[Player].totalAnimationFrames = 5;
	P[Player].animRow = 1;
	}
	else if(State==102){ //sword attack
	P[Player].dataAnim[1]  = 3;
	P[Player].dataAnim[2]  = 3;
	P[Player].dataAnim[3]  = 3;
	P[Player].dataAnim[4]  = 3;
	P[Player].dataAnim[5]  = 3;
	P[Player].dataAnim[6]  = 3;
	P[Player].totalAnimationFrames = 6;
	P[Player].animRow = 2;
	}
	else if(State==400){ //intro walk
	P[Player].dataAnim[1]  = 3;
	P[Player].totalAnimationFrames = 1;
	P[Player].animRow = 3;
	}
	else if(State==401){ //walk
	P[Player].dataAnim[1]  = 6;
	P[Player].dataAnim[2]  = 6;
	P[Player].dataAnim[3]  = 6;
	P[Player].dataAnim[4]  = 6;
	P[Player].dataAnim[5]  = 6;
	P[Player].dataAnim[6]  = 6;
	P[Player].totalAnimationFrames = 6;
	P[Player].animRow = 4;
	}
	else if(State==402){ //end walk
	P[Player].dataAnim[1]  = 3;
	P[Player].totalAnimationFrames = 1;
	P[Player].animRow = 3;
	}
	else if(State==300){ //jump
	P[Player].dataAnim[1]  = 4;
	P[Player].dataAnim[2]  = 6;
	P[Player].dataAnim[3]  = 6;
	P[Player].dataAnim[4]  = 250;
	P[Player].totalAnimationFrames = 4;
	P[Player].animRow = 5;
	}
	else if(State==606){ //landing on ground
	P[Player].dataAnim[1]  = 3;
	P[Player].dataAnim[2]  = 3;
	P[Player].totalAnimationFrames = 2;
	P[Player].animRow = 6;
	}
	else if(State==310){ //jump forward
	P[Player].dataAnim[1]  = 4;
	P[Player].dataAnim[2]  = 6;
	P[Player].dataAnim[3]  = 6;
	P[Player].dataAnim[4]  = 250;
	P[Player].totalAnimationFrames = 4;
	P[Player].animRow = 7;
	}
	else if(State==200){ //crouch
	P[Player].dataAnim[1]  = 250;
	P[Player].totalAnimationFrames = 1;
	P[Player].animRow = 8;
	}
	else if(State==201){ //crouch shuriken
	P[Player].dataAnim[1]  = 2;
	P[Player].dataAnim[2]  = 2;
	P[Player].dataAnim[3]  = 3;
	P[Player].dataAnim[4]  = 2;
	P[Player].dataAnim[5]  = 180;
	P[Player].totalAnimationFrames = 5;
	P[Player].animRow = 9;
	}
	else if(State==410){ //intro c.walk
	P[Player].dataAnim[1]  = 3;
	P[Player].totalAnimationFrames = 1;
	P[Player].animRow = 10;
	}
	else if(State==411){ //c.walk
	P[Player].dataAnim[1]  = 6;
	P[Player].dataAnim[2]  = 6;
	P[Player].dataAnim[3]  = 6;
	P[Player].dataAnim[4]  = 6;
	P[Player].dataAnim[5]  = 6;
	P[Player].dataAnim[6]  = 6;
	P[Player].totalAnimationFrames = 6;
	P[Player].animRow = 11;
	}
	else if(State==888){ //looking to up
	P[Player].dataAnim[1]  = 250;
	P[Player].totalAnimationFrames = 1;
	P[Player].animRow = 14;
	}
	else if(State==348){ //intro - jump layer up
	P[Player].dataAnim[1]  = 4;
	P[Player].dataAnim[2]  = 6;
	P[Player].dataAnim[3]  = 6;
	P[Player].totalAnimationFrames = 3;
	P[Player].animRow = 15;
	}
	else if(State==358){ //jump layer up
	P[Player].dataAnim[1]  = 6;
	P[Player].dataAnim[2]  = 6;
	P[Player].dataAnim[3]  = 6;
	P[Player].dataAnim[4]  = 6;
	P[Player].dataAnim[5]  = 6;
	P[Player].dataAnim[6]  = 250;
	P[Player].totalAnimationFrames = 6;
	P[Player].animRow = 16;
	}
	else if(State==342){ //intro - jump layer down
	P[Player].dataAnim[1]  = 14;
	P[Player].totalAnimationFrames = 1;
	P[Player].animRow = 17;
	}
	else if(State==352){ //jump layer down
	P[Player].dataAnim[1]  = 6;
	P[Player].dataAnim[2]  = 6;
	P[Player].dataAnim[3]  = 6;
	P[Player].dataAnim[4]  = 6;
	P[Player].dataAnim[5]  = 6;
	P[Player].dataAnim[6]  = 250;
	P[Player].totalAnimationFrames = 6;
	P[Player].animRow = 18;
	}
	else if(State==  8){ //contact
	P[Player].dataAnim[1]  = 6;
	P[Player].dataAnim[2]  = 250;
	P[Player].totalAnimationFrames = 2;
	P[Player].animRow = 20;
	}
	else if(State==  9){ //dead
	P[Player].dataAnim[1]  = 6;
	P[Player].dataAnim[2]  = 6;
	P[Player].dataAnim[3]  = 6;
	P[Player].dataAnim[4]  = 6;
	P[Player].dataAnim[5]  = 6;
	P[Player].dataAnim[6]  = 250;
	P[Player].totalAnimationFrames = 6;
	P[Player].animRow = 21;
	}
	
	
	P[Player].frameTimeTotal = P[Player].dataAnim[1];
	
	if (P[Player].sprite){ SPR_setAnimAndFrame(P[Player].sprite, P[Player].animRow, P[Player].animFrame-1); }
}

void INPUT_SYSTEM() //"GAMEDEVBOSS" Input System
{
    static u16 currentJoyState[2] = {0};
    static u16 previousJoyState[2] = {0};
	u16 buttons[] = {BUTTON_UP, BUTTON_DOWN, BUTTON_LEFT, BUTTON_RIGHT, BUTTON_A, BUTTON_B, BUTTON_C, BUTTON_X, BUTTON_Y, BUTTON_Z, BUTTON_START, BUTTON_MODE};
	
    for (int player = 0; player < 2; player++) {
        previousJoyState[player] = currentJoyState[player];
		if(player==0){ currentJoyState[player] = JOY_readJoypad(JOY_1); }
		else if(player==1){ currentJoyState[player] = JOY_readJoypad(JOY_2); }
		
        int numButtons = sizeof(buttons) / sizeof(buttons[0]);

        for (int i = 0; i < numButtons; i++) {
            u16 button = buttons[i];

            if (!(currentJoyState[player] & button)) {
                P[player].key_JOY_status[i] = 0;
            } else if ((currentJoyState[player] & button) && !(previousJoyState[player] & button)) {
                P[player].key_JOY_status[i] = 1;
            } else if (currentJoyState[player] & button) {
                P[player].key_JOY_status[i] = 2;
            } else if (!(currentJoyState[player] & button) && (previousJoyState[player] & button)) {
                P[player].key_JOY_status[i] = 3;
            }
			
			if(i==numButtons-1) //end of loop, assignments
			{
				P[player+1].key_JOY_UP_status    = P[player].key_JOY_status[ 0];
				P[player+1].key_JOY_DOWN_status  = P[player].key_JOY_status[ 1];
				P[player+1].key_JOY_LEFT_status  = P[player].key_JOY_status[ 2];
				P[player+1].key_JOY_RIGHT_status = P[player].key_JOY_status[ 3];
				P[player+1].key_JOY_A_status     = P[player].key_JOY_status[ 4];
				P[player+1].key_JOY_B_status     = P[player].key_JOY_status[ 5];
				P[player+1].key_JOY_C_status     = P[player].key_JOY_status[ 6];
				P[player+1].key_JOY_X_status     = P[player].key_JOY_status[ 7];
				P[player+1].key_JOY_Y_status     = P[player].key_JOY_status[ 8];
				P[player+1].key_JOY_Z_status     = P[player].key_JOY_status[ 9];
				P[player+1].key_JOY_START_status = P[player].key_JOY_status[10];
				P[player+1].key_JOY_MODE_status  = P[player].key_JOY_status[11];
			}
        }
    }
}

void FSM()
{
	//atirar shurikens
	if((P[1].state==101 || P[1].state==201) && P[1].animFrame==3 && P[1].frameTimeAtual==1)
	{
		if(P[1].state==101){OBJSHURIKENS(P[1].x+(32*P[1].dir), P[1].y-40, P[1].dir);}
		if(P[1].state==201){OBJSHURIKENS(P[1].x+(32*P[1].dir), P[1].y-16, P[1].dir);}
	}
	
	if(P[1].state==100)
	{ 
		if(P[1].key_JOY_LEFT_status >0){ PLAYER_STATE(1,400); P[1].dir=-1; SPR_setHFlip(P[1].sprite,  TRUE); }
		if(P[1].key_JOY_RIGHT_status>0){ PLAYER_STATE(1,400); P[1].dir= 1; SPR_setHFlip(P[1].sprite, FALSE); } 
		if(P[1].key_JOY_UP_status >0){ if(P[1].planeChangerSensor==TRUE && P[1].planeChangerUPReady==TRUE){ PLAYER_STATE(1,888); } } 
		if(P[1].key_JOY_DOWN_status >0){ PLAYER_STATE(1,200); } 
		if(P[1].key_JOY_B_status   ==1 && P[1].totalShurikens<MAX_SHURIKENS){ PLAYER_STATE(1,101); } 
		if(P[1].key_JOY_C_status   ==1)
		{
			if(P[1].key_JOY_LEFT_status==0 && P[1].key_JOY_RIGHT_status==0)
			{
				PLAYER_STATE(1,300); JUMP_INIT(1); 
			}else{
				PLAYER_STATE(1,310); JUMP_INIT(1); 
			}
		} 
	}
	if(P[1].state==101 && P[1].animFrame>=5)
	{ 
		if(P[1].key_JOY_LEFT_status >0){ PLAYER_STATE(1,401); P[1].dir=-1; SPR_setHFlip(P[1].sprite,  TRUE); }
		if(P[1].key_JOY_RIGHT_status>0){ PLAYER_STATE(1,401); P[1].dir= 1; SPR_setHFlip(P[1].sprite, FALSE); } 
		if(P[1].key_JOY_UP_status >0){ if(P[1].planeChangerSensor==TRUE && P[1].planeChangerUPReady==TRUE){ PLAYER_STATE(1,888); } } 
		if(P[1].key_JOY_DOWN_status >0){ PLAYER_STATE(1,200); } 
		if(P[1].key_JOY_B_status   ==1 && P[1].totalShurikens< MAX_SHURIKENS){ PLAYER_STATE(1,101); } 
		if(P[1].key_JOY_B_status   ==1 && P[1].totalShurikens>=MAX_SHURIKENS){ PLAYER_STATE(1,100); }
		if(P[1].key_JOY_C_status   ==1)
		{ 
			if(P[1].key_JOY_LEFT_status==0 && P[1].key_JOY_RIGHT_status==0)
			{
				PLAYER_STATE(1,300); JUMP_INIT(1); 
			}else{
				PLAYER_STATE(1,310); JUMP_INIT(1); 
			}
		}
	}
	if(P[1].state==200)
	{ 
		if(P[1].key_JOY_LEFT_status >0){ PLAYER_STATE(1,410); P[1].dir=-1; SPR_setHFlip(P[1].sprite,  TRUE); }
		if(P[1].key_JOY_RIGHT_status>0){ PLAYER_STATE(1,410); P[1].dir= 1; SPR_setHFlip(P[1].sprite, FALSE); } 
		if(P[1].key_JOY_DOWN_status==0){ PLAYER_STATE(1,100); }
		if(P[1].key_JOY_B_status   ==1 && P[1].totalShurikens<MAX_SHURIKENS){ PLAYER_STATE(1,201); } 
		if(P[1].key_JOY_C_status   ==1)
		{
			if(P[1].planeChangerDOWNReady==TRUE && P[1].key_JOY_DOWN_status>0)
			{
				PLAYER_STATE(1,342);
			}
			else{
				if(P[1].key_JOY_LEFT_status==0 && P[1].key_JOY_RIGHT_status==0)
				{
					PLAYER_STATE(1,300); JUMP_INIT(1); 
				}else{
					PLAYER_STATE(1,310); JUMP_INIT(1); 
				}
			}
		} 
	}
	if(P[1].state==201 && P[1].animFrame>=5)
	{ 
		if(P[1].key_JOY_LEFT_status >0){ PLAYER_STATE(1,411); P[1].dir=-1; SPR_setHFlip(P[1].sprite,  TRUE); }
		if(P[1].key_JOY_RIGHT_status>0){ PLAYER_STATE(1,411); P[1].dir= 1; SPR_setHFlip(P[1].sprite, FALSE); } 
		if(P[1].key_JOY_DOWN_status==0){ PLAYER_STATE(1,100); } 
		if(P[1].key_JOY_B_status   ==1 && P[1].totalShurikens< MAX_SHURIKENS){ PLAYER_STATE(1,201); } 
		if(P[1].key_JOY_B_status   ==1 && P[1].totalShurikens>=MAX_SHURIKENS){ PLAYER_STATE(1,200); } 
		if(P[1].key_JOY_C_status   ==1)
		{
			if(P[1].planeChangerDOWNReady==TRUE && P[1].key_JOY_DOWN_status>0)
			{
				PLAYER_STATE(1,342);
			}else{
				if(P[1].key_JOY_LEFT_status==0 && P[1].key_JOY_RIGHT_status==0)
				{
					PLAYER_STATE(1,300); JUMP_INIT(1); 
				}else{
					PLAYER_STATE(1,310); JUMP_INIT(1); 
				}
			}
		}
	}
	if(P[1].state==310)
	{
		if(P[1].jumpTimer>=40 && gMapID==5 && P[1].playerLayer==2)
		{
			if(P[1].x<=2072){ P[1].x=2072; } //fix
			if(P[1].y>=212 && P[1].y<=224 && P[1].x>=2064 && P[1].x<=2120) //fix 
			{ 
				P[1].y=224; 
				PLAYER_STATE(1,606); 
				P[1].extraImpulse = 0;
				P[1].jumpTimer = 0;
				P[1].impulseX  = 0;
				P[1].impulseY  = 0;
				P[1].velocityX = 0;
				P[1].velocityY = 0;
				FXOBJECT(P[1].x, P[1].y, 1); 
			} 
		} 
		
	}
	if(P[1].state==352)
	{
		if(P[1].jumpTimer>=40 && gMapID==14 && P[1].playerLayer!=2) //4-3 fix
		{
			if(P[1].x>=1336 && P[1].x<=1536 && P[1].y<=352){ P[1].playerLayer=2; } //fix
			if(P[1].x>= 904 && P[1].x<=1152 && P[1].y>=392 && P[1].y<=456){ P[1].playerLayer=2; } //fix
			if(P[1].y<=200){ P[1].playerLayer=2; } //fix
		} 
	}
	if(P[1].state==401)
	{ 
		if(P[1].key_JOY_LEFT_status >0 && P[1].dir!=-1){ P[1].dir=-1; SPR_setHFlip(P[1].sprite,  TRUE); }
		if(P[1].key_JOY_RIGHT_status>0 && P[1].dir!= 1){ P[1].dir= 1; SPR_setHFlip(P[1].sprite, FALSE); }
		if(P[1].key_JOY_LEFT_status==2 || P[1].key_JOY_RIGHT_status==2){P[1].releaseAnim=RELEASE_ANIM_TIMER+1;}
		if(P[1].releaseAnim>0){ P[1].releaseAnim--; }
		if(P[1].key_JOY_LEFT_status ==0 && P[1].dir==-1 && P[1].releaseAnim>0){ if(P[1].frameTimeAtual>0){P[1].frameTimeAtual--;} }
		if(P[1].key_JOY_RIGHT_status==0 && P[1].dir== 1 && P[1].releaseAnim>0){ if(P[1].frameTimeAtual>0){P[1].frameTimeAtual--;} }
		if(P[1].key_JOY_B_status   ==1 && P[1].totalShurikens<MAX_SHURIKENS){ PLAYER_STATE(1,101); } 
		if(P[1].key_JOY_C_status   ==1)
		{
			if(P[1].key_JOY_LEFT_status==0 && P[1].key_JOY_RIGHT_status==0)
			{
				PLAYER_STATE(1,300); JUMP_INIT(1); 
			}else{
				PLAYER_STATE(1,310); JUMP_INIT(1); 
			}
		} 
		if(P[1].releaseAnim==0)
		{
			if(P[1].key_JOY_LEFT_status ==0 && P[1].dir==-1){ PLAYER_STATE(1,402); }
			if(P[1].key_JOY_RIGHT_status==0 && P[1].dir== 1){ PLAYER_STATE(1,402); } 
		}
		if(P[1].key_JOY_DOWN_status>0)
		{
			if(P[1].key_JOY_LEFT_status==0 && P[1].key_JOY_RIGHT_status==0)
			{
				PLAYER_STATE(1,100); 
			}else{
				PLAYER_STATE(1,411); 
			}
		}
	}
	if(P[1].state==402)
	{ 
		if(P[1].key_JOY_B_status   ==1 && P[1].totalShurikens<MAX_SHURIKENS){ PLAYER_STATE(1,101); } 
		if(P[1].key_JOY_C_status   ==1)
		{
			if(P[1].key_JOY_LEFT_status==0 && P[1].key_JOY_RIGHT_status==0)
			{
				PLAYER_STATE(1,300); JUMP_INIT(1); 
			}else{
				PLAYER_STATE(1,310); JUMP_INIT(1); 
			}
		} 
	}
	if(P[1].state==411)
	{ 
		if(P[1].key_JOY_LEFT_status >0 && P[1].dir!=-1){ P[1].dir=-1; SPR_setHFlip(P[1].sprite,  TRUE); }
		if(P[1].key_JOY_RIGHT_status>0 && P[1].dir!= 1){ P[1].dir= 1; SPR_setHFlip(P[1].sprite, FALSE); }
		if(P[1].key_JOY_LEFT_status==2 || P[1].key_JOY_RIGHT_status==2){P[1].releaseAnim=RELEASE_ANIM_TIMER+1;}
		if(P[1].releaseAnim>0){ P[1].releaseAnim--; }
		if(P[1].key_JOY_LEFT_status ==0 && P[1].dir==-1 && P[1].releaseAnim>0){ if(P[1].frameTimeAtual>0){P[1].frameTimeAtual--;} }
		if(P[1].key_JOY_RIGHT_status==0 && P[1].dir== 1 && P[1].releaseAnim>0){ if(P[1].frameTimeAtual>0){P[1].frameTimeAtual--;} }
		if(P[1].key_JOY_B_status   ==1 && P[1].totalShurikens<MAX_SHURIKENS){ PLAYER_STATE(1,201); } 
		if(P[1].key_JOY_C_status   ==1)
		{
			if(P[1].key_JOY_LEFT_status==0 && P[1].key_JOY_RIGHT_status==0)
			{
				PLAYER_STATE(1,300); JUMP_INIT(1); 
			}else{
				PLAYER_STATE(1,310); JUMP_INIT(1); 
			}
		} 
		if(P[1].releaseAnim==0)
		{
			if(P[1].key_JOY_LEFT_status ==0 && P[1].dir==-1){ PLAYER_STATE(1,412); }
			if(P[1].key_JOY_RIGHT_status==0 && P[1].dir== 1){ PLAYER_STATE(1,412); } 
		}
		if(P[1].key_JOY_DOWN_status==0)
		{
			if(P[1].key_JOY_LEFT_status==0 && P[1].key_JOY_RIGHT_status==0)
			{
				PLAYER_STATE(1,100); 
			}else{
				PLAYER_STATE(1,401); 
			}
		}
	}
	if(P[1].state==412)
	{ 
		if(P[1].key_JOY_B_status   ==1 && P[1].totalShurikens<MAX_SHURIKENS){ PLAYER_STATE(1,201); } 
		if(P[1].key_JOY_C_status   ==1)
		{
			if(P[1].key_JOY_LEFT_status==0 && P[1].key_JOY_RIGHT_status==0)
			{
				PLAYER_STATE(1,300); JUMP_INIT(1); 
			}else{
				PLAYER_STATE(1,310); JUMP_INIT(1); 
			}
		} 
	}
	if(P[1].state==606)
	{ 
		if(P[1].key_JOY_LEFT_status >0){ PLAYER_STATE(1,400); P[1].dir=-1; SPR_setHFlip(P[1].sprite,  TRUE); }
		if(P[1].key_JOY_RIGHT_status>0){ PLAYER_STATE(1,400); P[1].dir= 1; SPR_setHFlip(P[1].sprite, FALSE); } 
		if(P[1].key_JOY_B_status   ==1 && P[1].totalShurikens<MAX_SHURIKENS)
		{ 
			if(P[1].key_JOY_DOWN_status>0){ PLAYER_STATE(1,201); }else{ PLAYER_STATE(1,101); }
		} 
		if(P[1].key_JOY_C_status   ==1)
		{
			if(P[1].key_JOY_LEFT_status==0 && P[1].key_JOY_RIGHT_status==0)
			{
				PLAYER_STATE(1,300); JUMP_INIT(1); 
			}else{
				PLAYER_STATE(1,310); JUMP_INIT(1); 
			}
		} 
	}
	if(P[1].state==888)
	{ 
		if(P[1].key_JOY_LEFT_status >0){ PLAYER_STATE(1,400); P[1].dir=-1; SPR_setHFlip(P[1].sprite,  TRUE); }
		if(P[1].key_JOY_RIGHT_status>0){ PLAYER_STATE(1,400); P[1].dir= 1; SPR_setHFlip(P[1].sprite, FALSE); } 
		if(P[1].key_JOY_UP_status   ==0){ PLAYER_STATE(1,100); } 
		if(P[1].key_JOY_DOWN_status >0){ PLAYER_STATE(1,200); } 
		if(P[1].key_JOY_B_status   ==1 && P[1].totalShurikens<MAX_SHURIKENS){ PLAYER_STATE(1,101); } 
		if(P[1].key_JOY_C_status   ==1)
		{
			//change of plane
			if(P[1].planeChangerSensor==TRUE)
			{
				if(P[1].key_JOY_UP_status>0){ PLAYER_STATE(1,348); }
			}
		}
	}

}

void ANIMATION()
{
	P[1].frameTimeAtual++; 
	if(P[1].frameTimeAtual > P[1].frameTimeTotal) //time to change the frame!
	{
		P[1].animFrame++;
		if(P[1].animFrame > P[1].totalAnimationFrames) //time to swap or reload the animation!
		{
			//Example; finish the state "888" and start the state "999"
			//if( P[1].state== 888 )PLAYER_STATE( 1, 999 ); 
			
			if( P[1].state == 100 ){ PLAYER_STATE(1,100); 
			}else if( P[1].state == 400 ){ if(P[1].key_JOY_LEFT_status>0 || P[1].key_JOY_RIGHT_status>0){ PLAYER_STATE(1,401); }else{ PLAYER_STATE(1,100); }
			}else if( P[1].state == 401 ){ PLAYER_STATE(1,401); 
			}else if( P[1].state == 402 ){ PLAYER_STATE(1,100); 
			}else if( P[1].state == 410 ){ if(P[1].key_JOY_LEFT_status>0 || P[1].key_JOY_RIGHT_status>0){ PLAYER_STATE(1,411); }else{ PLAYER_STATE(1,200); }
			}else if( P[1].state == 411 ){ PLAYER_STATE(1,411); 
			}else if( P[1].state == 412 ){ PLAYER_STATE(1,200); 
			}else if( P[1].state == 101 ){ PLAYER_STATE(1,100); 
			}else if( P[1].state == 102 ){ PLAYER_STATE(1,100); 
			}else if( P[1].state == 201 ){ PLAYER_STATE(1,200); 
			}else if( P[1].state == 202 ){ PLAYER_STATE(1,200); 
			}else if( P[1].state == 606 ){ if(P[1].key_JOY_DOWN_status>0){ PLAYER_STATE(1,200); }else{ PLAYER_STATE(1,100); }
			}else if( P[1].state == 888 ){ PLAYER_STATE(1,888); 
			}else if( P[1].state == 348 ){ PLAYER_STATE(1,358); JUMP_INIT(1); P[1].playerLayer+=1; XGM_setPCM(P1_SFX, snd_change_plane, sizeof(snd_change_plane)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); 
			}else if( P[1].state == 342 ){ PLAYER_STATE(1,352); JUMP_INIT(1); P[1].playerLayer-=1; if(P[1].playerLayer==0){P[1].playerLayer=1;} XGM_setPCM(P1_SFX, snd_change_plane, sizeof(snd_change_plane)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); 
			}else if( P[1].state ==   9 ){ PLAYER_STATE(1,100); //**DEAD**; RESTART THE ROUND
			
			//new animation definitions here, if you want
			}
		}
		P[1].frameTimeAtual = 1;
		P[1].frameTimeTotal = P[1].dataAnim[P[1].animFrame];
		
		//"GAMEDEVBOSS" anti glitch animation overflow
		if(P[1].sprite)
		{
			u16 totalreal = P[1].sprite->animation->numFrame;
			if( (P[1].animFrame-1) < totalreal)
			{ 
				if(P[1].sprite){ SPR_setAnimAndFrame(P[1].sprite, P[1].animRow, P[1].animFrame-1); } //set the animation if is OK
				//FUNCAO_DEPTH(i);
			}
		}
		
		//FUNCAO_FSM_HITBOXES(i); //Update Hurt/Hitboxes
	}
}

void PHYSICS()
{
	P[1].groundSensor=0; //check if the player touching ground
	
	//jump controller physics
	if(P[1].jumpTimer>=1 && P[1].jumpTimer<=50)
	{
		P[1].jumpTimer++;
	}else{
		P[1].jumpTimer=0;
	}
	
	//jump
	if( P[1].state==300 || P[1].state==310 )
	{ 
		if(P[1].key_JOY_LEFT_status >=1){ P[1].dir=-1; SPR_setHFlip(P[1].sprite,  TRUE); }
		if(P[1].key_JOY_RIGHT_status>=1){ P[1].dir= 1; SPR_setHFlip(P[1].sprite, FALSE); }
		if(P[1].state==300 && P[1].jumpTimer>20)
		{
			if(P[1].key_JOY_LEFT_status >=1){ P[1].impulseX=-1;}
			if(P[1].key_JOY_RIGHT_status>=1){ P[1].impulseX= 1;}
		}
		if(P[1].state==310)
		{
			if(P[1].key_JOY_LEFT_status >=1){ P[1].impulseX=-2;}
			if(P[1].key_JOY_RIGHT_status>=1){ P[1].impulseX= 2;}
		}		
		if(P[1].key_JOY_LEFT_status==0 && P[1].key_JOY_RIGHT_status==0){ P[1].impulseX=0; }
	}
	
	if(P[1].state==8){ if(P[1].dir==1){P[1].x--;}else{P[1].x++;} P[1].impulseX=0; }
	
	//jump SET forces
	if(( P[1].state>=300 && P[1].state<=399 ) || P[1].state==8 )
	{
		//normal jump definition
		if(P[1].jumpTimer>= 1 && P[1].jumpTimer<=10){ P[1].impulseY = -6; } 
		else if(P[1].jumpTimer >10 && P[1].jumpTimer<=15){ P[1].impulseY = -4; } 
		else if(P[1].jumpTimer >15 && P[1].jumpTimer<=20){ P[1].impulseY = -3; } 
		else if(P[1].jumpTimer >20 && P[1].jumpTimer<=30){ P[1].impulseY =  0; gGravity=0; } 
		else if(P[1].jumpTimer >30 && P[1].jumpTimer<=40){ P[1].impulseY =  3; gGravity=1; } 	
		else if(P[1].jumpTimer >40 && P[1].jumpTimer<=45){ P[1].impulseY =  4; } 
		else if(P[1].jumpTimer >45 && P[1].jumpTimer<=50){ P[1].impulseY =  6; } 
		else if(P[1].jumpTimer==50){P[1].impulseY =  8;}
		
		P[1].velocityY = P[1].impulseY + gGravity; //update velocity
		P[1].impulseY = P[1].velocityY;
		
		//special case 1
		if(P[1].state==358)
		{
			if(P[1].jumpTimer>= 1 && P[1].jumpTimer<= 5){ P[1].impulseY = -8; } //8
			if(P[1].jumpTimer>= 5 && P[1].jumpTimer<=10){ P[1].impulseY = -8; } //4
			if(P[1].jumpTimer >10 && P[1].jumpTimer<=20){ P[1].impulseY = -6; } 	
			if(P[1].jumpTimer >20 && P[1].jumpTimer<=30){ P[1].impulseY = -1; gGravity=0; } 
			if(P[1].jumpTimer >30 && P[1].jumpTimer<=40){ P[1].impulseY =  4; gGravity=1; } 	
			if(P[1].jumpTimer >40 && P[1].jumpTimer<=50){ P[1].impulseY =  8; } 
			
			if(P[1].extraImpulse!=0 && P[1].jumpTimer <30){P[1].impulseY+=P[1].extraImpulse;}
			if(P[1].jumpTimer>30){P[1].extraImpulse=0;}
			
			P[1].velocityY = P[1].impulseY;
		}
		
		//special case 2
		if(P[1].state==352)
		{
			if(P[1].jumpTimer>= 1 && P[1].jumpTimer<= 5){ P[1].impulseY = -4; } //8
			if(P[1].jumpTimer>= 5 && P[1].jumpTimer<=10){ P[1].impulseY = -4; } //4
			if(P[1].jumpTimer >10 && P[1].jumpTimer<=20){ P[1].impulseY = -2; } 	
			if(P[1].jumpTimer >20 && P[1].jumpTimer<=30){ P[1].impulseY =  4; gGravity=0; } 
			if(P[1].jumpTimer >30 && P[1].jumpTimer<=40){ P[1].impulseY =  8; gGravity=1; } 	
			if(P[1].jumpTimer >40 && P[1].jumpTimer<=50){ P[1].impulseY = 10; } 
			
			if(P[1].extraImpulse!=0 && P[1].jumpTimer <30){P[1].impulseY+=P[1].extraImpulse;}
			if(P[1].jumpTimer>30){P[1].extraImpulse=0;}
			
			P[1].velocityY = P[1].impulseY;
		}
		
		
	}else{
		P[1].velocityY = gGravity;
	}
	
	if(gMapID==4 && P[1].jumpTimer==30) //change priority in 2-1
	{
		if(P[1].state==358){ SPR_setPriority(P[1].sprite, 0); }//sobe
		if(P[1].state==352){ SPR_setPriority(P[1].sprite, 1); }//desce
	}
	
	//walk
	if(P[1].state==401){ P[1].impulseX=WALK_SPEED*P[1].dir; }; if(P[1].key_JOY_LEFT_status==0 && P[1].key_JOY_RIGHT_status==0){P[1].impulseX=0;}
	if(P[1].state==411){ P[1].impulseX=(WALK_SPEED/2)*P[1].dir; }; if(P[1].key_JOY_LEFT_status==0 && P[1].key_JOY_RIGHT_status==0){P[1].impulseX=0;}
	
	bool resetVelocity=FALSE;
	//static and landing on the ground; reset velocity
	if( 
		P[1].state==606 || 
		P[1].state==100 || 
		P[1].state==101 || 
		P[1].state==201 || 
		P[1].state==402 || 
		P[1].state==412 || 
		P[1].state==348 || 
		P[1].state==342 
	) 
	{
		resetVelocity=TRUE;
	}
	if(resetVelocity==TRUE)
	{
		P[1].jumpTimer = 0;
		P[1].impulseX  = 0;
		P[1].impulseY  = 0;
		P[1].velocityX = 0;
		P[1].velocityY = 0;
	}
	
	//--------------------------------------------------------------------------------------------
	
	//apply forces
	//P[1].velocityX = P[1].velocityX+P[1].impulseX;
	P[1].velocityX = P[1].impulseX;
	//if(P[1].velocityX<=-6){ P[1].velocityX=-6; }else if(P[1].velocityX>=6){ P[1].velocityX=6; } //maximum velocityX
	P[1].x += P[1].velocityX;
	//if(P[1].velocityY<=-8){ P[1].velocityY=-8; }else if(P[1].velocityY>=8){ P[1].velocityY=8; } //maximum velocityY
	P[1].y += P[1].velocityY;
	
	//stop player from leaving map
	if (P[1].x < 16) { P[1].x = 16; }
	if (P[1].x > gBG_Width-16) { P[1].x = gBG_Width-16; }
	if (P[1].y < 8) { P[1].y = 8; }
	if (P[1].y > gBG_Height) { P[1].y = gBG_Height; }
	
	//-------------------------------------------------------------------------------------------------------------------------
	//Check the Directional Movement
	
	// 7 | 8 | 9
	// --+---+---
	// 4 | x | 6
	// --+---+---
	// 1 | 2 | 3
	
	P[1].DM=0;
	
	//Horizontal Movement
	if(P[1].y_prev==P[1].y)
	{
		if(P[1].x_prev<P[1].x){ P[1].DM=6; } //left
		else if(P[1].x_prev>P[1].x){ P[1].DM=4; } //right
	}
	if(P[1].x_prev==P[1].x)
	{
		if(P[1].y_prev<P[1].y){ P[1].DM=2; } //up
		else if(P[1].y_prev>P[1].y){ P[1].DM=8; } //down
	}
	
	//Diagonal Movement
	if(P[1].x_prev<P[1].x) //right
	{
		if(P[1].y_prev<P[1].y){ P[1].DM=3; } //right+down
		else if(P[1].y_prev>P[1].y){ P[1].DM=9; } //right+up
		else if(P[1].y_prev==P[1].y){ P[1].DM=6; } //only right
	}
	else if(P[1].x_prev>P[1].x){ //left
		if(P[1].y_prev<P[1].y){ P[1].DM=1; } //left+down
		else if(P[1].y_prev>P[1].y){ P[1].DM=7; } //left+up
		else if(P[1].y_prev==P[1].y){ P[1].DM=4; } //only left
	}
	//-------------------------------------------------------------------------------------------------------------------------
	
	//check collision whith MAP scenario
	for (int i=0; i<=totalMapCollisionBoxes; i++)
	{
		bool collisionTest = 1; //only 1 collision by frame
		
		bool enableTestCollision = FALSE;
		//if((P[1].state==300 || P[1].state==310 || P[1].state==358 || P[1].state==352) && (P[1].jumpTimer>=30 || P[1].jumpTimer==0)){ enableTestCollision=TRUE; }
		if((P[1].state==300 || P[1].state==310 || P[1].state==358 || P[1].state==352) && (P[1].jumpTimer>=30 || P[1].jumpTimer==0)){ enableTestCollision=TRUE; }
		
		//if((P[1].state==310) && (P[1].jumpTimer>=2 || P[1].jumpTimer==0)){ enableTestCollision=TRUE; }
		if((P[1].state==300 || P[1].state==310) && (P[1].jumpTimer>=2 || P[1].jumpTimer==0)){ enableTestCollision=TRUE; } //fix
		
		if(P[1].state!=300 && P[1].state!=310 && P[1].state!=358 && P[1].state!=352){ enableTestCollision=TRUE; }
		
		if((P[1].state==8) && (P[1].jumpTimer>=30 || P[1].jumpTimer==0)){ enableTestCollision=TRUE; }
		
		//MAP PLANE A
		if(( CHECK_COLLISION
		(
			P[1].x-8, P[1].y-32, P[1].x+8, P[1].y, 
			collisionMatrix[i][0], collisionMatrix[i][1], collisionMatrix[i][2], collisionMatrix[i][3] )==1 && collisionTest==1
		) && enableTestCollision==TRUE ) 
		{
			//have collision!
			if(P[1].playerLayer==collisionMatrix[i][4])
			{
				COLLISION_HANDLING(1, collisionMatrix[i][0], collisionMatrix[i][1], collisionMatrix[i][2], collisionMatrix[i][3]);
				collisionTest=0;
			}
			
		}
	}
	
	//SENSOR
	for (int i=0; i<=totalMapCollisionBoxesB; i++)
	{
		bool collisionTest = 1; //only 1 collision by frame
		
		bool enableTestCollision = FALSE;
		if((P[1].state==300 || P[1].state==310) && (P[1].jumpTimer>=30 || P[1].jumpTimer==0)){ enableTestCollision=TRUE; }
		if(P[1].state!=300 && P[1].state!=310){ enableTestCollision=TRUE; }
		
		//DONT CHANGE MY LAYER
		if(gMapID== 6){ P[1].playerLayer=1; } //2-3
		if(gMapID== 8){ P[1].playerLayer=1; } //3-1
		if(gMapID== 9){ P[1].playerLayer=1; } //3-2
		if(gMapID==13){ P[1].playerLayer=1; } //4-2
		if(P[1].playerLayer>2){P[1].playerLayer=2;} //FIX, limit the Layer to 2
		
		//update planeChangerSensor
		if(P[1].state==100 || P[1].state==101 || P[1].state==888 || P[1].state==200)
		{
			if(( CHECK_COLLISION
			(
				P[1].x-8, P[1].y-32, P[1].x+8, P[1].y, 
				collisionMatrixB[i][0], collisionMatrixB[i][1], collisionMatrixB[i][2], collisionMatrixB[i][3] )==1 && collisionTest==1
			) && enableTestCollision==TRUE )
			{
				//have collision!
				//COLLISION_HANDLING(1, collisionMatrix[i][0], collisionMatrix[i][1], collisionMatrix[i][2], collisionMatrix[i][3]);
				collisionTest=0;
				P[1].planeChangerSensor=TRUE;
				
				if(collisionMatrixB[i][4]==1){ P[1].planeChangerUPReady = TRUE; }
				if(collisionMatrixB[i][4]==2){ P[1].planeChangerUPReady = TRUE; P[1].planeChangerDOWNReady = TRUE; }
				if(collisionMatrixB[i][4]==3){ P[1].planeChangerDOWNReady = TRUE; }
				
				//------------------------------------------------------------------------------------------------------------
				//CHANGE SENSOR BEHAVIOUR
				//EXTRA IMPULSE; 
				if(gMapID==4 && i==8){ P[1].extraImpulse=-2; }
				if(gMapID==9 && 
				(i==0 || i==2 || i==4 || i==6 || i==8 || i==10 || i==12 || i==14)
				){ if(P[1].state==200){P[1].extraImpulse= 4;}else{P[1].extraImpulse= 0;} }
				
				//------------------------------------------------------------------------------------------------------------
			}
		}else{
			P[1].planeChangerSensor=FALSE;
			P[1].planeChangerUPReady = FALSE;
			P[1].planeChangerDOWNReady = FALSE;
		}
		
	}
	
	if(P[1].state==401 || P[1].state==411)
	{
		bool collisionTest = 1;
		for(int i=0; i<TOTAL_MAP_BOXES; i++)
		{
			if(( CHECK_COLLISION
			(
				P[1].x-8, P[1].y, P[1].x+8, P[1].y+4, 
				collisionMatrix[i][0], collisionMatrix[i][1], collisionMatrix[i][2], collisionMatrix[i][3] )==1 && collisionTest==1
			) ) 
			{
				//have collision!
				P[1].groundSensor=1;
			}
		}
	}
	
	//fall
	if(P[1].groundSensor==0)
	{
		if(P[1].state==401 || P[1].state==411)
		{ 
			PLAYER_STATE(1, 300); 
			//SPR_setAnimAndFrame(P[1].sprite, 6, 7-1); 
			//P[1].animFrame=7; 
			//P[1].animTimeAtual=2; 
			//P[1].totalAnimationFrames=14; 
		}
		P[1].impulseY = 3; //suave falling
		P[1].velocityY = 0;
	}
	
}

void JUMP_INIT(u8 player)
{
	P[player].jumpTimer = 1;
	P[player].y -= gGravity+1;
	P[player].y -= 4;
}

void COLLISION_HANDLING(u8 player, s32 RectX1, s32 RectY1, s32 RectX2, s32 RectY2) // COLLISION_HANDLING by "GameDevBoss"
{
	//Esta é uma versao simplificada do meu sistema de colisao.
	//A versao completa nao é necessaria. GDB
	
	// Initial data
	s32 colisionBoxCenterY = (((RectY2-RectY1)/2)+RectY1); 
	s32 playerCenterY = P[player].y-16;
	
	//Collision Handling (Horizontal)
	if(P[player].DM==8){ P[player].y = RectY2+32; } //up
	if(P[player].DM==2){ P[player].y = RectY1;	  } //down
	if(P[player].DM==6){ P[player].x = RectX1-8;  } //left
	if(P[player].DM==4){ P[player].x = RectX2+8;  } //right
	
	u8 jumpmargin=8; //Decreases test accuracy by making it easier to jump on platforms
	
	//Collision Handling (Diagonal)
	if(P[player].DM==1){ if(playerCenterY+jumpmargin<RectY1){P[player].y = RectY1;}else{P[player].x = RectX2+8;} }
	if(P[player].DM==3){ if(playerCenterY+jumpmargin<RectY1){P[player].y = RectY1;}else{P[player].x = RectX1-8;} }
	if(P[player].DM==7){ if(playerCenterY-jumpmargin>RectY2){P[player].y = RectY2+32;}else{P[player].x = RectX2+8;} }
	if(P[player].DM==9){ if(playerCenterY-jumpmargin>RectY2){P[player].y = RectY2+32;}else{P[player].x = RectX1-8;} }
	if(P[player].DM==0){ if(playerCenterY<colisionBoxCenterY){P[player].y = RectY1;}else{P[player].y = RectY2+32;} } //prevent bug "stuck inside the wall"
	
	if(P[player].DM==1 || P[player].DM==2 || P[player].DM==3){ P[player].groundSensor=1; }
	
	//Change State
	if( ( ((P[player].state>=300 && P[player].state<=399)|| P[player].state==8) ) && (P[player].DM==1 || P[player].DM==2 || P[player].DM==3) && (P[player].y==RectY1) )
	{ 
		if( P[player].state==300 || P[player].state==310 || P[player].state==358 || P[player].state==352 || P[player].state==8) //landing in the ground
		{ 
			PLAYER_STATE(player, 606); //ground
			P[player].extraImpulse = 0;
			P[player].jumpTimer = 0;
			P[player].impulseX  = 0;
			P[player].impulseY  = 0;
			P[player].velocityX = 0;
			P[player].velocityY = 0;
			FXOBJECT(P[player].x, P[player].y, 1); 
		} 
	}
	
}

void PLAY_SND(u16 sndNumber)
{
	if(sndNumber==9){ XGM_setPCM(P1_SFX, snd_player_dead, sizeof(snd_player_dead)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3); }
}

void CAMERA() 
{
	//position of player on map as a whole number
	int player_x_map_integer = P[1].x; //fix32ToInt(P[1].x);
	int player_y_map_integer = P[1].y+5; //fix32ToInt(P[1].y);
	
	//player position on screen
	int player_x_position_on_screen = player_x_map_integer - current_camera_x;
	int player_y_position_on_screen = player_y_map_integer - current_camera_y;

	//calculate new camera position
	if (player_x_position_on_screen > HOW_FAR_TO_RIGHT_BEFORE_CAMERA_MOVES) {
		new_camera_x = player_x_map_integer - HOW_FAR_TO_RIGHT_BEFORE_CAMERA_MOVES;
	}
	else if (player_x_position_on_screen < HOW_FAR_TO_LEFT_BEFORE_CAMERA_MOVES) {
		new_camera_x = player_x_map_integer - HOW_FAR_TO_LEFT_BEFORE_CAMERA_MOVES;
	}
	else new_camera_x = current_camera_x;

	if (player_y_position_on_screen > HOW_FAR_TO_BOTTOM_BEFORE_CAMERA_MOVES) {
		new_camera_y = player_y_map_integer - HOW_FAR_TO_BOTTOM_BEFORE_CAMERA_MOVES;
	}
	else if (player_y_position_on_screen < HOW_FAR_TO_TOP_BEFORE_CAMERA_MOVES) {
		new_camera_y = player_y_map_integer - HOW_FAR_TO_TOP_BEFORE_CAMERA_MOVES;
	}
	else new_camera_y = current_camera_y;

	//stop camera from going beyond boundaries of map
	if (new_camera_x <= 0) {
		new_camera_x = 0;
	}
	else if (new_camera_x > (gBG_Width - HORIZONTAL_RESOLUTION)) {
		new_camera_x = gBG_Width - HORIZONTAL_RESOLUTION;
	}
	if (new_camera_y <= 0) {
		new_camera_y = 0;
	}
	else if (new_camera_y > (gBG_Height - VERTICAL_RESOLUTION)) {
		new_camera_y = gBG_Height - VERTICAL_RESOLUTION;
	}

	//check if camera needs to scroll and do the scroll
	if ( (current_camera_x != new_camera_x) || (current_camera_y != new_camera_y) ) {
		current_camera_x = new_camera_x;
		current_camera_y = new_camera_y;

		MAP_scrollTo(level_map, new_camera_x, new_camera_y);
		if(gMapID==13){ MAP_scrollTo(level_mapb, new_camera_x, new_camera_y); //fixed with to BGA
		}else{ MAP_scrollTo(level_mapb, new_camera_x/2, new_camera_y/2); } //BGB scrolling
	}

	SPR_setPosition(P[1].sprite, (P[1].x-P[1].axisX) - new_camera_x, (P[1].y-P[1].axisY) - new_camera_y);
	
	//anim objects
	if(AnimObj[0].sprite){ SPR_setPosition(AnimObj[0].sprite, (AnimObj[0].x) - new_camera_x, (AnimObj[0].y) - new_camera_y); }
	if(AnimObj[1].sprite){ SPR_setPosition(AnimObj[1].sprite, (AnimObj[1].x) - new_camera_x, (AnimObj[1].y) - new_camera_y); }
	if(AnimObj[3].sprite){ SPR_setPosition(AnimObj[3].sprite, (AnimObj[3].x) - new_camera_x, (AnimObj[3].y) - new_camera_y); }
	
	if(AnimObj[2].sprite)
	{ 
		if(gPing10==2 || gPing10==4 || gPing10==6){ 
			SPR_setPosition(AnimObj[2].sprite, (AnimObj[2].x) - new_camera_x, (AnimObj[2].y) - new_camera_y);
		}else{
			SPR_setPosition(AnimObj[2].sprite, (AnimObj[2].x) - new_camera_x, (AnimObj[2].y) - new_camera_y-96);
		}
	}
	
}

void CLEAR_VDP()
{
	SYS_disableInts();
	 SPR_reset();
	 //VDP_resetSprites();
	 //VDP_releaseAllSprites();
	 //SPR_defragVRAM();
	 VDP_clearPlane(BG_A, TRUE);
	 VDP_clearPlane(BG_B, TRUE);
	 VDP_setTextPlane(BG_A);  
	 VDP_setHorizontalScroll(BG_B, 0); 
	 VDP_setVerticalScroll(BG_B, 0); 
	 VDP_setHorizontalScroll(BG_A, 0); 
	 VDP_setVerticalScroll(BG_A, 0);
	 PAL_setColors(0, palette_black, 64, DMA);
	 VDP_setBackgroundColor(0);
	 //VDP_resetScreen();
	 SPR_initEx(330);             //420 is the default value of SGDK 1.80
	SYS_enableInts(); 
	gInd_tileset=0;
	gFrames=0;
	if(level_map ){ MEM_free(level_map) ; level_map  = NULL; }
	if(level_mapb){ MEM_free(level_mapb); level_mapb = NULL; }
	if(P[0].sprite){ SPR_releaseSprite(P[0].sprite); P[0].sprite = NULL; } //clear memory
	if(P[1].sprite){ SPR_releaseSprite(P[1].sprite); P[1].sprite = NULL; } //clear memory
	if(P[2].sprite){ SPR_releaseSprite(P[2].sprite); P[2].sprite = NULL; } //clear memory
	for (int i = 0; i < MAX_SMOKE_FX ; i++){ fxobjs[i].active           = FALSE; }
	for (int i = 0; i < MAX_BOMBS    ; i++){ bombs[i].active            = FALSE; }
	for (int i = 0; i < MAX_SHURIKENS; i++){ obj_shuriken[i].active     = FALSE; }
	for (int i = 0; i < MAX_SHURIKENS; i++){ obj_shurikendead[i].active = FALSE; }
	for (int i = 0; i < MAX_ENEMYS   ; i++){ enemys[i].active           = FALSE; }
}

bool CHECK_COLLISION(s32 R1x1, s32 R1y1, s32 R1x2, s32 R1y2, s32 R2x1, s32 R2y1, s32 R2x2, s32 R2y2)
{
	//if(R1x1>R1x2){ s32 temp=0; temp=R1x1; R1x1=R1x2; R1x2=temp; } //normalize box, dont used in this engine
	//if(R2x1>R2x2){ s32 temp=0; temp=R2x1; R2x1=R2x2; R2x2=temp; } //normalize box, dont used in this engine
	
	if ( (R1x1+(R1x2-R1x1)>R2x1) && (R1x1<R2x1+(R2x2-R2x1)) && 
		 (R1y1+(R1y2-R1y1)>R2y1) && (R1y1<R2y1+(R2y2-R2y1)) )
	{ 
		
		if(R1x1==R1x2 && R1y1==R1y2){
			return 0;
			}else if(R2x1==R2x2 && R2y1==R2y2){
				/*dont have collision*/
				return 0;
			}else{
				/*have collision*/
				return 1;
			}
	}else{ 
		/*dont have collision*/
		return 0;
	}
}

void CREATE_STAGE(s16 mapID) //**STG
{
	s16 a=0; s16 b=0; s16 c=0; s16 d=0; s16 e=0;
	
	gInd_tileset=0;
	PAL_setColors(0, palette_black, 64, DMA); 
	VDP_setBackgroundColor(0); //Range 0-63 //4 Paletas de 16 cores = 64 cores
	
	//stage config
	
	if(mapID==0) //reset configs
	{
		totalMapCollisionBoxes=TOTAL_MAP_BOXES;
		for( int i=0; i<totalMapCollisionBoxes; i++ ){
			//fill the matrix with zeros
			collisionMatrix[0][0] = a; 
			collisionMatrix[0][1] = b;
			collisionMatrix[0][2] = c; 
			collisionMatrix[0][3] = d;
			collisionMatrix[0][4] = e;
			
			//fill the matrixB with zeros
			collisionMatrixB[0][0] = a; 
			collisionMatrixB[0][1] = b;
			collisionMatrixB[0][2] = c; 
			collisionMatrixB[0][3] = d;
			collisionMatrixB[0][4] = e;
		}
		
	}
	
	if(mapID==1) //LVL1-1
	{
		gBG_Width  = 2048;
		gBG_Height = 256;
		
		P[1].x=32;
		P[1].y=gBG_Height-32;
		
		//bombs
		BOMB( 808, 112, 1);
		BOMB(1168, 112, 1);
		BOMB(1536, 112, 1);
		BOMB(1755, 192, 1);
		
		//backgrounds
		//BGB
		VDP_loadTileSet(&bg_bgb_lvl11_tileset, gInd_tileset, DMA);
		level_mapb = MAP_create(&bg_bgb_lvl11_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL0, bg_bgb_lvl11_pal.palette->data,DMA);
		gInd_tileset += bg_bgb_lvl11_tileset.numTile;
		MAP_scrollTo(level_mapb, 0, 0);

		//BGA
		VDP_loadTileSet(&bg_bga_lvl11_tileset, gInd_tileset, DMA);
		level_map = MAP_create(&bg_bga_lvl11_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL1, bg_bga_lvl11_pal.palette->data,DMA);
		gInd_tileset += bg_bga_lvl11_tileset.numTile;
		MAP_scrollTo(level_map, 0, 0);
		
		//MAP COLLISION BOXES ---------------------------------------------------------------------
		
		/// MAP BOXES ///
		totalMapCollisionBoxes=13; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxes; i++ )
		{
			//collision boxes coordenades
			
			//plane1
			if(i== 0){ a=   0; b= 224; c=2048; d= 256; e=1; }
			if(i== 1){ a= 736; b= 192; c= 768; d= 224; e=1; }
			if(i== 2){ a=1152; b= 192; c=1184; d= 224; e=1; }
			if(i== 3){ a=1352; b= 192; c=1384; d= 224; e=1; }
			if(i== 4){ a=1680; b= 160; c=1712; d= 192; e=1; }
			if(i== 5){ a=1648; b= 192; c=1776; d= 224; e=1; }
			
			//plane2
			if(i== 6){ a= 512; b=   0; c= 544; d= 120; e=2; }
			if(i== 7){ a= 544; b= 112; c= 896; d= 120; e=2; }
			if(i== 8){ a= 896; b=   0; c= 960; d= 120; e=2; }
			if(i== 9){ a= 960; b= 112; c=1448; d= 120; e=2; }
			if(i==10){ a=1448; b=   0; c=1512; d= 120; e=2; }
			if(i==11){ a=1512; b= 112; c=1864; d= 120; e=2; }
			if(i==12){ a=1864; b=   0; c=1896; d= 120; e=2; }
			if(i==13){ a=1088; b=  80; c=1120; d= 112; e=2; }
			
			//fills the matrix
			collisionMatrix[i][0] = a;
			collisionMatrix[i][1] = b;
			collisionMatrix[i][2] = c;
			collisionMatrix[i][3] = d;
			collisionMatrix[i][4] = e;
		}
		
		/// CHANGE LAYER AREA ///
		totalMapCollisionBoxesB=5; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxesB; i++ )
		{
			//collision boxes coordenades
			if(i== 0){ a= 544; b= 184; c= 896; d= 224; e=1; }
			if(i== 1){ a= 960; b= 184; c=1448; d= 224; e=1; }
			if(i== 2){ a=1512; b= 152; c=1864; d= 224; e=1; }
			if(i== 3){ a= 511; b= 104; c= 896; d= 112; e=3; }
			if(i== 4){ a= 960; b= 104; c=1448; d= 112; e=3; }
			if(i== 5){ a=1512; b= 104; c=1864; d= 112; e=3; }
			
			//prevent stuck glitch
			a+=16;
			c-=16;

			//fills the matrix
			collisionMatrixB[i][0] = a;
			collisionMatrixB[i][1] = b;
			collisionMatrixB[i][2] = c;
			collisionMatrixB[i][3] = d;
			collisionMatrixB[i][4] = e;
		}
		
		// ANIMATED OBJECTS AND OTHERS //
		//detector metais
		AnimObj[0].sprite = SPR_addSpriteExSafe(&spr_lvl11_anim_obj1, -99, -99, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
		AnimObj[0].x = 320;
		AnimObj[0].y = 144;
		if(AnimObj[0].sprite){ SPR_setDepth(AnimObj[0].sprite, 1); }
		
		//monitor
		AnimObj[1].sprite = SPR_addSpriteExSafe(&spr_lvl11_anim_obj2, -99, -99, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
		AnimObj[1].x = 336;
		AnimObj[1].y = 144;
		if(AnimObj[1].sprite){ SPR_setDepth(AnimObj[1].sprite, 255); }
		
		//telao
		//AnimObj[2].sprite = SPR_addSpriteExSafe(&spr_lvl11_anim_obj3, -99, -99, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
		//AnimObj[2].x =  48;
		//AnimObj[2].y =  32;
		//if(AnimObj[1].sprite){ SPR_setDepth(AnimObj[2].sprite, 255); }
		
		//in
		AnimObj[3].sprite = SPR_addSpriteExSafe(&spr_lvl11_anim_obj4, -99, -99, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
		AnimObj[3].x = 1968;
		AnimObj[3].y =  152;
		if(AnimObj[3].sprite){ SPR_setDepth(AnimObj[3].sprite, 255); }
		
	}
	
	if(mapID==2) //LVL1-2
	{
		gBG_Width  = 2000;
		gBG_Height = 416;
		
		P[1].x=32;
		P[1].y=gBG_Height-32;
		
		//bombs
		//BOMB( 176, 224, 1); 
		
		//backgrounds
		//BGB
		VDP_loadTileSet(&bg_bgb_lvl12_tileset, gInd_tileset, DMA);
		level_mapb = MAP_create(&bg_bgb_lvl12_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL0, bg_bgb_lvl12_pal.palette->data,DMA);
		gInd_tileset += bg_bgb_lvl12_tileset.numTile;
		MAP_scrollTo(level_mapb, 0, 0);
		
		//BGA
		VDP_loadTileSet(&bg_bga_lvl12A_tileset, gInd_tileset, DMA);
		level_map = MAP_create(&bg_bga_lvl12A_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL1, bg_bga_lvl12A_pal.palette->data,DMA);
		gInd_tileset += bg_bga_lvl12A_tileset.numTile;
		MAP_scrollTo(level_map, 0, 0);
		
		//MAP COLLISION BOXES ---------------------------------------------------------------------
		
		/// MAP BOXES ///
		totalMapCollisionBoxes=18; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxes; i++ )
		{
			//collision boxes coordenades
			
			//plane1
			if(i== 0){ a=   0; b= 384; c=1632; d= 416; e=1; }
			if(i== 1){ a= 512; b= 320; c=1536; d= 384; e=1; }
			if(i== 2){ a= 640; b= 256; c=1472; d= 320; e=1; }
			if(i== 3){ a= 704; b= 192; c=1408; d= 256; e=1; }
			if(i== 4){ a= 768; b= 128; c=1344; d= 192; e=1; }
			if(i== 5){ a= 832; b=  64; c=1088; d= 128; e=1; }
			if(i== 6){ a=1216; b=  64; c=1280; d= 128; e=1; }
			if(i== 7){ a=1632; b= 368; c=1648; d= 384; e=1; }
			if(i== 8){ a=1648; b= 352; c=1664; d= 368; e=1; }
			if(i== 9){ a=1664; b= 336; c=1680; d= 352; e=1; }
			if(i==10){ a=1680; b= 320; c=1696; d= 336; e=1; }
			if(i==11){ a=1696; b= 304; c=1712; d= 320; e=1; }
			if(i==12){ a=1712; b= 288; c=1728; d= 304; e=1; }
			if(i==13){ a=1728; b= 272; c=1760; d= 288; e=1; }
			if(i==14){ a=1760; b= 256; c=1776; d= 272; e=1; }
			if(i==15){ a=1776; b= 240; c=1792; d= 256; e=1; }
			if(i==16){ a=1792; b= 224; c=1808; d= 240; e=1; }
			if(i==17){ a=1808; b= 208; c=1888; d= 224; e=1; }
			if(i==18){ a=1888; b=  72; c=1904; d= 208; e=1; }
			
			//fills the matrix
			collisionMatrix[i][0] = a;
			collisionMatrix[i][1] = b;
			collisionMatrix[i][2] = c;
			collisionMatrix[i][3] = d;
			collisionMatrix[i][4] = e;
		}
		
		//escada aviao
		AnimObj[0].sprite = SPR_addSpriteExSafe(&spr_element_lvl12A, -99, -99, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
		AnimObj[0].x = 1648;
		AnimObj[0].y = 176;
		if(AnimObj[0].sprite){ SPR_setDepth(AnimObj[0].sprite, 1); }
		
	}

	if(mapID==3) //LVL1-3
	{
		gBG_Width  = 512;
		gBG_Height = 224;
		
		P[1].x=32;
		P[1].y=gBG_Height-56;
		
		//bombs
		//BOMB( 808, 112, 1);
		//BOMB(1168, 112, 1);
		//BOMB(1536, 112, 1);
		//BOMB(1755, 192, 1);
		
		//ENEMYS(104+32*1, 224, 1, -1);
		//ENEMYS(104+32*2, 224, 1, -1);
		//ENEMYS(104+32*3, 224, 1, -1);
		//ENEMYS(104+32*4, 224, 1, -1);
		//ENEMYS(104+32*5, 224, 1, -1);
		//ENEMYS(104+32*6, 224, 1, -1);
		
		//backgrounds
		//BGB
		VDP_loadTileSet(&bg_bgb_lvl13_tileset, gInd_tileset, DMA);
		level_mapb = MAP_create(&bg_bgb_lvl13_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL0, bg_bgb_lvl13_pal.palette->data,DMA);
		gInd_tileset += bg_bgb_lvl13_tileset.numTile;
		MAP_scrollTo(level_mapb, 0, 0);

		//BGA
		VDP_loadTileSet(&bg_bga_lvl13_tileset, gInd_tileset, DMA);
		level_map = MAP_create(&bg_bga_lvl13_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL1, bg_bga_lvl13_pal.palette->data,DMA);
		gInd_tileset += bg_bga_lvl13_tileset.numTile;
		MAP_scrollTo(level_map, 0, 0);
		
		//MAP COLLISION BOXES ---------------------------------------------------------------------
		
		/// MAP BOXES ///
		totalMapCollisionBoxes=0; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxes; i++ )
		{
			//collision boxes coordenades
			
			//plane1
			if(i== 0){ a=   0; b= gBG_Height-56; c=gBG_Width; d= gBG_Height; e=1; }
			
			//fills the matrix
			collisionMatrix[i][0] = a;
			collisionMatrix[i][1] = b;
			collisionMatrix[i][2] = c;
			collisionMatrix[i][3] = d;
			collisionMatrix[i][4] = e;
		}
		
		/*
		/// CHANGE LAYER AREA ///
		totalMapCollisionBoxesB=5; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxesB; i++ )
		{
			//collision boxes coordenades
			if(i== 0){ a= 544; b= 184; c= 896; d= 224; e=1; }
			if(i== 1){ a= 960; b= 184; c=1448; d= 224; e=1; }
			if(i== 2){ a=1512; b= 152; c=1864; d= 224; e=1; }
			if(i== 3){ a= 511; b= 104; c= 896; d= 112; e=3; }
			if(i== 4){ a= 960; b= 104; c=1448; d= 112; e=3; }
			if(i== 5){ a=1512; b= 104; c=1864; d= 112; e=3; }
			
			//prevent stuck glitch
			a+=16;
			c-=16;

			//fills the matrix
			collisionMatrixB[i][0] = a;
			collisionMatrixB[i][1] = b;
			collisionMatrixB[i][2] = c;
			collisionMatrixB[i][3] = d;
			collisionMatrixB[i][4] = e;
		}
		*/
		
	}
	
	if(mapID==4) //LVL2-1
	{
		gBG_Width  = 2048;
		gBG_Height = 256;
		
		P[1].x=32;
		P[1].y=gBG_Height-24;
		
		//bombs
		//BOMB( 808, 112, 1);
		//BOMB(1168, 112, 1);
		//BOMB(1536, 112, 1);
		//BOMB(1755, 192, 1);
		
		//ENEMYS(104+32*1, 224, 1, -1);
		//ENEMYS(104+32*2, 224, 1, -1);
		//ENEMYS(104+32*3, 224, 1, -1);
		//ENEMYS(104+32*4, 224, 1, -1);
		//ENEMYS(104+32*5, 224, 1, -1);
		//ENEMYS(104+32*6, 224, 1, -1);
		
		//backgrounds
		//BGB
		VDP_loadTileSet(&bg_bgb_lvl21_tileset, gInd_tileset, DMA);
		level_mapb = MAP_create(&bg_bgb_lvl21_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL0, bg_bgb_lvl21_pal.palette->data,DMA);
		gInd_tileset += bg_bgb_lvl21_tileset.numTile;
		MAP_scrollTo(level_mapb, 0, 0);

		//BGA
		VDP_loadTileSet(&bg_bga_lvl21_tileset, gInd_tileset, DMA);
		level_map = MAP_create(&bg_bga_lvl21_map, BG_A, TILE_ATTR_FULL(PAL1, TRUE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL1, bg_bga_lvl21_pal.palette->data,DMA);
		gInd_tileset += bg_bga_lvl21_tileset.numTile;
		MAP_scrollTo(level_map, 0, 0);
		
		//MAP COLLISION BOXES ---------------------------------------------------------------------
		
		/// MAP BOXES ///
		totalMapCollisionBoxes=25; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxes; i++ )
		{
			//collision boxes coordenades
			
			//plane1
			if(i== 0){ a=   0; b=232; c=2048; d= 256; e=1; }
			if(i== 1){ a= 304; b=160; c= 464; d= 232; e=1; }
			if(i== 2){ a= 464; b=192; c= 624; d= 232; e=1; }
			if(i== 3){ a= 624; b=160; c= 816; d= 232; e=1; }
			if(i== 4){ a= 816; b=128; c= 912; d= 232; e=1; }
			if(i== 5){ a= 912; b=160; c= 944; d= 232; e=1; }
			if(i== 6){ a=1456; b=112; c=1792; d= 232; e=1; }
			if(i== 7){ a=1792; b=144; c=1872; d= 232; e=1; }
			if(i== 8){ a=1872; b=160; c=1928; d= 232; e=1; }
			
			//plane2
			if(i== 9){ a= 144; b=   0; c= 152; d= 152; e=2; }
			if(i==10){ a= 152; b= 144; c= 480; d= 152; e=2; }
			if(i==11){ a= 480; b=   0; c= 488; d= 152; e=2; }
			if(i==12){ a= 632; b=   0; c= 640; d= 152; e=2; }
			if(i==13){ a= 640; b= 144; c= 800; d= 152; e=2; }
			if(i==14){ a= 800; b=   0; c= 808; d= 152; e=2; }
			if(i==15){ a= 960; b=  88; c= 968; d= 224; e=2; }
			if(i==16){ a= 968; b= 216; c=1176; d= 224; e=2; }
			if(i==17){ a=1176; b=  88; c=1184; d= 224; e=2; }
			if(i==18){ a=1336; b=   0; c=1344; d= 224; e=2; }
			if(i==19){ a=1344; b= 216; c=1408; d= 224; e=2; }
			if(i==20){ a=1408; b=  88; c=1416; d= 224; e=2; }
			if(i==21){ a=1408; b=  80; c=1600; d=  88; e=2; }
			if(i==22){ a=1600; b=   0; c=1608; d=  88; e=2; }
			if(i==23){ a=1720; b=   0; c=1728; d=  88; e=2; }
			if(i==24){ a=1728; b=  80; c=1856; d=  88; e=2; }
			if(i==25){ a=1856; b=   0; c=1864; d=  88; e=2; }
			
			//fills the matrix
			collisionMatrix[i][0] = a;
			collisionMatrix[i][1] = b;
			collisionMatrix[i][2] = c;
			collisionMatrix[i][3] = d;
			collisionMatrix[i][4] = e;
		}
		
		/// CHANGE LAYER AREA ///
		totalMapCollisionBoxesB=14; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxesB; i++ )
		{
			//collision boxes coordenades
			if(i== 0){ a= 152; b= 224-8; c= 304; d= 232; e=1; }
			if(i== 1){ a= 304; b= 152; c= 464; d= 160; e=1; }
			if(i== 2){ a= 464; b= 184; c= 480; d= 192; e=1; }
			if(i== 3){ a= 152; b= 136; c= 480+16; d= 144; e=3; }
			if(i== 4){ a= 640; b= 152; c= 800; d= 160; e=1; }
			if(i== 5){ a= 640; b= 136; c= 800+16; d= 144; e=3; }
			if(i== 6){ a= 968; b= 224; c=1176; d= 232; e=1; }
			if(i== 7){ a= 968; b= 208; c=1176; d= 216; e=3; }
			if(i== 8){ a=1344; b= 224; c=1456+16; d= 232; e=1; }
			if(i== 9){ a=1456; b= 104; c=1600; d= 112; e=1; }
			if(i==10){ a=1344; b= 208; c=1408; d= 216; e=3; }
			if(i==11){ a=1408; b=  72; c=1600+16; d=  80; e=3; }
			if(i==12){ a=1728; b= 104; c=1792; d= 112; e=1; }
			if(i==13){ a=1792; b= 136; c=1856; d= 144; e=1; }
			if(i==14){ a=1728; b=  72; c=1856+16; d=  80; e=3; }
			
			//prevent stuck glitch
			a+=16;
			c-=16;

			//fills the matrix
			collisionMatrixB[i][0] = a;
			collisionMatrixB[i][1] = b;
			collisionMatrixB[i][2] = c;
			collisionMatrixB[i][3] = d;
			collisionMatrixB[i][4] = e;
		}
		
		// ANIMATED OBJECTS AND OTHERS //
		//caminhaoA
		AnimObj[0].sprite = SPR_addSpriteExSafe(&spr_element_lvl21A, -99, -99, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
		AnimObj[0].x = 1480;
		AnimObj[0].y = 144;
		if(AnimObj[0].sprite){ SPR_setDepth(AnimObj[0].sprite, 255); }
		//caminhaoB
		AnimObj[1].sprite = SPR_addSpriteExSafe(&spr_element_lvl21B, -99, -99, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
		AnimObj[1].x = 1632+16;
		AnimObj[1].y = 144+16;
		if(AnimObj[1].sprite){ SPR_setDepth(AnimObj[1].sprite, 255); }
		//caminhaoC
		//AnimObj[2].sprite = SPR_addSpriteExSafe(&spr_element_lvl21C, -99, -99, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
		//AnimObj[2].x = 1784;
		//AnimObj[2].y = 144;
		//if(AnimObj[2].sprite){ SPR_setDepth(AnimObj[2].sprite, 1); }
		
	}
	
	if(mapID==5) //LVL2-2
	{
		gBG_Width  = 2560;
		gBG_Height = 512;
		
		P[1].x=32;
		P[1].y=224;
		
		//bombs
		//BOMB( 808, 112, 1);
		//BOMB(1168, 112, 1);
		//BOMB(1536, 112, 1);
		//BOMB(1755, 192, 1);
		
		//ENEMYS(104+32*1, 224, 1, -1);
		//ENEMYS(104+32*2, 224, 1, -1);
		//ENEMYS(104+32*3, 224, 1, -1);
		//ENEMYS(104+32*4, 224, 1, -1);
		//ENEMYS(104+32*5, 224, 1, -1);
		//ENEMYS(104+32*6, 224, 1, -1);
		
		//backgrounds
		//BGB
		VDP_loadTileSet(&bg_bgb_lvl22_tileset, gInd_tileset, DMA);
		level_mapb = MAP_create(&bg_bgb_lvl22_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL0, bg_bgb_lvl22_pal.palette->data,DMA);
		gInd_tileset += bg_bgb_lvl22_tileset.numTile;
		MAP_scrollTo(level_mapb, 0, 0);
		
		//BGA
		VDP_loadTileSet(&bg_bga_lvl22A_tileset, gInd_tileset, DMA);
		level_map = MAP_create(&bg_bga_lvl22A_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL1, bg_bga_lvl22A_pal.palette->data,DMA);
		gInd_tileset += bg_bga_lvl22A_tileset.numTile;
		MAP_scrollTo(level_map, 0, 0);
		
		//MAP COLLISION BOXES ---------------------------------------------------------------------
		
		/// MAP BOXES ///
		totalMapCollisionBoxes=64; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxes; i++ )
		{
			//collision boxes coordenades
			
			//plane1
			if(i== 0){ a=   0; b= 224; c= 456; d= 240; e=1; }
			if(i== 1){ a= 224; b= 192; c= 288; d= 224; e=1; }
			if(i== 2){ a= 288; b= 160; c= 320; d= 224; e=1; }
			if(i== 3){ a= 496; b=   0; c= 512; d= 240; e=1; }
			//...
			if(i==19){ a= 480; b= 464; c= 512; d= 480; e=1; }
			if(i==20){ a=   0; b= 488; c= 480; d= 512; e=1; }
			if(i==21){ a= 512; b= 416; c= 552; d= 464; e=1; }
			if(i==22){ a= 552; b= 352; c= 680; d= 464; e=1; }
			if(i==23){ a= 680; b= 448; c= 744; d= 464; e=1; }
			if(i==24){ a= 744; b= 416; c= 776; d= 464; e=1; }
			if(i==25){ a=0; b=0; c=0; d=0; e=1; }
			if(i==26){ a= 776; b= 448; c= 840; d= 464; e=1; }
			if(i==27){ a= 840; b= 416; c= 936; d= 464; e=1; }
			if(i==28){ a= 936; b= 448; c=1000; d= 464; e=1; }
			if(i==29){ a=0; b=0; c=0; d=0; e=1; }
			if(i==30){ a=1000; b= 416; c=1064; d= 464; e=1; }
			if(i==31){ a=1064; b= 384; c=1096; d= 464; e=1; }
			if(i==32){ a=1096; b= 352; c=1128; d= 464; e=1; }
			if(i==33){ a=1128; b= 384; c=1160; d= 464; e=1; }
			if(i==34){ a=1160; b= 416; c=1224; d= 464; e=1; }
			if(i==35){ a=0; b=0; c=0; d=0; e=1; }
			if(i==36){ a=1224; b= 448; c=1256; d= 464; e=1; }
			if(i==37){ a=1256; b= 416; c=1320; d= 464; e=1; }
			if(i==38){ a=1320; b= 384; c=1416; d= 464; e=1; }
			if(i==39){ a=1416; b= 352; c=1448; d= 464; e=1; }
			if(i==40){ a=1448; b= 384; c=1512; d= 464; e=1; }
			if(i==41){ a=1512; b= 416; c=1544; d= 464; e=1; }
			if(i==42){ a=0; b=0; c=0; d=0; e=1; }
			if(i==43){ a=1544; b= 448; c=1608; d= 464; e=1; }
			if(i==44){ a=1608; b= 416; c=1672; d= 464; e=1; }
			if(i==45){ a=1672; b= 448; c=1960; d= 464; e=1; }
			if(i==46){ a=1768; b= 416; c=1800; d= 448; e=1; }
			if(i==47){ a=1832; b= 416; c=1864; d= 448; e=1; }
			if(i==48){ a=1960; b= 416; c=2056; d= 448; e=1; }
			if(i==49){ a=1992; b= 384; c=2024; d= 416; e=1; }
			if(i==50){ a=2056; b= 416; c=2304; d= 416; e=1; }
			if(i==51){ a=2056; b= 352; c=2088; d= 416; e=1; }
			if(i==52){ a=2088; b= 384; c=2120; d= 416; e=1; }
			if(i==53){ a=2120; b= 320; c=2152; d= 416; e=1; }
			if(i==54){ a=2152; b= 288; c=2184; d= 416; e=1; }
			if(i==55){ a=2184; b= 320; c=2216; d= 416; e=1; }
			if(i==56){ a=2216; b= 352; c=2248; d= 416; e=1; }
			if(i==57){ a=2248; b= 384; c=2280; d= 416; e=1; }
			if(i==58){ a=2304; b= 232; c=2320; d= 464; e=1; }
			
			//plane2
			if(i==59){ a=2048; b=   0; c=2064; d= 224+ 8; e=2; }
			if(i==60){ a=2064-32; b= 224; c=2560; d= 232+16; e=2; }
			if(i==61){ a=2120; b= 192; c=2152; d= 224; e=2; }
			if(i==62){ a=2304; b= 192; c=2560; d= 224; e=2; }
			if(i==63){ a=2336; b= 128; c=2400; d= 192; e=2; }
			if(i==64){ a=2400; b= 160; c=2464; d= 192; e=2; }
			
			//fills the matrix
			collisionMatrix[i][0] = a;
			collisionMatrix[i][1] = b;
			collisionMatrix[i][2] = c;
			collisionMatrix[i][3] = d;
			collisionMatrix[i][4] = e;
		}
		
		/// CHANGE LAYER AREA ///
		totalMapCollisionBoxesB=1; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxesB; i++ )
		{
			//collision boxes coordenades
			if(i== 0){ a=2152; b= 280; c=2184; d= 288; e=1; }
			if(i== 1){ a=2152; b= 216; c=2184; d= 224; e=3; }
			
			//prevent stuck glitch
			//a+=16;
			//c-=16;

			//fills the matrix
			collisionMatrixB[i][0] = a;
			collisionMatrixB[i][1] = b;
			collisionMatrixB[i][2] = c;
			collisionMatrixB[i][3] = d;
			collisionMatrixB[i][4] = e;
		}
		
	}
	
	if(mapID==6) //LVL2-3
	{
		gBG_Width  = 2048;
		gBG_Height = 512;
		
		P[1].x=32;
		P[1].y=224;
		
		//bombs
		//BOMB( 808, 112, 1);
		//BOMB(1168, 112, 1);
		//BOMB(1536, 112, 1);
		//BOMB(1755, 192, 1);
		
		//ENEMYS(104+32*1, 224, 1, -1);
		
		//backgrounds
		//BGB
		VDP_loadTileSet(&bg_bgb_lvl23_tileset, gInd_tileset, DMA);
		level_mapb = MAP_create(&bg_bgb_lvl23_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL0, bg_bgb_lvl23_pal.palette->data,DMA);
		gInd_tileset += bg_bgb_lvl23_tileset.numTile;
		MAP_scrollTo(level_mapb, 0, 0);

		//BGA
		VDP_loadTileSet(&bg_bga_lvl23_tileset, gInd_tileset, DMA);
		level_map = MAP_create(&bg_bga_lvl23_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL1, bg_bga_lvl23_pal.palette->data,DMA);
		gInd_tileset += bg_bga_lvl23_tileset.numTile;
		MAP_scrollTo(level_map, 0, 0);
		
		//MAP COLLISION BOXES ---------------------------------------------------------------------
		
		totalMapCollisionBoxes=34; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxes; i++ )
		{
			//collision boxes coordenades
			
			//plane1
			if(i== 0){ a=   0; b= 224; c= 832; d= 256; e=1; }
			if(i== 1){ a= 352; b= 160; c= 384; d= 224; e=1; }
			if(i== 2){ a= 384; b= 192; c= 448; d= 224; e=1; }
			if(i== 3){ a= 600; b= 128; c= 672; d= 136; e=1; }
			if(i== 4){ a= 768; b= 128; c= 992; d= 136; e=1; }
			if(i== 5){ a=1024; b=   0; c=1536; d= 224; e=1; }
			if(i== 6){ a=1536; b= 128; c=1696; d= 136; e=1; }
			if(i== 7){ a=1824; b= 128; c=1968; d= 136; e=1; }
			if(i== 8){ a=1536; b= 224; c=2048; d= 256; e=1; }
			if(i== 9){ a= 928; b= 224; c=1024; d= 256; e=1; }
			if(i==10){ a= 896; b= 256; c= 912; d= 280; e=1; }
			if(i==11){ a= 912; b= 256; c= 936; d= 328; e=1; }
			if(i==12){ a= 936; b= 328; c= 968; d= 344; e=1; }
			if(i==13){ a=1016; b= 256; c=1040; d= 328; e=1; }
			if(i==14){ a= 528; b= 256; c= 544; d= 480; e=1; }
			if(i==15){ a= 528; b= 256; c= 616; d= 328; e=1; }
			if(i==16){ a= 616; b= 328; c= 696; d= 336; e=1; }
			if(i==17){ a= 696; b= 256; c= 704; d= 328; e=1; }
			if(i==18){ a= 576; b= 424; c= 736; d= 480; e=1; }
			if(i==19){ a= 544; b= 480; c=2048; d= 512; e=1; }
			if(i==20){ a= 800; b= 424; c= 832; d= 480; e=1; }
			if(i==21){ a= 896; b= 424; c=1056; d= 480; e=1; }
			if(i==22){ a=1160; b= 424+8; c=1200; d= 480; e=1; }
			if(i==23){ a=1200; b= 432; c=1488; d= 480; e=1; }
			if(i==24){ a=1216; b= 416; c=1472; d= 432; e=1; }
			if(i==25){ a=1248; b= 384; c=1344; d= 416; e=1; }
			if(i==26){ a=1376; b= 384; c=1440; d= 416; e=1; }
			if(i==27){ a=1488; b= 424+8; c=1528; d= 480; e=1; }
			if(i==28){ a=1600; b= 424; c=1632; d= 480; e=1; }
			if(i==29){ a=1696; b= 424; c=1856; d= 480; e=1; }
			if(i==30){ a=1712; b= 256; c=1736; d= 328; e=1; }
			if(i==31){ a=1784; b= 328; c=1816; d= 344; e=1; }
			if(i==32){ a=1816; b= 256; c=1840; d= 328; e=1; }
			if(i==33){ a=1888; b= 376; c=2016; d= 480; e=1; }
			if(i==34){ a=2016; b= 256; c=2048; d= 480; e=1; }
			
			//fills the matrix
			collisionMatrix[i][0] = a;
			collisionMatrix[i][1] = b;
			collisionMatrix[i][2] = c;
			collisionMatrix[i][3] = d;
			collisionMatrix[i][4] = e;
		}
		
		/// CHANGE LAYER AREA ///
		totalMapCollisionBoxesB=15; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxesB; i++ )
		{
			//collision boxes coordenades
			if(i== 0){ a= 600; b= 120; c= 672; d= 128; e=3; }
			if(i== 1){ a= 768; b= 120; c= 832; d= 128; e=3; }
			if(i== 2){ a= 928; b= 120; c= 992; d= 128; e=3; }
			if(i== 3){ a= 600; b= 216; c= 672; d= 224; e=1; }
			if(i== 4){ a= 768; b= 216; c= 832; d= 224; e=1; }
			if(i== 5){ a= 928; b= 216; c= 992; d= 224; e=1; }
			if(i== 6){ a= 616; b= 320; c= 696; d= 328; e=2; }
			if(i== 7){ a= 936; b= 320; c= 968; d= 328; e=2; }
			if(i== 8){ a= 616; b= 416; c= 696; d= 424; e=1; }
			if(i== 9){ a= 936; b= 416; c= 968; d= 424; e=1; }
			if(i==10){ a=1636; b= 120; c=1696; d= 128; e=3; }
			if(i==11){ a=1824; b= 120; c=1968; d= 128; e=3; }
			if(i==12){ a=1536; b= 216; c=1696; d= 224; e=1; }
			if(i==13){ a=1824; b= 216; c=1968; d= 224; e=1; }
			if(i==14){ a=1784; b= 320; c=1816; d= 328; e=2; }
			if(i==15){ a=1784; b= 416; c=1816; d= 424; e=1; }
			
			//prevent stuck glitch
			a+=16;
			c-=16;

			//fills the matrix
			collisionMatrixB[i][0] = a;
			collisionMatrixB[i][1] = b;
			collisionMatrixB[i][2] = c;
			collisionMatrixB[i][3] = d;
			collisionMatrixB[i][4] = e;
		}
		
	}
	
	if(mapID==7) //LVL2-4
	{
		gBG_Width  = 1024;
		gBG_Height = 256;
		
		P[1].x=32;
		P[1].y=gBG_Height-32;
		
		//backgrounds
		//BGB
		VDP_loadTileSet(&bg_bgb_lvl24_tileset, gInd_tileset, DMA);
		level_mapb = MAP_create(&bg_bgb_lvl24_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL0, bg_bgb_lvl24_pal.palette->data,DMA);
		gInd_tileset += bg_bgb_lvl24_tileset.numTile;
		MAP_scrollTo(level_mapb, 0, 0);

		//BGA
		VDP_loadTileSet(&bg_bga_lvl24_tileset, gInd_tileset, DMA);
		level_map = MAP_create(&bg_bga_lvl24_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL1, bg_bga_lvl24_pal.palette->data,DMA);
		gInd_tileset += bg_bga_lvl24_tileset.numTile;
		MAP_scrollTo(level_map, 0, 0);
		
		//MAP COLLISION BOXES ---------------------------------------------------------------------
		
		totalMapCollisionBoxes=0; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxes; i++ )
		{
			//collision boxes coordenades
			
			//plane1
			if(i== 0){ a=   0; b= gBG_Height-32; c=gBG_Width; d= gBG_Height; e=1; }
			
			//fills the matrix
			collisionMatrix[i][0] = a;
			collisionMatrix[i][1] = b;
			collisionMatrix[i][2] = c;
			collisionMatrix[i][3] = d;
			collisionMatrix[i][4] = e;
		}
		
	}
	
	if(mapID==8) //LVL3-1
	{
		gBG_Width  = 1024;
		gBG_Height = 904;
		
		P[1].x=32;
		P[1].y=880;
		
		//bombs
		//BOMB( 808, 112, 1);
		//BOMB(1168, 112, 1);
		//BOMB(1536, 112, 1);
		//BOMB(1755, 192, 1);
		
		//ENEMYS(104+32*1, 231, 1, -1);
		
		//backgrounds
		//BGB
		VDP_loadTileSet(&bg_bgb_lvl31_tileset, gInd_tileset, DMA);
		level_mapb = MAP_create(&bg_bgb_lvl31_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL0, bg_bgb_lvl31_pal.palette->data,DMA);
		gInd_tileset += bg_bgb_lvl31_tileset.numTile;
		MAP_scrollTo(level_mapb, 0, 0);
		
		//BGA
		VDP_loadTileSet(&bg_bga_lvl31_tileset, gInd_tileset, DMA);
		level_map = MAP_create(&bg_bga_lvl31_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL1, bg_bga_lvl31_pal.palette->data,DMA);
		gInd_tileset += bg_bga_lvl31_tileset.numTile;
		MAP_scrollTo(level_map, 0, 0);
		
		//MAP COLLISION BOXES ---------------------------------------------------------------------
		
		totalMapCollisionBoxes=22; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxes; i++ )
		{
			//collision boxes coordenades
			
			//plane1
			if(i== 0){ a=   0; b= 880; c= 608; d= 904; e=1; }
			if(i== 1){ a= 208; b= 808; c= 272; d= 880; e=1; }
			if(i== 2){ a= 400; b= 808; c= 464; d= 880; e=1; }
			if(i== 3){ a= 608; b= 840; c=1024; d= 880; e=1; }
			if(i== 4){ a= 672; b= 808; c=1024; d= 840; e=1; }
			if(i== 5){ a= 800; b= 776; c=1024; d= 808; e=1; }
			if(i== 6){ a= 896; b= 680; c=1024; d= 688; e=1; }
			if(i== 7){ a= 768; b= 648; c= 896; d= 688; e=1; }
			if(i== 8){ a= 656; b= 584; c= 768; d= 648; e=1; }
			if(i== 9){ a= 720; b= 520; c= 736; d= 584; e=1; }
			if(i==10){ a= 336; b= 520; c= 720; d= 536; e=1; }
			if(i==11){ a= 320; b= 520; c= 336; d= 552; e=1; }
			if(i==12){ a= 224; b= 552; c= 320; d= 568; e=1; }
			if(i==13){ a=   0; b= 488; c= 224; d= 552; e=1; }
			if(i==14){ a=   0; b= 136; c=  32; d= 488; e=1; }
			if(i==15){ a=  32; b= 392; c= 192; d= 408; e=1; }
			if(i==16){ a= 192; b= 360; c= 400; d= 392; e=1; }
			if(i==17){ a= 288; b= 296; c= 400; d= 360; e=1; }
			if(i==18){ a= 352; b= 232; c= 368; d= 296; e=1; }
			if(i==19){ a= 368; b= 232; c= 656; d= 248; e=1; }
			if(i==20){ a= 656; b= 232; c= 672; d= 264; e=1; }
			if(i==21){ a= 672; b= 264; c= 800; d= 280; e=1; }
			if(i==22){ a= 800; b= 232; c=1024; d= 264; e=1; }
			
			//fills the matrix
			collisionMatrix[i][0] = a;
			collisionMatrix[i][1] = b;
			collisionMatrix[i][2] = c;
			collisionMatrix[i][3] = d;
			collisionMatrix[i][4] = e;
		}
		
		/// CHANGE LAYER AREA ///
		totalMapCollisionBoxesB=3; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxesB; i++ )
		{
			//collision boxes coordenades
			if(i== 0){ a= 896; b= 768; c=1024; d= 776; e=1; }
			if(i== 1){ a= 896; b= 672; c=1024; d= 680; e=3; }
			if(i== 2){ a=  32; b= 480; c= 192; d= 488; e=1; }
			if(i== 3){ a=  32; b= 384; c= 192; d= 392; e=3; }
			
			//prevent stuck glitch
			a+=16;
			c-=16;

			//fills the matrix
			collisionMatrixB[i][0] = a;
			collisionMatrixB[i][1] = b;
			collisionMatrixB[i][2] = c;
			collisionMatrixB[i][3] = d;
			collisionMatrixB[i][4] = e;
		}
		
	}
	
	if(mapID==9) //LVL3-2 
	{
		gBG_Width  = 512;
		gBG_Height = 1664;
		
		P[1].x=32;
		P[1].y=112;
		
		//bombs
		//BOMB( 808, 112, 1);
		//BOMB(1168, 112, 1);
		//BOMB(1536, 112, 1);
		//BOMB(1755, 192, 1);
		
		//ENEMYS(104+32*1, 232, 1, -1);
		
		//backgrounds
		//BGB
		VDP_loadTileSet(&bg_bgb_lvl32_tileset, gInd_tileset, DMA);
		level_mapb = MAP_create(&bg_bgb_lvl32_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL0, bg_bgb_lvl32_pal.palette->data,DMA);
		gInd_tileset += bg_bgb_lvl32_tileset.numTile;
		MAP_scrollTo(level_mapb, 0, 0);

		//BGA
		VDP_loadTileSet(&bg_bga_lvl32_tileset, gInd_tileset, DMA);
		level_map = MAP_create(&bg_bga_lvl32_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL1, bg_bga_lvl32_pal.palette->data,DMA);
		gInd_tileset += bg_bga_lvl32_tileset.numTile;
		MAP_scrollTo(level_map, 0, 0);
		
		//MAP COLLISION BOXES ---------------------------------------------------------------------
		
		totalMapCollisionBoxes=70; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxes; i++ )
		{
			//collision boxes coordenades
			
			//plane1
			if(i== 0){ a=    0; b=    0; c=    8; d=  464; e=1; }
			if(i== 1){ a=  496; b=    0; c=  512; d= 1416; e=1; }
			if(i== 2){ a=    8; b=  112; c=  136; d=  120; e=1; }
			if(i== 3){ a=  136; b=  112; c=  144; d=  144; e=1; }
			if(i== 4){ a=  144; b=  144; c=  200; d=  152; e=1; }
			if(i== 5){ a=  200; b=  144; c=  208; d=  176; e=1; }
			if(i== 6){ a=  208; b=  176; c=  232; d=  184; e=1; }
			if(i== 7){ a=  232; b=  176; c=  240; d=  208; e=1; }
			if(i== 8){ a=  240; b=  208; c=  336; d=  216; e=1; }
			if(i== 9){ a=  336; b=  176; c=  344; d=  208; e=1; }
			if(i==10){ a=  344; b=  176; c=  400; d=  184; e=1; }
			if(i==11){ a=  400; b=  144; c=  408; d=  176; e=1; }
			if(i==12){ a=  408; b=  144; c=  432; d=  152; e=1; }
			if(i==13){ a=  432; b=  112; c=  440; d=  144; e=1; }
			if(i==14){ a=  440; b=  112; c=  496; d=  120; e=1; }
			if(i==15){ a=  464; b=  224; c=  496; d=  288; e=1; }
			if(i==16){ a=  400; b=  256; c=  464; d=  288; e=1; }
			if(i==17){ a=  240; b=  288; c=  400; d=  296; e=1; }
			if(i==18){ a=0; b=0; c=0; d=0; e=1; }
			if(i==19){ a=  232; b=  288; c=  240; d=  320; e=1; }
			if(i==20){ a=  176; b=  320; c=  232; d=  328; e=1; }
			if(i==21){ a=  168; b=  320; c=  176; d=  352; e=1; }
			if(i==22){ a=  112; b=  352; c=  168; d=  360; e=1; }
			if(i==23){ a=  104; b=  352; c=  112; d=  384; e=1; }
			if(i==24){ a=    8; b=  384; c=  104; d=  392; e=1; }
			if(i==25){ a=    0; b=  464; c=   16; d=  504; e=1; }
			if(i==26){ a=   16; b=  464; c=  200; d=  472; e=1; }
			if(i==27){ a=  200; b=  464; c=  208; d=  496; e=1; }
			if(i==28){ a=  208; b=  496; c=  264; d=  504; e=1; }
			if(i==29){ a=  264; b=  496; c=  272; d=  528; e=1; }
			if(i==30){ a=  272; b=  528; c=  328; d=  536; e=1; }
			if(i==31){ a=  328; b=  528; c=  336; d=  560; e=1; }
			if(i==32){ a=  336; b=  560; c=  496; d=  568; e=1; }
			if(i==33){ a=  120; b=  640; c=  496; d=  648; e=1; }
			if(i==34){ a=  144; b=  608; c=  208; d=  640; e=1; }
			if(i==35){ a=  112; b=  640; c=  120; d=  672; e=1; }
			if(i==36){ a=   88; b=  672; c=  112; d=  680; e=1; }
			if(i==37){ a=   80; b=  672; c=   88; d=  704; e=1; }
			if(i==38){ a=   16; b=  704; c=   80; d=  712; e=1; }
			if(i==39){ a=   16; b=  784; c=   80; d=  816; e=1; }
			if(i==40){ a=   80; b=  816; c=  112; d=  848; e=1; }
			if(i==41){ a=  112; b=  848; c=  144; d=  880; e=1; }
			if(i==42){ a=  144; b=  880; c=  176; d=  912; e=1; }
			if(i==43){ a=  176; b=  912; c=  240; d=  928; e=1; }
			if(i==44){ a=  240; b=  880; c=  272; d=  912; e=1; }
			if(i==45){ a=  272; b=  848; c=  304; d=  880; e=1; }
			if(i==46){ a=  304; b=  816; c=  368; d=  848; e=1; }
			if(i==47){ a=  368; b=  848; c=  400; d=  880; e=1; }
			if(i==48){ a=  400; b=  880; c=  496; d=  888; e=1; }
			if(i==49){ a=  432; b=  960; c=  496; d=  992; e=1; }
			if(i==50){ a=  368; b=  992; c=  432; d= 1024; e=1; }
			if(i==51){ a=  304; b= 1024; c=  368; d= 1088; e=1; }
			if(i==52){ a=  272; b= 1088; c=  304; d= 1120; e=1; }
			if(i==53){ a=  240; b= 1120; c=  272; d= 1152; e=1; }
			if(i==54){ a=  144; b= 1152; c=  240; d= 1192; e=1; }
			if(i==55){ a=   80; b= 1088; c=  144; d= 1192; e=1; }
			if(i==56){ a=   16; b= 1152; c=   80; d= 1160; e=1; }
			if(i==57){ a=  144; b= 1192; c=  176; d= 1264; e=1; }
			if(i==58){ a=   16; b= 1248; c=  144; d= 1256; e=1; }
			if(i==59){ a=   64; b= 1256; c=   80; d= 1344; e=1; }
			if(i==60){ a=   80; b= 1344; c=  144; d= 1376; e=1; }
			if(i==61){ a=  176; b= 1264; c=  352; d= 1296; e=1; }
			if(i==62){ a=  144; b= 1376; c=  208; d= 1408; e=1; }
			if(i==63){ a=  208; b= 1408; c=  352; d= 1416; e=1; }
			if(i==64){ a=  352; b= 1296; c=  368; d= 1408; e=1; }
			if(i==65){ a=   16; b= 1440; c=  208; d= 1456; e=1; }
			if(i==66){ a=  208; b= 1440; c=  352; d= 1448; e=1; }
			if(i==67){ a=  352; b= 1440; c=  480; d= 1456; e=1; }
			if(i==68){ a=   64; b= 1472; c=  128; d= 1504; e=1; }
			if(i==69){ a=    0; b= 1504; c=  512; d= 1544; e=1; }
			if(i==70){ a=  480; b= 1416; c=  496; d= 1440; e=1; }
			
			//fills the matrix
			collisionMatrix[i][0] = a;
			collisionMatrix[i][1] = b;
			collisionMatrix[i][2] = c;
			collisionMatrix[i][3] = d;
			collisionMatrix[i][4] = e;
		}
		
		/// CHANGE LAYER AREA ///
		totalMapCollisionBoxesB=15; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxesB; i++ )
		{
			//collision boxes coordenades
			if(i== 0){ a= 240; b= 200; c= 336; d= 208; e=3; }
			if(i== 1){ a= 240; b= 272; c= 336; d= 280; e=1; }
			if(i== 2){ a=   8; b= 376; c= 104; d= 384; e=3; }
			if(i== 3){ a=   8; b= 456; c= 104; d= 464; e=1; }
			if(i== 4){ a= 336; b= 552; c= 496; d= 560; e=3; }
			if(i== 5){ a= 336; b= 632; c= 496; d= 640; e=1; }
			if(i== 6){ a=  16; b= 696; c=  80; d= 704; e=3; }
			if(i== 7){ a=  16; b= 776; c=  80; d= 784; e=1; }
			if(i== 8){ a= 432; b= 872; c= 496; d= 880; e=3; }
			if(i== 9){ a= 432; b= 952; c= 496; d= 960; e=1; }
			if(i==10){ a=  16; b=1144; c=  80; d=1152; e=3; }
			if(i==11){ a=  16; b=1240; c=  80; d=1248; e=1; }
			if(i==12){ a=  80; b=1240; c= 144; d=1248; e=3; }
			if(i==13){ a=  80; b=1336; c= 144; d=1344; e=1; }
			if(i==14){ a= 208; b=1400; c= 352; d=1408; e=3; }
			if(i==15){ a= 208; b=1496; c= 352; d=1504; e=1; }
			
			//prevent stuck glitch
			a+=16;
			c-=16;

			//fills the matrix
			collisionMatrixB[i][0] = a;
			collisionMatrixB[i][1] = b;
			collisionMatrixB[i][2] = c;
			collisionMatrixB[i][3] = d;
			collisionMatrixB[i][4] = e;
		}
		
	}
	
	if(mapID==10) //LVL3-3
	{
		gBG_Width  = 2048;
		gBG_Height = 256;
		
		P[1].x=32;
		P[1].y=gBG_Height-32;
		
		//bombs
		//BOMB( 808, 112, 1);
		//BOMB(1168, 112, 1);
		//BOMB(1536, 112, 1);
		//BOMB(1755, 192, 1);
		
		//ENEMYS(104+32*1, 232, 1, -1);
		
		//backgrounds
		//BGB
		VDP_loadTileSet(&bg_bgb_lvl33_tileset, gInd_tileset, DMA);
		level_mapb = MAP_create(&bg_bgb_lvl33_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL0, bg_bgb_lvl33_pal.palette->data,DMA);
		gInd_tileset += bg_bgb_lvl33_tileset.numTile;
		MAP_scrollTo(level_mapb, 0, 0);

		//BGA
		VDP_loadTileSet(&bg_bga_lvl33_tileset, gInd_tileset, DMA);
		level_map = MAP_create(&bg_bga_lvl33_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL1, bg_bga_lvl33_pal.palette->data,DMA);
		gInd_tileset += bg_bga_lvl33_tileset.numTile;
		MAP_scrollTo(level_map, 0, 0);
		
		//MAP COLLISION BOXES ---------------------------------------------------------------------
		
		totalMapCollisionBoxes=21; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxes; i++ )
		{
			//collision boxes coordenades
			
			//plane1
			if(i== 0){ a=   0; b=  224; c=  224; d=  240; e=1; }
			if(i== 1){ a= 288; b=  224; c=  352; d=  240; e=1; }
			if(i== 2){ a= 416; b=  224; c=  512; d=  240; e=1; }
			if(i== 3){ a= 624; b=  224; c=  688; d=  240; e=1; }
			if(i== 4){ a= 768; b=  224; c=  800; d=  240; e=1; }
			if(i== 5){ a= 864; b=  224; c=  896; d=  240; e=1; }
			if(i== 6){ a= 960; b=  224; c=  992; d=  240; e=1; }
			if(i== 7){ a=1056; b=  192; c= 1088; d=  240; e=1; }
			if(i== 8){ a=1152; b=  224; c= 1184; d=  240; e=1; }
			if(i== 9){ a=1248; b=  224; c= 1312; d=  240; e=1; }
			if(i==10){ a=1440; b=  224; c= 1504; d=  240; e=1; }
			if(i==11){ a=1600; b=  192; c= 1632; d=  240; e=1; }
			if(i==12){ a=1632; b=  224; c= 1664; d=  240; e=1; }
			if(i==13){ a=1720; b=  224; c= 1784; d=  240; e=1; }
			if(i==14){ a=1856; b=  224; c= 2048; d=  240; e=1; }
			if(i==21){ a=   0; b=  240; c= 2048; d=  256; e=1; }
			
			//plane2
			if(i==15){ a= 472; b=   40; c=  480; d=  128; e=2; }
			if(i==16){ a= 480; b=  128; c=  832; d=  136; e=2; }
			if(i==17){ a= 832; b=   40; c=  840; d=  128; e=2; }
			if(i==18){ a=1208; b=   40; c= 1216; d=  128; e=2; }
			if(i==19){ a=1216; b=  128; c= 1632; d=  136; e=2; }
			if(i==20){ a=1632; b=   40; c= 1640; d=  128; e=2; }
			
			//fills the matrix
			collisionMatrix[i][0] = a;
			collisionMatrix[i][1] = b;
			collisionMatrix[i][2] = c;
			collisionMatrix[i][3] = d;
			collisionMatrix[i][4] = e;
		}
		
		/// CHANGE LAYER AREA ///
		totalMapCollisionBoxesB=11; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxesB; i++ )
		{
			//collision boxes coordenades
			if(i== 0){ a= 480+16; b= 216; c= 512; d= 224; e=1; }
			if(i== 1){ a= 624; b= 216; c= 688; d= 224; e=1; }
			if(i== 2){ a= 768; b= 216; c= 800; d= 224; e=1; }
			if(i== 3){ a= 480; b= 120; c= 512; d= 128; e=3; }
			if(i== 4){ a= 624; b= 120; c= 688; d= 128; e=3; }
			if(i== 5){ a= 768; b= 120; c= 800; d= 128; e=3; }
			if(i== 6){ a=1248; b= 216; c=1312; d= 224; e=1; }
			if(i== 7){ a=1440; b= 216; c=1504; d= 224; e=1; }
			if(i== 8){ a=1600; b= 184; c=1632-8; d= 192; e=1; }
			if(i== 9){ a=1248; b= 120; c=1312; d= 128; e=3; }
			if(i==10){ a=1440; b= 120; c=1504; d= 128; e=3; }
			if(i==11){ a=1600; b= 120; c=1632; d= 128; e=3; }
			
			//prevent stuck glitch
			//a+=16;
			//c-=16;

			//fills the matrix
			collisionMatrixB[i][0] = a;
			collisionMatrixB[i][1] = b;
			collisionMatrixB[i][2] = c;
			collisionMatrixB[i][3] = d;
			collisionMatrixB[i][4] = e;
		}
		
	}
	
	if(mapID==11) //LVL3-4
	{
		gBG_Width  = 512;
		gBG_Height = 224;
		
		P[1].x=32;
		P[1].y=gBG_Height-48;
		
		//bombs
		//BOMB( 808, 112, 1);
		//BOMB(1168, 112, 1);
		//BOMB(1536, 112, 1);
		//BOMB(1755, 192, 1);
		
		//ENEMYS(104+32*1, 232, 1, -1);
		
		//backgrounds
		//BGB
		VDP_loadTileSet(&bg_bgb_lvl34_tileset, gInd_tileset, DMA);
		level_mapb = MAP_create(&bg_bgb_lvl34_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL0, bg_bgb_lvl34_pal.palette->data,DMA);
		gInd_tileset += bg_bgb_lvl34_tileset.numTile;
		MAP_scrollTo(level_mapb, 0, 0);

		//BGA
		VDP_loadTileSet(&bg_bga_lvl34_tileset, gInd_tileset, DMA);
		level_map = MAP_create(&bg_bga_lvl34_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL1, bg_bga_lvl34_pal.palette->data,DMA);
		gInd_tileset += bg_bga_lvl34_tileset.numTile;
		MAP_scrollTo(level_map, 0, 0);
		
		//MAP COLLISION BOXES ---------------------------------------------------------------------
		
		totalMapCollisionBoxes=0; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxes; i++ )
		{
			//collision boxes coordenades
			
			//plane1
			if(i== 0){ a=   0; b= gBG_Height-48; c=gBG_Width; d= gBG_Height; e=1; }
			
			//fills the matrix
			collisionMatrix[i][0] = a;
			collisionMatrix[i][1] = b;
			collisionMatrix[i][2] = c;
			collisionMatrix[i][3] = d;
			collisionMatrix[i][4] = e;
		}
		
		/*
		/// CHANGE LAYER AREA ///
		totalMapCollisionBoxesB=5; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxesB; i++ )
		{
			//collision boxes coordenades
			if(i== 0){ a= 544; b= 184; c= 896; d= 232; e=1; }
			if(i== 1){ a= 960; b= 184; c=1448; d= 232; e=1; }
			if(i== 2){ a=1512; b= 152; c=1864; d= 232; e=1; }
			if(i== 3){ a= 511; b= 104; c= 896; d= 112; e=3; }
			if(i== 4){ a= 960; b= 104; c=1448; d= 112; e=3; }
			if(i== 5){ a=1512; b= 104; c=1864; d= 112; e=3; }
			
			//prevent stuck glitch
			a+=16;
			c-=16;

			//fills the matrix
			collisionMatrixB[i][0] = a;
			collisionMatrixB[i][1] = b;
			collisionMatrixB[i][2] = c;
			collisionMatrixB[i][3] = d;
			collisionMatrixB[i][4] = e;
		}
		*/
		
	}
	
	if(mapID==12) //LVL4-1
	{
		gBG_Width  = 1536;
		gBG_Height = 768;
		
		P[1].x=32;
		P[1].y=112;
		
		//bombs
		//BOMB( 808, 112, 1);
		//BOMB(1168, 112, 1);
		//BOMB(1536, 112, 1);
		//BOMB(1755, 192, 1);
		
		//ENEMYS(104+32*1, 232, 1, -1);
		
		//backgrounds
		//BGB
		VDP_loadTileSet(&bg_bgb_lvl41_tileset, gInd_tileset, DMA);
		level_mapb = MAP_create(&bg_bgb_lvl41_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL0, bg_bgb_lvl41_pal.palette->data,DMA);
		gInd_tileset += bg_bgb_lvl41_tileset.numTile;
		MAP_scrollTo(level_mapb, 0, 0);

		//BGA
		VDP_loadTileSet(&bg_bga_lvl41_tileset, gInd_tileset, DMA);
		level_map = MAP_create(&bg_bga_lvl41_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL1, bg_bga_lvl41_pal.palette->data,DMA);
		gInd_tileset += bg_bga_lvl41_tileset.numTile;
		MAP_scrollTo(level_map, 0, 0);
		
		//MAP COLLISION BOXES ---------------------------------------------------------------------
		
		totalMapCollisionBoxes=25; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxes; i++ )
		{
			//collision boxes coordenades
			
			//plane1
			if(i== 0){ a=   0; b= 112; c=  144; d=  144; e=1; }
			if(i== 1){ a= 144; b= 144; c=  176; d=  208; e=1; }
			if(i== 2){ a= 176; b= 208; c=  208; d=  272; e=1; }
			if(i== 3){ a= 208; b= 272; c=  272; d=  336; e=1; }
			if(i== 4){ a= 272; b= 336; c=  304; d=  400; e=1; }
			if(i== 5){ a= 304; b= 400; c=  336; d=  544; e=1; }
			if(i== 6){ a= 400; b= 368; c=  432; d=  544; e=1; }
			if(i== 7){ a= 432; b= 400; c=  592; d=  464; e=1; }
			if(i== 8){ a= 592; b= 464; c=  656; d=  528; e=1; }
			if(i== 9){ a= 656; b= 528; c=  688; d=  592; e=1; }
			if(i==10){ a= 688; b= 592; c=  720; d=  768; e=1; }
			if(i==11){ a= 784; b= 560; c=  848; d=  768; e=1; }
			if(i==12){ a= 848; b= 496; c=  880; d=  560; e=1; }
			if(i==13){ a= 880; b= 432; c=  912; d=  496; e=1; }
			if(i==14){ a= 912; b= 368; c= 1072; d=  432; e=1; }
			if(i==15){ a=1072; b= 432; c= 1136; d=  544; e=1; }
			if(i==16){ a=1200; b= 432; c= 1236; d=  544; e=1; }
			if(i==17){ a=1232; b= 400; c= 1296; d=  432; e=1; }
			if(i==18){ a=1296; b= 336; c= 1328; d=  400; e=1; }
			if(i==19){ a=1328; b= 272; c= 1360; d=  336; e=1; }
			if(i==20){ a=1360; b= 208; c= 1424; d=  272; e=1; }
			if(i==21){ a=1424; b= 144; c= 1472; d=  208; e=1; }
			if(i==22){ a=1472; b= 112; c= 1536; d=  144; e=1; }
			if(i==23){ a= 336; b= 432; c=  400; d=  448; e=1; }
			if(i==24){ a= 720; b= 608; c=  784; d=  624; e=1; }
			if(i==25){ a=1136; b= 464; c= 1200; d=  480; e=1; }
			
			//fills the matrix
			collisionMatrix[i][0] = a;
			collisionMatrix[i][1] = b;
			collisionMatrix[i][2] = c;
			collisionMatrix[i][3] = d;
			collisionMatrix[i][4] = e;
		}
		
	}
	
	if(mapID==13) //LVL4-2
	{
		gBG_Width  = 1024;
		gBG_Height = 768;
		
		P[1].x=32;
		P[1].y=720;
		
		//bombs
		//BOMB( 808, 112, 1);
		//BOMB(1168, 112, 1);
		//BOMB(1536, 112, 1);
		//BOMB(1755, 192, 1);
		
		//ENEMYS(104+32*1, 232, 1, -1);
		
		//backgrounds
		//BGB
		VDP_loadTileSet(&bg_bgb_lvl42_tileset, gInd_tileset, DMA);
		level_mapb = MAP_create(&bg_bgb_lvl42_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL0, bg_bgb_lvl42_pal.palette->data,DMA);
		gInd_tileset += bg_bgb_lvl42_tileset.numTile;
		MAP_scrollTo(level_mapb, 0, 0);

		//BGA
		VDP_loadTileSet(&bg_bga_lvl42_tileset, gInd_tileset, DMA);
		level_map = MAP_create(&bg_bga_lvl42_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL1, bg_bga_lvl42_pal.palette->data,DMA);
		gInd_tileset += bg_bga_lvl42_tileset.numTile;
		MAP_scrollTo(level_map, 0, 0);
		
		//MAP COLLISION BOXES ---------------------------------------------------------------------
		
		totalMapCollisionBoxes=36; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxes; i++ )
		{
			//collision boxes coordenades
			//plane1
			if(i== 0){ a=   0; b=  720; c=  208; d=  768; e=1; }
			if(i== 1){ a= 272; b=  720; c=  336; d=  728; e=1; }
			if(i== 2){ a= 304; b=  640; c=  368; d=  648; e=1; }
			if(i== 3){ a= 432; b=  608; c=  496; d=  616; e=1; }
			if(i== 4){ a= 560; b=  640; c=  592; d=  648; e=1; }
			if(i== 5){ a= 624; b=  608; c=  688; d=  616; e=1; }
			if(i== 6){ a= 752; b=  576; c=  816; d=  584; e=1; }
			if(i== 7){ a= 880; b=  496; c=  960; d=  528; e=1; }
			if(i== 8){ a= 960; b=  448; c=  968; d=  496; e=1; }
			if(i== 9){ a= 880; b=  288; c=  960; d=  448; e=1; }
			if(i==10){ a= 752; b=  416; c=  816; d=  424; e=1; }
			if(i==11){ a= 624; b=  384; c=  688; d=  392; e=1; }
			if(i==12){ a= 496; b=  416; c=  560; d=  424; e=1; }
			if(i==13){ a= 368; b=  448; c=  432; d=  456; e=1; }
			if(i==14){ a= 240; b=  448; c=  304; d=  456; e=1; }
			if(i==15){ a= 208; b=  368; c=  272; d=  376; e=1; }
			if(i==16){ a=  64; b=  336; c=  160; d=  368; e=1; }
			if(i==17){ a=  56; b=  288; c=   64; d=  336; e=1; }
			if(i==18){ a=  56; b=  256; c=  152; d=  288; e=1; }
			if(i==19){ a=  40; b=  152; c=   56; d=  256; e=1; }
			if(i==20){ a=   0; b=   96; c=  128; d=  152; e=1; }
			if(i==21){ a= 208; b=  224; c=  240; d=  232; e=1; }
			if(i==22){ a= 304; b=  192; c=  336; d=  200; e=1; }
			if(i==23){ a= 400; b=  224; c=  432; d=  232; e=1; }
			if(i==24){ a= 400; b=  144; c=  464; d=  152; e=1; }
			if(i==25){ a= 528; b=  144; c=  560; d=  152; e=1; }
			if(i==26){ a= 624; b=  144; c=  656; d=  152; e=1; }
			if(i==27){ a= 720; b=  176; c=  752; d=  184; e=1; }
			if(i==28){ a= 720; b=  256; c=  784; d=  264; e=1; }
			if(i==29){ a= 848; b=  256; c=  920; d=  288; e=1; }
			if(i==30){ a= 920; b=  152; c=  928; d=  256; e=1; }
			if(i==31){ a= 848; b=  168; c=  880; d=  176; e=1; }
			if(i==32){ a= 880; b=  144; c=  912; d=  160; e=1; }
			if(i==33){ a= 928; b=  128; c= 1024; d=  152; e=1; }
			if(i==34){ a= 936; b=   96; c= 1024; d=  128; e=1; }
			if(i==35){ a= 720; b=  496; c=  784; d=  504; e=1; }
			if(i==36){ a= 208; b=  760; c= 1024; d=  768; e=1; }
			
			//fills the matrix
			collisionMatrix[i][0] = a;
			collisionMatrix[i][1] = b;
			collisionMatrix[i][2] = c;
			collisionMatrix[i][3] = d;
			collisionMatrix[i][4] = e;
		}
		
		/// CHANGE LAYER AREA ///
		totalMapCollisionBoxesB=7; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxesB; i++ )
		{
			//collision boxes coordenades
			if(i== 0){ a= 304; b= 712; c= 336; d= 720; e=1; }
			if(i== 1){ a= 752; b= 568; c= 784; d= 576; e=1; }
			if(i== 2){ a= 752; b= 488; c= 784; d= 496; e=1; }
			if(i== 3){ a= 240; b= 440; c= 272; d= 448; e=1; }
			if(i== 4){ a=  64; b= 328; c= 152; d= 336; e=1; }
			if(i== 5){ a= 400; b= 216; c= 432; d= 224; e=1; }
			if(i== 6){ a= 720; b= 248; c= 752; d= 256; e=1; }
			if(i== 7){ a= 848; b= 248; c= 880; d= 256; e=1; }
			
			//prevent stuck glitch
			//a+=16;
			//c-=16;

			//fills the matrix
			collisionMatrixB[i][0] = a;
			collisionMatrixB[i][1] = b;
			collisionMatrixB[i][2] = c;
			collisionMatrixB[i][3] = d;
			collisionMatrixB[i][4] = e;
		}
		
	}
	
	if(mapID==14) //LVL4-3
	{
		gBG_Width  = 1536;
		gBG_Height = 512;
		
		P[1].x=32;
		P[1].y=368;
		
		//bombs
		//BOMB( 808, 112, 1);
		//BOMB(1168, 112, 1);
		//BOMB(1536, 112, 1);
		//BOMB(1755, 192, 1);
		
		//ENEMYS(104+32*1, 232, 1, -1);
		
		//backgrounds
		//BGB
		VDP_loadTileSet(&bg_bgb_lvl43_tileset, gInd_tileset, DMA);
		level_mapb = MAP_create(&bg_bgb_lvl43_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL0, bg_bgb_lvl43_pal.palette->data,DMA);
		gInd_tileset += bg_bgb_lvl43_tileset.numTile;
		MAP_scrollTo(level_mapb, 0, 0);

		//BGA
		VDP_loadTileSet(&bg_bga_lvl43_tileset, gInd_tileset, DMA);
		level_map = MAP_create(&bg_bga_lvl43_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL1, bg_bga_lvl43_pal.palette->data,DMA);
		gInd_tileset += bg_bga_lvl43_tileset.numTile;
		MAP_scrollTo(level_map, 0, 0);
		
		//MAP COLLISION BOXES ---------------------------------------------------------------------
		
		totalMapCollisionBoxes=31; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxes; i++ )
		{
			//collision boxes coordenades
			
			//plane2
			if(i== 0){ a=   0; b= 368; c= 432; d= 384; e=2; }
			if(i== 1){ a= 432; b= 240; c= 528; d= 368; e=2; }
			if(i== 2){ a= 528; b= 368; c= 712; d= 384; e=2; }
			if(i== 3){ a= 712; b= 384; c= 744; d= 400; e=2; }
			if(i== 4){ a= 744; b= 400; c= 776; d= 416; e=2; }
			if(i== 5){ a= 776; b= 416; c= 808; d= 432; e=2; }
			if(i== 6){ a= 808; b= 432; c= 840; d= 448; e=2; }
			if(i== 7){ a= 840; b= 448; c= 872; d= 464; e=2; }
			if(i== 8){ a= 872; b= 464; c=1184; d= 480; e=2; }
			if(i== 9){ a=1184; b= 448; c=1216; d= 464; e=2; }
			if(i==10){ a=1216; b= 432; c=1248; d= 448; e=2; }
			if(i==11){ a=1248; b= 416; c=1280; d= 432; e=2; }
			if(i==12){ a=1280; b= 400; c=1312; d= 416; e=2; }
			if(i==13){ a=1312; b= 384; c=1344; d= 400; e=2; }
			if(i==14){ a=1344; b= 368; c=1536; d= 384; e=2; }
			if(i==15){ a= 528; b=   0; c= 536; d= 128; e=2; }
			if(i==16){ a= 536; b= 128; c= 664; d= 136; e=2; }
			if(i==17){ a=1128; b= 128; c=1256; d= 136; e=2; }
			if(i==18){ a=1256; b=   0; c=1264; d= 128; e=2; }
			
			//plane1
			if(i==19){ a= 352; b= 384; c= 368; d= 440; e=1; }
			if(i==20){ a= 368; b= 440; c= 560; d= 456; e=1; }
			if(i==21){ a= 560; b= 384; c= 576; d= 440; e=1; }
			
			//plane2
			if(i==22){ a= 904; b= 240; c=1152; d= 280; e=2; }
			if(i==23){ a= 888; b= 272; c= 904; d= 376; e=2; }
			if(i==24){ a= 904; b= 376; c=1152; d= 392; e=2; }
			if(i==25){ a=1152; b= 272; c=1168; d= 376; e=2; }
			if(i==26){ a=1056; b= 344; c=1088; d= 376; e=2; }
			if(i==27){ a=   0; b= 224; c=1536; d= 240; e=2; }
			if(i==28){ a=  80; b= 160; c= 208; d= 224; e=2; }
			if(i==29){ a= 304; b= 184; c= 336; d= 224; e=2; }
			if(i==30){ a= 336; b= 160; c= 432; d= 224; e=2; }
			if(i==31){ a=1312; b= 192; c=1344; d= 224; e=2; }
			
			//fills the matrix
			collisionMatrix[i][0] = a;
			collisionMatrix[i][1] = b;
			collisionMatrix[i][2] = c;
			collisionMatrix[i][3] = d;
			collisionMatrix[i][4] = e;
		}
		
		/// CHANGE LAYER AREA ///
		totalMapCollisionBoxesB=11; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxesB; i++ )
		{
			//collision boxes coordenades
			if(i== 0){ a= 368; b= 360; c= 432; d= 368; e=3; }
			if(i== 1){ a= 368; b= 432; c= 432; d= 440; e=1; }
			if(i== 2){ a= 528; b= 432; c= 560; d= 440; e=1; }
			if(i== 3){ a= 528; b= 360; c= 560; d= 368; e=3; }
			if(i== 4){ a= 904; b= 456; c=1152; d= 464; e=1; }
			if(i== 5){ a= 904; b= 368; c=1152; d= 376; e=3; }
			if(i== 6){ a=1344; b= 360; c=1536; d= 368; e=1; }
			if(i== 7){ a=1344; b= 216; c=1536; d= 224; e=2; }
			if(i== 8){ a= 536; b= 216; c= 664; d= 224; e=1; }
			if(i== 9){ a=1128; b= 216; c=1256; d= 224; e=1; }
			if(i==10){ a= 536; b= 120; c= 664; d= 128; e=3; }
			if(i==11){ a=1128; b= 120; c=1256; d= 128; e=3; }
			
			//prevent stuck glitch
			a+=8;
			c-=8;

			//fills the matrix
			collisionMatrixB[i][0] = a;
			collisionMatrixB[i][1] = b;
			collisionMatrixB[i][2] = c;
			collisionMatrixB[i][3] = d;
			collisionMatrixB[i][4] = e;
		}
		
		// ANIMATED OBJECTS AND OTHERS //
		//plataformA
		//AnimObj[0].sprite = SPR_addSpriteExSafe(&spr_element_lvl43A, -99, -99, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
		//AnimObj[0].x = 744;
		//AnimObj[0].y = 160;
		//if(AnimObj[0].sprite){ SPR_setDepth(AnimObj[0].sprite, 255); }
		//plataformB
		//AnimObj[1].sprite = SPR_addSpriteExSafe(&spr_element_lvl43A, -99, -99, TILE_ATTR(PAL1, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
		//AnimObj[1].x = 896;
		//AnimObj[1].y = 160;
		//if(AnimObj[1].sprite){ SPR_setDepth(AnimObj[1].sprite, 255); SPR_setHFlip(AnimObj[1].sprite, TRUE); } 
		
	}
	
	if(mapID==15) //LVL4-4
	{
		gBG_Width  = 736;
		gBG_Height = 256;
		
		P[1].x=32;
		P[1].y=168;
		
		//bombs
		//BOMB( 808, 112, 1);
		//BOMB(1168, 112, 1);
		//BOMB(1536, 112, 1);
		//BOMB(1755, 192, 1);
		
		//ENEMYS(104+32*1, 232, 1, -1);
		
		//backgrounds
		//BGB
		VDP_loadTileSet(&bg_bgb_lvl44_tileset, gInd_tileset, DMA);
		level_mapb = MAP_create(&bg_bgb_lvl44_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL0, bg_bgb_lvl44_pal.palette->data,DMA);
		gInd_tileset += bg_bgb_lvl44_tileset.numTile;
		MAP_scrollTo(level_mapb, 0, 0);

		//BGA
		VDP_loadTileSet(&bg_bga_lvl44_tileset, gInd_tileset, DMA);
		level_map = MAP_create(&bg_bga_lvl44_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, gInd_tileset)); 
		//PAL_setPalette(PAL1, bg_bga_lvl44_pal.palette->data,DMA);
		gInd_tileset += bg_bga_lvl44_tileset.numTile;
		MAP_scrollTo(level_map, 0, 0);
		
		//MAP COLLISION BOXES ---------------------------------------------------------------------
		
		totalMapCollisionBoxes=0; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxes; i++ )
		{
			//collision boxes coordenades
			
			//plane1
			if(i== 0){ a=   0; b= 168; c=gBG_Width; d= gBG_Height; e=1; }
			
			//fills the matrix
			collisionMatrix[i][0] = a;
			collisionMatrix[i][1] = b;
			collisionMatrix[i][2] = c;
			collisionMatrix[i][3] = d;
			collisionMatrix[i][4] = e;
		}
		
		/*
		/// CHANGE LAYER AREA ///
		totalMapCollisionBoxesB=5; // <<< IMPORTANT!!! DO NOT FORGET!!! :)
		for( int i=0; i<=totalMapCollisionBoxesB; i++ )
		{
			//collision boxes coordenades
			if(i== 0){ a= 544; b= 184; c= 896; d= 232; e=1; }
			if(i== 1){ a= 960; b= 184; c=1448; d= 232; e=1; }
			if(i== 2){ a=1512; b= 152; c=1864; d= 232; e=1; }
			if(i== 3){ a= 511; b= 104; c= 896; d= 112; e=3; }
			if(i== 4){ a= 960; b= 104; c=1448; d= 112; e=3; }
			if(i== 5){ a=1512; b= 104; c=1864; d= 112; e=3; }
			
			//prevent stuck glitch
			a+=16;
			c-=16;

			//fills the matrix
			collisionMatrixB[i][0] = a;
			collisionMatrixB[i][1] = b;
			collisionMatrixB[i][2] = c;
			collisionMatrixB[i][3] = d;
			collisionMatrixB[i][4] = e;
		}
		*/
		
	}
	

}

void DYNAMIC_LOADING_TILESET()
{
	//---DINAMIC LOADING TEST!!! (start) ----------------------------------------------------------------------------------------
	if(gMapID==2)
	{
		if(gReloadPal==TRUE)
		{
			PAL_setPalette(PAL0, bg_bgb_lvl12_pal.palette->data,DMA);
			PAL_setPalette(PAL1, bg_bga_lvl12A_pal.palette->data,DMA);
			//PAL2...
			if(P[1].id==2){ PAL_setPalette(PAL3, spr_player2.palette->data,DMA);
			}else{ PAL_setPalette(PAL3, spr_player.palette->data,DMA); }
			gReloadPal=FALSE;
		}
		
		if(gSubMapID==2 && P[1].x<960)
		{
			gSubMapID=1;
			PAL_setColors(0, palette_black, 32, DMA);
			//backgrounds
			gInd_tileset=0;
			if(level_map ){ MEM_free(level_map) ; level_map  = NULL; }
			if(level_mapb){ MEM_free(level_mapb); level_mapb = NULL; }
			//BGB
			VDP_loadTileSet(&bg_bgb_lvl12_tileset, gInd_tileset, DMA);
			level_mapb = MAP_create(&bg_bgb_lvl12_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset)); 
			gInd_tileset += bg_bgb_lvl12_tileset.numTile;
			//BGA
			VDP_loadTileSet(&bg_bga_lvl12A_tileset, gInd_tileset, DMA);
			level_map = MAP_create(&bg_bga_lvl12A_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, gInd_tileset)); 
			gInd_tileset += bg_bga_lvl12A_tileset.numTile;
			//update screen
			MAP_scrollTo(level_map, new_camera_x, new_camera_y);
			MAP_scrollTo(level_mapb, new_camera_x/2, new_camera_y/2);
			gReloadPal=TRUE;
		}
		else if(gSubMapID==1 && P[1].x>=960)
		{
			gSubMapID=2;
			PAL_setColors(0, palette_black, 32, DMA);
			//backgrounds
			gInd_tileset=0;
			if(level_map ){ MEM_free(level_map) ; level_map  = NULL; }
			if(level_mapb){ MEM_free(level_mapb); level_mapb = NULL; }
			//BGB
			VDP_loadTileSet(&bg_bgb_lvl12_tileset, gInd_tileset, DMA);
			level_mapb = MAP_create(&bg_bgb_lvl12_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset)); 
			gInd_tileset += bg_bgb_lvl12_tileset.numTile;
			//BGA
			VDP_loadTileSet(&bg_bga_lvl12B_tileset, gInd_tileset, DMA);
			level_map = MAP_create(&bg_bga_lvl12B_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, gInd_tileset)); 
			gInd_tileset += bg_bga_lvl12B_tileset.numTile;
			//update screen
			MAP_scrollTo(level_map, new_camera_x, new_camera_y);
			MAP_scrollTo(level_mapb, new_camera_x/2, new_camera_y/2);
			gReloadPal=TRUE;
		}
	}
	//---DINAMIC LOADING TEST!!! (end) ---------------------------------------------------------------------------------------
	if(gMapID==5)
	{
		if(gReloadPal==TRUE)
		{
			PAL_setPalette(PAL0, bg_bgb_lvl22_pal.palette->data,DMA);
			PAL_setPalette(PAL1, bg_bga_lvl22A_pal.palette->data,DMA);
			//PAL2...
			if(P[1].id==2){ PAL_setPalette(PAL3, spr_player2.palette->data,DMA);
			}else{ PAL_setPalette(PAL3, spr_player.palette->data,DMA); }
			gReloadPal=FALSE;
		}
		
		if(gSubMapID==2 && P[1].x<680)
		{
			gSubMapID=1;
			PAL_setColors(0, palette_black, 32, DMA);
			//backgrounds
			gInd_tileset=0;
			if(level_map ){ MEM_free(level_map) ; level_map  = NULL; }
			if(level_mapb){ MEM_free(level_mapb); level_mapb = NULL; }
			//BGB
			VDP_loadTileSet(&bg_bgb_lvl22_tileset, gInd_tileset, DMA);
			level_mapb = MAP_create(&bg_bgb_lvl22_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset)); 
			gInd_tileset += bg_bgb_lvl22_tileset.numTile;
			//BGA
			VDP_loadTileSet(&bg_bga_lvl22A_tileset, gInd_tileset, DMA);
			level_map = MAP_create(&bg_bga_lvl22A_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, gInd_tileset)); 
			gInd_tileset += bg_bga_lvl22A_tileset.numTile;
			//update screen
			MAP_scrollTo(level_map, new_camera_x, new_camera_y);
			MAP_scrollTo(level_mapb, new_camera_x/2, new_camera_y/2);
			gReloadPal=TRUE;
		}
		else if(gSubMapID==1 && P[1].x>=680)
		{
			gSubMapID=2;
			PAL_setColors(0, palette_black, 32, DMA);
			//backgrounds
			gInd_tileset=0;
			if(level_map ){ MEM_free(level_map) ; level_map  = NULL; }
			if(level_mapb){ MEM_free(level_mapb); level_mapb = NULL; }
			//BGB
			VDP_loadTileSet(&bg_bgb_lvl22_tileset, gInd_tileset, DMA);
			level_mapb = MAP_create(&bg_bgb_lvl22_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, gInd_tileset)); 
			gInd_tileset += bg_bgb_lvl22_tileset.numTile;
			//BGA
			VDP_loadTileSet(&bg_bga_lvl22B_tileset, gInd_tileset, DMA);
			level_map = MAP_create(&bg_bga_lvl22B_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, gInd_tileset)); 
			gInd_tileset += bg_bga_lvl22B_tileset.numTile;
			//update screen
			MAP_scrollTo(level_map, new_camera_x, new_camera_y);
			MAP_scrollTo(level_mapb, new_camera_x/2, new_camera_y/2);
			gReloadPal=TRUE;
		}
	}
}

void FXOBJECT(s32 x, s32 y, u8 dir)
{
    for (int i = 0; i < MAX_SMOKE_FX; i++) {
        if (!fxobjs[i].active) {
            fxobjs[i].x = x;
            fxobjs[i].y = y;
			fxobjs[i].dir = dir;
			fxobjs[i].life = 14*2;
            fxobjs[i].active = 1;
			if(fxobjs[i].sprite){ SPR_releaseSprite(fxobjs[i].sprite); fxobjs[i].sprite = NULL; } //clear memory
			//load sprite outside of the view (320,224)
			fxobjs[i].sprite = SPR_addSpriteExSafe(&spr_smoke_fx, 320, 224, TILE_ATTR(PAL3, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
			if(fxobjs[i].sprite){ SPR_setDepth(fxobjs[i].sprite, 1); }
			break;
        }
    }
}

void UPDATE_FXOBJS()
{
    for (int i = 0; i < MAX_SMOKE_FX; i++) {
        if (fxobjs[i].active) {
			//destroy fxobjs
			if( fxobjs[i].life>0 ){ fxobjs[i].life--; }
			
			if (fxobjs[i].life==0)
			{ 
				fxobjs[i].active = 0; 
				if(fxobjs[i].sprite)
				{ 
					SPR_releaseSprite(fxobjs[i].sprite); 
					fxobjs[i].sprite = NULL; 
				} 
			}
			
        }
    }
}

void DRAW_FXOBJS()
{
    for (int i = 0; i < MAX_SMOKE_FX; i++) {
        if (fxobjs[i].active) {
			if(fxobjs[i].sprite)
			{ 
				//draw sprite
				SPR_setPosition(fxobjs[i].sprite, (fxobjs[i].x-24) - new_camera_x, (fxobjs[i].y-20) - new_camera_y);
			}
        }
    }
}

void BOMB(s32 x, s32 y, u8 dir)
{
    for (int i = 0; i < MAX_BOMBS; i++) {
        if (!bombs[i].active) {
            bombs[i].x = x;
            bombs[i].y = y;
			bombs[i].dir = dir;
			bombs[i].life = 1; if(bombs[i].dir==99){bombs[i].life=90;}
            bombs[i].active = 1;
			if(bombs[i].sprite){ SPR_releaseSprite(bombs[i].sprite); bombs[i].sprite = NULL; } //clear memory
			//load sprite outside of the view (320,224)
			if(bombs[i].dir!=99)
			{
				bombs[i].sprite = SPR_addSpriteExSafe(&spr_bomb, 320, 224, TILE_ATTR(PAL3, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
				SPR_setVRAMTileIndex(bombs[i].sprite, 1441); //define uma posicao especifica para o GFX na VRAM
			}else{
				bombs[i].sprite = SPR_addSpriteExSafe(&spr_bomb_dead, 320, 224, TILE_ATTR(PAL3, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );  //bomb dead
			}
			if(bombs[i].sprite){ SPR_setDepth(bombs[i].sprite, 6); }
			break;
        }
    }
}

void UPDATE_BOMBS()
{
    for (int i = 0; i < MAX_BOMBS; i++) {
        if (bombs[i].active) {
			//destroy bombs
			if( bombs[i].dir==99 && bombs[i].life>0 ){ bombs[i].life--; } 
			
			if(P[1].x>bombs[i].x-16 && P[1].x<bombs[i].x+16 && P[1].y==bombs[i].y && bombs[i].dir!=99){ bombs[i].life=0; }
			
			if (bombs[i].life==0)
			{ 
				bombs[i].active = 0; 
				if(bombs[i].sprite)
				{ 
					SPR_releaseSprite(bombs[i].sprite); 
					bombs[i].sprite = NULL; 
					if(bombs[i].dir!=99) //99 is bomb_dead animation
					{ 
						BOMB(bombs[i].x, bombs[i].y, 99); 
						XGM_setPCM(P1_SFX, snd_bomb_dead, sizeof(snd_bomb_dead)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3);
					} 
					
				} 
			}
			
        }
    }
}

void DRAW_BOMBS()
{
    for (int i = 0; i < MAX_BOMBS; i++) {
        if (bombs[i].active) {
			if(bombs[i].sprite)
			{ 
				//draw sprite
				if(bombs[i].dir!=99)
				{
					SPR_setPosition(bombs[i].sprite, (bombs[i].x-16) - new_camera_x, (bombs[i].y-32) - new_camera_y);
				}else{
					SPR_setPosition(bombs[i].sprite, (bombs[i].x-23) - new_camera_x, (bombs[i].y-96) - new_camera_y); //bomb dead
				}
				
			}
        }
    }
}

void OBJSHURIKENS(s32 x, s32 y, u8 dir)
{
    for (int i = 0; i < MAX_SHURIKENS; i++) {
        if (!obj_shuriken[i].active) {
			P[1].totalShurikens++;
			XGM_setPCM(P1_SFX, snd_shuriken, sizeof(snd_shuriken)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3);
            obj_shuriken[i].x = x;
            obj_shuriken[i].y = y;
			obj_shuriken[i].dir = dir;
			obj_shuriken[i].life = 120;
            obj_shuriken[i].active = 1;
			obj_shuriken[i].myLayer = P[1].playerLayer;
			if(obj_shuriken[i].sprite){ SPR_releaseSprite(obj_shuriken[i].sprite); obj_shuriken[i].sprite = NULL; } //clear memory
			//load sprite outside of the view (320,224)
			obj_shuriken[i].sprite = SPR_addSpriteExSafe(&spr_shuriken, 320, 224, TILE_ATTR(PAL3, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
			if(obj_shuriken[i].sprite){ SPR_setDepth(obj_shuriken[i].sprite, 1); }
			break;
        }
    }
}

void UPDATE_OBJSHURIKENS()
{
    for (int i = 0; i < MAX_SHURIKENS; i++) {
        if (obj_shuriken[i].active) {
			
			//move
			if(obj_shuriken[i].dir==1){obj_shuriken[i].x+=SHURIKEN_SPEED;}else{obj_shuriken[i].x-=SHURIKEN_SPEED;}
			
			// shuriken collision logic with walls
			// o gPing2 aqui é usado apenas para poupar processamento, testando 1x a cada 2 frames
			if(gPing2==0)
			{
				for( int j=0; j<=totalMapCollisionBoxes; j++ )
				{
					if(( CHECK_COLLISION
					(
						obj_shuriken[i].x-1, obj_shuriken[i].y-1, obj_shuriken[i].x, obj_shuriken[i].y, 
						collisionMatrix[j][0], collisionMatrix[j][1], collisionMatrix[j][2], collisionMatrix[j][3] )==1
					)&& obj_shuriken[i].dir!=90 && obj_shuriken[i].myLayer==collisionMatrix[j][4]){
						obj_shuriken[i].life=0;
						OBJSHURIKENSDEAD(obj_shuriken[i].x, obj_shuriken[i].y, obj_shuriken[i].dir);
						XGM_setPCM(P1_SFX, snd_shuriken_dead2, sizeof(snd_shuriken_dead2)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3);
					}
				}
			}
			
			//destroy obj_shuriken
			if( obj_shuriken[i].life>0 ){ obj_shuriken[i].life--; }
			if(obj_shuriken[i].x<new_camera_x- 16){ obj_shuriken[i].life=0; }
			if(obj_shuriken[i].x>new_camera_x+336){ obj_shuriken[i].life=0; }
			
			if (obj_shuriken[i].life==0)
			{ 
				obj_shuriken[i].active = 0; 
				P[1].totalShurikens--;
				if(obj_shuriken[i].sprite)
				{ 
					SPR_releaseSprite(obj_shuriken[i].sprite); 
					obj_shuriken[i].sprite = NULL; 
				} 
			}
			
        }
    }
}

void DRAW_OBJSHURIKENS()
{
    for (int i = 0; i < MAX_SHURIKENS; i++) {
        if (obj_shuriken[i].active) {
			if(obj_shuriken[i].sprite)
			{ 
				//draw sprite
				SPR_setPosition(obj_shuriken[i].sprite, (obj_shuriken[i].x-8) - new_camera_x, (obj_shuriken[i].y-8) - new_camera_y);
			}
        }
    }
}

void OBJSHURIKENSDEAD(s32 x, s32 y, u8 dir)
{
    for (int i = 0; i < MAX_SHURIKENS; i++) {
        if (!obj_shurikendead[i].active) {
			P[1].totalShurikens++;
			XGM_setPCM(P1_SFX, snd_shuriken, sizeof(snd_shuriken)); XGM_startPlayPCM(P1_SFX, 1, SOUND_PCM_CH3);
            obj_shurikendead[i].x = x;
            obj_shurikendead[i].y = y;
			obj_shurikendead[i].dir = dir;
			obj_shurikendead[i].life = 9;
            obj_shurikendead[i].active = 1;
			if(obj_shurikendead[i].sprite){ SPR_releaseSprite(obj_shurikendead[i].sprite); obj_shurikendead[i].sprite = NULL; } //clear memory
			//load sprite outside of the view (320,224)
			obj_shurikendead[i].sprite = SPR_addSpriteExSafe(&spr_shuriken_dead2, 320, 224, TILE_ATTR(PAL3, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 );
			if(obj_shurikendead[i].sprite){ SPR_setDepth(obj_shurikendead[i].sprite, 1); }
			break;
        }
    }
}

void UPDATE_OBJSHURIKENSDEAD()
{
    for (int i = 0; i < MAX_SHURIKENS; i++) {
        if (obj_shurikendead[i].active) {
			
			//destroy obj_shurikendead
			if( obj_shurikendead[i].life>0 ){ obj_shurikendead[i].life--; }
			
			if (obj_shurikendead[i].life==0)
			{ 
				obj_shurikendead[i].active = 0; 
				P[1].totalShurikens--;
				if(obj_shurikendead[i].sprite)
				{ 
					SPR_releaseSprite(obj_shurikendead[i].sprite); 
					obj_shurikendead[i].sprite = NULL; 
				} 
			}
			
        }
    }
}

void DRAW_OBJSHURIKENSDEAD()
{
    for (int i = 0; i < MAX_SHURIKENS; i++) {
        if (obj_shurikendead[i].active) {
			if(obj_shurikendead[i].sprite)
			{ 
				//draw sprite
				SPR_setPosition(obj_shurikendead[i].sprite, (obj_shurikendead[i].x-12) - new_camera_x, (obj_shurikendead[i].y-12) - new_camera_y);
			}
        }
    }
}

void ENEMYS(s32 x, s32 y, u8 type, s8 dir, u8 layer)
{
    for (int i = 0; i < MAX_ENEMYS; i++) {
        if (!enemys[i].active) {
            enemys[i].x = x;
            enemys[i].y = y;
			enemys[i].dir = dir;
			enemys[i].layer = layer;
			enemys[i].life = 14*2;
			enemys[i].state = 0;
			enemys[i].type = type;
            enemys[i].active = 1;
			if(enemys[i].sprite){ SPR_releaseSprite(enemys[i].sprite); enemys[i].sprite = NULL; } //clear memory
			//load sprite outside of the view (320,224)
			if(type==1){ enemys[i].sprite = SPR_addSpriteExSafe(&spr_enemy01, 320, 224, TILE_ATTR(PAL3, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 ); }
			if(type==2){ enemys[i].sprite = SPR_addSpriteExSafe(&spr_enemy02, 320, 224, TILE_ATTR(PAL2, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 ); }
			if(type==3){ enemys[i].sprite = SPR_addSpriteExSafe(&spr_enemy03, 320, 224, TILE_ATTR(PAL2, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 ); }
			if(type==4){ enemys[i].sprite = SPR_addSpriteExSafe(&spr_enemy04, 320, 224, TILE_ATTR(PAL2, TRUE, FALSE, FALSE), 1, SPR_FLAG_AUTO_VISIBILITY | SPR_FLAG_AUTO_VRAM_ALLOC | SPR_FLAG_AUTO_TILE_UPLOAD | 0x800 ); }
			if(enemys[i].sprite)
			{ 
				SPR_setDepth(enemys[i].sprite, 2); 
				if(enemys[i].dir == -1){ SPR_setHFlip(enemys[i].sprite, TRUE); }
			}
			break;
        }
    }
}

void UPDATE_ENEMYS()
{
    for (int i = 0; i < MAX_ENEMYS; i++) {
        if (enemys[i].active) {
			
			if(enemys[i].state!=8)
			{
				if(enemys[i].myTimer<=60){enemys[i].myTimer++;}else{enemys[i].myTimer=0;}
			}else{
				if(enemys[i].myTimer<=20){enemys[i].myTimer++;}else{enemys[i].myTimer=0;}
			}
			
			if(enemys[i].x<8){enemys[i].x=8;}
			if(enemys[i].x>gBG_Width-8){enemys[i].x=gBG_Width-8;}
			
			//FSM STATES ----------------------------------------------------------------------------------------------------
			
			//KNIFE MAN
			if(enemys[i].type==1) 
			{
				//CONTACT collision
				if(P[1].x>=enemys[i].x-16 && P[1].x<=enemys[i].x+16 && P[1].y>=enemys[i].y-24 && P[1].y<=enemys[i].y && enemys[i].state!=8)
				{
					enemys[i].state = 8;
					SPR_setAnimAndFrame(enemys[i].sprite, 2, 0); 
					enemys[i].myTimer = 1;
					PLAYER_STATE(1, 8);
					JUMP_INIT(1); 
					P[1].jumpTimer=10;
					
					if(P[1].dir==1){
						P[1].x-=3; 
						enemys[i].x+=3; 
						enemys[i].dir=-1; 
						SPR_setHFlip(enemys[i].sprite, TRUE); 
					}else{
						P[1].x+=3; 
						enemys[i].x-=3; 
						enemys[i].dir=1; 
						SPR_setHFlip(enemys[i].sprite, FALSE); 
					}
				}
				//FORCE STATIC
				if(enemys[i].layer!=P[1].playerLayer)
				{
					if(enemys[i].state!=0)
					{
						enemys[i].state = 0; 
						SPR_setAnimAndFrame(enemys[i].sprite, 0, 0); 
						if(P[1].x<enemys[i].x){ enemys[i].dir=-1; SPR_setHFlip(enemys[i].sprite,  TRUE); }
						if(P[1].x>enemys[i].x){ enemys[i].dir= 1; SPR_setHFlip(enemys[i].sprite, FALSE); }
					}
				}
				//ATTACK1
				bool enableAttack = FALSE;
				if(enemys[i].dir== 1 && P[1].x>=enemys[i].x+ 2 && P[1].x<=enemys[i].x+32 && enemys[i].y==P[1].y){ enableAttack=TRUE; }
				if(enemys[i].dir==-1 && P[1].x>=enemys[i].x-32 && P[1].x<=enemys[i].x- 2 && enemys[i].y==P[1].y){ enableAttack=TRUE; }
				if(enemys[i].layer!=P[1].playerLayer){ enableAttack = FALSE; }
				if(enableAttack==TRUE && enemys[i].state!=1)
				{
					enemys[i].state=1;
					SPR_setAnimAndFrame(enemys[i].sprite, 4, 0); 
					enemys[i].myTimer=1;
				}
				if(enemys[i].state==1 && enemys[i].myTimer==30)
				{
					enemys[i].state = 0; 
					SPR_setAnimAndFrame(enemys[i].sprite, 0, 0); 
					if(P[1].x<enemys[i].x){ enemys[i].dir=-1; SPR_setHFlip(enemys[i].sprite,  TRUE); }
					if(P[1].x>enemys[i].x){ enemys[i].dir= 1; SPR_setHFlip(enemys[i].sprite, FALSE); }
				}
				//STATIC -> WALK
				if(enemys[i].state==0 && enemys[i].myTimer==0 && enemys[i].layer==P[1].playerLayer)
				{
					enemys[i].state = 4;
					SPR_setAnimAndFrame(enemys[i].sprite, 1, 0);
				}
				//WALK
				else if(enemys[i].state==4)
				{
					if(P[1].x<enemys[i].x){ enemys[i].x-=1; }else{ enemys[i].x+=1; } 
					if(enemys[i].dir== 1 && P[1].x<enemys[i].x)
					{ 
						enemys[i].state = 0; 
						SPR_setAnimAndFrame(enemys[i].sprite, 0, 0); 
						enemys[i].dir=-1; 
						SPR_setHFlip(enemys[i].sprite, TRUE); 
					}
					else if(enemys[i].dir==-1 && P[1].x>enemys[i].x)
					{ 
						enemys[i].state = 0; 
						SPR_setAnimAndFrame(enemys[i].sprite, 0, 0); 
						enemys[i].dir= 1; 
						SPR_setHFlip(enemys[i].sprite, FALSE); 
					}
				}
				//CONTACT PHYSICS
				else if(enemys[i].state==8)
				{
					if(enemys[i].myTimer>0)
					{ 
						//X
						if(enemys[i].dir==1){enemys[i].x-=2;}else{enemys[i].x+=2;} 
						//Y
						if(enemys[i].myTimer>  0 && enemys[i].myTimer<=10){ enemys[i].y--; }
						if(enemys[i].myTimer>=11 && enemys[i].myTimer<=20){ enemys[i].y++; }
					}
					if(enemys[i].myTimer==0)
					{
						if(P[1].x<enemys[i].x)
						{ 
							enemys[i].state = 0; 
							SPR_setAnimAndFrame(enemys[i].sprite, 0, 0); 
							enemys[i].dir=-1; 
							SPR_setHFlip(enemys[i].sprite, TRUE); 
						}
						else if(P[1].x>=enemys[i].x)
						{ 
							enemys[i].state = 0; 
							SPR_setAnimAndFrame(enemys[i].sprite, 0, 0); 
							enemys[i].dir= 1; 
							SPR_setHFlip(enemys[i].sprite, FALSE); 
						}
					}
				}
				
				
			}
			
			//FSM STATES (end) ----------------------------------------------------------------------------------------------------
			
			//destroy enemys
			//if( enemys[i].life>0 ){ enemys[i].life--; }
			
			//if(enemys[i].dir==1 && P[1].x<enemys[i].x){ enemys[i].dir=-1; if(enemys[i].sprite){ SPR_setHFlip(enemys[i].sprite, TRUE); }
			//}else if(enemys[i].dir==-1 && P[1].x>enemys[i].x){ enemys[i].dir=1; if(enemys[i].sprite){ SPR_setHFlip(enemys[i].sprite, FALSE); }
			//}
			
			if (enemys[i].life==0)
			{ 
				enemys[i].active = 0; 
				if(enemys[i].sprite)
				{ 
					SPR_releaseSprite(enemys[i].sprite); 
					enemys[i].sprite = NULL; 
				} 
			}
			
        }
    }
}

void DRAW_ENEMYS()
{
    for (int i = 0; i < MAX_ENEMYS; i++) {
        if (enemys[i].active) {
			if(enemys[i].sprite)
			{ 
				//draw sprite
				SPR_setPosition(enemys[i].sprite, (enemys[i].x-48) - new_camera_x, (enemys[i].y-96) - new_camera_y);
			}
        }
    }
}

void OBJ_SEED(s32 x, s32 y, u8 type, u8 layer)
{
    for (int i = 0; i < 25; i++) {
        if (!seed[i].active) {
            seed[i].x = x;
            seed[i].y = y;
			seed[i].type = type;
			seed[i].layer = layer;
			seed[i].active = 1;
			break;
        }
    }
}

void UPDATE_SEED()
{
    for (int i = 0; i < 25; i++) {
        if (seed[i].active) {
			u8 distanceActivation = 160;
			if((P[1].x>seed[i].x-distanceActivation || P[1].x<seed[i].x+distanceActivation) && seed[i].active==1)
			{
				seed[i].active = 0; 
				//ENEMYS(s32 x, s32 y, u8 type, s8 dir)
				ENEMYS(seed[i].x, seed[i].y, seed[i].type, P[1].dir*-1, seed[i].layer);
			}
        }
    }
}

void UPDATE_SCORE(u16 score)
{
	gScore+=score;
	if(gScore>999999){gScore=999999;}
	
	u8 aa = (gScore / 100000);
	u8 bb = (gScore /  10000) % 10;
	u8 cc = (gScore /   1000) % 10;
	u8 dd = (gScore /    100) % 10;
	u8 ee = (gScore /     10) % 10;
	u8 ff = (gScore % 10);
	
	u8 a = 0;
	if(gPing20<10){a=0;}else{a=1;}
	
	if(hudElement[11]){ SPR_setAnimAndFrame(hudElement[11], a, aa); }
	if(hudElement[12]){ SPR_setAnimAndFrame(hudElement[12], a, bb); }
	if(hudElement[13]){ SPR_setAnimAndFrame(hudElement[13], a, cc); }
	if(hudElement[14]){ SPR_setAnimAndFrame(hudElement[14], a, dd); }
	if(hudElement[15]){ SPR_setAnimAndFrame(hudElement[15], a, ee); }
	if(hudElement[16]){ SPR_setAnimAndFrame(hudElement[16], a, ff); }
}

//EOF - End of File