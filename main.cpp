/*
	Units are as follows:
	meters
	seconds
	newtons

	and derivatives of those so for example velocity would be meters per second.
*/

#include<chrono>
#include<iostream>
#include<SDL2/SDL.h>
#include<thread>

#include<fstream>

#include "essentials.h"
#include "fabric.h"
#include "fabric_model_5.h"

void draw_circle(SDL_Renderer* renderer, int cx, int cy, int radius) {
	const int segments = 100;
	for (int i = 0; i < segments; i++) {
		float theta1 = 2.0f * M_PI * i / segments;
		float theta2 = 2.0f * M_PI * (i+1) / segments;

		int x1 = cx + radius * cos(theta1);
		int y1 = cy + radius * sin(theta1);
		int x2 = cx + radius * cos(theta2);
		int y2 = cy + radius * sin(theta2);

		SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
	}
}

int orient(pos2 a, pos2 b, pos2 c) {
	float val = (b.y - a.y) * (c.x - b.x) - (b.x - a.x) * (c.y - b.y);

	if (fabs(val) < 1e-9)
		return 0;
	return (val > 0) ? 1 : -1;
}

bool on_segment(pos2 a, pos2 b, pos2 c) {
		return c.x >= std::min(a.x, b.x) && c.x <= std::max(a.x, b.x) && c.y >= std::min(a.y, b.y) && c.y <= std::max(a.y, b.y);
}

bool intersect(pos2 a, pos2 b, pos2 c, pos2 d) {
	int o1 = orient(a, b, c);
	int o2 = orient(a, b, d);
	int o3 = orient(c, d, a);
	int o4 = orient(c, d, b);

	if (o1 != o2 && o3 != o4)
		return true;

	if (o1 == 0 && on_segment(a, b, c))
		return true;
	if (o2 == 0 && on_segment(a, b, d))
		return true;
	if (o3 == 0 && on_segment(c, d, a))
		return true;
	if (o4 == 0 && on_segment(c, d, b))
		return true;

	return false;
}

