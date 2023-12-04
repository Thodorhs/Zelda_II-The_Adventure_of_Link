#include <filesystem>
#include <cmath>
#include "../../../Engine/Include/ZeldaApp.h"
//#include "../../../Engine/Include/ViewWindow.h"
//#include "../../../Engine/Include/Util/ConfiguratorManager.h"
//#include "../../../Engine/Include/Util/ConfigFuncs.h"
#include "../../../Engine/Include/Grid/Grid.h"
#include "../../../Engine/Include/TileLayer.h"
#include "../../../Engine/Include/Util/Print.h"

SDL_Rect moving_rect = { 0,48,16,16 };
Render* global_render_vars;
Engine_Consts_t Engine_Consts;
bool is_running; //used by done()
bool mouse_down = false; //bool to check if i hold down the the left click
bool display_grid = false;
KEY_MAP_t pressed_keys;
KEY_MAP_t released_keys;
std::unique_ptr<_Grid_> grid_class;

Dim get_2_power(Dim w, Dim h) {
	assert(w == h && (~(w & (w - 1)))); // assert that dims are powers of two
	return (Dim)log2(w);
}

void disable_pr() {
	KEY_MAP_t::iterator pr_it;
	for (pr_it = pressed_keys.begin(); pr_it != pressed_keys.end(); pr_it++) {
		pr_it->second = false;
	}
}

void update_keys() {
	KEY_MAP_t::iterator pr_it;
	for (auto& it : released_keys) {
		if (it.second) {
			pr_it = pressed_keys.find(it.first);
			pr_it->second = false;
			auto val = released_keys.find(it.first);
			val->second = false;
		}
	}
}

void update_released(Sint32 code, bool state) {
	KEY_MAP_t::iterator it = released_keys.find(code);
	if (it != released_keys.end())
		it->second = state;
}

void update_press(Sint32 code, bool state) {
	KEY_MAP_t::iterator it = pressed_keys.find(code);
	if (it != pressed_keys.end()) {
		it->second = state;
	}
}

void move_rect() {
	int dx = 1;
	FilterGridMotionRight(&grid_class->get_s_grid(), moving_rect, &dx);
	moving_rect.x += dx;
}

void move_tiles_x(int tiles) {
	int scroll_dist = MUL_TILE_WIDTH(tiles, Engine_Consts.power);
	if (CanScrollHoriz((global_render_vars->ViewWindowR), scroll_dist)) 
		Scroll(&(global_render_vars->ViewWindowR), scroll_dist, 0);
}

void move_tiles_y(int tiles) {
	int scroll_dist = MUL_TILE_WIDTH(tiles, Engine_Consts.power);
	if (CanScrollVert((global_render_vars->ViewWindowR), scroll_dist))
		Scroll(&(global_render_vars->ViewWindowR), 0, scroll_dist);
}

void move_pixels_x(int pixels) {
	move_rect();
}

void move_pixels_y(int pixels) {
	if (CanScrollVert((global_render_vars->ViewWindowR), pixels))
		Scroll(&(global_render_vars->ViewWindowR), 0, pixels);
}

void move() {
	for (auto it : pressed_keys) {
		if (it.second) {
			switch (it.first) {
			case SDL_KeyCode::SDLK_UP:
				move_pixels_y(-1);
				break;
			case SDL_KeyCode::SDLK_DOWN:
				move_pixels_y(1);
				break;
			case SDL_KeyCode::SDLK_LEFT:
				move_pixels_x(-1);
				break;
			case SDL_KeyCode::SDLK_RIGHT:
				move_pixels_x(1);
				break;
			case SDLK_HOME:
				global_render_vars->ViewWindowR.x = 0;
				global_render_vars->ViewWindowR.y = 0;
				break;
			case SDLK_END:
				global_render_vars->ViewWindowR.x = MUL_TILE_WIDTH(GetMapData()->at(0).size(), Engine_Consts.power) - global_render_vars->ViewWindowR.w;
				global_render_vars->ViewWindowR.y = MUL_TILE_HEIGHT(GetMapData()->size(), Engine_Consts.power) - global_render_vars->ViewWindowR.h;
				break;
			default:
				break;
			}
		}
	}
}

