#include <iostream>

using namespace std;
class route {
    int *r, n; // r – массив, представляющий маршрут; n – количество городов
public:
    route(int num = 0);
    route(const route &); // конструктор копирования
    route &operator=(const route &); // операция присваивания
    ~route() {
        if (r) delete[]r; // Освобождение памяти, занятой массивом.
        r = NULL;
    } // деструктор
    int route_price(int **); // вычисляет стоимость маршрута по матрице стоимости
    bool next_route(); // вычисляет следующий маршрут, используя алгоритм Дейкстра
    friend ostream &operator<<(ostream &, const route &); // вывод маршрута

};

route::route(int num) {
    n = num; // кол. городов
    r = new int[num]; // память
    for (int i = 0; i < num; i++) {
        r[i] = i + 1;//заполняем массив числами от 1 до n
    }
}

route::route(const route &arr) {//копирует длину маршрута
    n = arr.n; // коп. количество городов
    r = new int[arr.n];
    for (int i = 0; i < arr.n; i++) {
        r[i] = arr.r[i];
    }
}

route& route::operator=(const route &x) {//оператор присваивания
    if(this==&x){
        return *this;
    }
    delete(r);
    n=x.n;//присваеваем длину маршрута
    r=new int [x.n];
    for(int i=0; i<x.n; i++){
        r[i]=x.r[i]; // коп знач элементов маршрута
    }
    return *this; // возвращаем ссылку на текущий объект
}

int route::route_price(int **arr) {//подсчёт матрицы стоимости
    int sum=0, b;
    for(int i=0; i<n-1;i++){ // проход по городам
        b=r[i+1];//второй город
        sum+=arr[r[i]-1][b-1];//заносим стоимость пути i в второй
    }
    b=r[0]; // индекс первого города для завершения цикла.
    sum+=arr[r[n-1]-1][b-1];// путь из последнего в первый для цикла
    return sum;
}

bool route::next_route() {
    int i;
    int j;
    bool flag = false; // флаг наличия некст маршрута
    for (i = n - 2; i > -1; i--) // поиск i для перестановки
    {
        if (r[i] < r[i + 1]) // проверка на возрастанию
        {
            flag = true; // next route
            break;
        }
    }//поиск i

    if (flag) { // next route have
        for (j = n - 1; j > i; j--) // j индекс поиск
        {
            if (r[i] < r[j]) // j элемент
            {
                break;
            }
        }//поиск j

        int c;
        c = r[i];
        r[i] = r[j];
        r[j] = c;


        if (i < n - 2) { // Если есть элементы для инвертирования.
            int left = i + 1; // левая граница
            int right = n - 1; // правая граница
            while (left < right) {
                c = r[left];
                r[left] = r[right];
                r[right] = c;
                left++;
                right--;
            }
        }//инвертирование хвоста
        if (r[0] != 1) { // Если первый элемент маршрута не равен 1.
            flag = false; // нету след,цикличен
        }
        return flag;
    }
}

ostream &operator<<(ostream &p, const route &x) {//дружественная ф-ия вывода
    for (int i = 0; i < x.n; i++) { // Перебор элементов маршрута.
        p << x.r[i] << " "; // Вывод текущего элемента маршрута.
    }
    return p; // возвр. потока вывода
}
route task(int num){//решение задачи коммивояжера
    int**matrix=new int* [num];//выделяем память
    for (int i=0; i<num; i++) matrix[i]=new int [num]; // память под строки
    for(int i =0; i<num; i++){//заполняем матрицу (строки)
        for(int j =0; j<num; j++){ // перебор столбцы
            cin >> matrix[i][j]; // ввод элементов
        }
    }
    route r(num);//начальный путь по возрастанию
    route final_r = r;//создание конечного значения
    int sum = r.route_price(matrix);//стоймость начального пути
    cout<<r<<" : "<<sum<<endl;//вывод начального пути и его стоимость
    while(r.next_route()){
        int new_sum = r.route_price(matrix);//стоимость следующего пути
        cout<<r<<" : "<<new_sum<<endl;//вывод следующего пути и его стоимость
        if(sum > new_sum && new_sum>0){//если путь дешевле выбираем его
            sum = new_sum;
            final_r = r;
        }
    }

    return final_r;//возвращаем лучший путь
}
int main(){
    int n=4;
    cout<< task(n);
}

/* дейкстер, гамильтонов, комвияжор */