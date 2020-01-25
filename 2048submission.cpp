#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <string>

int twod_to_oned(int row, int col, int rowlen);
bool proc_num(std::vector<int>& v, int bi, int ei);
bool game_over(const std::vector<int>& v);
bool wasd_control(std::vector<int>& v, std::string& in);
void get_col(int c, const std::vector<int>& in, std::vector<int>& out);
void rotate_anti_clock(std::vector<int>& v);
void print_grid(const std::vector<int>& v);
void two_generator(std::vector<int>& v);

// Main
int main() {
  // Load file
  srand(time(0)); // Rand seed based on time

  std::string filename;

  std::cout << "enter intial configuration file name:" << std::endl;
  std::cin >> filename;

  std::ifstream infile;
  infile.open(filename.c_str());

  // Read file
  std::vector<int> s; // This is the vector containing the nxn grid
  if(!infile.is_open()){ // If invalid file
      std::cout << "file not found, using default start configuration" << std::endl;
      // Default grid contains 15 0s and 1 2
      for (int i = 0; i < 15; i++) {
        s.push_back(0);
      }
      s.push_back(2);
  }
  else{ // If valid file
    int tmp;
    while(infile >> tmp){
        s.push_back(tmp);
    }
  }

  print_grid(s);
  std::cout << '\n';

  // Control
  std::string input;
  while (!game_over(s)) { // Game runs until there are no moves left
    std::cin >> input;
    bool valid_move = wasd_control(s, input);
    while(!valid_move){ // Ask for new input every time an invalid move entered
      std::cin >> input;
      valid_move = wasd_control(s, input);
    }
    two_generator(s);
    std::cout << '\n';
    print_grid(s);
    std::cout << '\n';
  }
  std::cout << "game over" << '\n';

  return 0;
}


// Function definitions
int twod_to_oned(int row, int col, int rowlen){
  // function twod_to_oned maps two-dimensional indices to a one-dimensional one
  // it takes in input:
  // - the two indices row and col of a two dimensional data structure
  // - the length of the row, rowlen
  // it returns:
  // - the corresponding one-dimensional index
    return row*rowlen+col;
}

void get_col(int c, const std::vector<int>& in, std::vector<int>& out){
  // function get_col
  // c is column number, out returns the column
  // for example:
  // c: 0; in: 1 2 3 4 4 1 2 76 4 4 4 1 9 4 1 -7 | out: 1 4 4 9
    int side = std::sqrt(in.size()); // Length of a side
    for(int i = 0; i < side; i++){ // Iterate side times through the rows
      out.push_back(in[twod_to_oned(i, c, side)]);
    }
}

bool proc_num(std::vector<int>& v, int bi, int ei){
  // Proc num moves all the numbers to the left and adds them if possible, in the range [bi, ei)
  // ei is 1+ the index of the last element involved in the transformation
  // bi is the start index of the range and includes that element
  // e.g. for[1 2 3 4 5 6 7 8 9 0 0] if [bi, ei) = [3, 8) then v[3, 8) = [4 5 6 7 8]
  //std::cout << max_merges << '\n';
  bool v_changed = false;
  for (int i = bi+1; i < ei; i++) { // For each element in range [bi, ei)
    int current = i;
    while ((v[current-1] == 0) && (current > bi)) { // Swap while left element = 0 and above lower limit bi
      v[current-1] = v[current];
      v[current] = 0;
      current--;
      v_changed = true;
    }
    // If loop is exit means non-zero element found or reached bi
    if ((v[current-1] == v[current]) && (current > bi)) { // if left is same and within range
      v[current-1] += v[current];
      v[current] = 0;
      v_changed = true;
      bi = current; // Change the lower bound such that the function stops combining past this point
    }
  }
  return v_changed;
}

void rotate_anti_clock(std::vector<int>& v){
  // This function performs an “anti-clockwise rotation” of the grid
  // contained in vector v (again this is both an input and output parameter).
  // For example if v initially contains 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16,
  // 1  2  3  4             4 8 12 16
  // 5  6  7  8       ->    3 7 11 15
  // 9 10  11 12            2 6 10 14
  // 13 14 15 16            1 5 9  13
  // the function should change the content to 4 8 12 16 3 7 11 15 2 6 10 14 1 5 9 13.
  // Note that this is a typical example in which twod_to_oned, as mentioned above,
  // must be used for the sake of good design and readability.
  std::vector<int> temp; //  Temporary vector to manipulate
  int side = std::sqrt(v.size()); // Side length
  for (int i = side-1; i >= 0; i--) { // For each column starting with side-1
    get_col(i, v, temp); // Column i stored in current_col
  }
  v = temp;
}

