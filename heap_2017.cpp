// #include <bits/stdc++.h>
#include <utility>
#include <iterator>
#include <algorithm>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <math.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <time.h>
#include <bitset>
#include "heap.h"

using namespace std;

typedef vector<int> vi;
typedef vector<vi> vvi;


template <typename Type>
vector<vector<Type>> create_multivector(int dim1, int dim2, Type init_data = 0) {
    return vector<vector<Type>> (dim1, vector<Type> (dim2, init_data));
}

vector<string> filenames {"0", "a", "b", "c", "d"};
int file_index = 0;











double param2 = 10;

char WALL = '#';
char BBONE = 'b';
char EMPTY = '-';
char TARGET = '.';
char REACH = '+';
char ROUTER = 'r';



vector<vector<char>> grid;

vector<vector<char>> grid_bb;

vector<pair<int, int>> backbone_cells;

map<pair<int, int>, int> coord_to_pos;


int nRows;
int nCols;
int Radius;

int price_Backbone;
int price_Router;
int Budget;

pair<int, int> reachable(int row, int col, bool apply=false) {
    int R = Radius;
    int min_row = max(0, row - R);
    int max_row = min(nRows - 1, row + R);
    int min_col = max(0, col - R);
    int max_col = min(nCols - 1, col + R);


    int span_rows = max_row - min_row + 1; // actual number of rows
    int span_cols = max_col - min_col + 1; // actual number of rows


    int res_points = 0;
    int double_points = 0;


    //  bottom part
    int left_most = min_col - 1;
    int right_most = max_col + 1;

    for (int j = left_most + 1; j <= col; j++) {
        if (grid[row][j] == WALL) {
            left_most = j;
        }
    }
    for (int j = right_most - 1; j >= col; j--) {
        if (grid[row][j] == WALL) {
            right_most = j;
        }
    }

    for (int curr_row = row + 1; curr_row <= max_row; curr_row++) {
        for (int j = left_most + 1; j <= col; j++) {
            if (grid[curr_row][j] == WALL) {
                left_most = j;
            }
        }
        for (int j = right_most - 1; j >= col; j--) {
            if (grid[curr_row][j] == WALL) {
                right_most = j;
            }
        }


        for (int j = left_most + 1; j <= right_most - 1; j++) {
            if (grid[curr_row][j] == REACH || grid[curr_row][j] == 'S') {
                double_points += 1;
            }

            if (grid[curr_row][j] == TARGET) {
                res_points += 1;
                if (apply) {
                    grid[curr_row][j] = REACH;
                }
            }

        }
    }

    //  top part
    left_most = min_col - 1;
    right_most = max_col + 1;
    for (int curr_row = row; curr_row >= min_row; curr_row--) {
        for (int j = left_most + 1; j <= col; j++) {
            if (grid[curr_row][j] == WALL) {
                left_most = j;
            }
        }
        for (int j = right_most - 1; j >= col; j--) {
            if (grid[curr_row][j] == WALL) {
                right_most = j;
            }
        }

        for (int j = left_most + 1; j <= right_most - 1; j++) {
            // grid[curr_row][j] = REACH;
            if (grid[curr_row][j] == REACH || grid[curr_row][j] == 'S') {
                double_points += 1;
            }

            if (grid[curr_row][j] == TARGET) {
                res_points += 1;
                if (apply) {
                    // cout << "here\n";
                    grid[curr_row][j] = REACH;
                }
            }
        }
    }

    if (apply) {
        grid[row][col] = 'S';
    }

    return {res_points, double_points};
}

int closest_backbone(int row, int col, bool apply=false) {
    bool found  = false;

    int min_dist = 0;
    int R = 0;
    int ans_row, ans_col;

    while (!found) {
        int min_row = max(0, row - R);
        int max_row = min(nRows - 1, row + R);
        int min_col = max(0, col - R);
        int max_col = min(nCols - 1, col + R);
        for (int i = min_row; i <= max_row; i++) {
            if (grid_bb[i][min_col] == BBONE) {
                min_dist = R;
                ans_row = i;
                ans_col = min_col;
                found = true;
            }

            if (found) {
                break;
            }

            if (grid_bb[i][max_col] == BBONE) {
                min_dist = R;
                ans_row = i;
                ans_col = max_col;
                found = true;
            }
        }   

        if (found) {
            break;
        }

        for (int j = min_col; j <= max_col; j++) {
            if (grid_bb[min_row][j] == BBONE) {
                min_dist = R;
                ans_row = min_row;
                ans_col = j;
                found = true;
            }

            if (found) {
                break;
            }

            if (grid_bb[max_row][j] == BBONE) {
                min_dist = R;
                ans_row = max_row;
                ans_col = j;
                found = true;
            }
        }   
        R++;
    }

    if (apply) {
        if (ans_row == row && ans_col == col){
            return min_dist;
        }
        while (row != ans_row || col != ans_col) {
            if (ans_row > row) {
                ans_row--;
            } else if (ans_row < row) {
                ans_row++;
            }
            if (ans_col > col) {
                ans_col--;
            } else if (ans_col < col) {
                ans_col++;
            }
            grid_bb[ans_row][ans_col] = BBONE;
            backbone_cells.push_back(make_pair(ans_row, ans_col));
        }
    }

    return min_dist;
}

vector<pair<int, int>> routers;



