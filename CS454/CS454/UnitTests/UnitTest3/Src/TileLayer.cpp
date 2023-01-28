#include "../../../Engine/Include/TileLayer.h"

TileLayer::TileLayer() {}
TileLayer::TileLayer(Dim _rows, Dim _cols, BitmapSurface _tileSet, TileMap _map) { // BackGround Layer
	map = _map;
	totalRows = _rows, totalColumns = _cols;
	tileSet = _tileSet;
	viewWin = { 0, 0, 640, 480 };
}
TileLayer::TileLayer(Dim _rows, Dim _cols, BitmapSurface _tileSet, TileMap _map, GridLayer* _grid) { //Action Layer
	map = _map;
	totalRows = _rows, totalColumns = _cols;
	grid = _grid;
	tileSet = _tileSet;
	viewWin = { 0, 0, 640, 480 };
}

void TileLayer::PutTile(Dim x, Dim y, Index tile, SDL_Renderer* myrenderer, SDL_Texture* texture) {
	SDL_Rect PTsrcrect{};
	SDL_Rect PTdstrect{};

	PTsrcrect.x = ((tile % TILESETWIDTH) << 4);
	PTsrcrect.y = ((tile / TILESETHEIGHT) << 4);
	PTsrcrect.h = PTsrcrect.w = 16;

	PTdstrect.x = x;
	PTdstrect.y = y;
	PTdstrect.h = PTdstrect.w = 32;
	SDL_RenderCopy(myrenderer, texture, &PTsrcrect, &PTdstrect); //Same as SDL_BlitSurface but uses the gpu so its faster
}

//If you have a previous layer you need to pass the texture of the previous layer to this function.
void TileLayer::Display(SDL_Surface * ImgSurface, SDL_Renderer* myrenderer, SDL_Texture* PrevLayerBuffer, bool FinalLayer) {
	SDL_Rect PTdstrect{};
	if (dpyChanged) {
		auto startCol = DIV_TILE_WIDTH(viewWin.x);
		auto startRow = DIV_TILE_HEIGHT(viewWin.y);
		auto endCol = DIV_TILE_WIDTH(viewWin.x + viewWin.w - 1);
		auto endRow = DIV_TILE_HEIGHT(viewWin.y + viewWin.h - 1);

		SDL_Texture* Tileset = SDL_CreateTextureFromSurface(myrenderer, ImgSurface); //Loading the tileset
		
		if (PrevLayerBuffer == nullptr)
			dpyBuffer = SDL_CreateTexture(myrenderer, 0, SDL_TEXTUREACCESS_TARGET, viewWin.w, viewWin.h); //Preparing to load the map to the texture
		else
			dpyBuffer = PrevLayerBuffer;

		SDL_SetRenderTarget(myrenderer, dpyBuffer); //Setting the target of SDL_RenderCopy to be the texture

		for (unsigned short row = startRow; row <= endRow; ++row)
			for (unsigned short col = startCol; col <= endCol; ++col) {
				if (map[row][col] == -1) continue;
				PutTile(MUL_TILE_WIDTH(col - startCol), MUL_TILE_HEIGHT(row - startRow), map[row][col], myrenderer, Tileset);
			}
		dpyChanged = false;
	}

	if (FinalLayer) {
		SDL_SetRenderTarget(myrenderer, NULL); //Unsetting the target of SDL_RenderCopy (now the target is the screen render)
		SDL_RenderCopy(myrenderer, dpyBuffer, NULL, NULL);
	}//Setting the texture we loaded earlier(dpyBuffer) to be displayed on our window
}

const SDL_Point TileLayer::Pick(Dim x, Dim y) const {
	return { DIV_TILE_WIDTH(x + viewWin.x),
	DIV_TILE_HEIGHT(y + viewWin.y) };
}

void TileLayer::FilterScrollDistance(
	int viewStartCoord, // x or y
	int viewSize, // w or h
	float* d, // dx or dy
	int maxMapSize // w or h 
) {
	auto val = *d + viewStartCoord;
	if (val < 0)
		*d = viewStartCoord; // cross low bound
	else
		if (viewSize >= maxMapSize) { *d = 0;}// fits entirely
		else
			if ((val + viewSize) >= maxMapSize){ // cross upper bound
				//*d = maxMapSize - (viewStartCoord + viewSize);
				*d = 0;
				std::cout << "Left Corner d=" << *d << std::endl;
			}else if (val<= GetMapPixelMinWidth()) {
				*d = 0;
				std::cout << "Right Corner d=" << *d << std::endl;
			}

}

void TileLayer::FilterScroll(float* dx, float* dy) {
	FilterScrollDistance(viewWin.x, viewWin.w, dx, GetMapPixelWidth());
	FilterScrollDistance(viewWin.y, viewWin.h, dy, GetMapPixelHeight());
}

void TileLayer::ScrollWithBoundsCheck(float& dx, float dy) {
	FilterScroll(&dx, &dy);
	Scroll(dx, dy);
}


void TileLayer::Scroll(float dx, float dy) {
	dpyChanged = true;
	viewWin.x += dx; viewWin.y += dy;
}

bool TileLayer::CanScrollHoriz(float dx) const {
	return viewWin.x >= -dx && (viewWin.x + viewWin.w + dx) <= 336;
}

bool TileLayer::CanScrollVert(float dy) const {
	return viewWin.y >= -dy && (viewWin.y + viewWin.h + dy) <= 672;
}

