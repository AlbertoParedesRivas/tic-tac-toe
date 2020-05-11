/*
* @file control.c
*
* @brief Este archivo contiene las funciones que interactuan directamente
* con la vista del programa del juego de gato. Se obtienen los nombres de
* los participantes y se puede jugar solo o con otra persona, tambien se puede
* guardar la partida en un archivo asi como recuperarla.
* 
* @authors  Perla Hernández Cardoso
*           Areli Rabner Concha
* @date 27/04/2020
*/

#include <gtk/gtk.h>
#include "../modelo/modelo.h"

typedef struct
{
    GtkWidget *imagenes[9];
    GtkWidget *labelEstado;
    GtkWidget *ventanaInicial;
    GtkWidget *ventanaNombres;
    GtkWidget *ventanaNombre;
    GtkWidget *campoJugadorX;
    GtkWidget *campoJugadorXC;
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

/*
* En la función main se inicializan las vistas y las estructuras que guardan información
* que se recopila en la interfaz y el historial.
* @author   Perla Hernández Cardoso
* @param    *argv[]    Arreglo de cadenas donde se guardan los argumentos del programa
* @param    argc       Variable que contiene el numero de argumentos del programa
* @return   int
*/
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
    componentes->ventanaNombre = GTK_WIDGET(gtk_builder_get_object(constructor, "ventanaNombre"));
    componentes->campoJugadorX = GTK_WIDGET(gtk_builder_get_object(constructor, "campoJugadorX"));
    componentes->campoJugadorXC = GTK_WIDGET(gtk_builder_get_object(constructor, "campoJugadorXC"));
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

/*
* Esta funcion se ejecuta cuando se da clic en los botones del tablero del juego de gato para registrar un turno.
* Se registra el turno en la variable de tablero, se actualiza la estructura de historial y se actualiza la interfaz
* con el turno registrado. También se encuentra el bloque de código que genera el turno automático cuando solo está
* jugando una persona.
* @author   Areli Rabner Concha
* @param    *boton          Apuntador al objeto boton que fue presionado en la interfaz
* @param    *componentes    Estructura que contiene las variables principales del programa
* @return   void
*/
void botonPresionado(GtkButton *boton, componentesApp *componentes)
{
    gint numeroBoton;
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
        componentes->estadoDelJuego = obtenerEstadoDelJuego(componentes->tablero);
        registrarHistorial(&componentes->historial, componentes->tablero, &componentes->idTablero, &componentes->tamanoHistorial, componentes->turno, componentes->estadoDelJuego);
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
        // Seccion que genera el turno de la computadora
        if (componentes->estadoDelJuego == JUGANDO)
        {
            if (componentes->computadora)
            {
                componentes->textoEstado = g_strconcat(texto, componentes->turno ? componentes->jugadorX : componentes->jugadorO, NULL);
                gtk_label_set_text(GTK_LABEL(componentes->labelEstado), componentes->textoEstado);
                numeroBoton = generarTiro(componentes->tablero);
                componentes->tablero[numeroBoton] = componentes->turno ? 'o' : 'x';
                componentes->turno = !componentes->turno;
                componentes->estadoDelJuego = obtenerEstadoDelJuego(componentes->tablero);
                registrarHistorial(&componentes->historial, componentes->tablero, &componentes->idTablero, &componentes->tamanoHistorial, componentes->turno, componentes->estadoDelJuego);
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
        mostrarTablero(componentes);
    }
}
/*
* Función que se ejecuta al momento de que el usuario elige un nuevo juego contra otra persona,
* se oculta la ventana inicial y se muestra la ventana donde se capturan los nombres de las personas
* que jugarán.
* @author   Perla Hernández Cardoso
* @param    *boton       Apuntador al objeto boton que fue presionado en la interfaz
* @param    *componentes Estructura que contiene las variables principales del programa
* @return   void
*/
void nuevoJuegoPersona(GtkButton *boton, componentesApp *componentes)
{
    componentes->computadora = FALSE;
    gtk_widget_hide(componentes->ventanaInicial);
    gtk_widget_show(componentes->ventanaNombres);
}
/*
* Función que se ejecuta al momento de que el usuario elige un nuevo juego contra la computadora,
* se oculta la ventana inicial y se muestra la ventana donde se captura el nombre de la persona
* que jugará
* @author   Areli Rabner Concha
* @param    *boton       Apuntador al objeto boton que fue presionado en la interfaz
* @param    *componentes Estructura que contiene las variables principales del programa
* @return   void
*/
void nuevoJuegoComputadora(GtkButton *boton, componentesApp *componentes)
{
    gchar computadora[] = "computadora";
    componentes->computadora = TRUE;
    componentes->jugadorO = g_strdup(computadora);
    gtk_widget_hide(componentes->ventanaInicial);
    gtk_widget_show(componentes->ventanaNombre);
}
/*
* Función que recupera los nombres de los campos de la interfaz para guardarlos en la
* estructura que guarda los datos del programa. Después inicia un nuevo juego.
* @author   Perla Hernández Cardoso
* @param    *boton       Apuntador al objeto boton que fue presionado en la interfaz
* @param    *componentes Estructura que contiene las variables principales del programa
* @return   void
*/
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
/*
* Función que recupera el nombre del campo de la interfaz para guardarlo en la
* estructura que guarda los datos del programa. Después inicia un nuevo juego.
* @author   Areli Rabner Concha
* @param    *boton       Apuntador al objeto boton que fue presionado en la interfaz
* @param    *componentes Estructura que contiene las variables principales del programa
* @return   void
*/
void guardarNombre(GtkButton *boton, componentesApp *componentes)
{
    if (gtk_entry_get_text_length(GTK_ENTRY(componentes->campoJugadorXC)) > 0)
    {
        componentes->jugadorX = g_strdup(gtk_entry_get_text(GTK_ENTRY(componentes->campoJugadorXC)));
        gtk_widget_hide(componentes->ventanaNombre);
        nuevoJuego(componentes);
    }
}
/*
* Función que inicializa el tablero y los componentes de la interfaz para iniciar un
* nuevo juego.
* @author   Perla Hernández Cardoso
* @param    *componentes Estructura que contiene las variables principales del programa
* @return   void
*/
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
/*
* Función que muestra en pantalla el tablero activo en el historial
* @author   Areli Rabner Concha
* @param    *componentes Estructura que contiene las variables principales del programa
* @return   void
*/
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
/*
* Función que se ejecuta para mostrar jugadas anteriores del historial de partida.
* @author   Perla Hernández Cardoso
* @param    *boton       Apuntador al objeto boton que fue presionado en la interfaz
* @param    *componentes Estructura que contiene las variables principales del programa
* @return   void
*/
void mostrarJugadaAnterior(GtkButton *boton, componentesApp *componentes)
{
    const gchar texto[] = "Turno de ";
    const gchar textoGanador[] = "Gano ";
    if (componentes->idTablero != 1)
    {
        // Sección de código que se encarga de regresar dos jugadas en el historial si se está jugando contra la computadora 
        if (componentes->computadora)
        {
            if (componentes->idTablero == componentes->tamanoHistorial)
            {
                if ((componentes->tamanoHistorial % 2) == 0)
                {
                    componentes->idTablero--;
                }
                else
                {
                    componentes->idTablero -= 2;
                }
            }
            else
            {
                componentes->idTablero -= 2;
            }
        }
        else
        {
            componentes->idTablero--;
        }
        recuperarTablero(componentes->historial, &componentes->tablero, &componentes->turno, &componentes->estadoDelJuego, componentes->idTablero);
        //Sección que actualiza el label de estado en la interfaz
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
/*
* Función que se ejecuta para mostrar jugadas siguientes del historial de partida.
* @author   Areli Rabner Concha
* @param    *boton       Apuntador al objeto boton que fue presionado en la interfaz
* @param    *componentes Estructura que contiene las variables principales del programa
* @return   void
*/
void mostrarJugadaSiguiente(GtkButton *boton, componentesApp *componentes)
{
    const gchar texto[] = "Turno de ";
    const gchar textoGanador[] = "Gano ";
    if (componentes->idTablero < componentes->tamanoHistorial)
    {
        // Sección de código que se encarga de regresar dos jugadas en el historial si se está jugando contra la computadora 
        if (componentes->computadora)
        {
            if (componentes->idTablero == componentes->tamanoHistorial - 1)
            {
                componentes->idTablero++;
            }
            else
            {
                componentes->idTablero += 2;
            }
        }
        else
        {
            componentes->idTablero++;
        }
        //Sección que actualiza el label de estado en la interfaz
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
/*
* Función que se ejecuta para terminar el proceso de gtk al momento de que se cierra la ventana principal.
* @author   Perla Hernández Cardoso
* @return   void
*/
void on_window_main_destroy()
{
    gtk_main_quit();
}
/*
* Función que abre la ventana que muestra la vista de acerca de.
* @author   Areli Rabner Concha
* @return   void
*/
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
/*
* Función que abre la ventana que muestra la vista de como jugar.
* @author   Areli Rabner Concha
* @return   void
*/
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
/*
* Función que muestra la ventana inicial al iniciar el programa.
* @author   Perla Hernández Cardoso
* @param    *window         Apuntador al objeto ventana en la que se generó el evento
* @param      *widget         Apuntador al objeto widget al que se le hizo focus
* @param    *componentes    Estructura que contiene las variables principales del programa
* @return   void
*/
void mostrarVentanaInicial(GtkWindow *window, GtkWidget *widget, componentesApp *componentes)
{
    if (componentes->inicial)
    {
        gtk_widget_show(componentes->ventanaInicial);
        componentes->inicial = FALSE;
    }
}
/*
* Función que muetra el selector de archivo y recupera la dirección del archivo que contiene la partida
* @author   Areli Rabner Concha
* @param    *boton       Apuntador al objeto boton que fue presionado en la interfaz
* @param    *componentes Estructura que contiene las variables principales del programa
* @return   void
*/
void mostrarCargarPartida(GtkMenuItem *boton, componentesApp *componentes)
{
    gchar *nombreArchivo = NULL; // Name of file to open from dialog box

    gtk_widget_show(componentes->ventanaCargar);

    if (gtk_dialog_run(GTK_DIALOG(componentes->ventanaCargar)) == GTK_RESPONSE_OK)
    {
        nombreArchivo = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(componentes->ventanaCargar));
        if (nombreArchivo != NULL)
        {
            cargarPartida(componentes, nombreArchivo);
        }
        g_free(nombreArchivo);
    }

    gtk_widget_hide(componentes->ventanaCargar);
}
/*
* Función que muetra el selector de archivo y recupera la dirección del archivo nuevo para guardar la partida
* @author   Perla Hernández Cardoso
* @param    *boton       Apuntador al objeto boton que fue presionado en la interfaz
* @param    *componentes Estructura que contiene las variables principales del programa
* @return   void
*/
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
        }
        g_free(nombreArchivo);
    }

    gtk_widget_hide(componentes->ventanaGuardar);
}
/*
* Función que guarda los datos de la partida en un archivo
* @author   Perla Hernández Cardoso
* @param    *componentes Estructura que contiene las variables principales del programa
* @param    *gchar       Apuntador a la cadena de la ruta del archivo
* @return   void
*/
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
/*
* Función que carga los datos de la partida de un archivo
* @author   Areli Rabner Concha
* @param    *componentes Estructura que contiene las variables principales del programa
* @param    *gchar       Apuntador a la cadena de la ruta del archivo
* @return   void
*/
void cargarPartida(componentesApp *componentes, gchar *direccionArchivo)
{
    FILE *archivo;
    char computadora[] = "computadora\0";
    archivo = fopen(direccionArchivo, "r");
    char buffer[128], tablero[9], numero[2];
    int contador = 0;
    limpiarHistorial(&componentes->historial, &componentes->idTablero, &componentes->tamanoHistorial);
    // Ciclo que recupera una linea del archivo de guardado y almacena los datos en sus respectivas variables
    while (fgets(buffer, sizeof(buffer), archivo))
    {
        buffer[strlen(buffer) - 1] = '\0';
        if (contador == 0)
        {
            componentes->jugadorX = g_strdup(buffer);
        }
        else if (contador == 1)
        {
            componentes->jugadorO = g_strdup(buffer);
            if (g_strcmp0(computadora, componentes->jugadorO) == 0)
            {
                componentes->computadora = TRUE;
            }
            else
            {
                componentes->computadora = FALSE;
            }
            
            
        }
        else if (contador == 2)
        {
            numero[0] = buffer[0];
            numero[1] = '\0';
            componentes->turno = atoi(numero);
        }
        else if (contador == 3)
        {
            numero[0] = buffer[0];
            numero[1] = '\0';
            componentes->estadoDelJuego = atoi(numero);
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
    //Muestra la última jugada del historial y determina el estado del juego.
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
    fclose(archivo);
}