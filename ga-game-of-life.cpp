#include <iostream>
#include <random>
#include <algorithm>
#include <bitset>
#include <string>

#ifdef _WIN32
    #include <windows.h>
    #define msleep(x) Sleep(x)
#else
    #include <unistd.h>
    #define msleep(x) usleep(x * 1000)
#endif

using namespace std;

#define BOARD_HEIGHT 20
#define BOARD_WIDTH 20
#define ENERGY 100
#define CHROMOSOME_LIMIT 10

class gameOfLife;
class GeneticAlgorithm;
class Chromosome;

class Chromosome{
    private:
    int cell_energy;
    public:
    
    int GetCellEnergy() { return cell_energy; }
    string conversion_binary(int number);
    int binaryToDecimal(string binary_number);
    
    void SetCellEnergy(int Cell_energy) { cell_energy = Cell_energy; }
    
};

class GeneticAlgorithm : public Chromosome{
    private:
    string gen;
    int cell;
    public:
    
    int GetNewCell() { return cell; }
    string GetNewGen() { return gen; }
    int chromosome(int cell_parent_1, int cell_parent_2);
    int crossing(string parent_1, string parent_2);
    int mutation(string new_cell);
    
    void SetNewGen(string New_gen) { gen = New_gen; }
    void read_point_neighbors(int nb[][2], int x, int y);
    void new_cell(Chromosome new_board[BOARD_WIDTH][BOARD_HEIGHT], 
                 Chromosome board[BOARD_WIDTH][BOARD_HEIGHT], int x, int y);
    void overpopulation_or_loneliness(Chromosome board[BOARD_WIDTH][BOARD_HEIGHT], 
                                      int x, int y, int kol);
    void SetNewCell(int New_cell) { cell = New_cell; }
    
};

class gameOfLife : public GeneticAlgorithm{
    private:
    int quantity;
    public:
    
    int GetQuantity() { return quantity; }
    int cmp_board(Chromosome board_1[BOARD_WIDTH][BOARD_HEIGHT], Chromosome board_2[BOARD_WIDTH][BOARD_HEIGHT]);
    int get_live_count(Chromosome board[BOARD_WIDTH][BOARD_HEIGHT]);
    int count_live_neighbors(Chromosome board[][BOARD_HEIGHT], int x, int y);
    
    void SetQuantity(int Quantity) { quantity = Quantity; }
    void read_point_neighbors(int nb[][2], int x, int y);
    void init_board(Chromosome board[BOARD_WIDTH][BOARD_HEIGHT]);
    void print_board(Chromosome board[BOARD_WIDTH][BOARD_HEIGHT]);
    void next_generation(Chromosome board[BOARD_WIDTH][BOARD_HEIGHT], Chromosome prev_board[BOARD_WIDTH][BOARD_HEIGHT]);
    void copy_board(Chromosome src[BOARD_WIDTH][BOARD_HEIGHT], Chromosome dest[BOARD_WIDTH][BOARD_HEIGHT]);
    void GameStart();
    
};

/*The function of converting a number to a binary number of the string type*/
string Chromosome::conversion_binary(int number) {
    bitset<CHROMOSOME_LIMIT> bitset = number;
    string binary_number = bitset.to_string<char, char_traits<char>, allocator<char> >();
    return binary_number;
}

/*The function of converting a binary number of the string type into a decimal number of the numeric type*/
int Chromosome::binaryToDecimal(string binary_number) { 
    int dec = stoi(binary_number, nullptr, 2);
    return dec;
}

/*The function of calculating the chromosomes of a new cell is crossing and mutation in genes*/
int GeneticAlgorithm::chromosome(int cell_parent_1, int cell_parent_2) {
    string binary_cell_1 = conversion_binary(cell_parent_1);
    string binary_cell_2 = conversion_binary(cell_parent_2);
    
    int new_chromosome = crossing(binary_cell_1, binary_cell_2);
    string new_chom = conversion_binary(new_chromosome);
    
    new_chromosome = mutation(new_chom);
    
    return new_chromosome;
}

/*The crossing function is a two—point crossing*/
int GeneticAlgorithm::crossing(string parent_1, string parent_2) {
    string new_cell_1 = "";
    string new_cell_2 = "";
    
    char chars_1[parent_1.length() + 1];
    parent_1.copy(chars_1, parent_1.length() + 1);
    
    char chars_2[parent_2.length() + 1];
    parent_2.copy(chars_2, parent_2.length() + 1);
    
    for (int i = 0; i < CHROMOSOME_LIMIT; i++) {
        if (i < 2) {
            new_cell_1 += chars_1[i];
            new_cell_2 += chars_2[i];
        }
        else if (i < 5 && i >= 2) {
            new_cell_1 += chars_2[i];
            new_cell_2 += chars_1[i];
        }
        else {
            new_cell_1 += chars_1[i];
            new_cell_2 += chars_2[i];
        }
    }
    
    int dec_cell_1 = binaryToDecimal(new_cell_1);
    int dec_cell_2 = binaryToDecimal(new_cell_2);
    
    if (dec_cell_1 >= dec_cell_2) return dec_cell_1;
    else return dec_cell_2;
}

