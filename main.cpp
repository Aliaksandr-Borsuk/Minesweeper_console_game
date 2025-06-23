#include <iostream>     // ввод вывод в консоль
#include <vector>       // для работы с дин массивами
#include <ctime>        // для установки seed для генератора
#include <cstdlib>      // для рандома
#include <windows.h>    // для корректного вывода кирилицы в консоли Windows

const int FIELD_SIZE = 10;          //
const int MINES = 10;               //

struct Cell {
    bool has_mine = false;   // есть ли мина
    bool revealed = false;  // открыта ли игроком
    int neighbor_mines = 0;  // сколько мин рядом
};

void place_mines(std::vector<std::vector<Cell>>& field) {
    /* функция установки мин
    создаём случайные координаты и ставит туда мины
    пока оне не закончатся
    */
    int placed = 0;
    while (placed < MINES) {
        int x = rand() % FIELD_SIZE;
        int y = rand() % FIELD_SIZE;
        if (!field[y][x].has_mine) {
            field[y][x].has_mine = true;
            placed++;
        }
    }
}

void count_neighbor_mines(std::vector<std::vector<Cell>>& field) {
    // считаем мины в соседних клетках
    for (int y = 0; y < FIELD_SIZE; ++y) {
        for (int x = 0; x < FIELD_SIZE; ++x) {
            if (field[y][x].has_mine) continue;  // если в клетке мина считать не будем
            int count = 0;
            // перебираем квадрат 3*3
            for (int dy = -1; dy <= 1; ++dy)
                for (int dx = -1; dx <= 1; ++dx){
                    // можно исключать из пересчёта центральную клетку проверяя if (dy || dx)
                    // но,кмк,  проще сложить лишний ноль чем 9 проверок
                    int ny = y + dy, nx = x + dx;
                    if (ny >= 0 && ny < FIELD_SIZE && nx >= 0 && nx < FIELD_SIZE && field[ny][nx].has_mine)
                        count++;
                }
            field[y][x].neighbor_mines = count;
        }
    }
}

void print_field(const std::vector<std::vector<Cell>>& field, bool show_all = false) {
    /*печатаем игровое поле в консоль , при желании можно открыть скрытые клетки
    вход -
    двумерный вектор клеток поля , тип Cell,
    флаг для открытия всего поля.*/
    // печать верхней строки - номера колонок
    std::cout << "  ";
    for (int x = 0; x < FIELD_SIZE; ++x) std::cout << x << " ";
    std::cout << "\n";

    for (int y = 0; y < FIELD_SIZE; ++y) {
        std::cout << y << " ";  // печать номера строки
        for (int x = 0; x < FIELD_SIZE; ++x) {  // печать клеток строки
            const Cell& cell = field[y][x];
            if (show_all  || cell.revealed) {   // если флаг - true или клетка раскрыта
                if (cell.has_mine) std::cout << "* ";            // если мина есть
                else std::cout << cell.neighbor_mines << " ";    // если нет то кол-во мин вокруг
            } else {    // если клетка не раскрыта
                std::cout << "# ";
            }
        }
        std::cout << "\n";
    }
}

bool show_cell(std::vector<std::vector<Cell>>& field, int x, int y, int& quantity_of_safe_cell) {
    /*ф-я открывает клетку на позиции x, y
    если в клетке  и рядом с ней нет мин она рекурсивно открывает соседние клнтки
    возвращает false если рвануло или true если всё норм*/

    // если вышли за границы, или клетка уже открыта то заканчиваем
    if (x < 0 || x >= FIELD_SIZE || y < 0 || y >= FIELD_SIZE || field[y][x].revealed)
        return true;
    // меняем состояние клетки на открытая
    field[y][x].revealed = true;
    // уменьшаем счётчик закрытых безопасных клеток
    quantity_of_safe_cell--;
    // если нашли мину - проиграли
    if (field[y][x].has_mine) return false;
    // если мин рядом нет осматриваем соседей рекурсивно
    if (field[y][x].neighbor_mines == 0) {
        for (int dy = -1; dy <= 1; ++dy)
            for (int dx = -1; dx <= 1; ++dx)
                if (dx || dy)    // саму клетку второй раз не смотрим хотя можно было бы ибо метка уже есть
                    show_cell(field, x + dx, y + dy, quantity_of_safe_cell);
    }
    // когда все соседи осмотрены - заканчиваем
    return true;
}

void get_x_y(int& x, int& y){
    while (true){
        std::cout << "Введите координаты (x y): ";
        std::cin >> x >> y;

        // проверяем что числа
        if (std::cin.fail()){
            std::cin.clear();             // сброс флага ошибки
            std::cin.ignore(1000, '\n');  // очистка буфера ввода
            std::cout << "Ошибка. Введите два целых числа.\n";
            continue;
        }

        // проверка значений
        if (x<0 || x>=FIELD_SIZE || y<0 || y>=FIELD_SIZE){
            std::cout << "Ошибка. Введите числа от 0 до " << FIELD_SIZE - 1 << ".\n";
            continue;
        }

        break;  // если всё хорошо
    }
}

int main() {
    SetConsoleOutputCP(65001); // Устанавливаем UTF-8
    std::cout<< "\nДобро пожаловать в консольного Сапёра!\n" << std ::endl;

    int quantity_of_safe_cell = FIELD_SIZE*FIELD_SIZE - MINES;  //  счётчик закрытых безопасных клеток
    srand(time(nullptr));   // инициализируем генератор случайных чисел
    std::vector<std::vector<Cell>> field(FIELD_SIZE, std::vector<Cell>(FIELD_SIZE));    // создание поля
    place_mines(field);  // ставим мины
    count_neighbor_mines(field);  // считаем для каждой пустой клетки кол-во мин вокруг

    //////////////////////////////////////////////
    // std::cout<<"\nНужно открыть "<< quantity_of_safe_cell<<" закрытых безопасных клеток.\n";


    while (quantity_of_safe_cell) { // игровой цикл пока не открыл мину или все чистые клетки
        print_field(field);      // печать поля
        int x, y;  // ход игрока
        get_x_y(x, y);

        if (!show_cell(field, x, y, quantity_of_safe_cell)) {     // открываем и если попал на мину - проигрыш, конец игры
            std::cout << "Бум! Вы попали на мину.\n";
            print_field(field, true);    // открытие всего поля
            return 0;
        }

        std::cout<<"\nОсталось открыть "<< quantity_of_safe_cell<<" безопасных клеток.\n";

    }

    std::cout << "Поздравляем! Вы победили!\n";
        print_field(field, true);

    return 0;
}