void myInput() {
	int CameraPosX, CameraPosY;
	int PrevCameraPosX = 0, PrevCameraPosY = 0;
	SDL_Event event;
	const Uint8* keys = SDL_GetKeyboardState((int*)nullptr);
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_MOUSEMOTION:
			if (mouse_down) { // if i am holding down the left click button and i am moving it then scroll..
				int offsetX = 0, offsetY = 0;
				SDL_GetMouseState(&CameraPosX, &CameraPosY);

				if (CameraPosX - PrevCameraPosX > 0) offsetX = 1;  
				else if (CameraPosX - PrevCameraPosX < 0) offsetX = -1;

				move_pixels_x(offsetX);

				if (CameraPosY - PrevCameraPosY > 0) offsetY = 1;
				else if (CameraPosY - PrevCameraPosY < 0) offsetY = -1;

				move_pixels_y(offsetY);
				//ScrollWithBoundsCheck(&render_vars->ViewWindowR, offsetX, offsetY);
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT) {
				mouse_down = true; // i'am holding it down so set it true
				SDL_GetMouseState(&PrevCameraPosX, &PrevCameraPosY);
			}
			break;
		case SDL_MOUSEBUTTONUP:
			if (event.button.button == SDL_BUTTON_LEFT) {
				//PrevCameraPosY = 0;
				//PrevCameraPosX = 0;
				mouse_down = false; // i realesed it
			}
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_g)
				display_grid = !display_grid;
			update_press(event.key.keysym.sym, true);
			break;
		case SDL_QUIT:
			is_running = false;
			break;

		case SDL_KEYUP:
			update_released(event.key.keysym.sym, true);
			break;
		default:
			break;
		}
	}
	move();
	disable_pr();
	//update_keys();
}

void show_grid() {
	DisplayGrid(global_render_vars->ViewWindowR, global_render_vars->myrenderer, grid_class, global_render_vars->view_scale);
}

void myRender() {
	SDL_RenderClear(global_render_vars->myrenderer);
	SDL_SetRenderDrawColor(global_render_vars->myrenderer, 200, 0, 200, 255);
	SDL_RenderFillRect(global_render_vars->myrenderer, &moving_rect);
	if(display_grid) show_grid();
	SDL_RenderPresent(global_render_vars->myrenderer);
}

bool myDone() {
	return is_running;
}

void init_key_map() {
	pr_info("Initializing keyboard..");
	/*PRESS*/
	pressed_keys.insert(std::make_pair(SDL_KeyCode::SDLK_UP, false));
	pressed_keys.insert(std::make_pair(SDL_KeyCode::SDLK_DOWN, false));
	pressed_keys.insert(std::make_pair(SDL_KeyCode::SDLK_LEFT, false));
	pressed_keys.insert(std::make_pair(SDL_KeyCode::SDLK_RIGHT, false));
	pressed_keys.insert(std::make_pair(SDL_KeyCode::SDLK_HOME, false));
	pressed_keys.insert(std::make_pair(SDL_KeyCode::SDLK_END, false));
	/*RELEASE*/
	released_keys.insert(std::make_pair(SDL_KeyCode::SDLK_UP, false));
	released_keys.insert(std::make_pair(SDL_KeyCode::SDLK_DOWN, false));
}

