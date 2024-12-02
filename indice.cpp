#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <limits>
#include <unordered_map>

using namespace std;

// Función para leer un archivo CSV con ';' como delimitador y construir un índice
void leerArchivoCSV(const string& nombreArchivo, string*& encabezados, string**& datos, int& filas, int& columnas, unordered_map<string, int>& indice) {
    ifstream archivo(nombreArchivo);

    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo: " << nombreArchivo << endl;
        exit(1);
    }

    string linea;
    filas = 0;
    columnas = 0;

    // Determinar el número de filas y columnas
    while (getline(archivo, linea)) {
        filas++;
        if (filas == 1) { // Contar columnas en la primera línea
            stringstream ss(linea);
            string celda;
            while (getline(ss, celda, ';')) {
                columnas++;
            }
        }
    }

    archivo.clear();
    archivo.seekg(0, ios::beg); // Volver al inicio del archivo

    // Asignar memoria para encabezados y datos
    encabezados = new string[columnas];
    datos = new string*[filas - 1];
    for (int i = 0; i < filas - 1; ++i) {
        datos[i] = new string[columnas];
    }

    // Leer los datos del archivo y construir el índice
    bool esPrimeraLinea = true;
    int filaActual = 0;

    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string celda;
        int columnaActual = 0;

        while (getline(ss, celda, ';')) {
            if (esPrimeraLinea) {
                encabezados[columnaActual] = celda;
            } else {
                datos[filaActual][columnaActual] = celda;
            }
            columnaActual++;
        }

        if (!esPrimeraLinea) {
            // Agregar el identificador único al índice
            indice[datos[filaActual][0]] = filaActual; // Supone que el identificador único está en la primera columna
            filaActual++;
        }
        esPrimeraLinea = false;
    }

    archivo.close();
}

// Función para buscar al cliente simulando iteraciones
void buscarClienteSimulado(string** datos, string* encabezados, int columnas, int filas, const string& clienteID) {
    auto inicio = chrono::high_resolution_clock::now();

    int iteracionesBusqueda = 0; // Contador de iteraciones de búsqueda
    int filaEncontrada = -1;

    // Simular búsqueda lineal en los datos
    for (int i = 0; i < filas; ++i) {
        iteracionesBusqueda++;
        if (datos[i][0] == clienteID) { // Supone que el clienteID está en la primera columna
            filaEncontrada = i;
            break;
        }
    }

    if (filaEncontrada != -1) {
        cout << "Cliente encontrado:\n";

        // Mostrar todos los datos de la fila
        for (int j = 0; j < columnas; ++j) {
            cout << encabezados[j] << ": " << datos[filaEncontrada][j] << endl;
        }

        // Sumar los valores de las columnas 7 a 17
        double suma = 0, mayorValor = numeric_limits<double>::lowest();
        int columnaMayor = -1;

        for (int j = 6; j < 17 && j < columnas; ++j) {
            try {
                double valor = stod(datos[filaEncontrada][j]);
                suma += valor;
                if (valor > mayorValor) {
                    mayorValor = valor;
                    columnaMayor = j;
                }
            } catch (const invalid_argument&) {
                cerr << "Error al convertir el valor: " << datos[filaEncontrada][j] << ".\n";
            }
        }

        cout << "En 11 meses gasto " << suma << " watts de energia" << endl;
        if (columnaMayor != -1) {
            cout << "En el mes de " << encabezados[columnaMayor]
                 << " gasto la mayor cantidad de energia electrica, que es de " << mayorValor << " watts" << endl;
        } else {
            cout << "No se encontraron valores válidos en las columnas 7 a 17.\n";
        }
    } else {
        cout << "Cliente no encontrado.\n";
    }

    cout << "Numero de iteraciones en la busqueda: " << iteracionesBusqueda << endl;

    auto fin = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> tiempo = fin - inicio;

    cout << "Tiempo de busqueda: " << tiempo.count() << " ms\n";
}

// Liberar memoria asignada dinámicamente
void liberarMemoria(string*& encabezados, string**& datos, int filas) {
    delete[] encabezados;
    for (int i = 0; i < filas; ++i) {
        delete[] datos[i];
    }
    delete[] datos;
}

int main() {
    string* encabezados = nullptr;       // Arreglo dinámico para los encabezados
    string** datos = nullptr;           // Arreglo dinámico para los datos
    int filas = 0, columnas = 0;        // Número de filas y columnas
    unordered_map<string, int> indice; // Índice para búsqueda rápida
    string nombreArchivo = "data_medidor.csv";  // Nombre del archivo CSV

    // Leer el archivo automáticamente al iniciar
    leerArchivoCSV(nombreArchivo, encabezados, datos, filas, columnas, indice);
    cout << "Archivo leido correctamente.\n";

    while (true) {
        string clienteID;
        cout << "Ingrese el numero de cliente (o escriba 'salir' para terminar): ";
        getline(cin, clienteID);

        if (clienteID == "salir") {
            cout << "Saliendo del programa...\n";
            break;
        }

        // Buscar cliente simulando iteraciones
        buscarClienteSimulado(datos, encabezados, columnas, filas - 1, clienteID);
    }

    // Liberar memoria asignada dinámicamente
    liberarMemoria(encabezados, datos, filas - 1);

    return 0;
}
