#include <iostream>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <climits>
#include <mutex>

using namespace std;
using namespace chrono;

mutex mtx;

// ============================
// GENERAR MATRIZ ALEATORIA
// ============================

vector<vector<int>> generarMatriz(int n) {

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 10);

    vector<vector<int>> matriz(n, vector<int>(n));

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {

            if(i == j)
                matriz[i][j] = 0;
            else
                matriz[i][j] = dis(gen);
        }
    }

    return matriz;
}

// ============================
// MOSTRAR MATRIZ
// ============================

void mostrarMatriz(vector<vector<int>>& matriz) {

    for(int i = 0; i < matriz.size(); i++) {

        for(int j = 0; j < matriz.size(); j++) {

            cout << matriz[i][j] << "\t";
        }

        cout << endl;
    }
}

// ============================
// BACKTRACKING
// ============================

void backtracking(
    vector<vector<int>>& matriz,
    vector<bool>& visitado,
    int actual,
    int destino,
    int costoActual,
    int& minCosto
) {

    if(actual == destino) {

        if(costoActual < minCosto) {
            minCosto = costoActual;
        }

        return;
    }

    visitado[actual] = true;

    for(int i = 0; i < matriz.size(); i++) {

        if(!visitado[i] && matriz[actual][i] > 0) {

            backtracking(
                matriz,
                visitado,
                i,
                destino,
                costoActual + matriz[actual][i],
                minCosto
            );
        }
    }

    visitado[actual] = false;
}

// ============================
// EJECUCIÓN SECUENCIAL
// ============================

int resolverSecuencial(vector<vector<int>>& matriz) {

    int n = matriz.size();

    vector<bool> visitado(n, false);

    int minCosto = INT_MAX;

    backtracking(
        matriz,
        visitado,
        0,
        n - 1,
        0,
        minCosto
    );

    return minCosto;
}

// ============================
// FUNCIÓN PARA HILOS
// ============================

void backtrackingParalelo(
    vector<vector<int>>& matriz,
    int nodoInicial,
    int& minGlobal
) {

    int n = matriz.size();

    vector<bool> visitado(n, false);

    visitado[0] = true;

    int minLocal = INT_MAX;

    backtracking(
        matriz,
        visitado,
        nodoInicial,
        n - 1,
        matriz[0][nodoInicial],
        minLocal
    );

    lock_guard<mutex> lock(mtx);

    if(minLocal < minGlobal) {
        minGlobal = minLocal;
    }
}

// ============================
// EJECUCIÓN PARALELA
// ============================

int resolverParalelo(
    vector<vector<int>>& matriz,
    int numHilos
) {

    int n = matriz.size();

    int minGlobal = INT_MAX;

    vector<thread> hilos;

    for(int i = 1; i < n && hilos.size() < numHilos; i++) {

        hilos.push_back(
            thread(
                backtrackingParalelo,
                ref(matriz),
                i,
                ref(minGlobal)
            )
        );
    }

    for(auto& t : hilos) {
        t.join();
    }

    return minGlobal;
}

// ============================
// MAIN
// ============================

int main() {

    vector<int> tamanos = {2,3,4,5,6};

    for(int n : tamanos) {

        cout << "\n==============================" << endl;
        cout << "MATRIZ " << n << "x" << n << endl;
        cout << "==============================" << endl;

        auto matriz = generarMatriz(n);

        mostrarMatriz(matriz);

        // ======================
        // 1 HILO
        // ======================

        auto inicio1 = high_resolution_clock::now();

      int resultado1;

        for(int i = 0; i < 1000; i++) {
        resultado1 = resolverSecuencial(matriz);
}

        auto fin1 = high_resolution_clock::now();

        auto tiempo1 =
            duration_cast<microseconds>(fin1 - inicio1);

        cout << "\n[1 HILO]" << endl;
        cout << "Costo minimo: " << resultado1 << endl;
        cout << "Tiempo: "
             << tiempo1.count()
             << " us" << endl;

        // ======================
        // 2 HILOS
        // ======================

        auto inicio2 = high_resolution_clock::now();

       int resultado2;

        for(int i = 0; i < 1000; i++) {
        resultado2 = resolverSecuencial(matriz);
}

        auto fin2 = high_resolution_clock::now();

        auto tiempo2 =
            duration_cast<microseconds>(fin2 - inicio2);

        cout << "\n[2 HILOS]" << endl;
        cout << "Costo minimo: " << resultado2 << endl;
        cout << "Tiempo: "
             << tiempo2.count()
             << " us" << endl;

        // ======================
        // 4 HILOS
        // ======================

        auto inicio4 = high_resolution_clock::now();

       int resultado4;

        for(int i = 0; i < 1000; i++) {
        resultado4 = resolverSecuencial(matriz);
}
        auto fin4 = high_resolution_clock::now();

        auto tiempo4 =
            duration_cast<microseconds>(fin4 - inicio4);

        cout << "\n[4 HILOS]" << endl;
        cout << "Costo minimo: " << resultado4 << endl;
        cout << "Tiempo: "
             << tiempo4.count()
             << " us" << endl;
    }

    return 0;
}