void init_engine_constants() {
	pr_info("Initializing engine constants..");

	configurators_t map = configurators_t::MAP_CONFIG;

	int px_h = get_config_value<int>(map, "pixel_height");
	int px_w = get_config_value<int>(map, "pixel_width");

	Dim tile_h =(Dim) get_config_value<int>(map, "tile_height");
	Dim tile_w =(Dim) get_config_value<int>(map, "tile_width");

	Dim grid_el_h = (Dim)get_config_value<int>(map, "Grid_el_w");
	Dim grid_el_w = (Dim)get_config_value<int>(map, "Grid_el_h");

	Dim power_tiles = get_2_power(tile_h, tile_w);
	Dim power_grid = get_2_power(grid_el_h, grid_el_w);


	Engine_Consts.Map_height = px_h;
	Engine_Consts.Map_width = px_w;
	Engine_Consts.Tile_height = tile_h;
	Engine_Consts.Tile_width = tile_w;
	Engine_Consts.power = power_tiles;
	Engine_Consts.grid_power = power_grid;
	Engine_Consts.Grid_el_sz = grid_el_h;
	Engine_Consts.map_rows = get_config_value<int>(map, "map_rows");
	Engine_Consts.map_cols = get_config_value<int>(map, "map_columns");
	
	grid_class = std::make_unique<_Grid_>();
	
	
}

void fill_grid() {
	pr_info("Initializing grid..");
	ComputeTileGridBlocks(GetMapData(), grid_class); //grid_old supports action layer 
}

void ZeldaApp::Initialise(void) {
	pr_start_msg();
	pr_info("Initializing configurators..");
	init_configurators();
	configurators_t map = configurators_t::MAP_CONFIG;
	configurators_t render = configurators_t::RENDER_CONFIG;
	const int view_w = get_config_value<int>(render, "view_win_w");
	const int view_h = get_config_value<int>(render, "view_win_h");
	int start_x =get_config_value<int>(render, "start_x");
	int start_y = get_config_value<int>(render, "start_y");
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		int scale = get_config_value<int>(render, "view_scale_global");

		pr_info("Subsystems Initialised!...");
		global_render_vars = new Render(start_x, start_y, view_w, view_h, scale);

		int win_w = get_config_value<int>(render, "render_w_w");
		int win_h = get_config_value<int>(render, "render_w_h");
		init_engine_constants();

		global_render_vars->Gwindow = SDL_CreateWindow("ZeldaEngine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_w, win_h, 0);
		if (global_render_vars->Gwindow) std::cout << "Window created!" << std::endl;
		
		global_render_vars->myrenderer = SDL_CreateRenderer(global_render_vars->Gwindow, -1, 0);
		if (global_render_vars->myrenderer)
		{
			SDL_SetRenderDrawColor(global_render_vars->myrenderer, 0, 0, 0, 0);
			pr_info("Renderer created!");
		}
	}

	init_key_map();
	pre_cache();

	std::filesystem::path cwd = std::filesystem::current_path();
	std::string find_first_part_path = cwd.string();
	size_t pos = find_first_part_path.find("out");
	std::string half_path = find_first_part_path.substr(0, pos);
	std::string const full_asset_path = half_path + "UnitTests\\UnitTest2\\UnitTest2Media";
	ReadTextMap(full_asset_path + "\\" + get_config_value<std::string>(configurators_t::MAP_CONFIG, "text_map"));
	global_render_vars->ImgSurface = IMG_Load((full_asset_path + "\\" + get_config_value<std::string>(configurators_t::MAP_CONFIG, "tileset")).c_str());
	fill_grid();
	
	global_render_vars->Tileset = SDL_CreateTextureFromSurface(global_render_vars->myrenderer, global_render_vars->ImgSurface);
	
	int w;
	int h;
	SDL_QueryTexture(global_render_vars->Tileset,
		NULL, NULL,
		&w, &h);
	//std::cout << " w=" << w << " h=" << h << std::endl;

	game.SetInput(myInput);
	game.SetRender(myRender);
	game.SetDone(myDone);
	is_running = true;
	pr_info("Done!");
}

void ZeldaApp::Load() {

}

void ZeldaApp::Run() {
	game.MainLoop();
}

void ZeldaApp::RunIteration() {
	game.MainLoopIteration();
}

void ZeldaApp::Clear() {

}