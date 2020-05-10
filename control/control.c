#include <gtk/gtk.h>
#include "../modelo/modelo.h"

typedef struct
{
    GtkWidget *imagenes[9];
    GtkWidget *labelEstado;
    GtkWidget *ventanaInicial;
    GtkWidget *ventanaNombres;
    GtkWidget *campoJugadorX;
    GtkWidget *campoJugadorO;
    GtkWidget *ventanaCargar;
    GtkWidget *ventanaGuardar;
    gboolean turno;
    gboolean computadora;
    gboolean inicial;
    gchar *textoEstado;
    gchar *jugadorX;
    gchar *jugadorO;
    gchar tablero[9];
    estadoJuego estadoDelJuego;
    struct nodoHistorial *historial;
    int idTablero;
    int tamanoHistorial;
} componentesApp;

void mostrarVentanaInicial();
void mostrarAcercaDe();
void mostrarComoJugar();
void mostrarCampoNombreVsJugador();
void nuevoJuego(componentesApp *);
void mostrarTablero(componentesApp *);
void guardarPartida(componentesApp *, gchar *);
void cargarPartida(componentesApp *, gchar *);

int main(int argc, char *argv[])
{
    GtkBuilder *constructor;
    GtkWidget *ventana;
    componentesApp *componentes = g_slice_new(componentesApp);
    gchar nombreImagenes[] = "imagen1";

    componentes->turno = FALSE;
    componentes->computadora = FALSE;
    componentes->estadoDelJuego = DETENIDO;
    componentes->inicial = TRUE;
    gtk_init(&argc, &argv);

    constructor = gtk_builder_new_from_file("vista/tablero.ui");
    ventana = GTK_WIDGET(gtk_builder_get_object(constructor, "TicTacToe"));

    for (gint i = 0; i < 9; i++)
    {
        nombreImagenes[6] = i + '1';
        componentes->imagenes[i] = GTK_WIDGET(gtk_builder_get_object(constructor, nombreImagenes));
        componentes->tablero[i] = '-';
    }
    componentes->idTablero = 0;
    componentes->tamanoHistorial = 0;
    componentes->labelEstado = GTK_WIDGET(gtk_builder_get_object(constructor, "estadoJuego"));
    componentes->ventanaInicial = GTK_WIDGET(gtk_builder_get_object(constructor, "VentanaInicial"));
    componentes->ventanaNombres = GTK_WIDGET(gtk_builder_get_object(constructor, "NombresDosPersonas"));
    componentes->campoJugadorX = GTK_WIDGET(gtk_builder_get_object(constructor, "campoJugadorX"));
    componentes->campoJugadorO = GTK_WIDGET(gtk_builder_get_object(constructor, "campoJugadorO"));
    componentes->ventanaCargar = GTK_WIDGET(gtk_builder_get_object(constructor, "cargarArchivo"));
    componentes->ventanaGuardar = GTK_WIDGET(gtk_builder_get_object(constructor, "guardarArchivo"));
    registrarHistorial(&componentes->historial, componentes->tablero, &componentes->idTablero, &componentes->tamanoHistorial, componentes->turno, componentes->estadoDelJuego);

    gtk_builder_connect_signals(constructor, componentes);
    g_object_unref(G_OBJECT(constructor));

    gtk_widget_show(ventana);
    gtk_main();
    g_slice_free(componentesApp, componentes);

    return 0;
}