/*Mutation function — mutation by conversion*/
int GeneticAlgorithm::mutation(string new_cell) {
    random_device rd;        
    mt19937_64 gen(rd());
    uniform_int_distribution<> dist(0, 9);
    int random_mut = dist(gen);
    
    GeneticAlgorithm new_cell_;
    new_cell_.SetNewGen("");
    char chars[new_cell.length() + 1];
    new_cell.copy(chars, new_cell.length() + 1);
    int j = 0;
    if (random_mut == 2 || random_mut == 4 || random_mut == 8) {
        for (int i = 0; i < CHROMOSOME_LIMIT; i++) {
            if (i < random_mut) new_cell_.SetNewGen(new_cell_.GetNewGen()+chars[i]);
            else {
                new_cell_.SetNewGen(new_cell_.GetNewGen()+chars[random_mut-j]);
                j++;
            }    
        }
        int dec_cell = binaryToDecimal(new_cell_.GetNewGen());
        return dec_cell;
    }
    else {
        int dec_cell = binaryToDecimal(new_cell);
        return dec_cell;
    }
        
}

/*The function of determining the coordinates of living neighboring cells is the Moore neighborhood*/
void GeneticAlgorithm::read_point_neighbors(int nb[][2], int x, int y) {
    int i, j;
    int k = 0;
    for (i = x - 1; i <= x + 1; i++) {
        for (j = y - 1; j <= y + 1; j++) {
            if (i == x && j == y) continue;
            nb[k][0] = i;
            nb[k][1] = j;
            k++;
        }
    }
}

/*The function of selecting the parents of a cell and obtaining a descendant*/
void GeneticAlgorithm::new_cell(Chromosome new_board[BOARD_WIDTH][BOARD_HEIGHT], 
                 Chromosome board[BOARD_WIDTH][BOARD_HEIGHT], int x, int y) {
    GeneticAlgorithm cell_1, cell_2, cell_3;
    int nb[8][2];
    int x_, y_;
    int count = 0;
    read_point_neighbors(nb, x, y);
    for (int l = 0; l < 8; l++) {
        x_ = nb[l][0];
        y_ = nb[l][1];
        if (x_ < 0 || y_ < 0) continue;
        if (x_ >= BOARD_WIDTH || y_ >= BOARD_HEIGHT) continue;
        if (board[x_][y_].GetCellEnergy() > 0) {
            count++;
            switch (count) {
                case 1: cell_1.SetNewCell(board[x_][y_].GetCellEnergy()); break; 
                case 2: cell_2.SetNewCell(board[x_][y_].GetCellEnergy()); break;
                case 3: cell_3.SetNewCell(board[x_][y_].GetCellEnergy()); break;
            }
        }
    }
    int max_cell = max(max(cell_1.GetNewCell(), cell_2.GetNewCell()), cell_3.GetNewCell());
    int min_cell = min(min(cell_1.GetNewCell(), cell_2.GetNewCell()), cell_3.GetNewCell());
    new_board[x][y].SetCellEnergy(chromosome(max_cell, min_cell));
}

/*A function performed when cells are overpopulated or when alone*/
void GeneticAlgorithm::overpopulation_or_loneliness(Chromosome board[BOARD_WIDTH][BOARD_HEIGHT], 
                                                    int x, int y, int kol) {
    if (kol > 3) board[x][y].SetCellEnergy(board[x][y].GetCellEnergy() - (ENERGY/5));
    else 
        //board[x][y].SetCellEnergy(0);
        board[x][y].SetCellEnergy(board[x][y].GetCellEnergy() - (ENERGY/20));
    if (board[x][y].GetCellEnergy() < 0) board[x][y].SetCellEnergy(0);
}

/*The function of determining the coordinates of living neighboring cells is the Moore neighborhood*/
void gameOfLife::read_point_neighbors(int nb[][2], int x, int y) {
    int k = 0;
    for (int i = x - 1; i <= x + 1; i++) {
        for (int j = y - 1; j <= y + 1; j++) {
            if (i == x && j == y)
                continue;
            nb[k][0] = i;
            nb[k][1] = j;
            k++;
        }
    }
}
    
/*The function of counting the number of living neighboring cells relative to the selected one*/
int gameOfLife::count_live_neighbors(Chromosome board[][BOARD_HEIGHT], int x, int y) {
    gameOfLife count;
    count.SetQuantity(0);
    int x_, y_;
    int nb[8][2];
    read_point_neighbors(nb, x, y);
    for (int i = 0; i < 8; i++) {
        x_ = nb[i][0];
        y_ = nb[i][1];
        if (x_ < 0 || y_ < 0) continue;
        if (x_ >= BOARD_WIDTH || y_ >= BOARD_HEIGHT) continue;
        if (board[x_][y_].GetCellEnergy() > 0) count.SetQuantity(count.GetQuantity()+1);
    }
    return count.GetQuantity();
}