struct Position {
    int row;
    int col;

    int n_targets_covered = 0;
    int n_score = 0;

    int heap_position;


    Position(int r_, int c_) : row{r_}, col{c_} {
        heap_position = -1;
        n_targets_covered = reachable(row, col).first;
        n_score = param2*reachable(row, col).first;
    }

    void update() {
        pair<int, int> temp = reachable(row, col);
        n_targets_covered = temp.first;
        n_score = param2*temp.first - temp.second;
    }
};


struct ComparePositions {
  bool operator()(const Position* first, const Position* second) const {
    return first->n_score < second->n_score;
  }
};


using Heap = MaxHeap<Position, ComparePositions>;
Heap heap;

vector<Position> all_positions;



void update_all(int row, int col) {
    int R = Radius;
    int min_row = max(0, row - 2*R);
    int max_row = min(nRows - 1, row + 2*R);
    int min_col = max(0, col - 2*R);
    int max_col = min(nCols - 1, col + 2*R);
    for (int i = min_row; i <= max_row; i++) {
        for (int j = min_col; j <= max_col; j++) {
            if (coord_to_pos.count(make_pair(i, j))) {
                int position_idx = coord_to_pos[make_pair(i, j)];
                all_positions[position_idx].update();
                if (all_positions[position_idx].heap_position >= 0) {
                    heap.Modify(all_positions[position_idx].heap_position);
                }
            }
        }
    }
}



int main(int argc, char* argv[]) {
    clock_t ttime;
    ttime = clock();
    ios_base::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    if (argc >= 2) {
        file_index = atoi(argv[1]);
    }

    if (argc >= 3) {
        param2 = stof(argv[2]);
    } 


    ifstream infile("data/" + filenames[file_index] + ".in");
    if (!infile) {
        cerr << "Cannot open infile!\n";
        return 0;
    }


    infile >> nRows >> nCols >> Radius;

    infile >> price_Backbone >> price_Router >> Budget;

    int temp1, temp2;

    infile >> temp1 >> temp2;

    backbone_cells.push_back(make_pair(temp1, temp2));


    grid = create_multivector<char>(nRows, nCols);
    grid_bb = create_multivector<char>(nRows, nCols, '-');
    grid_bb[temp1][temp2] = BBONE;


    for (int i = 0; i < nRows; i++) {
        for (int j = 0; j < nCols; j++) {
            char temp_char;
            infile >> temp_char;
            grid[i][j] = temp_char;
        }
    }

    int score = 0;


    cout.flush();

    for (int i = 0; i < nRows; i++) {
        for (int j = 0; j < nCols; j++) {
            if (grid[i][j] == TARGET) {
                Position temp_pos(i, j);
                all_positions.push_back(temp_pos);
                coord_to_pos[make_pair(i, j)] = all_positions.size() - 1;
            }
        }
    }

    for (int i = 0; i < all_positions.size(); i++) {
        heap.Push(&(all_positions[i]));
    }


    cout.flush();

    bool flag = true;

    int counter = 0;

    while (flag) {

        if (heap.empty()) {
            flag = false;
            continue;
        }

       cout.flush();

        if (!heap.size() %1000) {
            cout << "Heapsize: " << heap.size() << endl;
        }

        Position* temp_position = heap.top();

       cout << "Top at (" <<temp_position->row << ", " << temp_position->col << "), reaches " << temp_position->n_targets_covered << endl ;
       cout.flush();

        if (temp_position->n_targets_covered == 0 || Budget < 100) {
            flag = false;
            continue;
        }


       cout.flush();

        int dist = closest_backbone(temp_position->row, temp_position->col);
        if (Budget < price_Router + dist*price_Backbone) {
            heap.PopTop();
            continue;
        }

       cout.flush();

        int reachable_now = reachable(temp_position->row, temp_position->col, true).first;
        dist = closest_backbone(temp_position->row, temp_position->col, true);
        score += 1000*reachable_now;
        Budget -= price_Router + dist*price_Backbone;
        routers.push_back(make_pair(temp_position->row, temp_position->col));
        heap.PopTop();
        update_all(temp_position->row, temp_position->col);
        counter++;
        cout << counter << endl;

    }

    cout << endl << endl << "N of routes: " << counter << endl;
    cout << "Leftover budget: " << Budget << endl << endl;

    score += Budget;



    

    ttime = clock() - ttime;
    cout << "Time " << ((float) ttime )/CLOCKS_PER_SEC << "\n";

    
    cout << "Scores: " << score << endl;

    ofstream outfile("output/" + filenames[file_index] + ".out");

    if (!outfile) {
        cout << "Cannot open outfile!\n";
        return 0;
    }


    int NN = backbone_cells.size();
    outfile << NN- 1 << endl;
    for (int i = 1; i < NN; i++) {
        outfile << backbone_cells[i].first << " " << backbone_cells[i].second << endl;
    }

    int NRouters = routers.size();
    outfile << NRouters << endl;
    for (auto router : routers) {
        outfile << router.first << " " << router.second << endl;
    }

    outfile.close();



    string temp = "python scorer.py --input data/" + filenames[file_index] + ".in --submission output/" + filenames[file_index] + ".out";
    const char* temp3 = temp.c_str();

    cout << "Scorer\n";
    cout.flush();
    system(temp3);

    return 0;
}
