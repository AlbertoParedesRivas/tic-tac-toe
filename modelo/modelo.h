/*
* @file modelo.h
*
* @brief Archivo que contiene los prototipos de las funciones en modelo.c que
* se utilizan en control.c
* 
* @authors  Perla Hernández Cardoso
*           Areli Rabner Concha
* @date 27/04/2020
*/
#ifndef MODELO_H
#define MODELO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum
{
    DETENIDO,
    JUGANDO,
    GANO_O,
    GANO_X,
    EMPATE
} estadoJuego;
struct nodoHistorial
{
    gint id;
    gchar tablero[9];
    gboolean turno;
    estadoJuego estado;
    struct nodoHistorial *anterior;
    struct nodoHistorial *siguiente;
};

estadoJuego obtenerEstadoDelJuego(const gchar *);
void registrarHistorial(struct nodoHistorial **, gchar[9], int *, int *, gboolean, estadoJuego);
void limpiarHistorial(struct nodoHistorial **, int *, int *);
void recuperarTablero(struct nodoHistorial *, gchar (*)[9], gboolean *, estadoJuego *, int);
gint generarTiro(gchar[9]);

#endif // MODELO_H