/*The function of filling with pseudorandom cells of the first generation*/
void gameOfLife::init_board(Chromosome board[BOARD_WIDTH][BOARD_HEIGHT]) {
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<> dist(0, ENERGY);
    for (int i = 0; i < BOARD_WIDTH; i++) {
        for (int j = 0; j < BOARD_HEIGHT; j++) {
            int num = dist(gen);
            if (num % 2 == 0) board[i][j].SetCellEnergy(num);
            else board[i][j].SetCellEnergy(0);
        }
    }
}

/*The function of displaying the game board on the console*/
void gameOfLife::print_board(Chromosome board[BOARD_WIDTH][BOARD_HEIGHT]) {
    for (int i = 0; i < BOARD_WIDTH; i++) {
        for (int j = 0; j < BOARD_HEIGHT; j++) {
            if (board[i][j].GetCellEnergy() > 0)
                cout << '*';
            else cout << ' ';
            cout << ' ';
        }
        ///*
        cout << " | ";
        for (int j = 0; j < BOARD_HEIGHT; j++) {
            if (board[i][j].GetCellEnergy() > 0)
                cout << board[i][j].GetCellEnergy();
            else cout << ' ';
            cout << ' ';
        }
        //*/
        cout << endl;
    }
}

/*The function of counting live cells on the board*/
int gameOfLife::get_live_count(Chromosome board[BOARD_WIDTH][BOARD_HEIGHT]) {
    gameOfLife count;
    count.SetQuantity(0);
    for (int i = 0; i < BOARD_WIDTH; i++) {
        for (int j = 0; j < BOARD_HEIGHT; j++) {
            if (board[i][j].GetCellEnergy() > 0) count.SetQuantity(count.GetQuantity()+1);
        }
    }
    return count.GetQuantity();
}

/*The function of creating a new generation*/
void gameOfLife::next_generation(Chromosome board[BOARD_WIDTH][BOARD_HEIGHT], 
                                 Chromosome prev_board[BOARD_WIDTH][BOARD_HEIGHT]) {
    gameOfLife live_nb;
    live_nb.SetQuantity(0);
    for (int i = 0; i < BOARD_WIDTH; i++) {
        for (int j = 0; j < BOARD_HEIGHT; j++) {
            live_nb.SetQuantity(count_live_neighbors(prev_board, i, j));
            if (prev_board[i][j].GetCellEnergy() == 0) {
                if (live_nb.GetQuantity() == 3) new_cell(board, prev_board, i, j);
            }
            else if (live_nb.GetQuantity() < 2 || live_nb.GetQuantity() > 3) 
                    overpopulation_or_loneliness(board, i, j, live_nb.GetQuantity());
        }
    }
}

/*The function of saving the previous state of the board — the previous generation*/
void gameOfLife::copy_board(Chromosome src[BOARD_WIDTH][BOARD_HEIGHT], 
                            Chromosome dest[BOARD_WIDTH][BOARD_HEIGHT]) {
    for (int i = 0; i < BOARD_WIDTH; i++) {
        for (int j = 0; j < BOARD_HEIGHT; j++) {
            dest[i][j] = src[i][j];
        }
    }
}

/*The function of detecting changes on the board of cells*/
int gameOfLife::cmp_board(Chromosome board_1[BOARD_WIDTH][BOARD_HEIGHT], 
                          Chromosome board_2[BOARD_WIDTH][BOARD_HEIGHT]) {
    for (int i = 0; i < BOARD_WIDTH; i++) {
        for (int j = 0; j < BOARD_HEIGHT; j++) {
            if (board_1[i][j].GetCellEnergy() != board_2[i][j].GetCellEnergy())
                return -1;
        }
    }
    return 0;
}

/*The launch function of the game "Life"*/
void gameOfLife::GameStart() {
    Chromosome board[BOARD_WIDTH][BOARD_HEIGHT];
    Chromosome prev_board[BOARD_WIDTH][BOARD_HEIGHT];
    init_board(board);
    int live_points = -1;
    bool optimal = false;
    gameOfLife count;
    count.SetQuantity(0);
    
    int num_generations = 0;
    int time_delay = 1000;
    cout << "Enter the number of generations or specify 0 if there are no restrictions on the number:";
    cin >> num_generations;
    cout << "Enter the generation display delay time or leave the value 1000:";
    cin >> time_delay;
    
    do {
        print_board(board);
        count.SetQuantity(count.GetQuantity()+1);
        cout << endl << "  " << count.GetQuantity() << "  " << endl;
        copy_board(board, prev_board);
        next_generation(board, prev_board);
        optimal = cmp_board(board, prev_board) == 0;
        live_points = get_live_count(board);
        if (optimal) cout << "Optimal configuration detected" << endl;
        if (live_points == 0) cout << "All points died" << endl;
        msleep(time_delay);
    } while (live_points != 0 && !optimal && ((num_generations > count.GetQuantity() 
             && num_generations != 0) || (num_generations == 0)));
}

int main() {
    gameOfLife go;
    go.GameStart();
    
    return 0;
}
