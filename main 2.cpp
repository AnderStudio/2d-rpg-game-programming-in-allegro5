#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <math.h>

#define LOG_ENABLED

// Frame rate (frame per second)
const int FPS = 60;
// Display (screen) width.
const int SCREEN_W = 1024;
// Display (screen) height.
const int SCREEN_H= 615;
// At most 4 audios can be played at a time.
const int RESERVE_SAMPLES = 4;

enum {
	SCENE_menu = 1,
	SCENE_help1 = 2,
	SCENE_help2 = 3,
	SCENE_help3 = 4,
	SCENE_first_level = 5,
	SCENE_second_level = 6,
	SCENE_boss_level = 7,
    SCENE_equipment = 8,
    badEnd = 9,
    goodEnd = 10
};
enum {DOWN, LEFT, RIGHT,UP};
enum {BASIC, SPEAR, WATERBALL};

int active_scene;
int active_weapon;
bool key_state[ALLEGRO_KEY_MAX];
bool *mouse_state;
int mouse_x, mouse_y, mouse_button;

// ALLEGRO Variables
ALLEGRO_DISPLAY* display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER* game_update_timer;
ALLEGRO_KEYBOARD_STATE keyState ;

ALLEGRO_BITMAP *menu = NULL;
ALLEGRO_BITMAP *help_background = NULL;
ALLEGRO_BITMAP *help_book1 = NULL;
ALLEGRO_BITMAP *help_book2 = NULL;
ALLEGRO_BITMAP *help_book3 = NULL;
ALLEGRO_BITMAP *first_level_background = NULL;
ALLEGRO_BITMAP *second_level_background = NULL;
ALLEGRO_BITMAP *fireplace = NULL;
ALLEGRO_BITMAP *magicpot = NULL;
ALLEGRO_BITMAP *boss_level_background = NULL;
ALLEGRO_BITMAP *win_img = NULL;

ALLEGRO_BITMAP *bullet_img = NULL;
ALLEGRO_BITMAP *character_img = NULL;
ALLEGRO_BITMAP *first_level_mon1_img = NULL;
ALLEGRO_BITMAP *first_level_mon2_img = NULL;
ALLEGRO_BITMAP *first_level_mon3_img = NULL;
ALLEGRO_BITMAP *second_level_mon1_img = NULL;
ALLEGRO_BITMAP *boss2_img = NULL;
ALLEGRO_BITMAP *boss_img = NULL;
ALLEGRO_BITMAP *boss_rotate_img = NULL;
ALLEGRO_BITMAP *spear_img = NULL;
ALLEGRO_BITMAP *waterball_img = NULL;
ALLEGRO_BITMAP *waterball2_img = NULL;
ALLEGRO_BITMAP *water_weapon_img = NULL;
ALLEGRO_BITMAP *grass_weapon_img = NULL;
ALLEGRO_BITMAP *fire_weapon_img = NULL;
ALLEGRO_BITMAP *water_skill_img = NULL;
ALLEGRO_BITMAP *grass_skill_img = NULL;
ALLEGRO_BITMAP *fire_skill_img = NULL;
ALLEGRO_BITMAP *vine_img = NULL;

ALLEGRO_BITMAP *C_key_white_img = NULL;
ALLEGRO_BITMAP *F_key_white_img = NULL;
ALLEGRO_BITMAP *F_key_black_img = NULL;
ALLEGRO_BITMAP *win_background = NULL;
ALLEGRO_BITMAP *lose_background = NULL;

ALLEGRO_BITMAP *lux[78];
ALLEGRO_BITMAP *fire[8];
ALLEGRO_BITMAP *water[7];



ALLEGRO_SAMPLE *silver;
ALLEGRO_SAMPLE_ID silver_id;
ALLEGRO_SAMPLE *warriors;
ALLEGRO_SAMPLE_ID warriors_id;


ALLEGRO_FONT *font16 = NULL;
ALLEGRO_FONT *font32 = NULL;
ALLEGRO_FONT *font64 = NULL;

typedef struct {
	// The center coordinate of the image.
	float x, y;
	// The width and height of the object.
	float w, h;
	// The velocity in x, y axes.
	float vx, vy;
	// Should we draw this object on the screen.
	bool hidden;
	// The pointer to the object?™s image.
	ALLEGRO_BITMAP* img;

	int hp;

} MovableObject;

MovableObject character;
MovableObject first_level_mon1;
MovableObject first_level_mon2;
MovableObject first_level_mon3;
MovableObject second_level_mon1;
MovableObject boss2[6];
MovableObject boss;
MovableObject basic_bullet[100];
MovableObject spear[100];
MovableObject waterball[100];
MovableObject waterball2[100];


double last_time_temp_firstmon_emerge;
double first_level_mon1_emerge_cooldown = 2;
double last_time_temp_basic_bullet_shoot;
double basic_bullet_shoot_cooldown = 0.5;
double last_time_temp_firstmon_shoot;
double first_level_mon1_shoot_cooldown = 0.5;
double last_time_temp_secmon_emerge;
double second_level_mon1_emerge_cooldown = 0.05;
double last_time_temp_spear_shoot;
double spear_shoot_cooldown = 2;
double last_time_temp_waterball_bomb;
double water_ball_cooldown = 1;
double last_time_temp_boss2_run;
double boss2_run_cooldown = 2;

int dir;
bool active = true;
int sourceX, sourceY;
float temp_vx, temp_vy;
int cool_basic;
int cool_spear;
int cool_waterball;
int cool_strap;
int cool_firstmon_atk;
int cool_boss_atk;
int cool_boss2_atk;
int cool_lux;
int cool_f = 300;
int lux_number;
bool strap;
bool skill_image_hidden = false;
bool silver_play = false;
int black, black2, black3;
bool fire_emerge = false;
int cool_fire;
int fire_number;
bool water_emerge = false;
int cool_water;
int water_number;

int max_bullet = 100;

bool exited=false;

bool point_in_the_region(int px, int py, int x, int y, int w, int h);
float calvx(float y2,float y1,float x2,float x1);
float calvy(float y2,float y1,float x2,float x1);
void on_mouse_down(int bn,int x,int y);
void change_weapon(void);

/* Declare function prototypes. */

// Initialize variables and resources.
// Allows the game to perform any initialization it needs before
// starting to run.
void game_init(void);
// Initialize allegro5 library
void allegro5_init(void);
// Process events inside the event queue using an infinity loop.
void game_start_event_loop(void);
// Run game logic such as updating the world, checking for collision,
// switching scenes and so on.
// This is called when the game should update its logic.
void game_update(void);
// Draw to display.
// This is called when the game should draw itself.
void game_draw(void);
// Release resources.
// Free the pointers we allocated.
void game_destroy(void);

void game_change_scene(int next_scene);

/* Declare function prototypes for debugging. */

// Display error message and exit the program, used like 'printf'.
// Write formatted output to stdout and file from the format string.
// If the program crashes unexpectedly, you can inspect "log.txt" for
// further information.
void game_abort(const char* format, ...);
// Log events for later debugging, used like 'printf'.
// Write formatted output to stdout and file from the format string.
// You can inspect "log.txt" for logs in the last run.
void game_log(const char* format, ...);
// Called by 'game_abort', 'game_log' to deal with va_lists.
void game_vlog(const char* format, va_list arg);


int main(int argc, char** argv) {
	allegro5_init();
	game_log("Allegro5 initialized");
	game_log("Game begin");
	// Initialize game variables.
	game_init();
	game_log("Game initialized");
	// Draw the first frame.
	game_draw();
	game_log("Game start event loop");
	// This call blocks until the game is finished.
	game_start_event_loop();
	game_log("Game end");
	game_destroy();
	return 0;
}

