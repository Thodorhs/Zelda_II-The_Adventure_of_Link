
#include "../../../../Engine/Include/Sprites/Sprite.h"
void Sprite::Display(BitmapSurface dest, const SDL_Rect& dpyArea, const Clipper& clipper) const {
	SDL_Rect clippedBox;
	Point dpyPos;
	if (clipper.Clip(GetBox(), dpyArea, &dpyPos, &clippedBox)) {
		SDL_Rect clippedFrame{
		frameBox.x + clippedBox.x,
		frameBox.y + clippedBox.y,
		clippedBox.w,
		clippedBox.h
		};
		/*MaskedBlit(
			currFilm->GetBitmap(),
			clippedFrame,
			dest,
			dpyPos
		);*/ //the fuck is dis
	}
}const Sprite::Mover MakeSpriteGridLayerMover(GridLayer* gridLayer, Sprite* sprite) {
	return [gridLayer, sprite](const SDL_Rect& r, int* dx, int* dy) {
		// the r is actually awlays the sprite->GetBox():
		assert(r.x == sprite->GetBox().x && 
			   r.y == sprite->GetBox().y && 
			   r.h == sprite->GetBox().h && 
			   r.w == sprite->GetBox().w);
		gridLayer->FilterGridMotion(r, dx, dy);
		if (*dx || *dy){}
			//sprite->SetHasDirectMotion(true).Move(*dx, *dy).SetHasDirectMotion(false); //kati einai auto twra den kserw fua
	};
};template <class T> bool clip_rect(
	T x, T y, T w, T h,
	T wx, T wy, T ww, T wh,
	T* cx, T* cy, T* cw, T* ch
) {
	*cw = T(std::min(wx + ww, x + w)) - (*cx = T(std::max(wx, x)));
	*ch = T(std::min(wy + wh, y + h)) - (*cy = T(std::max(wy, y)));
	return *cw > 0 && *ch > 0;
}
bool clip_rect(const SDL_Rect& r, const SDL_Rect& area, SDL_Rect* result) {
	return clip_rect(
		r.x,
		r.y,
		r.w,
		r.h,
		area.x,
		area.y,
		area.w,
		area.h,
		&result->x,
		&result->y,
		&result->w,
		&result->h
	);
}bool Clipper::Clip(const SDL_Rect& r, const SDL_Rect& dpyArea, Point* dpyPos, SDL_Rect* clippedBox) const {
	SDL_Rect visibleArea;
	if (!clip_rect(r, view(), &visibleArea))
	{
		clippedBox->w = clippedBox->h = 0; return false;
	}
	else {
		// clippedBox is in �r� coordinates, sub-rectangle of the input rectangle
		clippedBox->x = r.x - visibleArea.x;
		clippedBox->y = r.y - visibleArea.y;
		clippedBox->w = visibleArea.w;
		clippedBox->h = visibleArea.h;
		dpyPos->x = dpyArea.x + (visibleArea.x - view().x);
		dpyPos->y = dpyArea.y + (visibleArea.y - view().y);
		return true;
	}
}

const Clipper MakeTileLayerClipper(TileLayer* layer) {
	return Clipper().SetView(
		[layer](void)
		{ return layer->GetViewWindow(); }
	);
}