int main(int argc, char** argv) {
	initialize_fabric();

	std::ofstream fout("state.txt");

	int target_fps = 6000;
	int mouse_fps = 120;
	auto before = std::chrono::high_resolution_clock::now();

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("Fabric Simulation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 2400, 1200, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_Event event;

	float zoom = 20.0f;
	float right = 0.0f;
	float up = 0.0f;
	float speed = 500.0f;
	float scroll_speed = 300.0f;

	bool w = false, a = false, s = false, d = false, t = false, l = false, u = false, f = false, r = false, e = false;

	float x_offset = 1200.0f;
	float y_offset = 600.0f;

	std::pair<pos2, pos2> mouse_positions = std::make_pair(pos2{0, 0}, pos2{0, 0}); // .first -> current position, .second -> last position
	std::pair<int, int> fabric_ids = std::make_pair(-1, -1);
	bool mouse_holding = false;
	int dragging_id = -1;
	float mouse_delta_time = 0.0f;

	bool simulating = true;

	bool running = true;
	while (running) {
		auto now = std::chrono::high_resolution_clock::now();
		float delta_time = (now-before).count()/1000000000.0f;
		before = now;

		mouse_delta_time += delta_time;
		if (mouse_delta_time >= 1.0f/(float)mouse_fps) {
			mouse_positions.second = mouse_positions.first;
			int xx, yy;
			SDL_GetMouseState(&xx, &yy);
			float x = ((float)xx - x_offset) / zoom;
			float y = (y_offset - (float)yy) / zoom;
			mouse_positions.first = pos2{x, y};

			mouse_delta_time = 0.0f;
		}

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			} else if (event.type == SDL_KEYDOWN) {
				SDL_Keycode key = event.key.keysym.sym;
				
				if (key == SDLK_w && !w) {
					w = true;
					up += 1.0f;
				} else if (key == SDLK_s && !s) {
					s = true;
					up -= 1.0f;
				} else if (key == SDLK_a && !a) {
					a = true;
					right -= 1.0f;
				} else if (key == SDLK_d && !d) {
					d = true;
					right += 1.0f;
				} else if (key == SDLK_t && !t) {
					t = true;
					simulating = !simulating;
				} else if (key == SDLK_l && !l) {
					l = true;
					main_fabric.points.push_back(point(mouse_positions.first.x, mouse_positions.first.y, 5, true));
				} else if (key == SDLK_u && !u) {
					u = true;
					main_fabric.points.push_back(point(mouse_positions.first.x, mouse_positions.first.y, 5, false));
				} else if (key == SDLK_f && !f) {
					f = true;
					int found_id = main_fabric.closest(mouse_positions.first);
					if (distance(mouse_positions.first, main_fabric.points[found_id].get_position()) <= 1.0f) {
						if (fabric_ids.first == -1)
							fabric_ids.first = found_id;
						else if (fabric_ids.second == -1)
							fabric_ids.second = found_id;
					}

					if (fabric_ids.first != -1 and fabric_ids.second != -1) {
						bool failed = false;
						for (std::pair<int, int> conn : main_fabric.connections) {
							if (conn == fabric_ids or conn == std::make_pair(fabric_ids.second, fabric_ids.first))
								failed = true;
						}
						if (fabric_ids.first == fabric_ids.second)
							failed = true;

						if (!failed)
							main_fabric.connections.push_back(fabric_ids);
						fabric_ids = std::make_pair(-1, -1);
					}
				} else if (key == SDLK_r && !r) {
					r = true;

					initialize_fabric();
				} else if (key == SDLK_e && !e) {
					e = true;

					int closest_id = main_fabric.closest(mouse_positions.first);
					if (distance(main_fabric.points[closest_id].get_position(), mouse_positions.first) <= 1.0f) { // try to delete a ball
						main_fabric.points.erase(main_fabric.points.begin()+closest_id);
						for (int i = 0; i < main_fabric.connections.size(); i++) {
							if (main_fabric.connections[i].first == closest_id || main_fabric.connections[i].second == closest_id) {
								main_fabric.connections.erase(main_fabric.connections.begin()+i);
								i--;
							} else {
								if (main_fabric.connections[i].first > closest_id)
									main_fabric.connections[i].first--;
								if (main_fabric.connections[i].second > closest_id)
									main_fabric.connections[i].second--;
							}
						}
					}
				}
			} else if (event.type == SDL_KEYUP) {
				SDL_Keycode key = event.key.keysym.sym;

				if (key == SDLK_w && w) {
					w = false;
					up -= 1.0f;
				} else if (key == SDLK_s && s) {
					s = false;
					up += 1.0f;
				} else if (key == SDLK_a && a) {
					a = false;
					right += 1.0f;
				} else if (key == SDLK_d && d) {
					d = false;
					right -= 1.0f;
				} else if (key == SDLK_t && t) {
					t = false;
				} else if (key == SDLK_l && l) {
					l = false;
				} else if (key == SDLK_u && u) {
					u = false;
				} else if (key == SDLK_f && f) {
					f = false;
				} else if (key == SDLK_r && r) {
					r = false;
				} else if (key == SDLK_e && e) {
					e = false;
				}
			} else if (event.type == SDL_MOUSEWHEEL) {
				if (event.wheel.y > 0) {
					zoom += scroll_speed * delta_time;
				} else {
					zoom -= scroll_speed * delta_time;
					zoom = std::max(zoom, 0.0001f);
				}
			} else if (event.type == SDL_MOUSEBUTTONDOWN) {
				mouse_holding = true;

				int closest_id = main_fabric.closest(mouse_positions.first);
				if (!simulating && distance(main_fabric.points[closest_id].get_position(), mouse_positions.first) <= 1.0f)
					dragging_id = closest_id;
			} else if (event.type == SDL_MOUSEBUTTONUP) {
				mouse_holding = false;
			}
		}

		x_offset -= right * delta_time * speed;
		y_offset += up * delta_time * speed;


		// simulate

		if (simulating) {
			main_fabric.simulate(delta_time);
			main_fabric.apply_ripping();
		}

		// apply the mouse stuff like dragging the balls and the velocity

		if (mouse_holding) {
			if (dragging_id != -1) {
				main_fabric.points[dragging_id].x = mouse_positions.first.x;
				main_fabric.points[dragging_id].y = mouse_positions.first.y;
			} else {
				// slice connections
				for (int i = 0; i < main_fabric.connections.size(); i++) {
					if (intersect(mouse_positions.first, mouse_positions.second, main_fabric.points[main_fabric.connections[i].first].get_position(), main_fabric.points[main_fabric.connections[i].second].get_position())) {
						main_fabric.connections.erase(main_fabric.connections.begin() + i);
						break;
					}
				}
			}
		} else {
			if (dragging_id != -1) {
				vec2 velocity = vec2{mouse_positions.first, mouse_positions.second} / delta_time * 0.1f;
				main_fabric.points[dragging_id].x_force = 0.0f;
				main_fabric.points[dragging_id].y_force = 0.0f;
				main_fabric.points[dragging_id].x_velocity = -velocity.x;
				main_fabric.points[dragging_id].y_velocity = -velocity.y;
			}

			dragging_id = -1;
		}

		// display the actual stuff
	
		SDL_SetRenderDrawColor(renderer, 45, 45, 45, 255);
		SDL_RenderClear(renderer);

		if (!simulating) {
			for (int i = 0; i < main_fabric.points.size(); i++) {
				if (i == fabric_ids.first || i == fabric_ids.second)
					SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
				else
					SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				draw_circle(renderer, x_offset + main_fabric.points[i].x*zoom, y_offset - main_fabric.points[i].y*zoom, 1.0f*zoom);
			}
		}

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		for (std::pair<int, int> &conn : main_fabric.connections) {
			SDL_RenderDrawLine(renderer, x_offset + main_fabric.points[conn.first].x*zoom, y_offset - main_fabric.points[conn.first].y*zoom, x_offset + main_fabric.points[conn.second].x*zoom, y_offset - main_fabric.points[conn.second].y*zoom);
		}

		// make the stuff appear on the screen

		SDL_RenderPresent(renderer);

		now = std::chrono::high_resolution_clock::now();
		delta_time = (now-before).count()/1000000000.0f;

		if (1/target_fps > delta_time)
			std::this_thread::sleep_for(std::chrono::milliseconds((int)(1.0f/target_fps-delta_time)*1000));
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	fout<< "#pragma once\n\n";
	fout<< "#include\"fabric.h\"\n\n";
	fout<< "fabric main_fabric(500.0f, 2.0f);\n";
	fout<< "void initialize_fabric() {\n";
	fout<< "\tmain_fabric.points = {\n";
	for (point &p : main_fabric.points)
		fout<< "\t\tpoint(" << p.x << ", " << p.y << ", " << p.mass << ", " << (p.pinned ? "true" : "false") << "),\n";
	fout<< "\t};\n";
	fout<< "\tmain_fabric.connections = {\n";
	for (std::pair<int, int> &conn : main_fabric.connections)
		fout<< "\t\tstd::make_pair(" << conn.first << ", " << conn.second << "),\n";
	fout<< "\t};\n";
	fout<< "}\n";

	return 0;
}
