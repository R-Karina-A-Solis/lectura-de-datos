#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <limits>
#include <unordered_map>

using namespace std;

// Función para leer un archivo CSV con ';' como delimitador
void leerArchivoCSV(const string& nombreArchivo, string*& encabezados, string**& datos, int& filas, int& columnas, unordered_map<string, int>& hashClientes) {
    ifstream archivo(nombreArchivo);

    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo: " << nombreArchivo << endl;
        exit(1);
    }

    string linea;
    filas = 0;
    columnas = 0;

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

    encabezados = new string[columnas];
    datos = new string*[filas - 1];
    for (int i = 0; i < filas - 1; ++i) {
        datos[i] = new string[columnas];
    }

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
            hashClientes[datos[filaActual][0]] = filaActual; // Supone que el clienteID está en la primera columna
            filaActual++;
        } else {
            esPrimeraLinea = false;
        }
    }

    archivo.close();
}

// Función para buscar al cliente y procesar sus datos
void buscarCliente(const unordered_map<string, int>& hashClientes, string** datos, string* encabezados, int columnas, const string& clienteID) {
    auto inicio = chrono::high_resolution_clock::now();
    int iteraciones = 0;

    // Buscar el cliente en la tabla hash
    iteraciones++; // Incrementa una iteración al buscar en el hash
    auto it = hashClientes.find(clienteID);

    if (it != hashClientes.end()) {
        int fila = it->second;
        cout << "Cliente encontrado:\n";

        // Mostrar todos los datos de la fila
        for (int j = 0; j < columnas; ++j) {
            cout << encabezados[j] << ": " << datos[fila][j] << endl;
            iteraciones++; // Una iteración por columna mostrada
        }

        // Sumar valores de las columnas 7 a 17 y encontrar el mayor
        double suma = 0, mayorValor = numeric_limits<double>::lowest();
        int columnaMayor = -1;

        for (int j = 6; j < 17 && j < columnas; ++j) {
            iteraciones++; // Incrementa iteraciones por operación en cada columna
            try {
                double valor = stod(datos[fila][j]);
                suma += valor;
                if (valor > mayorValor) {
                    mayorValor = valor;
                    columnaMayor = j;
                }
            } catch (const invalid_argument&) {
                cerr << "Error al convertir el valor: " << datos[fila][j] << ".\n";
            }
        }

        cout << "En 11 meses gasto: " << suma << " watts de energia." << endl;
        if (columnaMayor != -1) {
            cout << "En el mes de " << encabezados[columnaMayor]
                 << " gasto la mayor cantidad de energia electrica: " << mayorValor << " watts." << endl;
        } else {
            cout << "No se encontraron valores validos en las columnas 7 a 17.\n";
        }
    } else {
        cout << "Cliente no encontrado.\n";
    }

    auto fin = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> tiempo = fin - inicio;

    cout << "Tiempo de busqueda: " << tiempo.count() << " ms\n";
    cout << "Numero de iteraciones realizadas: " << iteraciones << endl;
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
    string* encabezados = nullptr;
    string** datos = nullptr;
    int filas = 0, columnas = 0;
    string nombreArchivo = "data_medidor.csv";

    unordered_map<string, int> hashClientes;

    leerArchivoCSV(nombreArchivo, encabezados, datos, filas, columnas, hashClientes);
    cout << "Archivo leido correctamente.\n";

    while (true) {
        string clienteID;
        cout << "Ingrese el numero de cliente (o escriba 'salir' para terminar): ";
        getline(cin, clienteID);

        if (clienteID == "salir") {
            cout << "Saliendo del programa...\n";
            break;
        }

        buscarCliente(hashClientes, datos, encabezados, columnas, clienteID);
    }

    liberarMemoria(encabezados, datos, filas - 1);

    return 0;
}



