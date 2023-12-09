#include "../../../Engine/Include/ZeldaApp.h"
#include "../../../Engine/Include/ViewWindow.h"
#include "../../../Engine/Include/Util/ConfigFuncs.h"
#include <filesystem>
#include <cmath>

Render* global_render_vars;
Engine_Consts_t Engine_Consts;
bool is_running; //used by done()
bool mouse_down=false; //bool to check if i hold down the the left click
KEY_MAP_t pressed_keys;
KEY_MAP_t released_keys;

void disable_pr() {
	KEY_MAP_t::iterator pr_it;
	for (pr_it = pressed_keys.begin(); pr_it != pressed_keys.end(); pr_it++) {
		pr_it->second = false;
	}
}

void update_keys() {
	KEY_MAP_t::iterator pr_it;
	for (auto it : released_keys) {
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

void update_press(Sint32 code,bool state) {
	KEY_MAP_t::iterator it = pressed_keys.find(code);
	if (it != pressed_keys.end()) {
		it->second = state;
	}
}

void move_pixels_x(int pixels) {
	if (CanScrollHoriz((global_render_vars->ViewWindowR), pixels))
		Scroll(&(global_render_vars->ViewWindowR), pixels, 0);
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
				global_render_vars->ViewWindowR.x = MUL_TILE_WIDTH(GetMapData()->at(0).size(),Engine_Consts.power) - global_render_vars->ViewWindowR.w;
				global_render_vars->ViewWindowR.y = MUL_TILE_HEIGHT(GetMapData()->size(),Engine_Consts.power) - global_render_vars->ViewWindowR.h;
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
	const Uint8* keys = SDL_GetKeyboardState((int*)0);
	while (SDL_PollEvent(&event)) { 
		switch (event.type) {	
		case SDL_MOUSEMOTION:
			if (mouse_down) { // if i am holding down the left click button and i am moving it then scroll..
				int offsetX = 0, offsetY = 0;
				SDL_GetMouseState(&CameraPosX, &CameraPosY);
				if (CameraPosX - PrevCameraPosX > 0) offsetX = 1;
				else if (CameraPosX - PrevCameraPosX < 0) offsetX = -1;

				if (CameraPosY - PrevCameraPosY > 0) offsetY = 1;
				else if (CameraPosY - PrevCameraPosY < 0) offsetY = -1;

				ScrollWithBoundsCheck(&global_render_vars->ViewWindowR, offsetX, offsetY);
				PrevCameraPosX, PrevCameraPosY = CameraPosX, CameraPosY;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT) {
				mouse_down = true; // i'am holding it down so set it true
			}
			break;
		case SDL_MOUSEBUTTONUP:
			if (event.button.button == SDL_BUTTON_LEFT){
				mouse_down = false; // i realesed it
			}
			break;
		case SDL_KEYDOWN:
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

void myRender() {
	SDL_RenderClear(global_render_vars->myrenderer);
	TileTerrainDisplay(GetMapData(), global_render_vars->ViewWindowR, { 0, 0,-1,0 }, global_render_vars->myrenderer, global_render_vars->Tileset, global_render_vars->RenderTextureTarget);
	SDL_RenderPresent(global_render_vars->myrenderer);
}

bool myDone() {
	return is_running;
}

void init_key_map() {
	pressed_keys.insert(std::make_pair(SDL_KeyCode::SDLK_UP, false));
	pressed_keys.insert(std::make_pair(SDL_KeyCode::SDLK_DOWN, false));
	pressed_keys.insert(std::make_pair(SDL_KeyCode::SDLK_LEFT, false));
	pressed_keys.insert(std::make_pair(SDL_KeyCode::SDLK_RIGHT, false));
	pressed_keys.insert(std::make_pair(SDL_KeyCode::SDLK_HOME, false));
	pressed_keys.insert(std::make_pair(SDL_KeyCode::SDLK_END, false));
	released_keys.insert(std::make_pair(SDL_KeyCode::SDLK_UP, false));
	released_keys.insert(std::make_pair(SDL_KeyCode::SDLK_DOWN, false));
}

Dim get_2_power(Dim w,Dim h){
	assert(w == h && (~(w & (w-1)))); // assert that dims are powers of two
	return (Dim)log2(w);
}

void init_engine_constants() {
	configurators_t map = configurators_t::MAP_CONFIG;

	int px_h = get_config_value<int>(map, "pixel_height");
	int px_w = get_config_value<int>(map, "pixel_width");
	Dim tile_h = get_config_value<int>(map, "tile_height");
	Dim tile_w = get_config_value<int>(map, "tile_width");
	Dim grid_el_h = get_config_value<int>(map, "Grid_el_w");
	Dim grid_el_w = get_config_value<int>(map, "Grid_el_h");
	Dim power_tiles = get_2_power(tile_h, tile_w);
	Dim power_grid = get_2_power(grid_el_h, grid_el_w);

	Engine_Consts.Map_height = px_h;
	Engine_Consts.Map_width = px_w;
	Engine_Consts.Tile_height = tile_h;
	Engine_Consts.Tile_width = tile_w;
	Engine_Consts.power = power_tiles;
	Engine_Consts.grid_power = power_grid;
}

auto asset_path() {
	std::filesystem::path cwd = std::filesystem::current_path();
	std::string find_first_part_path = cwd.string();
	size_t pos = find_first_part_path.find("out");
	std::string half_path = find_first_part_path.substr(0, pos);
	std::string full_asset_path = half_path + "UnitTests\\UnitTest1";
	return full_asset_path;
}

void ZeldaApp::Initialise(void) {
	assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);
	std::cout << "Subsystems Initialised!..." << std::endl;

	configurators_t map = configurators_t::MAP_CONFIG;
	configurators_t render = configurators_t::RENDER_CONFIG;
	std::string full_asset_path = asset_path();
	init_configurators(full_asset_path);

	int scale = get_config_value<int>(render, "view_scale_global");
	int view_w = get_config_value<int>(render, "view_win_w");
	int view_h = get_config_value<int>(render, "view_win_h");
	int win_w = get_config_value<int>(render, "render_w_w");
	int win_h = get_config_value<int>(render, "render_w_h");

	global_render_vars = new Render(0,0,view_w, view_h,scale);

	init_engine_constants();

	global_render_vars->Gwindow = SDL_CreateWindow("ZeldaEngine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_w, win_h, 0);
	if (global_render_vars->Gwindow) std::cout << "Window created!" << std::endl;

	global_render_vars->myrenderer = SDL_CreateRenderer(global_render_vars->Gwindow, -1, 0);
	if (global_render_vars->myrenderer) {
		SDL_SetRenderDrawColor(global_render_vars->myrenderer, 0, 0, 0, 0);
		std::cout << "Renderer created!" << std::endl;
	}
	
	init_key_map();
	pre_cache();

	ReadTextMap(full_asset_path + "\\UnitTest1Media\\" + get_config_value<std::string>(configurators_t::MAP_CONFIG,"text_map"));
	global_render_vars -> ImgSurface = IMG_Load((full_asset_path + "\\UnitTest1Media\\" + get_config_value<std::string>(configurators_t::MAP_CONFIG, "tileset")).c_str());

	//std::cout << " w="<< global_render_vars->ImgSurface->w << " h=" << global_render_vars->ImgSurface->h << std::endl;
	global_render_vars ->Tileset = SDL_CreateTextureFromSurface(global_render_vars->myrenderer, global_render_vars->ImgSurface);

	global_render_vars->RenderTextureTarget = SDL_CreateTexture(global_render_vars->myrenderer, 0, SDL_TEXTUREACCESS_TARGET, global_render_vars->ViewWindowR.w, global_render_vars->ViewWindowR.h);
	//print();
	game.SetInput(myInput);
	game.SetRender(myRender);
	game.SetDone(myDone);
	is_running = true;
}

void ZeldaApp::Load() {}

void ZeldaApp::Run() { game.MainLoop(); }

void ZeldaApp::RunIteration() {	game.MainLoopIteration(); }

void ZeldaApp::Clear() {}