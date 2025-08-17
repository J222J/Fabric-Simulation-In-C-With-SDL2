#pragma once

#include<cmath>
#include<vector>

#include "../SSTDLIB/essentials.h"

float drag_coefficient = 2.0f;
float g = 10.0f;

class point {
public:
	float x, y;
	float mass;
	float x_velocity, y_velocity;
	float x_force, y_force;

	bool pinned;

	pos2 get_position() {
		return pos2{x, y};
	}


	void apply_velocity(float delta_time) {
		if (pinned)
			return;

		x_force -= x_velocity * drag_coefficient;
		y_force -= y_velocity * drag_coefficient;

		x_velocity += x_force / mass * delta_time;
		y_velocity += y_force / mass * delta_time;

		x += x_velocity * delta_time;
		y += y_velocity * delta_time;
	}

	void apply_forces(point &p, float k, float resting_length) {
		if (pinned && p.pinned)
			return;
		
		float dist = distance(pos2{x, y}, pos2{p.x, p.y});
		float force_val = (dist-resting_length) * k;
		vec2 force_vector = vec2{pos2{x, y}, pos2{p.x, p.y}}.normalized() * force_val;
		
		if (!pinned) {
			x_force += force_vector.x;
			y_force += force_vector.y;
		}

		if (!p.pinned) {
			p.x_force -= force_vector.x;
			p.y_force -= force_vector.y;
		}
	}

	void apply_force(vec2 force) {
		x_force += force.x;
		y_force += force.y;
	}

	void reset_forces() {
		x_force = 0.0f;
		y_force = 0.0f;
	}

	point(float xx, float yy, float mmass, bool ppinned = false) {
		x = xx;
		y = yy;
		mass = mmass;
		pinned = ppinned;

		x_velocity = 0.0f;
		y_velocity = 0.0f;

		x_force = 0.0f;
		y_force = 0.0f;
	}

	point() {
		x = 0.0f;
		y = 0.0f;
		mass = 0.0000001f;
		pinned = false;

		x_velocity = 0.0f;
		y_velocity = 0.0f;

		x_force = 0.0f;
		y_force = 0.0f;
	}

	~point() = default;
};

class fabric {
public:
	std::vector<point> points;
	std::vector<std::pair<int, int>> connections;
	float k;
	float resting_length;
	
	void apply_ripping() {
		for (int i = 0; i < connections.size(); i++) {
			pos2 first = points[connections[i].first].get_position();
			pos2 second = points[connections[i].second].get_position();

			if (distance(first, second) >= 3.0f * resting_length) {
				connections.erase(connections.begin() + i);
				i--;
			}
		}
	}

	void add_point(point p) {
		points.push_back(p);
	}

	void add_connection(std::pair<int, int> conn) {
		connections.push_back(conn);
	}

	void add_points(std::vector<point> ps) {
		for (point &p : ps)
			points.push_back(p);
	}

	void simulate(float delta_time) {
		for (point &p : points) {
			p.reset_forces();
		}

		for (std::pair<int, int> &conn : connections)
			points[conn.first].apply_forces(points[conn.second], k, resting_length);

		for (point &p : points) {
			p.apply_force(vec2{0, -g * p.mass});
			p.apply_velocity(delta_time);
		}
	}

	int closest(pos2 position) {
		int id = 0;
		float dist = distance(points[0].get_position(), position);
		for (int i = 1; i < points.size(); i++) {
			float cur_dist = distance(points[i].get_position(), position);
			if (cur_dist < dist) {
				id = i;
				dist = cur_dist;
			}
		}

		return id;
	}

	void add_connetions(std::vector<std::pair<int, int>> conns) {
		for (std::pair<int, int> &cn : conns)
			connections.push_back(cn);
	}

	fabric(float kk, float rresting_length) {
		k = kk;
		resting_length = rresting_length;
	}

	fabric() {
		k = 0.0f;
		resting_length = 0.0f;
	}

	~fabric() = default;
};