void botonPresionado(GtkButton *boton, componentesApp *componentes)
{
    gint numeroBoton;
    gint bloquesGanadores[3] = {0};
    const gchar texto[] = "Turno de ";
    const gchar textoGanador[] = "Gano ";
    if (componentes->estadoDelJuego != JUGANDO)
    {
        return;
    }

    numeroBoton = gtk_widget_get_name(GTK_WIDGET(boton))[0] - '0';
    if (componentes->tablero[numeroBoton] == '-')
    {
        // Se actualiza el estado del juego indicando a quien le toca tirar
        componentes->textoEstado = g_strconcat(texto, componentes->turno ? componentes->jugadorX : componentes->jugadorO, NULL);
        gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
        // Se actualiza el en la variable que lo maneja
        componentes->tablero[numeroBoton] = componentes->turno ? 'o' : 'x';
        // Se registra el tablero actualizado en el historial y se muestra el tablero actualizado
        componentes->turno = !componentes->turno;
        componentes->estadoDelJuego = obtenerEstadoDelJuego(componentes->tablero, bloquesGanadores);
        registrarHistorial(&componentes->historial, componentes->tablero, &componentes->idTablero, &componentes->tamanoHistorial, componentes->turno, componentes->estadoDelJuego);
        mostrarTablero(componentes);
        // Se obtiene el estado del juego y se muestra en la interfaz si alguien gano, perdio o si fue empate

        if (componentes->estadoDelJuego == GANO_O)
        {
            componentes->textoEstado = g_strconcat(textoGanador, componentes->jugadorO, NULL);
            gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
        }
        else if (componentes->estadoDelJuego == GANO_X)
        {
            componentes->textoEstado = g_strconcat(textoGanador, componentes->jugadorX, NULL);
            gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
        }
        else if (componentes->estadoDelJuego == EMPATE)
        {
            gtk_label_set_text(GTK_LABEL(componentes->labelEstado), "¡El juego fue un empate!");
        }
    }
}

void nuevoJuegoPersona(GtkButton *boton, componentesApp *componentes)
{
    componentes->computadora = FALSE;
    gtk_widget_hide(componentes->ventanaInicial);
    gtk_widget_show(componentes->ventanaNombres);
}

void guardarNombres(GtkButton *boton, componentesApp *componentes)
{
    if (gtk_entry_get_text_length(GTK_ENTRY(componentes->campoJugadorX)) > 0 && gtk_entry_get_text_length(GTK_ENTRY(componentes->campoJugadorO)) > 0)
    {
        componentes->jugadorX = g_strdup(gtk_entry_get_text(GTK_ENTRY(componentes->campoJugadorX)));
        componentes->jugadorO = g_strdup(gtk_entry_get_text(GTK_ENTRY(componentes->campoJugadorO)));
        gtk_widget_hide(componentes->ventanaNombres);
        nuevoJuego(componentes);
    }
}

void nuevoJuego(componentesApp *componentes)
{
    const gchar cadena[] = "Turno de ";
    for (int i = 0; i < 9; i++)
    {
        componentes->tablero[i] = '-';
    }
    for (int i = 0; i < 9; i++)
    {
        gtk_image_set_from_file(GTK_IMAGE(componentes->imagenes[i]), "img/fondoBlanco.png");
    }
    componentes->turno = FALSE;
    componentes->textoEstado = g_strconcat(cadena, componentes->jugadorX, NULL);
    gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
    limpiarHistorial(&componentes->historial, &componentes->idTablero, &componentes->tamanoHistorial);
    componentes->estadoDelJuego = JUGANDO;
}

void mostrarTablero(componentesApp *componentes)
{
    struct nodoHistorial *historial = componentes->historial;
    int idTablero = componentes->idTablero;
    if (!historial)
    {
        printf("Historial vacio\n");
    }
    while (historial)
    {
        if (historial->id == idTablero)
        {
            for (int i = 0; i < 9; i++)
            {
                if (historial->tablero[i] == 'x')
                {
                    gtk_image_set_from_file(GTK_IMAGE(componentes->imagenes[i]), "img/x.png");
                }
                if (historial->tablero[i] == 'o')
                {
                    gtk_image_set_from_file(GTK_IMAGE(componentes->imagenes[i]), "img/o.png");
                }
                if (historial->tablero[i] == '-')
                {
                    gtk_image_set_from_file(GTK_IMAGE(componentes->imagenes[i]), "img/fondoBlanco.png");
                }
            }
            break;
        }
        else
        {
            historial = historial->siguiente;
        }
    }
}

void mostrarJugadaAnterior(GtkButton *boton, componentesApp *componentes)
{
    const gchar texto[] = "Turno de ";
    const gchar textoGanador[] = "Gano ";
    if (componentes->idTablero != 1)
    {
        componentes->idTablero--;
        recuperarTablero(componentes->historial, &componentes->tablero, &componentes->turno, &componentes->estadoDelJuego, componentes->idTablero);
        componentes->textoEstado = g_strconcat(texto, componentes->turno ? componentes->jugadorO : componentes->jugadorX, NULL);
        gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
        if (componentes->estadoDelJuego == GANO_O)
        {
            componentes->textoEstado = g_strconcat(textoGanador, componentes->jugadorO, NULL);
            gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
        }
        else if (componentes->estadoDelJuego == GANO_X)
        {
            componentes->textoEstado = g_strconcat(textoGanador, componentes->jugadorX, NULL);
            gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
        }
        else if (componentes->estadoDelJuego == EMPATE)
        {
            gtk_label_set_text(GTK_LABEL(componentes->labelEstado), "¡El juego fue un empate!");
        }
    }
    mostrarTablero(componentes);
}