void allegro5_init(void) {
	if (!al_init())
		game_abort("failed to initialize allegro");

	// Initialize add-ons.
	if (!al_init_primitives_addon())
		game_abort("failed to initialize primitives add-on");
	al_init_font_addon();
	if (!al_init_ttf_addon())
		game_abort("failed to initialize ttf add-on");
	if (!al_init_image_addon())
		game_abort("failed to initialize image add-on");
	if (!al_install_audio())
		game_abort("failed to initialize audio add-on");
	if (!al_init_acodec_addon())
		game_abort("failed to initialize audio codec add-on");
    if (!al_reserve_samples(RESERVE_SAMPLES))
		game_abort("failed to reserve samples");
	if (!al_install_keyboard())
		game_abort("failed to install keyboard");
	if (!al_install_mouse())
		game_abort("failed to install mouse");
	// TODO: Initialize other addons such as video, ...

	// Setup game display.
	display = al_create_display(SCREEN_W, SCREEN_H);
	if (!display)
		game_abort("failed to create display");
	al_set_window_title(display, "Final Project <108020022>");

	// Setup update timer.
	game_update_timer = al_create_timer(1.0f / FPS);
	if (!game_update_timer)
		game_abort("failed to create timer");

	// Setup event queue.
	event_queue = al_create_event_queue();
	if (!event_queue)
		game_abort("failed to create event queue");

	// Malloc mouse buttons state according to button counts.
	const unsigned m_buttons = al_get_mouse_num_buttons();
	game_log("There are total %u supported mouse buttons", m_buttons);
	// mouse_state[0] will not be used.
	mouse_state = (bool *)malloc((m_buttons + 1) * sizeof(bool));
	memset(mouse_state, false, (m_buttons + 1) * sizeof(bool));

	// Register display, timer, keyboard, mouse events to the event queue.
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(game_update_timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_mouse_event_source());
	// TODO: Register other event sources such as timer, video, ...

	// Start the timer to update and draw the game.
	al_start_timer(game_update_timer);
}

void game_init() {
    //font resources
    font16 = al_load_font("pirulen.ttf", 16, 0);
    font32 = al_load_font("pirulen.ttf", 32, 0);
    font64 = al_load_font("pirulen.ttf", 64, 0);

    //song resources
    silver = al_load_sample("warriors.ogg");
    warriors = al_load_sample("silver.ogg");

    //shared resources
    bullet_img = al_load_bitmap("bullet.png");
    spear_img = al_load_bitmap("spear.png");
    character_img = al_load_bitmap("character.png");
    first_level_mon1_img = al_load_bitmap("first_level_monster1.png");
    first_level_mon2_img = al_load_bitmap("first_level_monster2.png");
    first_level_mon3_img = al_load_bitmap("first_level_monster3.png");
    second_level_mon1_img = al_load_bitmap("ice_monster.png");
    boss2_img = al_load_bitmap("first_level_monster3.png");
    boss_img = al_load_bitmap("boss.png");
    boss_rotate_img = al_load_bitmap("boss_rotate.png");

    water_weapon_img = al_load_bitmap("water_weapon.jpg");
    grass_weapon_img = al_load_bitmap("grass_weapon.jpg");
    fire_weapon_img = al_load_bitmap("fire_weapon.jpg");
    water_skill_img = al_load_bitmap("water_skill.png");
    grass_skill_img = al_load_bitmap("grass_skill.png");
    fire_skill_img = al_load_bitmap("fire_skill.png");
    vine_img = al_load_bitmap("vine.png");

    C_key_white_img = al_load_bitmap("C_key_white.png");
    F_key_white_img = al_load_bitmap("F_key_white.png");
    F_key_black_img = al_load_bitmap("F_key_black.png");

    lux[0] = al_load_bitmap("20161129100551384-0000.jpg");
    lux[1] = al_load_bitmap("20161129100551384-0001.jpg");
    lux[2] = al_load_bitmap("20161129100551384-0002.jpg");
    lux[3] = al_load_bitmap("20161129100551384-0003.jpg");
    lux[4] = al_load_bitmap("20161129100551384-0004.jpg");
    lux[5] = al_load_bitmap("20161129100551384-0005.jpg");
    lux[6] = al_load_bitmap("20161129100551384-0006.jpg");
    lux[7] = al_load_bitmap("20161129100551384-0007.jpg");
    lux[8] = al_load_bitmap("20161129100551384-0008.jpg");
    lux[9] = al_load_bitmap("20161129100551384-0009.jpg");
    lux[10] = al_load_bitmap("20161129100551384-0010.jpg");
    lux[11] = al_load_bitmap("20161129100551384-0011.jpg");
    lux[12] = al_load_bitmap("20161129100551384-0012.jpg");
    lux[13] = al_load_bitmap("20161129100551384-0013.jpg");
    lux[14] = al_load_bitmap("20161129100551384-0014.jpg");
    lux[15] = al_load_bitmap("20161129100551384-0015.jpg");
    lux[16] = al_load_bitmap("20161129100551384-0016.jpg");
    lux[17] = al_load_bitmap("20161129100551384-0017.jpg");
    lux[18] = al_load_bitmap("20161129100551384-0018.jpg");
    lux[19] = al_load_bitmap("20161129100551384-0019.jpg");
    lux[20] = al_load_bitmap("20161129100551384-0020.jpg");
    lux[21] = al_load_bitmap("20161129100551384-0021.jpg");
    lux[22] = al_load_bitmap("20161129100551384-0022.jpg");
    lux[23] = al_load_bitmap("20161129100551384-0023.jpg");
    lux[24] = al_load_bitmap("20161129100551384-0024.jpg");
    lux[25] = al_load_bitmap("20161129100551384-0025.jpg");
    lux[26] = al_load_bitmap("20161129100551384-0026.jpg");
    lux[27] = al_load_bitmap("20161129100551384-0027.jpg");
    lux[28] = al_load_bitmap("20161129100551384-0028.jpg");
    lux[29] = al_load_bitmap("20161129100551384-0029.jpg");
    lux[30] = al_load_bitmap("20161129100551384-0030.jpg");
    lux[31] = al_load_bitmap("20161129100551384-0031.jpg");
    lux[32] = al_load_bitmap("20161129100551384-0032.jpg");
    lux[33] = al_load_bitmap("20161129100551384-0033.jpg");
    lux[34] = al_load_bitmap("20161129100551384-0034.jpg");
    lux[35] = al_load_bitmap("20161129100551384-0035.jpg");
    lux[36] = al_load_bitmap("20161129100551384-0036.jpg");
    lux[37] = al_load_bitmap("20161129100551384-0037.jpg");
    lux[38] = al_load_bitmap("20161129100551384-0038.jpg");
    lux[39] = al_load_bitmap("20161129100551384-0039.jpg");
    lux[40] = al_load_bitmap("20161129100551384-0040.jpg");
    lux[41] = al_load_bitmap("20161129100551384-0041.jpg");
    lux[42] = al_load_bitmap("20161129100551384-0042.jpg");
    lux[43] = al_load_bitmap("20161129100551384-0043.jpg");
    lux[44] = al_load_bitmap("20161129100551384-0044.jpg");
    lux[45] = al_load_bitmap("20161129100551384-0045.jpg");
    lux[46] = al_load_bitmap("20161129100551384-0046.jpg");
    lux[47] = al_load_bitmap("20161129100551384-0047.jpg");
    lux[48] = al_load_bitmap("20161129100551384-0048.jpg");
    lux[49] = al_load_bitmap("20161129100551384-0049.jpg");
    lux[50] = al_load_bitmap("20161129100551384-0050.jpg");
    lux[51] = al_load_bitmap("20161129100551384-0051.jpg");
    lux[52] = al_load_bitmap("20161129100551384-0052.jpg");
    lux[53]= al_load_bitmap("20161129100551384-0053.jpg");
    lux[54] = al_load_bitmap("20161129100551384-0054.jpg");
    lux[55] = al_load_bitmap("20161129100551384-0055.jpg");
    lux[56] = al_load_bitmap("20161129100551384-0056.jpg");
    lux[57] = al_load_bitmap("20161129100551384-0057.jpg");
    lux[58] = al_load_bitmap("20161129100551384-0058.jpg");
    lux[59] = al_load_bitmap("20161129100551384-0059.jpg");
    lux[60] = al_load_bitmap("20161129100551384-0060.jpg");
    lux[61] = al_load_bitmap("20161129100551384-0061.jpg");
    lux[62] = al_load_bitmap("20161129100551384-0062.jpg");
    lux[63] = al_load_bitmap("20161129100551384-0063.jpg");
    lux[64] = al_load_bitmap("20161129100551384-0064.jpg");
    lux[65] = al_load_bitmap("20161129100551384-0065.jpg");
    lux[66] = al_load_bitmap("20161129100551384-0066.jpg");
    lux[67] = al_load_bitmap("20161129100551384-0067.jpg");
    lux[68] = al_load_bitmap("20161129100551384-0068.jpg");
    lux[69] = al_load_bitmap("20161129100551384-0069.jpg");
    lux[70] = al_load_bitmap("20161129100551384-0070.jpg");
    lux[71] = al_load_bitmap("20161129100551384-0071.jpg");
    lux[72] = al_load_bitmap("20161129100551384-0072.jpg");
    lux[73] = al_load_bitmap("20161129100551384-0073.jpg");
    lux[74] = al_load_bitmap("20161129100551384-0074.jpg");
    lux[75] = al_load_bitmap("20161129100551384-0075.jpg");
    lux[76] = al_load_bitmap("20161129100551384-0076.jpg");
    lux[77] = al_load_bitmap("20161129100551384-0077.jpg");

    fire[0] = al_load_bitmap("1185-1.png");
    fire[1] = al_load_bitmap("1185-2.png");
    fire[2] = al_load_bitmap("1185-3.png");
    fire[3] = al_load_bitmap("1185-4.png");
    fire[4] = al_load_bitmap("1185-5.png");
    fire[5] = al_load_bitmap("1185-6.png");
    fire[6] = al_load_bitmap("1185-7.png");
    fire[7] = al_load_bitmap("1185-8.png");

    water[0] = al_load_bitmap("1150-1.png");
    water[1] = al_load_bitmap("1150-2.png");
    water[2] = al_load_bitmap("1150-3.png");
    water[3] = al_load_bitmap("1150-4.png");
    water[4] = al_load_bitmap("1150-5.png");
    water[5] = al_load_bitmap("1150-6.png");
    water[6] = al_load_bitmap("1150-7.png");


    //menu resources
    menu = al_load_bitmap("menu.jpg");
    //help resources
    help_background = al_load_bitmap("instruction.jpg");
	help_book1 = al_load_bitmap("instruction1.png");
	help_book2 = al_load_bitmap("instruction2.png");
	help_book3 = al_load_bitmap("instruction3.png");
	//first level resources
	first_level_background = al_load_bitmap("first_level.jpg");

	//second level resources
	second_level_background = al_load_bitmap("second_level.jpg");
	//fireplace resources
	fireplace = al_load_bitmap("fireplace.jpg");
	magicpot = al_load_bitmap("magicpot.jpg");
	//boss level resources
	boss_level_background = al_load_bitmap("boss_level.jpg");
	waterball_img = al_load_bitmap("waterball.png");
	waterball2_img = al_load_bitmap("waterball2.png");
	//win
	win_img = al_load_bitmap("win.jpg");
	win_background = al_load_bitmap("win_background.jpg");
	//lose
    lose_background = al_load_bitmap("lose_background.png");
	//movable object img
	character.img = character_img;
    first_level_mon1.img = first_level_mon1_img;
    first_level_mon2.img = first_level_mon2_img;
    first_level_mon3.img = first_level_mon3_img;
    second_level_mon1.img = second_level_mon1_img;
    for(int i=0;i<6;i++) boss2[i].img = boss2_img;
    boss.img = boss_img;

    // Change to first scene.

	game_change_scene(SCENE_menu);

}