bool game_over(const std::vector<int>& v){
    // If proc_num is true means there is an existing move
    // When && with true it remains true
    // If proc_num is false means there is
    int row_len = std::sqrt(v.size()); // Row length (normal is 4)
    std::vector<int> a = v;
    bool move_a = false, move_d = false, move_s = false, move_w = false;
    // Check 'a'
    for (int i = 0; i < a.size(); i+=row_len) {
      move_a = move_a || proc_num(a, i, i+row_len); // Proc_num on each row
    }
    // Check 'w'
    rotate_anti_clock(a);
    for (int i = 0; i < a.size(); i+=row_len) {
      move_w = move_w || proc_num(a, i, i+row_len); // Proc_num on each row
    }

    // Check 'd'
    rotate_anti_clock(a);
    for (int i = 0; i < a.size(); i+=row_len) {
      move_d = move_d || proc_num(a, i, i+row_len); // Proc_num on each row
    }

    // Check 's'
    rotate_anti_clock(a);
    for (int i = 0; i < a.size(); i+=row_len) {
      move_s = move_s || proc_num(a, i, i+row_len); // Proc_num on each row
    }

    return !(move_a || move_d || move_s || move_w);
}

void print_grid(const std::vector<int>& v){
  int side = std::sqrt(v.size());
  for (int i = 0; i < v.size(); i++) {
    std::cout << v[i] << "\t";
    if ((i+1) % side == 0) {
      std::cout << '\n';
    }
  }

}

bool wasd_control(std::vector<int>& v, std::string& in){
  // If valid_move returns true means there can move
  std::vector<int> v_check = v; // Temporary vector
  int row_len = std::sqrt(v_check.size()); // Row length (normal is 4)
  bool valid_move = false, test;
  // Shift whole grid left
  if (in == "a" && in.length() == 1) {
    for (int i = 0; i < v_check.size(); i += row_len) {
      test = proc_num(v_check, i, i + row_len); // Proc_num on each row
      valid_move = valid_move || test;
    }
  // Shift whole grid down
  } else if (in == "s" && in.length() == 1) {
    rotate_anti_clock(v_check);
    rotate_anti_clock(v_check);
    rotate_anti_clock(v_check);
    for (int i = 0; i < v_check.size(); i += row_len) {
      test = proc_num(v_check, i, i + row_len); // Proc_num on each row
      valid_move = valid_move || test;
    }
    rotate_anti_clock(v_check);
  // Shift whole grid right
  } else if (in == "d" && in.length() == 1) {
    rotate_anti_clock(v_check);
    rotate_anti_clock(v_check);
    for (int i = 0; i < v_check.size(); i += row_len) {
      test = proc_num(v_check, i, i + row_len); // Proc_num on each row
      valid_move = valid_move || test;
    }
    rotate_anti_clock(v_check);
    rotate_anti_clock(v_check);
  // Shift whole grid up
  } else if (in == "w" && in.length() == 1) {
    rotate_anti_clock(v_check);
    for (int i = 0; i < v_check.size(); i += row_len) {
      test = proc_num(v_check, i, i + row_len); // Proc_num on each row
      valid_move = valid_move || test;
    }
    rotate_anti_clock(v_check);
    rotate_anti_clock(v_check);
    rotate_anti_clock(v_check);
  }
  if (v_check == v) {
    valid_move = false;
  }
  else {
    v = v_check;
  }
  return valid_move;
}

void two_generator(std::vector<int>& v){
  std::vector<int> zeroes;
  for (int i = 0; i < v.size(); i++) { // Check # of 0s
    if (v[i] == 0) {
      zeroes.push_back(i); // Store index of zeroes
    }
  }
  // Randomly choose one of the zeroes and set it to 2
  int random_index = rand() % zeroes.size(); // Random index in the range 0 to zeroes.size -1
  v[zeroes[random_index]] = 2;
}
