/*
* @file modelo.c
*
* @brief Archivo que contiene las funciones que tienen que ver
* directamente con la lógica del juego de gato, como determinar ganador y generar
* turno de la computadora
* 
* @authors  Perla Hernández Cardoso
*           Areli Rabner Concha
* @date 27/04/2020
*/
#include <gtk/gtk.h>
#include "modelo.h"

static gint revisarGanador(const gint combinacionesGanadoras[][3], const gint tamano, const gchar jugadorProbado, const char *tablero);
static gboolean tableroLleno(const gchar *tablero);

const gint combinacionesGanadorasHorizontales[3][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}};
const gint combinacionesGanadorasVerticales[3][3] = {{0, 3, 6}, {1, 4, 7}, {2, 5, 8}};
const gint combinacionesGanadorasDiagonales[2][3] = {{0, 4, 8}, {2, 4, 6}};
/*
* Función que verifica todas las opciones de victoria posibles en el tablero y determina un ganador o empate
* @author   Perla Hernández Cardoso
* @param    *tablero        Arreglo que representa el tablero de juego.
* @return   estadoJuego
*/
estadoJuego obtenerEstadoDelJuego(const gchar *tablero)
{
    estadoJuego estado = JUGANDO;
    gint resultado;

    resultado = revisarGanador(combinacionesGanadorasHorizontales, 3, 'x', tablero);
    if (resultado > -1)
    {
        return GANO_X;
    }
    resultado = revisarGanador(combinacionesGanadorasVerticales, 3, 'x', tablero);
    if (resultado > -1)
    {
        return GANO_X;
    }
    resultado = revisarGanador(combinacionesGanadorasDiagonales, 2, 'x', tablero);
    if (resultado > -1)
    {
        return GANO_X;
    }
    resultado = revisarGanador(combinacionesGanadorasHorizontales, 3, 'o', tablero);
    if (resultado > -1)
    {
        return GANO_O;
    }
    resultado = revisarGanador(combinacionesGanadorasVerticales, 3, 'o', tablero);
    if (resultado > -1)
    {
        return GANO_O;
    }
    resultado = revisarGanador(combinacionesGanadorasDiagonales, 2, 'o', tablero);
    if (resultado > -1)
    {
        return GANO_O;
    }

    if (tableroLleno(tablero))
    {
        estado = EMPATE;
    }

    return estado;
}
/*
* Función que ingresa en la estructura del historial una entrada nueva con el tablero, su id, el turno y el estado de juego actual. Si la entrada a registrar
* no se quiere ingresa al final de la lista, se borran todas las entradas que están después del nuevo registro para que sea el nuevo final de la lista
* También actualiza el tamaño total del historial
* @author   Areli Rabner Concha
* @param    **historial         Apuntador a la estructura del historial.
* @param    tablero             Arreglo que representa el tablero de juego.
* @param    *idTablero          ID con el que se guardará el tablero en el historial.
* @param    *tamanoHistorial    Entero con el numero de entradas en el historial.
* @param    turno               Boolean que controla el turno de los jugadores.
* @param    estado              Estado actual del juego.
* @return   void
*/
void registrarHistorial(struct nodoHistorial **historial, gchar tablero[9], int *idTablero, int *tamanoHistorial, gboolean turno, estadoJuego estado)
{
    // Si el historial está vacio se crea la primer entrada.
    if ((*historial) == NULL)
    {
        (*historial) = (struct nodoHistorial *)malloc(sizeof(struct nodoHistorial));
        (*historial)->id = 1;
        (*historial)->anterior = NULL;
        (*historial)->siguiente = NULL;
        (*historial)->turno = turno;
        (*historial)->estado = JUGANDO;
        memcpy((*historial)->tablero, tablero, sizeof(gchar) * 9);
        (*idTablero) = (*idTablero) + 1;
        (*tamanoHistorial) = (*tamanoHistorial) + 1;
        return;
    }

    struct nodoHistorial *temporal = (*historial);
    struct nodoHistorial *temporalDos = NULL, *temporalEliminar = NULL;
    if ((*idTablero) == (*tamanoHistorial))
    {
        // Sección que ingresa una entrada al final de la estructura.
        while (temporal)
        {
            // Si llegamos al final de la lista se ingresa una nueva entrada con los datos correspondientes
            if (temporal->siguiente == NULL)
            {
                temporalDos = (struct nodoHistorial *)malloc(sizeof(struct nodoHistorial));
                temporalDos->id = temporal->id + 1;
                memcpy(temporalDos->tablero, tablero, sizeof(gchar) * 9);
                temporalDos->anterior = temporal;
                temporalDos->siguiente = NULL;
                temporalDos->turno = turno;
                temporalDos->estado = estado;
                temporal->siguiente = temporalDos;
                break;
            }
            else
            {
                temporal = temporal->siguiente;
            }
        }
        (*idTablero) = (*idTablero) + 1;
        (*tamanoHistorial) = (*tamanoHistorial) + 1;
    }
    // Si se quiere ingresar una entrada en alguna posición que no es el final, se borran las entradas que siguen después a la nueva entrada.
    else if ((*idTablero) < (*tamanoHistorial))
    {
        while (temporal)
        {
            if (temporal->siguiente == NULL)
            {
                //Se recorre el historial desde el final hasta la entrada donde se quiere ingresar el nuevo registro, eliminando las entradas necesarias para que 
                //el nuevo registro sea el último en la lista
                do
                {
                    if (temporal->id == (*idTablero))
                    {
                        break;
                    }
                    else
                    {
                        temporalEliminar = temporal;
                        temporal = temporal->anterior;
                        temporal->siguiente = NULL;
                        free(temporalEliminar);
                    }
                } while (temporal);
                temporalDos = (struct nodoHistorial *)malloc(sizeof(struct nodoHistorial));
                temporalDos->id = temporal->id + 1;
                memcpy(temporalDos->tablero, tablero, sizeof(gchar) * 9);
                temporalDos->anterior = temporal;
                temporalDos->siguiente = NULL;
                temporalDos->turno = turno;
                temporalDos->estado = estado;
                temporal->siguiente = temporalDos;
                break;
            }
            else
            {
                temporal = temporal->siguiente;
            }
        }
        (*idTablero) = (*idTablero) + 1;
        (*tamanoHistorial) = (*idTablero);
    }
}
/*
* Función que se encarga de cargar en los variables correspondientes una entrada del historial
* @author   Perla Hernández Cardoso
* @param    *historial      Apuntador a la estructura del historial.
* @param    *tablero[9]     Arreglo que representa el tablero de juego.
* @param    turno           Boolean que controla el turno de los jugadores.
* @param    estado          Estado actual del juego.
* @param    idTablero       ID con el que se guardará el tablero en el historial.
* @return   void
*/
void recuperarTablero(struct nodoHistorial *historial, gchar (*tablero)[9], gboolean *turno, estadoJuego *estado, const int idTablero)
{
    struct nodoHistorial *temporal = historial;
    if (historial == NULL)
    {
        return;
    }
    while (temporal)
    {
        if (temporal->id == idTablero)
        {
            for (int i = 0; i < 9; i++)
            {
                (*tablero)[i] = temporal->tablero[i];
            }
            (*turno) = temporal->turno;
            (*estado) = temporal->estado;
            break;
        }
        else
        {
            temporal = temporal->siguiente;
        }
    }
}
/*
* Función que limpia todas las entradas del historial excepto la primera (tablero vacio) y actualiza el tamaño del historial.
* @author   Areli Rabner Concha
* @param    **historial         Apuntador a la estructura del historial.
* @param    *idTablero          ID con el que se guardará el tablero en el historial.
* @param    *tamanoHistorial    Entero con el numero de entradas en el historial.
* @return   void
*/
void limpiarHistorial(struct nodoHistorial **historial, int *idTablero, int *tamanoHistorial)
{
    struct nodoHistorial *temporal = (*historial), *temporalEliminar = NULL;
    if ((*tamanoHistorial) != 1)
    {
        while (temporal)
        {
            if (temporal->siguiente == NULL)
            {
                do
                {
                    if (temporal->id == 1)
                    {
                        break;
                    }
                    else
                    {
                        temporalEliminar = temporal;
                        temporal = temporal->anterior;
                        temporal->siguiente = NULL;
                        free(temporalEliminar);
                    }
                } while (temporal);
                break;
            }
            else
            {
                temporal = temporal->siguiente;
            }
        }
        (*idTablero) = 1;
        (*tamanoHistorial) = 1;
    }
}
/*
* Función que genera un tiro para jugar contra la computadora
* @author   Perla Hernández Cardoso
* @param    tablero             Arreglo que representa el tablero de juego.
* @return   gint
*/
gint generarTiro(gchar tablero[9])
{
    gint numero = 0;
    srand(time(NULL));
    do
    {
        numero = 1 + rand() % (10-1);
    } while (tablero[numero] != '-');
    return numero;
}
/*
* Función que revisa si el caracter recibido se encuentra en un estado ganador en el tablero
* @author   Perla Hernández Cardoso
* @param    combinacionesGanadoras[][3]     Arreglo de arreglos que contiene las combinaciones posibles de victoria.
* @param    tamano                          Numero de arreglos almacenados en combinacionesGanadoras.
* @param    jugadorProbado                  Caracter que representa al ganador que se está evaluando.
* @param    *tablero                        Arreglo que representa el tablero de juego.
* @return   gint
*/
static gint revisarGanador(const gint combinacionesGanadoras[][3], const gint tamano, const gchar jugadorProbado, const char *tablero)
{
    gint i, j;
    gint contador = 0;

    for (i = 0; i < tamano; i++)
    {
        contador = 0;
        for (j = 0; j < 3; j++)
        {
            if (jugadorProbado == tablero[combinacionesGanadoras[i][j]])
            {
                contador++;
                if (contador == 3)
                {
                    return i;
                }
            }
        }
    }

    return -1;
}
/*
* Función que revisa si el tablero se encuentra lleno.
* @author   Perla Hernández Cardoso
* @param    *tablero             Arreglo que representa el tablero de juego.
* @return   gboolean
*/
static gboolean tableroLleno(const gchar *tablero)
{
    int i;

    for (i = 0; i < 9; i++)
    {
        if (tablero[i] == '-')
        {
            return FALSE;
        }
    }

    return TRUE;
}