void game_start_event_loop(void) {
	bool done = false;
	ALLEGRO_EVENT event;
	int redraws = 0;
	while (!done) {

		al_wait_for_event(event_queue, &event);

		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE||exited==true) {
			// Event for clicking the window close button.
			game_log("Window close button clicked");
			done = true;
		} else if (event.type == ALLEGRO_EVENT_TIMER) {
			// Event for redrawing the display.
			if (event.timer.source == game_update_timer)
				// The redraw timer has ticked.
				redraws++;
		} else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
			// Event for keyboard key down.
			game_log("Key with keycode %d down", event.keyboard.keycode);
			key_state[event.keyboard.keycode] = true;
			//on_key_down(event.keyboard.keycode);
			if(event.keyboard.keycode == ALLEGRO_KEY_C){
                if(active_scene == SCENE_second_level){
                    if(active_weapon == BASIC) active_weapon =  SPEAR;
                    else if(active_weapon == SPEAR) active_weapon =  BASIC;
                    change_weapon();
                    //printf("active weapon change to %d\n",active_weapon);
                }
                else if(active_scene == SCENE_boss_level){
                    if(active_weapon == BASIC) active_weapon =  SPEAR;
                    else if(active_weapon == SPEAR) active_weapon =  WATERBALL;
                    else if(active_weapon ==  WATERBALL) active_weapon =  BASIC;
                    change_weapon();
                }
            }
            if(cool_f >= 300){
                skill_image_hidden = false;
                if(event.keyboard.keycode == ALLEGRO_KEY_F){
                    if(active_weapon == BASIC){
                        basic_bullet_shoot_cooldown = 0.1;
                    }
                    else if(active_weapon == SPEAR){
                        strap = true;
                    }
                    else if(active_weapon ==  WATERBALL){
                        character.hp = 20;
                        water_emerge = true;
                    }
                    cool_f = 0;
                    skill_image_hidden = true;
                }

           }

		} else if (event.type == ALLEGRO_EVENT_KEY_UP) {
			// Event for keyboard key up.
			game_log("Key with keycode %d up", event.keyboard.keycode);
			key_state[event.keyboard.keycode] = false;
		} else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			// Event for mouse key down.
			//game_log("Mouse button %d down at (%d, %d)", event.mouse.button, event.mouse.x, event.mouse.y);
			mouse_state[event.mouse.button] = true;
			mouse_button = event.mouse.button;
			on_mouse_down(event.mouse.button, event.mouse.x, event.mouse.y);
			mouse_x = event.mouse.x;
			mouse_y = event.mouse.y;
			//printf("after on mouse down");
		} else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
			// Event for mouse key up.
			game_log("Mouse button %d up at (%d, %d)", event.mouse.button, event.mouse.x, event.mouse.y);
			mouse_state[event.mouse.button] = false;
			mouse_button = 0;
		} else if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
			if (event.mouse.dx != 0 || event.mouse.dy != 0) {
				// Event for mouse move.
				//game_log("Mouse move to (%d, %d)", event.mouse.x, event.mouse.y);
				mouse_x = event.mouse.x;
				mouse_y = event.mouse.y;
			} else if (event.mouse.dz != 0) {
				// Event for mouse scroll.
				game_log("Mouse scroll at (%d, %d) with delta %d", event.mouse.x, event.mouse.y, event.mouse.dz);
			}
		}

		// Redraw
		//printf("%d\n",active_scene);
		if (redraws > 0 && al_is_event_queue_empty(event_queue)) {
			// if (redraws > 1)
			// 	game_log("%d frame(s) dropped", redraws - 1);
			// Update and draw the next frame.
			game_update();
			game_draw();

			redraws = 0;
		}
	}
}
void game_update(void){
    double now=al_get_time();
    cool_f++;

    //character movement
        if (key_state[ALLEGRO_KEY_UP] || key_state[ALLEGRO_KEY_W]){
            if(character.y>=180)
                character.y -= 5;
            dir = UP;
        }
        else if (key_state[ALLEGRO_KEY_DOWN] || key_state[ALLEGRO_KEY_S]){
            if(character.y<=580)
                character.y += 5;
            dir = DOWN;
        }
        else if (key_state[ALLEGRO_KEY_LEFT] || key_state[ALLEGRO_KEY_A]){
            if(character.x>=0)
                character.x -= 5;
            dir = LEFT;
        }
        else if (key_state[ALLEGRO_KEY_RIGHT] || key_state[ALLEGRO_KEY_D]){
            if(character.x<=990)
                character.x += 5;
            dir = RIGHT;
        }
        else active = false;
        if(active) sourceX += (al_get_bitmap_width(character.img)/3);

        if(sourceX >= al_get_bitmap_width(character.img))
        sourceX = 0;

        sourceY = dir;


    if(active_scene == SCENE_first_level){



        //first level monster movement
        if(now - last_time_temp_firstmon_emerge >= first_level_mon1_emerge_cooldown){
            first_level_mon1.x = rand() % (SCREEN_W/2-100) + 500;
            first_level_mon1.y = rand() %  (SCREEN_H-100);
            last_time_temp_firstmon_emerge = now;
        }

        /*first_level_mon2.x += first_level_mon2.vx;
        first_level_mon2.y += first_level_mon2.vy;
        if(first_level_mon2.x<512) first_level_mon2.vx=2;
        if(first_level_mon2.x>900) first_level_mon2.vx=-2;
        if(first_level_mon2.y<0) first_level_mon2.vy=2;
        if(first_level_mon2.y>500) first_level_mon2.vy=-2;*/


        int i;
        for (i=0;i<max_bullet;i++) {
			if 	(basic_bullet[i].hidden == true)
				continue;

			if(point_in_the_region(basic_bullet[i].x, basic_bullet[i].y, character.x, character.y, character.w, character.h)){
                basic_bullet[i].vx = calvx(mouse_y,character.y,mouse_x,character.x);
                basic_bullet[i].vy = calvy(mouse_y,character.y,mouse_x,character.x);
			}

			basic_bullet[i].x += basic_bullet[i].vx*4;
			basic_bullet[i].y += basic_bullet[i].vy*4;

			if(point_in_the_region(basic_bullet[i].x, basic_bullet[i].y, first_level_mon1.x+first_level_mon1.w/2, first_level_mon1.y+first_level_mon1.h/2, first_level_mon1.w, first_level_mon1.h)){
					basic_bullet[i].hidden=true;
					first_level_mon1.hp -=1 ;
					if(first_level_mon1.hp<=0){
                        first_level_mon1.hidden = true;
                        change_weapon();
                        game_change_scene(SCENE_second_level);
					}

            }
            if (basic_bullet[i].y<0||basic_bullet[i].y>615||basic_bullet[i].x<0||basic_bullet[i].x>1024)
				basic_bullet[i].hidden = true;
        }

        if(basic_bullet_shoot_cooldown == 0.1){
            cool_basic++;
            fire_emerge = true;
            if(cool_basic >= 120){
                basic_bullet_shoot_cooldown = 0.5;
                cool_basic = 0;
                fire_emerge = false;
            }

        }
        if (mouse_button==1) {
		    for (i=0;i<max_bullet;i++) {
		        if (basic_bullet[i].hidden)
		           break;
		    }
		    if (i<max_bullet && (now - last_time_temp_basic_bullet_shoot >= basic_bullet_shoot_cooldown)){
		        basic_bullet[i].hidden = false;
		        basic_bullet[i].x = character.x;
		        basic_bullet[i].y = character.y;
		        last_time_temp_basic_bullet_shoot = now;

		    }
		}

		for (i=0;i<max_bullet;i++) {
			if 	(spear[i].hidden == true)
				continue;

			if(spear[i].x==first_level_mon1.x && spear[i].y==first_level_mon1.y){
                spear[i].vx = calvx(character.y,first_level_mon1.y,character.x,first_level_mon1.x);
                spear[i].vy = calvy(character.y,first_level_mon1.y,character.x,first_level_mon1.x);
			}

			spear[i].x += spear[i].vx*2;
			spear[i].y += spear[i].vy*2;

			if(point_in_the_region(spear[i].x+spear[0].w/2, spear[i].y+spear[0].h/2, character.x+character.w/2, character.y+character.h/2, character.w, character.h)){
					spear[i].hidden=true;
					character.hp -=2 ;
					printf("%d\n",character.hp);
					if(character.hp<=0){
                        game_change_scene(badEnd);
                        //game_change_scene(SCENE_second_level);
					}

            }
            if (spear[i].y<0||spear[i].y>615||spear[i].x<0||spear[i].x>1024)
				spear[i].hidden = true;
        }
        if (now - last_time_temp_firstmon_shoot >= first_level_mon1_shoot_cooldown) {
		    for (i=0;i<max_bullet;i++) {
		        if (spear[i].hidden)
		           break;
		    }
		    if (i<max_bullet && (now - last_time_temp_basic_bullet_shoot >= basic_bullet_shoot_cooldown)){
		        spear[i].hidden = false;
		        spear[i].x = first_level_mon1.x;
		        spear[i].y = first_level_mon1.y;
		        last_time_temp_firstmon_shoot = now;

		    }
		}




    }
    else if(active_scene == SCENE_menu){


    }
    else if(active_scene == SCENE_second_level){



            second_level_mon1.vx = calvx(character.y,second_level_mon1.y,character.x,second_level_mon1.x);
            second_level_mon1.vy = calvy(character.y,second_level_mon1.y,character.x,second_level_mon1.x);
            if(strap){
                        second_level_mon1.vx = 0;
                        second_level_mon1.vy = 0;
                        cool_strap++;
                        if(cool_strap == 60){
                            strap = false;
                            cool_strap = 0;
                        }

            }

            if (now - last_time_temp_secmon_emerge >= second_level_mon1_emerge_cooldown) {
                second_level_mon1.x += second_level_mon1.vx*8;
                second_level_mon1.y += second_level_mon1.vy*8;
                last_time_temp_secmon_emerge = now;
            }

            if(point_in_the_region(character.x+character.w/2, character.y+character.h/2, second_level_mon1.x+second_level_mon1.w/2, second_level_mon1.y+second_level_mon1.h/2, second_level_mon1.w, second_level_mon1.h)){
                cool_firstmon_atk++;
                if(cool_firstmon_atk == 60){
                    character.hp -= 1;
                    cool_firstmon_atk = 0;
                }
                if(character.hp<=0)
                    game_change_scene(badEnd);

                printf("%d\n",character.hp);
            }

            if(active_weapon == SPEAR){
                int i;
                for (i=0;i<max_bullet;i++) {
                    if 	(spear[i].hidden == true)
                        continue;

                    if(point_in_the_region(spear[i].x, spear[i].y, character.x, character.y, character.w, character.h)){
                        spear[i].vx = calvx(mouse_y,character.y,mouse_x,character.x);
                        spear[i].vy = calvy(mouse_y,character.y,mouse_x,character.x);
                    }



                    spear[i].x += spear[i].vx*4;
                    spear[i].y += spear[i].vy*4;

                    if(point_in_the_region(spear[i].x, spear[i].y, second_level_mon1.x+second_level_mon1.w/2, second_level_mon1.y+second_level_mon1.h/2, second_level_mon1.w, second_level_mon1.h)){
                            second_level_mon1.x += calvx(second_level_mon1.y,character.y,second_level_mon1.x,character.x)*100;
                            second_level_mon1.y += calvy(second_level_mon1.y,character.y,second_level_mon1.x,character.x)*100;
                            spear[i].hidden=true;
                            second_level_mon1.hp -= 3;
                            if(second_level_mon1.hp<=0){
                                second_level_mon1.hidden = true;
                                change_weapon();
                                game_change_scene(SCENE_boss_level);
                            }

                    }
                    if (spear[i].y<0||spear[i].y>615||spear[i].x<0||spear[i].x>1024)
                        spear[i].hidden = true;
                }

                if (mouse_button==1) {
                    for (i=0;i<max_bullet;i++) {
                        if (spear[i].hidden)
                        break;
                    }
                    if (i<max_bullet && (now - last_time_temp_spear_shoot >= spear_shoot_cooldown)){
                        spear[i].hidden = false;
                        spear[i].x = character.x;
                        spear[i].y = character.y;
                        last_time_temp_spear_shoot = now;
                    }
                }
            }
            if(active_weapon == BASIC){
                int i;
                for (i=0;i<max_bullet;i++) {
                if 	(basic_bullet[i].hidden == true)
                    continue;

                if(point_in_the_region(basic_bullet[i].x, basic_bullet[i].y, character.x, character.y, character.w, character.h)){
                    basic_bullet[i].vx = calvx(mouse_y,character.y,mouse_x,character.x);
                    basic_bullet[i].vy = calvy(mouse_y,character.y,mouse_x,character.x);
                }

                basic_bullet[i].x += basic_bullet[i].vx*4;
                basic_bullet[i].y += basic_bullet[i].vy*4;

                if(point_in_the_region(basic_bullet[i].x, basic_bullet[i].y, second_level_mon1.x+second_level_mon1.w/2, second_level_mon1.y+second_level_mon1.h/2, second_level_mon1.w, second_level_mon1.h)){
                        basic_bullet[i].hidden=true;
                        second_level_mon1.hp -=1 ;
                        if(second_level_mon1.hp<=0){
                            second_level_mon1.hidden = true;
                            change_weapon();
                            game_change_scene(SCENE_boss_level);
                        }

                }
                if (basic_bullet[i].y<0||basic_bullet[i].y>615||basic_bullet[i].x<0||basic_bullet[i].x>1024)
                    basic_bullet[i].hidden = true;
                }

                if(basic_bullet_shoot_cooldown == 0.1){
                    cool_basic++;
                    fire_emerge = true;
                    if(cool_basic >= 120){
                        basic_bullet_shoot_cooldown = 0.5;
                        cool_basic = 0;
                        fire_emerge = false;
                    }

                }
                if (mouse_button==1) {
                    for (i=0;i<max_bullet;i++) {
                        if (basic_bullet[i].hidden)
                        break;
                    }
                    if (i<max_bullet && (now - last_time_temp_basic_bullet_shoot >= basic_bullet_shoot_cooldown)){
                        basic_bullet[i].hidden = false;
                        basic_bullet[i].x = character.x;
                        basic_bullet[i].y = character.y;
                        last_time_temp_basic_bullet_shoot = now;

                    }
                }



            }





    }
    else if(active_scene == SCENE_boss_level){


        boss.vx = calvx(character.y,boss.y,character.x,boss.x);
        boss.vy = calvy(character.y,boss.y,character.x,boss.x);
        if(strap){
                boss.vx = 0;
                boss.vy = 0;
                for(int i=0;i<6;i++) boss2[i].vx = 0;

                cool_strap++;
                if(cool_strap == 60){
                    strap = false;
                    cool_strap = 0;
                }

        }
        else for(int i=0;i<6;i++) boss2[i].vx = -5;



        if (now - last_time_temp_secmon_emerge >= second_level_mon1_emerge_cooldown) {
            boss.x += boss.vx*8;
            boss.y += boss.vy*8;
            last_time_temp_secmon_emerge = now;
        }

        if(boss.vx > 0) boss.img = boss_rotate_img;
        else if(boss.vx < 0) boss.img = boss_img;

        int i;
        for (i=0;i<6;i++) {
			if 	(boss2[i].hidden == true)
				continue;

			boss2[i].x += boss2[i].vx;
			boss2[i].y += boss2[i].vy;


			/*if(sqrt((boss.x+69-waterball[i].x)*(boss.x+69-waterball[i].x)+(boss.y+59-waterball[i].y)*(boss.y+59-waterball[i].y))<100){

					boss.hp -=1 ;
					if(!boss.hp){
                        boss.hidden = true;
					}

            }*/
            if (boss2[i].y<0||boss2[i].y>615||boss2[i].x<0||boss2[i].x>1024)
				boss2[i].hidden = true;
        }
        for (i=0;i<max_bullet;i++) {
            if (boss2[i].hidden)
                break;
        }
        if (i<6 && (now - last_time_temp_boss2_run >= boss2_run_cooldown)){
            boss2[i].hidden = false;
            boss2[i].x = 1024;
            boss2[i].y = rand() % SCREEN_H;
            last_time_temp_boss2_run = now;

        }

        if(point_in_the_region(character.x+character.w/2, character.y+character.h/2, boss.x+boss.w/2, boss.y+boss.h/2, boss.w, boss.h)){
            cool_boss_atk++;
            if(cool_boss_atk == 60){
                character.hp -= 3;
                cool_boss_atk = 0;
            }
            if(character.hp<=0)
                game_change_scene(badEnd);

            printf("%d\n",character.hp);
        }
        for(int i=0;i<6;i++){

            if(point_in_the_region(character.x+character.w/2, character.y+character.h/2, boss2[i].x+boss2[i].w/2, boss2[i].y+boss2[i].h/2, boss2[i].w, boss2[i].h)){
                cool_boss_atk++;
                if(cool_boss_atk == 60){
                    character.hp -= 3;
                    cool_boss_atk = 0;
                }
                if(character.hp<=0)
                    game_change_scene(badEnd);
                printf("%d\n",character.hp);
            }
        }





            if(active_weapon == SPEAR){
                int i;
                for (i=0;i<max_bullet;i++) {
                    if 	(spear[i].hidden == true)
                        continue;

                    if(point_in_the_region(spear[i].x, spear[i].y, character.x, character.y, character.w, character.h)){
                        spear[i].vx = calvx(mouse_y,character.y,mouse_x,character.x);
                        spear[i].vy = calvy(mouse_y,character.y,mouse_x,character.x);
                    }



                    spear[i].x += spear[i].vx*4;
                    spear[i].y += spear[i].vy*4;

                    if(point_in_the_region(spear[i].x, spear[i].y, boss.x+boss.w/2, boss.y+boss.h/2, boss.w, boss.h)){
                            boss.x += calvx(boss.y,character.y,boss.x,character.x)*100;
                            boss.y += calvy(boss.y,character.y,boss.x,character.x)*100;
                            spear[i].hidden=true;
                            boss.hp -=1 ;
                            if(boss.hp<=0){
                                boss.hidden = true;
                                game_change_scene(goodEnd);
                            }

                    }
                    if (spear[i].y<0||spear[i].y>615||spear[i].x<0||spear[i].x>1024)
                        spear[i].hidden = true;
                }

                if (mouse_button==1) {
                    for (i=0;i<max_bullet;i++) {
                        if (spear[i].hidden)
                        break;
                    }
                    if (i<max_bullet && (now - last_time_temp_spear_shoot >= spear_shoot_cooldown)){
                        spear[i].hidden = false;
                        spear[i].x = character.x;
                        spear[i].y = character.y;
                        last_time_temp_spear_shoot = now;
                    }
                }
            }
            if(active_weapon == BASIC){
                int i;
                for (i=0;i<max_bullet;i++) {
                if 	(basic_bullet[i].hidden == true)
                    continue;

                if(point_in_the_region(basic_bullet[i].x, basic_bullet[i].y, character.x, character.y, character.w, character.h)){
                    basic_bullet[i].vx = calvx(mouse_y,character.y,mouse_x,character.x);
                    basic_bullet[i].vy = calvy(mouse_y,character.y,mouse_x,character.x);
                }

                basic_bullet[i].x += basic_bullet[i].vx*4;
                basic_bullet[i].y += basic_bullet[i].vy*4;

                if(point_in_the_region(basic_bullet[i].x, basic_bullet[i].y, boss.x+boss.w/2, boss.y+boss.h/2, boss.w, boss.h)){
                        basic_bullet[i].hidden=true;
                        boss.hp -=1 ;
                        if(boss.hp<=0){
                            boss.hidden = true;
                            game_change_scene(goodEnd);
                        }

                }
                if (basic_bullet[i].y<0||basic_bullet[i].y>615||basic_bullet[i].x<0||basic_bullet[i].x>1024)
                    basic_bullet[i].hidden = true;
                }

                if(basic_bullet_shoot_cooldown == 0.1){
                    cool_basic++;
                    fire_emerge = true;
                    if(cool_basic >= 120){
                        basic_bullet_shoot_cooldown = 0.5;
                        cool_basic = 0;
                        fire_emerge = false;
                    }

                }
                if (mouse_button==1) {
                    for (i=0;i<max_bullet;i++) {
                        if (basic_bullet[i].hidden)
                        break;
                    }
                    if (i<max_bullet && (now - last_time_temp_basic_bullet_shoot >= basic_bullet_shoot_cooldown)){
                        basic_bullet[i].hidden = false;
                        basic_bullet[i].x = character.x;
                        basic_bullet[i].y = character.y;
                        last_time_temp_basic_bullet_shoot = now;

                    }
                }



            }
            if(active_weapon == WATERBALL){

                int i;
                for (i=0;i<max_bullet;i++) {
                    if 	(waterball[i].hidden == true)
                        continue;

                    if(point_in_the_region(waterball[i].x, waterball[i].y, character.x, character.y, character.w, character.h)){
                        waterball[i].vx = calvx(mouse_y,character.y,mouse_x,character.x);
                        waterball[i].vy = calvy(mouse_y,character.y,mouse_x,character.x);
                    }

                    waterball[i].x += waterball[i].vx*12;
                    waterball[i].y += waterball[i].vy*12;


                    if(point_in_the_region(waterball[i].x, waterball[i].y, boss.x+boss.w/2, boss.y+boss.h/2, boss.w, boss.h)){

                        waterball[i].hidden = true;
                        waterball2[i].hidden = false;
                        waterball2[i].x = boss.x;
                        waterball2[i].y = boss.y;
                        boss.hp -=1 ;
                        if(character.hp<=20)
                            character.hp += 1;
                        if(boss.hp<=0){
                            boss.hidden = true;
                            game_change_scene(goodEnd);
                        }

                    }
                    if (waterball[i].y<0||waterball[i].y>615||waterball[i].x<0||waterball[i].x>1024)
                        waterball[i].hidden = true;
                }

                if (mouse_button==1) {
                    for (i=0;i<max_bullet;i++) {
                        if (waterball[i].hidden)
                            break;
                    }
                if (i<max_bullet && (now - last_time_temp_spear_shoot >= spear_shoot_cooldown)){
                        waterball[i].hidden = false;
                        waterball[i].x = character.x;
                        waterball[i].y = character.y;
                        last_time_temp_spear_shoot = now;

                }
                }
            }













    }
}
void game_draw(void){

    double now = al_get_time();

    if(active_scene == SCENE_first_level){

        black++;
        if(black<180){
            al_clear_to_color(al_map_rgb(0,0,0));
            al_draw_text(font32, al_map_rgb(255,255,255), 512, 200, ALLEGRO_ALIGN_CENTER, "FIRST LEVEL! enemy:20hp");
            al_draw_text(font32, al_map_rgb(255,255,255), 512, 300, ALLEGRO_ALIGN_CENTER, "YOU GET NEW WEAPON!");
            al_draw_bitmap(fire_weapon_img, 500, 400, 0);
        }
        else{

        al_draw_bitmap(first_level_background, 0, 0, 0);
        al_draw_bitmap_region(character_img, sourceX, sourceY*al_get_bitmap_height(character.img)/4, al_get_bitmap_width(character.img)/3, al_get_bitmap_height(character.img)/4,character.x,character.y,0);
        if(fire_emerge == true){
            cool_fire++;
            if(cool_fire == 10){
                al_draw_bitmap(fire[fire_number], character.x-18, character.y-30, 0);
                cool_fire = 0;
                fire_number++;
            }
            if(fire_number == 8){
                fire_number = 0;
            }
        }

        if(first_level_mon1.hidden == false)
        {
            al_draw_bitmap(first_level_mon1.img, first_level_mon1.x, first_level_mon1.y, 0);
            al_draw_rectangle(first_level_mon1.x-30,first_level_mon1.y-20,first_level_mon1.x+50,first_level_mon1.y-10,al_map_rgb(227,23,13),2);
            al_draw_filled_rectangle(first_level_mon1.x-30, first_level_mon1.y-20, first_level_mon1.x-30 + 80 * (first_level_mon1.hp) / 20,first_level_mon1.y-10,al_map_rgb(227,23,13));
        }
        /*if(first_level_mon2.hidden == false)
        {
            al_draw_bitmap(first_level_mon2.img, first_level_mon2.x, first_level_mon2.y, 0);
        }*/
        for (int i=0;i<max_bullet;i++){
            if(basic_bullet[i].hidden == false)
                al_draw_bitmap(basic_bullet[i].img, basic_bullet[i].x, basic_bullet[i].y, 0);
        }
        for (int i=0;i<max_bullet;i++){
            if(spear[i].hidden == false)
                al_draw_bitmap(spear[i].img, spear[i].x, spear[i].y, 0);
        }
        al_draw_rectangle(character.x-30,character.y-20,character.x+50,character.y-10,al_map_rgb(227,23,13),2);
        al_draw_filled_rectangle(character.x-30, character.y-20, character.x-30 + 80 * (character.hp) / 20,character.y-10,al_map_rgb(227,23,13));
        //al_draw_text(font16, al_map_rgb(255,255,255), 256, 550, 1, "Press C to change weapon");
        if(!skill_image_hidden)
            al_draw_text(font16, al_map_rgb(255,255,255), 210, 570, 1, "Press F to use skill");
        if(active_weapon == BASIC){
            al_draw_bitmap(fire_weapon_img,600,550,0);
            if(!skill_image_hidden)
                al_draw_bitmap(fire_skill_img,690,550,0);
        }
        else if(active_weapon == SPEAR){
            al_draw_bitmap(grass_weapon_img,600,550,0);
            if(!skill_image_hidden)
                al_draw_bitmap(grass_skill_img,600,550,0);
        }
        else if(active_weapon == WATERBALL){
            al_draw_bitmap(water_weapon_img,600,550,0);
            if(!skill_image_hidden)
                al_draw_bitmap(water_skill_img,600,550,0);
        }
        }

    }
    else if(active_scene == SCENE_menu){
        al_draw_bitmap(menu, 0, 0, 0);
        al_draw_text(font64, al_map_rgb(255,255,255), 512, 158, ALLEGRO_ALIGN_CENTER, "ELEMENTAL WAR");
        if(point_in_the_region(mouse_x,mouse_y,510,381,154,42))
            al_draw_text(font32, al_map_rgb(224,255,255), 512, 360, ALLEGRO_ALIGN_CENTER, "START");
        else
            al_draw_text(font32, al_map_rgb(255,255,255), 512, 360, ALLEGRO_ALIGN_CENTER, "START");
        if(point_in_the_region(mouse_x,mouse_y,510,461,154,42))
            al_draw_text(font32, al_map_rgb(224,255,255), 512, 440, ALLEGRO_ALIGN_CENTER, "HELP");
        else
            al_draw_text(font32, al_map_rgb(255,255,255), 512, 440, ALLEGRO_ALIGN_CENTER, "HELP");
        if(point_in_the_region(mouse_x,mouse_y,510,541,154,42))
            al_draw_text(font32, al_map_rgb(224,255,255), 512, 520, ALLEGRO_ALIGN_CENTER, "EXIT");
        else
            al_draw_text(font32, al_map_rgb(255,255,255), 512, 520, ALLEGRO_ALIGN_CENTER, "EXIT");


    }
    else if(active_scene == SCENE_second_level){

        black2++;
        if(black2<180){
            al_clear_to_color(al_map_rgb(0,0,0));
            al_draw_text(font32, al_map_rgb(255,255,255), 512, 200, ALLEGRO_ALIGN_CENTER, "SECOND LEVEL! enemy:40hp");
            al_draw_text(font32, al_map_rgb(255,255,255), 512, 300, ALLEGRO_ALIGN_CENTER, "YOU GET NEW WEAPON!");
            al_draw_bitmap(grass_weapon_img, 500, 400, 0);
        }
        else{

        al_draw_bitmap(second_level_background, 0, 0, 0);
        al_draw_bitmap_region(character_img, sourceX, sourceY*al_get_bitmap_height(character.img)/4, al_get_bitmap_width(character.img)/3, al_get_bitmap_height(character.img)/4,character.x,character.y,0);
        if(fire_emerge == true){
            cool_fire++;
            if(cool_fire == 10){
                al_draw_bitmap(fire[fire_number], character.x-18, character.y-30, 0);
                cool_fire = 0;
                fire_number++;
            }
            if(fire_number == 8){
                fire_number = 0;
            }
        }

        if(second_level_mon1.hidden == false)
        {
            al_draw_bitmap(second_level_mon1.img, second_level_mon1.x, second_level_mon1.y, 0);
            al_draw_rectangle(second_level_mon1.x+20,second_level_mon1.y-20,second_level_mon1.x+100,second_level_mon1.y-10,al_map_rgb(227,23,13),2);
            al_draw_filled_rectangle(second_level_mon1.x+20, second_level_mon1.y-20, second_level_mon1.x+20 + 80 * (second_level_mon1.hp) / 40,second_level_mon1.y-10,al_map_rgb(227,23,13));
            if(strap)
                al_draw_bitmap(vine_img, second_level_mon1.x+40, second_level_mon1.y, 0);

        }
        /*for (int i=0;i<max_bullet;i++){
            if(basic_bullet[i].hidden == false)
                al_draw_bitmap(basic_bullet[i].img, basic_bullet[i].x, basic_bullet[i].y, 0);
        }*/
        if(active_weapon == SPEAR){
            for (int i=0;i<max_bullet;i++){
                if(spear[i].hidden == false)
                    al_draw_bitmap(spear[i].img, spear[i].x, spear[i].y, 0);
            }
        }
        else if(active_weapon == BASIC){
            for (int i=0;i<max_bullet;i++){
                if(basic_bullet[i].hidden == false)
                    al_draw_bitmap(basic_bullet[i].img, basic_bullet[i].x, basic_bullet[i].y, 0);
            }
        }
        al_draw_rectangle(character.x-30,character.y-20,character.x+50,character.y-10,al_map_rgb(227,23,13),2);
        al_draw_filled_rectangle(character.x-30, character.y-20, character.x-30 + 80 * (character.hp) / 20,character.y-10,al_map_rgb(227,23,13));
        al_draw_text(font16, al_map_rgb(255,255,255), 256, 550, 1, "Press C to change weapon");
        if(!skill_image_hidden)
            al_draw_text(font16, al_map_rgb(255,255,255), 210, 570, 1, "Press F to use skill");
        if(active_weapon == BASIC){
            al_draw_bitmap(fire_weapon_img,600,550,0);
            if(!skill_image_hidden)
                al_draw_bitmap(fire_skill_img,690,550,0);
        }
        else if(active_weapon == SPEAR){
            al_draw_bitmap(grass_weapon_img,600,550,0);
            if(!skill_image_hidden)
                al_draw_bitmap(grass_skill_img,690,550,0);
        }
        else if(active_weapon == WATERBALL){
            al_draw_bitmap(water_weapon_img,600,550,0);
            if(!skill_image_hidden)
                al_draw_bitmap(water_skill_img,690,550,0);
        }



        }
        }

    else if(active_scene == SCENE_boss_level){

        black3++;
        if(black3<180){
            al_clear_to_color(al_map_rgb(0,0,0));
            al_draw_text(font32, al_map_rgb(255,255,255), 512, 200, ALLEGRO_ALIGN_CENTER, "BOSS LEVEL! enemy:50hp");
            al_draw_text(font32, al_map_rgb(255,255,255), 512, 300, ALLEGRO_ALIGN_CENTER, "YOU GET NEW WEAPON!");
            al_draw_bitmap(water_weapon_img, 500, 400, 0);
        }
        else{

        al_draw_bitmap(boss_level_background, 0, 0, 0);
        al_draw_bitmap_region(character_img, sourceX, sourceY*al_get_bitmap_height(character.img)/4, al_get_bitmap_width(character.img)/3, al_get_bitmap_height(character.img)/4,character.x,character.y,0);
        if(fire_emerge == true){
            cool_fire++;
            if(cool_fire == 10){
                al_draw_bitmap(fire[fire_number], character.x-18, character.y-30, 0);
                cool_fire = 0;
                fire_number++;
            }
            if(fire_number == 8){
                fire_number = 0;
            }
        }
        if(water_emerge == true){
            cool_water++;
            if(cool_water == 10){
                al_draw_bitmap(water[water_number], character.x-200, character.y-200, 0);
                cool_water = 0;
                water_number++;
            }
            if(water_number == 6){
                water_number = 0;
                water_emerge = false;
            }
        }

        if(boss.hidden == false)
        {
            al_draw_bitmap(boss.img, boss.x, boss.y, 0);
            al_draw_rectangle(boss.x+20,boss.y-20,boss.x+100,boss.y-10,al_map_rgb(227,23,13),2);
            al_draw_filled_rectangle(boss.x+20, boss.y-20, boss.x+20 + 80 * (boss.hp) / 50,boss.y-10,al_map_rgb(227,23,13));
            if(strap)
                al_draw_bitmap(vine_img, boss.x+40, boss.y, 0);
        }

        if(active_weapon == SPEAR){
            for (int i=0;i<max_bullet;i++){
                if(spear[i].hidden == false)
                    al_draw_bitmap(spear[i].img, spear[i].x, spear[i].y, 0);
            }
        }
        else if(active_weapon == BASIC){
            for (int i=0;i<max_bullet;i++){
                if(basic_bullet[i].hidden == false)
                    al_draw_bitmap(basic_bullet[i].img, basic_bullet[i].x, basic_bullet[i].y, 0);
            }
        }
        else if(active_weapon == WATERBALL){
            for (int i=0;i<max_bullet;i++){
                if(waterball[i].hidden == false)
                    al_draw_bitmap(waterball[i].img, waterball[i].x,  waterball[i].y, 0);
                if(now - last_time_temp_waterball_bomb >= water_ball_cooldown){
                    waterball2[i].hidden = true;
                    last_time_temp_waterball_bomb = now;
                }
                if(waterball2[i].hidden == false){
                    al_draw_bitmap(waterball2[i].img, waterball2[i].x,  waterball2[i].y, 0);

                }

            }
        }

        for (int i=0;i<6;i++){
            if(boss2[i].hidden == false){
                al_draw_bitmap(boss2[i].img, boss2[i].x, boss2[i].y, 0);
                if(strap)
                    al_draw_bitmap(vine_img, boss2[i].x+40, boss2[i].y, 0);

            }

        }
        al_draw_rectangle(character.x-30,character.y-20,character.x+50,character.y-10,al_map_rgb(227,23,13),2);
        al_draw_filled_rectangle(character.x-30, character.y-20, character.x-30 + 80 * (character.hp) / 20,character.y-10,al_map_rgb(227,23,13));
        al_draw_text(font16, al_map_rgb(255,255,255), 256, 550, 1, "Press C to change weapon");
        if(!skill_image_hidden)
            al_draw_text(font16, al_map_rgb(255,255,255), 210, 570, 1, "Press F to use skill");
        if(active_weapon == BASIC){
            al_draw_bitmap(fire_weapon_img,600,550,0);
            if(!skill_image_hidden)
                al_draw_bitmap(fire_skill_img,690,550,0);
        }
        else if(active_weapon == SPEAR){
            al_draw_bitmap(grass_weapon_img,600,550,0);
            if(!skill_image_hidden)
                al_draw_bitmap(grass_skill_img,690,550,0);
        }
        else if(active_weapon == WATERBALL){
            al_draw_bitmap(water_weapon_img,600,550,0);
            if(!skill_image_hidden)
                al_draw_bitmap(water_skill_img,690,550,0);
        }
        }
    }
    else if(active_scene == goodEnd || active_scene == badEnd){
        if(!lux_number){
            al_clear_to_color(al_map_rgb(0,0,0));
        }
        if(active_scene == goodEnd)
            al_draw_text(font64, al_map_rgb(255,255,255), 512, 475, 1, "YOU WIN!");
        else
            al_draw_text(font64, al_map_rgb(255,255,255), 512, 475, 1, "YOU LOSE!");

        cool_lux++;
        if(cool_lux == 5){
            al_draw_bitmap(lux[lux_number],280,150,ALLEGRO_ALIGN_CENTER);
            cool_lux = 0;
            if(lux_number<77)
                lux_number++;
        }
        if(lux_number == 77){
            lux_number = 0;
        }
        al_draw_text(font16, al_map_rgb(255,255,255), 900, 550, 1, "BACK TO MENU");

    }
    else if(active_scene == SCENE_help1){
        al_draw_bitmap(help_background,0,0,ALLEGRO_ALIGN_CENTER);
        al_draw_bitmap(help_book1,100,0,ALLEGRO_ALIGN_CENTER);
        if(point_in_the_region(mouse_x,mouse_y,931,572,140,50))
            al_draw_text(font32, al_map_rgb(224,255,255), 930, 550, ALLEGRO_ALIGN_CENTER, "NEXT!");
        else
            al_draw_text(font32, al_map_rgb(255,255,255), 930, 550, 1, "NEXT!");

    }
    else if(active_scene == SCENE_help2){
        al_draw_bitmap(help_background,0,0,ALLEGRO_ALIGN_CENTER);
        al_draw_bitmap(help_book2,100,0,ALLEGRO_ALIGN_CENTER);
        if(point_in_the_region(mouse_x,mouse_y,931,572,140,50))
            al_draw_text(font32, al_map_rgb(224,255,255), 930, 550, ALLEGRO_ALIGN_CENTER, "NEXT!");
        else
            al_draw_text(font32, al_map_rgb(255,255,255), 930, 550, 1, "NEXT!");

    }
    else if(active_scene == SCENE_help3){
        al_draw_bitmap(help_background,0,0,ALLEGRO_ALIGN_CENTER);
        al_draw_bitmap(help_book3,100,0,ALLEGRO_ALIGN_CENTER);
        if(point_in_the_region(mouse_x,mouse_y,931,572,140,50))
            al_draw_text(font32, al_map_rgb(224,255,255), 930, 550, ALLEGRO_ALIGN_CENTER, "NEXT!");
        else
            al_draw_text(font32, al_map_rgb(255,255,255), 930, 550, 1, "NEXT!");

    }

    al_flip_display();

}

