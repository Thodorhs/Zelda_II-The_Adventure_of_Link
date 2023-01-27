#pragma once

#include "GameCharacters.h"

class Link_Class : public GameCharacter{
private:
	std::map<std::string, Animator*> animators;
	std::map<std::string, Animation*>animations;
	std::map<std::string, AnimationFilm*> link_films;
	static  Link_Class singleton;
	Sprite* current;
	bool isLinkPressingDown=false;
public:
	void SetisLinkPressingDown(bool b) {
		isLinkPressingDown = b;
	}

	bool GetisLinkPressingDown(void) {
		return isLinkPressingDown;
	}

	void print_character() { std::cout << "Link!" << "\n"; }

	void set_current(Sprite* s) { current = s; }
	Sprite& get_current(){return *current;}
	void set_animator(std::string id, Animator* anim) {
		animators.insert({ id,anim });
	}
	void set_animation(std::string id, Animation* anim) {
		animations.insert({ id,anim });
	}

	Animator* get_animator(std::string id) {
		auto it = animators.find(id);
		if (it == animators.end())
			return NULL;
		return it->second;
	}

	Animation* get_animation(std::string id) {
		auto it = animations.find(id);
		if (it == animations.end())
			return NULL;
		return it->second;
	}

	void set_film(std::string id, AnimationFilm* anim) {
		link_films.insert({ id,anim });
	}

	AnimationFilm* get_film(std::string id) {
		auto it = link_films.find(id);
		if (it == link_films.end())
			return NULL;
		return it->second;
	}

	void stop_animators() {
		for (auto it : animators) {
			it.second->Stop();
		}
	}
	static auto GetSingleton(void) -> Link_Class& { return singleton; }
	static auto GetSingletonConst(void) -> const Link_Class& { return singleton; }

	
};