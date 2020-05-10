#ifndef MODELO_H
#define MODELO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

estadoJuego obtenerEstadoDelJuego(const gchar *, gint *);
void registrarHistorial(struct nodoHistorial **, gchar[9], int *, int *, gboolean, estadoJuego);
void limpiarHistorial(struct nodoHistorial **, int *, int *);
void mostrarHistorial(struct nodoHistorial *);
void recuperarTablero(struct nodoHistorial *, gchar (*)[9], gboolean *, estadoJuego *, int);

#endif // MODELO_H