void game_change_scene(int next_scene) {


    if(active_scene == SCENE_help3 && next_scene == SCENE_menu){}
    else if(active_scene == SCENE_menu && next_scene== SCENE_first_level){
        al_stop_sample(&silver_id);
        silver_play = false;
    }
    else if(active_scene == goodEnd || active_scene == badEnd){
        al_stop_sample(&warriors_id);
    }



    game_log("Change scene from %d to %d", active_scene, next_scene);

    active_scene = next_scene;

    if (active_scene == SCENE_first_level){

        al_play_sample(warriors, 1, 1.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &warriors_id);

        change_weapon();
        active_weapon = BASIC;
        lux_number = 0;

        character.x = SCREEN_W/4;
        character.y = SCREEN_H/2;
        character.img = character_img;
        character.w = al_get_bitmap_width(character_img)/3;
        character.h = al_get_bitmap_height(character_img)/4;
        character.hp = 20;


        first_level_mon1.x = 775;
        first_level_mon1.y = 158;
        first_level_mon1.img = first_level_mon1_img;
        first_level_mon1.hidden = false;
        first_level_mon1.w = al_get_bitmap_width(first_level_mon1_img);
        first_level_mon1.h = al_get_bitmap_height(first_level_mon1_img);
        first_level_mon1.hp = 20;


        first_level_mon2.x = 660;
        first_level_mon2.y = 315;
        first_level_mon2.img = first_level_mon2_img;
        first_level_mon2.vx = 2;
        first_level_mon2.vy = 2;
        first_level_mon2.hidden = false;
        first_level_mon2.w = al_get_bitmap_width(first_level_mon2_img);
        first_level_mon2.h = al_get_bitmap_height(first_level_mon2_img);


        first_level_mon3.x = 775;
        first_level_mon3.y = 473;
        first_level_mon3.img = first_level_mon3_img;
        first_level_mon3.hidden = false;
        first_level_mon3.w = al_get_bitmap_width(first_level_mon3_img);
        first_level_mon3.h = al_get_bitmap_height(first_level_mon3_img);

        for(int i=0;i<max_bullet;i++){
            basic_bullet[i].x = character.x;
            basic_bullet[i].y = character.y;
            basic_bullet[i].hidden = true;
            basic_bullet[i].img = bullet_img;
        }
        for(int i=0;i<max_bullet;i++){
            spear[i].x = first_level_mon1.x;
            spear[i].y = first_level_mon1.y;
            spear[i].hidden = true;
            spear[i].img = spear_img;
        }



    }
    else if(active_scene == SCENE_menu){
        if(!silver_play){
            al_play_sample(silver, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &silver_id);
            silver_play = true;
        }
        black = 0;
        black2 = 0;
        black3 = 0;

    }
    else if(active_scene == SCENE_second_level){

        active_weapon = BASIC;

        character.x = SCREEN_W/4;
        character.y = SCREEN_H/2;

        second_level_mon1.x = 775;
        second_level_mon1.y = 308;
        second_level_mon1.img = second_level_mon1_img;
        second_level_mon1.hidden = false;
        second_level_mon1.w = al_get_bitmap_width(second_level_mon1.img);
        second_level_mon1.h = al_get_bitmap_height(second_level_mon1.img);
        second_level_mon1.hp = 40;

        for(int i=0;i<max_bullet;i++){
            spear[i].x = character.x;
            spear[i].y = character.y;
            spear[i].hidden = true;
            spear[i].img = spear_img;
        }
    }
    else if(active_scene == SCENE_boss_level){

        character.x = SCREEN_W/4;
        character.y = SCREEN_H/2;

        boss.x = 308;
        boss.y = 768;
        boss.vx = 3;
        boss.vy = 2;
        boss.img = boss_img;
        boss.hidden = false;
        boss.w = al_get_bitmap_width(boss.img);
        boss.h = al_get_bitmap_height(boss.img);
        boss.hp = 50;

        for(int i=0;i<6;i++){
        boss2[i].x = 1024;
        boss2[i].y = rand() % SCREEN_H;
        boss2[i].img = boss2_img;
        boss2[i].hidden = true;
        boss2[i].w = al_get_bitmap_width(boss2[i].img);
        boss2[i].h = al_get_bitmap_height(boss2[i].img);
        boss2[i].vx = -5;
        boss2[i].vy = 0;
        }
        for(int i=0;i<max_bullet;i++){
            waterball[i].x = character.x;
            waterball[i].y = character.y;
            waterball[i].hidden = true;
            waterball[i].img = waterball_img;

            waterball2[i].hidden = true;
            waterball2[i].img = waterball2_img;
        }



    }
}
bool point_in_the_region(int px, int py, int x, int y, int w, int h){
    if((px<=(x+w/2))&&(px>=(x-w/2))&&(py<=(y+h/2))&&(py>=(y-h/2)))
        return true;
    else
        return false;
}
void on_mouse_down(int btn, int x, int y){
    if(active_scene == SCENE_menu){
        if(btn == true){
            if(point_in_the_region(x,y,510,381,154,20))
                game_change_scene(SCENE_first_level);
            else if(point_in_the_region(x,y,510,541,154,20))
                exited = true;
            else if(point_in_the_region(x,y,510,461,154,20))
                game_change_scene(SCENE_help1);

        }


    }
    else if(active_scene == goodEnd || active_scene == badEnd){
        if(btn == true){
            if(point_in_the_region(x,y,914,560,200,10))
                game_change_scene(SCENE_menu);
        }
    }
    else if(active_scene == SCENE_help1){
        if(btn == true){
            if(point_in_the_region(x,y,931,572,140,50))
                game_change_scene(SCENE_help2);
        }
    }
    else if(active_scene == SCENE_help2){
        if(btn == true){
            if(point_in_the_region(x,y,931,572,140,50))
                game_change_scene(SCENE_help3);
        }
    }
    else if(active_scene == SCENE_help3){
        if(btn == true){
            if(point_in_the_region(x,y,931,572,140,50))
                game_change_scene(SCENE_menu);
        }

    }
}
float calvx(float y2,float y1,float x2,float x1){
	float dx = x2-x1;
	float dy = y2-y1;
	return dx/sqrt(dx*dx+dy*dy);
}
float calvy(float y2,float y1,float x2,float x1){
	float dx = x2-x1;
	float dy = y2-y1;
	return dy/sqrt(dx*dx+dy*dy);
}
void change_weapon(void){
    for(int i=0;i<max_bullet;i++){
        spear[i].x = character.x;
        spear[i].y = character.y;
        spear[i].hidden = true;
        basic_bullet[i].x = character.x;
        basic_bullet[i].y = character.y;
        basic_bullet[i].hidden = true;
        waterball[i].x = character.x;
        waterball[i].y = character.y;
        waterball[i].hidden = true;
        waterball2[i].hidden = true;
    }


}

