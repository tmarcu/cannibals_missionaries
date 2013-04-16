/*
 * Authors: Tudor Marcu and Vinay Bikkina
 * Copyright (C) 2013 Tudor Marcu, Vinay Bikkina. All rights reserved.
 */

#include <iostream>
#include <list>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

#include <stdio.h>
#include <stdlib.h>

using namespace std;

struct node
{
	int ml;
	int cl;
	int mr;
	int cr;
	bool bl;
	bool br;
	struct node *parent;

	bool operator==(const struct node &goal)
	{
		return ((*this).ml == goal.ml &&
				(*this).cl == goal.cl &&
				(*this).mr == goal.mr &&
				(*this).cr == goal.cr &&
				(*this).bl == goal.bl &&
				(*this).br == goal.br);
	}

	struct node operator+(const struct node &goal)
	{
		struct node sum;
		sum.ml = (*this).ml + goal.ml;
		sum.cl = (*this).cl + goal.cl;
		sum.mr = (*this).mr + goal.mr;
		sum.cr = (*this).cr + goal.cr;
		sum.bl = !(*this).bl;
		sum.br = !(*this).br;
		sum.parent = this;

		return sum;
	}
};

list<struct node> graph_search(struct node, struct node);
list<struct node> expand(struct node *);
bool valid_mode(struct node);
void print_state(struct node);

int counter = 0;
const struct node actionr[5] = {{1, 0, -1, 0, NULL, NULL, NULL},
								{2, 0, -2, 0, NULL, NULL, NULL},
								{0, 1, 0, -1, NULL, NULL, NULL},
								{1, 1, -1, -1, NULL, NULL, NULL},
								{0, 2, 0, -2, NULL, NULL, NULL}};

const struct node actionl[5] = {{-1, 0, 1, 0, NULL, NULL, NULL},
								{-2, 0, -2, 0, NULL, NULL, NULL},
								{0, -1, 0, 1, NULL, NULL, NULL},
								{-1, -1, 1, 1, NULL, NULL, NULL},
								{0, -2, 0, 2, NULL, NULL, NULL}};


int main(int argc, char *argv[])
{
	list<struct node> solution;
	struct node start = {};
	struct node goal = {};
	ifstream startfile(argv[1]);
	ifstream goalfile(argv[2]);
	string line;
	int i;
	istringstream *iss;

	if (!startfile.good() || !goalfile.good()) {
		fprintf(stderr, "Could not open files for reading.\n");
		exit(EXIT_FAILURE);
	}

	/* Read first and second line of start and goal files */
	getline(startfile, line);
	iss = new istringstream(line);
	iss->str(line);
	*iss >> start.ml; iss->ignore();
	*iss >> start.cl; iss->ignore();
	*iss >> start.bl; iss->ignore();
	delete iss;
	getline(startfile, line);
	iss = new istringstream(line);
	*iss >> start.mr; iss->ignore();
	*iss >> start.cr; iss->ignore();
	*iss >> start.br; iss->ignore();
	delete iss;

	getline(goalfile, line);
	iss = new istringstream(line);
	iss->str(line);
	*iss >> goal.ml; iss->ignore();
	*iss >> goal.cl; iss->ignore();
	*iss >> goal.bl; iss->ignore();
	delete iss;
	getline(goalfile, line);
	iss = new istringstream(line);
	*iss >> goal.mr; iss->ignore();
	*iss >> goal.cr; iss->ignore();
	*iss >> goal.br; iss->ignore();
	delete iss;

	printf("Running...\n");

	print_state(start);
	print_state(goal);

	solution = graph_search(start, goal);

	while(!solution.empty()) {
		print_state(solution.front());
		solution.pop_front();
	}
	printf("Finished\n");
	printf("Iterations: %d\n", counter);
	startfile.close();
	goalfile.close();
	system("Pause");
	return 0;
}


void print_state(struct node nprint)
{
	printf("Left bank: %d M %d C %d Boat\nRight Bank: %d M %d C %d Boat\n\n",
			nprint.ml, nprint.cl, nprint.bl, nprint.mr, nprint.cr, nprint.br);
}

list<struct node> graph_search(struct node start, struct node goal)
{
		list<struct node> closed;
		list<struct node> fringe;
		list<struct node> solution;
		list<struct node> expanded;
		struct node curr;

		fringe.push_front(start);

		while (!fringe.empty()) {
			curr = fringe.front();
			fringe.pop_front();

			if (curr == goal) {
				while (!(curr == start)) {
					solution.push_front(curr);
					curr = *curr.parent;
				}
				solution.push_front(start);
				return solution;
			}

			/*  node from solution is not in closed */
			if (find(closed.begin(), closed.end(), curr) == closed.end()) {
				closed.push_front(curr);

				struct node *storenode = new node;
				*storenode = curr;
				/* Add the expanded nodes to the fringe */
				expanded = expand(storenode);
				fringe.insert(fringe.begin(), expanded.begin(), expanded.end());
				/* For some reason GCC does not like splice... works in xcode or visual studio though
				fringe.splice(fringe.begin(), expand(storenode)); */
				counter++;

			}
		}

	return fringe;
}

/* Build the list of successors */
list<struct node> expand(struct node *exnode){
    list<struct node> successor;
	struct node s;
	int i;

	for (i = 0; i < 5; i++) {
		s = *exnode + (exnode->bl ? actionl[i] : actionr[i]);
		if (valid_mode(s)) {
			successor.push_front(s);
		}
	}

    return successor;
}

bool valid_mode(struct node newnode)
{
	/* Check for positive; missionaries > cannibals */
	if (newnode.ml < 0 ||newnode.cl < 0 || newnode.mr < 0 || newnode.cr < 0)
		return false;

	if ((newnode.ml != 0 && newnode.ml < newnode.cl) ||
			(newnode.mr != 0 && newnode.mr < newnode.cr))
		return false;

	return true;
}