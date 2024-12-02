#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <limits>

using namespace std;

// Función para leer un archivo CSV con ';' como delimitador
void leerArchivoCSV(const string& nombreArchivo, string*& encabezados, string**& datos, int& filas, int& columnas) {
    ifstream archivo(nombreArchivo);

    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo: " << nombreArchivo << endl;
        exit(1);
    }

    string linea;
    filas = 0;
    columnas = 0;

    cout << "Leyendo el archivo: " << nombreArchivo << endl;

    // Determinar el número de filas y columnas
    while (getline(archivo, linea)) {
        cout << "Línea leída: " << linea << endl; // Mostrar cada línea leída
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

    // Leer los datos del archivo
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
            cout << "Fila " << filaActual + 1 << " procesada: ";
            for (int j = 0; j < columnas; ++j) {
                cout << datos[filaActual][j] << (j < columnas - 1 ? ", " : "");
            }
            cout << endl;
            filaActual++;
        } else {
            cout << "Encabezados procesados: ";
            for (int j = 0; j < columnas; ++j) {
                cout << encabezados[j] << (j < columnas - 1 ? ", " : "");
            }
            cout << endl;
            esPrimeraLinea = false;
        }
    }

    archivo.close();
}

// Función para buscar al cliente, mostrar todos los datos, sumar y encontrar el mayor
void buscarCliente(string** datos, string* encabezados, int filas, int columnas, const string& clienteID) {
    if (filas <= 0 || columnas <= 0) {
        cout << "No hay datos disponibles.\n";
        return;
    }

    auto inicio = chrono::high_resolution_clock::now();
    bool encontrado = false;
    int iteraciones = 0;

    for (int i = 0; i < filas; ++i) {
        iteraciones++;
        if (datos[i][0] == clienteID) { // Supone que el identificador único está en la primera columna
            encontrado = true;
            cout << "Cliente encontrado:\n";

            // Mostrar todos los datos de la fila
            for (int j = 0; j < columnas; ++j) {
                cout << encabezados[j] << ": " << datos[i][j] << endl;
            }

            // Sumar los valores de las columnas 7 a 17
            double suma = 0, mayorValor = numeric_limits<double>::lowest();
            int columnaMayor = -1;

            for (int j = 6; j < 17 && j < columnas; ++j) { // Columnas 7 (índice 6) a 17
                try {
                    double valor = stod(datos[i][j]);
                    suma += valor;
                    if (valor > mayorValor) {
                        mayorValor = valor;
                        columnaMayor = j;
                    }
                } catch (const invalid_argument&) {
                    cerr << "Error al convertir el valor: " << datos[i][j] << ".\n";
                }
            }

            cout << "En 11 meses gasto " << suma << " watts de energia" << endl;
            if (columnaMayor != -1) {
                cout << "En el mes de " << encabezados[columnaMayor]
                     << " gasto la mayor cantidad de energia electrica, que es de " << mayorValor << " watts" << endl;
            } else {
                cout << "No se encontraron valores válidos en las columnas 7 a 17.\n";
            }

            break;
        }
    }

    if (!encontrado) {
        cout << "Cliente no encontrado.\n";
    }

    auto fin = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> tiempo = fin - inicio;

    cout << "Tiempo de busqueda: " << tiempo.count() << " ms\n";
    cout << "Numero de iteraciones: " << iteraciones << endl;
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
    string nombreArchivo = "data_medidor.csv";  // Nombre del archivo CSV

    // Leer el archivo automáticamente al iniciar
    leerArchivoCSV(nombreArchivo, encabezados, datos, filas, columnas);
    cout << "Archivo leído correctamente.\n";

    while (true) {
        string clienteID;
        cout << "Ingrese el numero de cliente (o escriba 'salir' para terminar): ";
        getline(cin, clienteID);

        if (clienteID == "salir") {
            cout << "Saliendo del programa...\n";
            break;
        }

        // Buscar cliente y procesar los datos
        buscarCliente(datos, encabezados, filas - 1, columnas, clienteID);
    }

    // Liberar memoria asignada dinámicamente
    liberarMemoria(encabezados, datos, filas - 1);

    return 0;
}