void mostrarJugadaSiguiente(GtkButton *boton, componentesApp *componentes)
{
    const gchar texto[] = "Turno de ";
    const gchar textoGanador[] = "Gano ";
    if (componentes->idTablero != componentes->tamanoHistorial)
    {
        componentes->idTablero++;
        recuperarTablero(componentes->historial, &componentes->tablero, &componentes->turno, &componentes->estadoDelJuego, componentes->idTablero);
        componentes->textoEstado = g_strconcat(texto, componentes->turno ? componentes->jugadorO : componentes->jugadorX, NULL);
        gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
        if (componentes->estadoDelJuego == GANO_O)
        {
            componentes->textoEstado = g_strconcat(textoGanador, componentes->jugadorO, NULL);
            gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
        }
        else if (componentes->estadoDelJuego == GANO_X)
        {
            componentes->textoEstado = g_strconcat(textoGanador, componentes->jugadorX, NULL);
            gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
        }
        else if (componentes->estadoDelJuego == EMPATE)
        {
            gtk_label_set_text(GTK_LABEL(componentes->labelEstado), "¡El juego fue un empate!");
        }
    }
    mostrarTablero(componentes);
}

void on_window_main_destroy()
{
    gtk_main_quit();
}

void mostrarAcercaDe()
{
    GtkBuilder *constructor;
    GtkWidget *ventanaAcercaDe = NULL;

    constructor = gtk_builder_new();
    gtk_builder_add_from_file(constructor, "vista/acercaDe.ui", NULL);

    ventanaAcercaDe = GTK_WIDGET(gtk_builder_get_object(constructor, "acercaDe"));

    g_object_unref(G_OBJECT(constructor));

    gtk_window_set_modal(GTK_WINDOW(ventanaAcercaDe), TRUE);
    gtk_widget_show_all(ventanaAcercaDe);
}

void mostrarComoJugar()
{
    GtkBuilder *constructor;
    GtkWidget *ventanaComoJugar = NULL;

    constructor = gtk_builder_new();
    gtk_builder_add_from_file(constructor, "vista/comoJugar.ui", NULL);

    ventanaComoJugar = GTK_WIDGET(gtk_builder_get_object(constructor, "comoJugar"));

    g_object_unref(G_OBJECT(constructor));

    gtk_window_set_modal(GTK_WINDOW(ventanaComoJugar), TRUE);
    gtk_widget_show_all(ventanaComoJugar);
}

void mostrarVentanaInicial(GtkWindow *window, GtkWidget *widget, componentesApp *componentes)
{
    if (componentes->inicial)
    {
        gtk_widget_show(componentes->ventanaInicial);
        componentes->inicial = FALSE;
    }
}

void mostrarCampoNombreVsJugador()
{
    GtkBuilder *constructor;
    GtkWidget *ventanaCampoNombresVsPersona = NULL;

    constructor = gtk_builder_new();
    gtk_builder_add_from_file(constructor, "vista/ventanaCampoNombresVsPersona.ui", NULL);

    ventanaCampoNombresVsPersona = GTK_WIDGET(gtk_builder_get_object(constructor, "ventanaCampoNombresVsPersona"));

    g_object_unref(G_OBJECT(constructor));

    gtk_window_set_modal(GTK_WINDOW(ventanaCampoNombresVsPersona), TRUE);
    gtk_widget_show_all(ventanaCampoNombresVsPersona);
}

void mostrarCargarPartida(GtkMenuItem *boton, componentesApp *componentes)
{
    gchar *nombreArchivo = NULL; // Name of file to open from dialog box

    gtk_widget_show(componentes->ventanaCargar);

    if (gtk_dialog_run(GTK_DIALOG(componentes->ventanaCargar)) == GTK_RESPONSE_OK)
    {
        nombreArchivo = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(componentes->ventanaCargar));
        if (nombreArchivo != NULL)
        {
            printf("%s\n", nombreArchivo);
            cargarPartida(componentes, nombreArchivo);
        }
        g_free(nombreArchivo);
    }

    gtk_widget_hide(componentes->ventanaCargar);
}

