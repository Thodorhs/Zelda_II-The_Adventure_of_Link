﻿#pragma once
#include "Grid/GridLayer.h"
#include "ViewWindow.h"

class TileLayer {
private:
	
	TileMap map;
	GridLayer* grid = nullptr;
	Dim totalRows = 0, totalColumns = 0;
	BitmapSurface tileSet;
	SDL_Rect viewWin;
	SDL_Texture* dpyBuffer;
	struct cord {
		int min, max;
	}typedef cords;

	cords MaxRoomWidth[NUMBEROFROOMS] = { {0, 40 * 32 }, {50 * 32, 200 * 32} };

	//ElevatorCordPairs[0][0].x = 36;
	

	unsigned CurrentRoom = 0;
	bool dpyChanged = true; //this has changed to cache VW
	void Allocate(void) {
		/*dpyBuffer = BitmapCreate(
			GetResWidth() + 2 * TILE_WIDTH,
			GetResHeight() + 2 * TILE_HEIGHT
		);*/
	}

public:
	
	bool GetDpyChanged() { return dpyChanged; }
	void SetTile(Dim col, Dim row, Index index) { map[row][col] = index;}
	Index GetTile(Dim col, Dim row) const { return map[row][col]; };
	void PutTile(Dim x, Dim y, Index tile, SDL_Renderer* myrenderer, SDL_Texture* texture);

	const SDL_Rect& GetViewWindow(void) const { return viewWin;}
	void SetViewWindow(const SDL_Rect& r) { viewWin = r; dpyChanged = true; }
	
	BitmapTexture* GetBitmap(void) const { return dpyBuffer; }
	int GetPixelWidth(void) const { return viewWin.w; }
	int GetPixelHeight(void) const { return viewWin.h; }
	int GetMapPixelHeight() { return 16 * 32; }
	int GetMapPixelWidth() { return MaxRoomWidth[CurrentRoom].max; }
	int GetMapPixelMinWidth() { return MaxRoomWidth[CurrentRoom].min; }
	void NextRoom() { CurrentRoom++; }
	void PrevRoom() { CurrentRoom--; }
	unsigned GetTileWidth(void) const { return DIV_TILE_WIDTH(viewWin.w); }
	unsigned GetTileHeight(void) const { return DIV_TILE_HEIGHT(viewWin.h); }

	void Display(SDL_Surface* ImgSurface, SDL_Renderer* myrenderer, SDL_Texture* PrevLayerBuffer, bool FinalLayer);
	const SDL_Point Pick(Dim x, Dim y) const;

	void ScrollWithBoundsCheck(float& dx, float dy);
	void FilterScrollDistance(
		int viewStartCoord, // x or y
		int viewSize, // w or h
		float* d, // dx or dy
		int maxMapSize // w or h 
	);
	void FilterScroll(float* dx, float* dy);
	void Scroll(float dx, float dy);
	bool CanScrollHoriz(float dx) const;
	bool CanScrollVert(float dy) const;


	auto ToString(void) const -> const std::string; // unparse
	bool FromString(const std::string&); // parse
	void Save(const std::string& path) const
	{
		fclose(WriteText(fopen(path.c_str(), "wt")));
	}
	bool Load(const std::string& path);
	FILE* WriteText(FILE* fp) const
	{
		fprintf(fp, "%s", ToString().c_str()); return fp;
	}
	bool ReadText(FILE* fp); // TODO: carefull generic parsing
	TileLayer();
	TileLayer(Dim _rows, Dim _cols, BitmapSurface _tileSet, TileMap _map);
	TileLayer(Dim _rows, Dim _cols, BitmapSurface _tileSet, TileMap _map, GridLayer* grid);
	//~TileLayer(); // cleanup here with care!
};