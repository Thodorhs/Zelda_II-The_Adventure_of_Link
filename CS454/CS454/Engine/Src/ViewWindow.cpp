#include "../Include/ViewWindow.h"

SDL_Texture* RenderTextureTarget;
Point viewPosCached{ -1, -1 };
SDL_Rect PTsrcrect;
SDL_Rect PTdstrect;

void ViewWindow::PutTile(Dim x, Dim y, Index tile, SDL_Renderer* myrenderer, SDL_Texture* texture) {
	PTsrcrect.x = MUL_TILE_WIDTH(tile % 12);
	PTsrcrect.y = MUL_TILE_HEIGHT(tile / 12);
	PTsrcrect.h = PTsrcrect.w = 16;

	PTdstrect.x = x;
	PTdstrect.y = y;
	PTdstrect.h = PTdstrect.w = 16;
	SDL_RenderCopy(myrenderer, texture, &PTsrcrect, &PTdstrect); //Same as SDL_BlitSurface but uses the gpu so its faster
}


void ViewWindow::TileTerrainDisplay(TileMap* map, const Rect& viewWin, const Rect& displayArea, SDL_Surface* ImgSurface, SDL_Renderer* myrenderer) {
	if (viewPosCached.x != viewWin.x || viewPosCached.y != viewWin.y) {
		auto startCol = DIV_TILE_WIDTH(viewWin.x);
		auto startRow = DIV_TILE_HEIGHT(viewWin.y);
		auto endCol = DIV_TILE_WIDTH(viewWin.x + viewWin.w - 1);
		auto endRow = DIV_TILE_HEIGHT(viewWin.y + viewWin.h - 1);

		viewPosCached.x = viewWin.x, viewPosCached.y = viewWin.y;

		SDL_Texture* texture = SDL_CreateTextureFromSurface(myrenderer, ImgSurface); //Loading the tileset
		RenderTextureTarget = SDL_CreateTexture(myrenderer, 0, SDL_TEXTUREACCESS_TARGET, viewWin.w, viewWin.h); //Preparing to load the map to the texture
		SDL_SetRenderTarget(myrenderer, RenderTextureTarget); //Setting the target of SDL_RenderCopy to be the texture

		for (unsigned short row = startRow; row <= endRow; ++row)
			for (unsigned short col = startCol; col <= endCol; ++col) 
				PutTile(MUL_TILE_WIDTH(col - startCol), MUL_TILE_HEIGHT(row - startRow), (*map)[row][col], myrenderer, texture);
		
		SDL_SetRenderTarget(myrenderer, NULL); //Unsetting the target of SDL_RenderCopy (now the target is the screen render)
	}


	SDL_RenderCopy(myrenderer, RenderTextureTarget, NULL, NULL); //Setting the texture we loaded earlier(Rendertexture) to be displayed on our window
}