void mostrarGuardarPartida(GtkMenuItem *boton, componentesApp *componentes)
{
    gchar *nombreArchivo = NULL; // Name of file to open from dialog box

    gtk_widget_show(componentes->ventanaGuardar);

    if (gtk_dialog_run(GTK_DIALOG(componentes->ventanaGuardar)) == GTK_RESPONSE_OK)
    {
        nombreArchivo = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(componentes->ventanaGuardar));
        if (nombreArchivo != NULL)
        {
            guardarPartida(componentes, nombreArchivo);
            printf("%s\n", nombreArchivo);
        }
        g_free(nombreArchivo);
    }

    gtk_widget_hide(componentes->ventanaGuardar);
}

void guardarPartida(componentesApp *componentes, gchar *direccionArchivo)
{
    FILE *guardado;
    struct nodoHistorial *temporal = componentes->historial;
    guardado = fopen(direccionArchivo, "w");
    if (guardado == NULL)
    {
        printf("Error guardando\n");
        return;
    }

    fprintf(guardado, "%s\n", componentes->jugadorX);
    fprintf(guardado, "%s\n", componentes->jugadorO);

    while (temporal)
    {
        if (temporal->id != 1)
        {
            fprintf(guardado, "%d\n", temporal->turno);
            fprintf(guardado, "%d\n", (int)temporal->estado);
            for (int i = 0; i < 9; i++)
            {
                fprintf(guardado, "%c", temporal->tablero[i]);
            }
            fprintf(guardado, "\n");
            temporal = temporal->siguiente;
        }
        else
        {
            temporal = temporal->siguiente;
        }
    }

    fclose(guardado);
}

void cargarPartida(componentesApp *componentes, gchar *direccionArchivo)
{
    FILE *archivo;
    archivo = fopen(direccionArchivo, "r");
    char buffer[128], jugadorX[50], jugadorO[50], tablero[9], numero[2];
    int contador = 0;
    limpiarHistorial(&componentes->historial, &componentes->idTablero, &componentes->tamanoHistorial);
    while (fgets(buffer, sizeof(buffer), archivo))
    {
        buffer[strlen(buffer) - 1] = '\0';
        if (contador == 0)
        {
            strcpy(componentes->jugadorX, buffer);
        }
        else if (contador == 1)
        {
            strcpy(componentes->jugadorO, buffer);
        }
        else if (contador == 2)
        {
            numero[0] = buffer[0];
            numero[1] = '\0';
            // printf("2:%d\n", atoi(numero));
            componentes->turno = atoi(numero);
        }
        else if (contador == 3)
        {
            numero[0] = buffer[0];
            numero[1] = '\0';
            componentes->estadoDelJuego = atoi(numero);
            // printf("3:%d\n", atoi(numero));
        }
        else if (contador == 4)
        {
            for (int k = 0; k < 9; k++)
            {
                tablero[k] = buffer[k];
            }
            registrarHistorial(&componentes->historial, tablero, &componentes->idTablero, &componentes->tamanoHistorial, componentes->turno, componentes->estadoDelJuego);
        }
        if (contador >= 4)
        {
            contador = 2;
        }
        else
        {
            contador++;
        }
    }
    const gchar texto[] = "Turno de ";
    const gchar textoGanador[] = "Gano ";
    recuperarTablero(componentes->historial, &componentes->tablero, &componentes->turno, &componentes->estadoDelJuego, componentes->idTablero);
    componentes->textoEstado = g_strconcat(texto, componentes->turno ? componentes->jugadorO : componentes->jugadorX, NULL);
    gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
    if (componentes->estadoDelJuego == GANO_O)
    {
        componentes->textoEstado = g_strconcat(textoGanador, componentes->jugadorO, NULL);
        gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
    }
    else if (componentes->estadoDelJuego == GANO_X)
    {
        componentes->textoEstado = g_strconcat(textoGanador, componentes->jugadorX, NULL);
        gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
    }
    else if (componentes->estadoDelJuego == EMPATE)
    {
        gtk_label_set_text(GTK_LABEL(componentes->labelEstado), "¡El juego fue un empate!");
    }
    mostrarTablero(componentes);
    // printf("1:%s 2:%s", jugadorX, jugadorO);
    fclose(archivo);
}