void game_destroy() {
    // Make sure you destroy all things
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
    al_destroy_timer(game_update_timer);

    al_destroy_font(font16);
    al_destroy_font(font32);
    al_destroy_font(font64);

    al_destroy_bitmap(menu);
    al_destroy_bitmap(help_background);
    al_destroy_bitmap(help_book1);
    al_destroy_bitmap(help_book2);
    al_destroy_bitmap(help_book3);
    al_destroy_bitmap(first_level_background);
    al_destroy_bitmap(second_level_background);
    al_destroy_bitmap(fireplace);
    al_destroy_bitmap(magicpot);
    al_destroy_bitmap(boss_level_background);
    al_destroy_bitmap(win_img);
    al_destroy_bitmap(bullet_img);
    al_destroy_bitmap(spear_img);
    al_destroy_bitmap(character_img);
    al_destroy_bitmap(first_level_mon1_img);
    al_destroy_bitmap(first_level_mon2_img);
    al_destroy_bitmap(first_level_mon3_img);
    al_destroy_bitmap(second_level_mon1_img);
    al_destroy_bitmap(boss2_img);
    al_destroy_bitmap(boss_img);
    al_destroy_bitmap(boss_rotate_img);

    al_destroy_bitmap(water_weapon_img);
    al_destroy_bitmap(grass_weapon_img);
    al_destroy_bitmap(fire_weapon_img);
    al_destroy_bitmap(water_skill_img);
    al_destroy_bitmap(grass_skill_img);
    al_destroy_bitmap(fire_skill_img);
    al_destroy_bitmap(vine_img);


    al_destroy_bitmap(C_key_white_img);
    al_destroy_bitmap(F_key_white_img);
    al_destroy_bitmap(F_key_black_img);
    for(int i=0;i<78;i++)
        al_destroy_bitmap(lux[i]);
    for(int i=0;i<8;i++)
        al_destroy_bitmap(fire[i]);
    for(int i=0;i<7;i++)
        al_destroy_bitmap(water[i]);

    al_destroy_bitmap(waterball_img);
    al_destroy_bitmap(waterball2_img);
    al_destroy_bitmap(win_background);
    al_destroy_bitmap(lose_background);

    al_destroy_sample(silver);
    al_destroy_sample(warriors);

}
// +=================================================================+
// | Code below is for debugging purpose, it's fine to remove it.    |
// | Deleting the code below and removing all calls to the functions |
// | doesn't affect the game.                                        |
// +=================================================================+

void game_abort(const char* format, ...) {
	va_list arg;
	va_start(arg, format);
	game_vlog(format, arg);
	va_end(arg);
	fprintf(stderr, "error occured, exiting after 2 secs");
	// Wait 2 secs before exiting.
	al_rest(2);
	// Force exit program.
	exit(1);
}


void game_log(const char* format, ...) {
#ifdef LOG_ENABLED
	va_list arg;
	va_start(arg, format);
	game_vlog(format, arg);
	va_end(arg);
#endif
}

void game_vlog(const char* format, va_list arg) {
#ifdef LOG_ENABLED
	static bool clear_file = true;
	vprintf(format, arg);
	printf("\n");
	// Write log to file for later debugging.
	FILE* pFile = fopen("log.txt", clear_file ? "w" : "a");
	if (pFile) {
		vfprintf(pFile, format, arg);
		fprintf(pFile, "\n");
		fclose(pFile);
	}
	clear_file = false;
#endif
}

