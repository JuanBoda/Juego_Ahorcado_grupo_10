#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

// ================================================================================
//                                  CONSTANTES
// ================================================================================

#define ARCHIVO_PALABRAS "palabras.txt" // para el archivo de palabras
#define MAX_PALABRAS 150                // para el número máximo de palabras
#define MAX_PALABRA 20                  // para la longitud máxima de cada palabra

// ================================================================================
//                                  COLORES ANSI
// ================================================================================

#define COLOR_RESET "\033[0m"        // Resetear color
#define COLOR_ROJO "\033[31m"        // Rojo para errores
#define COLOR_VERDE "\033[32m"       // Verde para éxito
#define COLOR_AMARILLO "\033[33m"    // Amarillo para advertencias
#define COLOR_AZUL "\033[34m"        // Azul para información
#define COLOR_CYAN "\033[36m"        // Cyan para menús
#define COLOR_MARRON "\033[38;5;94m" // Marrón para ahorcado
#define COLOR_BLANCO "\033[37m"      // Blanco para texto normal

// ================================================================================
//                                  EMOJIS
// ================================================================================

#define EMOJI_SALTO "😀"
#define EMOJI_PERDER1 "😢"
#define EMOJI_PERDER2 "😓"
#define EMOJI_PERDER3 "😐"
#define EMOJI_VICTORIA "🎉"
#define EMOJI_DERROTA "💀"
#define EMOJI_CORRECTO "✅"
#define EMOJI_ERROR "❌"
#define EMOJI_JUEGO "🎮"
#define EMOJI_PALABRA "📝"
#define EMOJI_LETRA "🔤"
#define EMOJI_INTENTOS "⚡"
#define EMOJI_MENU "📋"
#define EMOJI_ESTADIST "📊"

// ================================================================================
//                                  PROTOTIPOS
// ================================================================================

void utf_8();                                                                         // para que caracteres especiales se muestren correctamente
void dibujar_ahorcado(int errores);                                                   // para dibujar el ahorcado
int cargar_palabras_archivo(char banco_palabras[][MAX_PALABRA], int *total_palabras); // para cargar las palabras del archivo
void mostrar_palabra(char palabra[], char adivinadas[]);                              // para mostrar la palabra
void mostrar_instrucciones(int total_palabras);                                       // para mostrar las instrucciones
void jugar_partida(char banco_palabras[][MAX_PALABRA], int total_palabras);           // para manejar una partida completa
void victoria(char palabra[]);                                                        // para mostrar la victoria
void derrota(char palabra[]);                                                         // para mostrar la derrota

// ================================================================================
//                                  MAIN
// ================================================================================

int main()
{
    utf_8();           // para que caracteres especiales se muestren correctamente
    srand(time(NULL)); // Inicializar generador de números aleatorios (una sola vez)

    // Variables locales para la eleccion de palabras.
    char banco_palabras[MAX_PALABRAS][MAX_PALABRA] = {0}; // Inicializar array de palabras
    int total_palabras = 0;                               // Contador local

    // Cargar palabras al inicio del programa
    int resultado_carga = cargar_palabras_archivo(banco_palabras, &total_palabras);
    if (resultado_carga != 0)
    {
        printf("\n%s%s Error: No se pudieron cargar palabras del archivo '%s'%s\n", COLOR_ROJO, EMOJI_ERROR, ARCHIVO_PALABRAS, COLOR_RESET);
        printf("%sAsegúrate de que el archivo existe en el mismo directorio y contiene palabras válidas.%s\n", COLOR_AMARILLO, COLOR_RESET);
        printf("\n%sPresiona Enter para salir...%s", COLOR_AMARILLO, COLOR_RESET);
        getchar();
        return 1; // Código de error
    }

    int opcion; // para la opción del menú

    do
    {
        system("cls");
        printf("%s%s====== MENÚ PRINCIPAL ======%s\n", COLOR_CYAN, EMOJI_MENU, COLOR_RESET);
        printf(" %s1. %s Jugar%s\n", COLOR_BLANCO, EMOJI_JUEGO, COLOR_RESET);
        printf(" %s2. %s Instrucciones%s\n", COLOR_BLANCO, EMOJI_PALABRA, COLOR_RESET);
        printf(" %s3. %s Salir%s\n", COLOR_BLANCO, EMOJI_DERROTA, COLOR_RESET);
        printf("%s==============================%s\n", COLOR_CYAN, COLOR_RESET);
        printf("\n%sElige una opción: %s", COLOR_AMARILLO, COLOR_RESET);
        scanf("%d", &opcion);
        getchar(); // limpiar buffer, para que no se quede esperando una tecla.

        switch (opcion)
        {
        case 1:
            jugar_partida(banco_palabras, total_palabras);
            break;
        case 2:
            system("cls");
            mostrar_instrucciones(total_palabras);
            printf("\n%sPresiona una tecla para volver al menú...%s", COLOR_AMARILLO, COLOR_RESET);
            getchar();
            break;
        case 3:
            printf("\n%s%s ¡Saliendo... Hasta pronto!%s\n\n", COLOR_VERDE, EMOJI_VICTORIA, COLOR_RESET);
            Sleep(1000);
            break;
        default:
            printf("%s%s Opción inválida. Intenta de nuevo.%s\n", COLOR_ROJO, EMOJI_ERROR, COLOR_RESET);
            Sleep(1000);
        }

    } while (opcion != 3);

    return 0;
}

// ================================================================================
//                                  FUNCIONES
// ================================================================================

void utf_8() // para que caracteres especiales se muestren correctamente
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
}

void mostrar_instrucciones(int total_palabras) // para mostrar las instrucciones
{
    printf("%s====================== %s INSTRUCCIONES ======================%s\n\n", COLOR_CYAN, EMOJI_PALABRA, COLOR_RESET);
    printf("%s•%s El objetivo es adivinar la palabra secreta letra por letra%s\n", COLOR_AZUL, COLOR_BLANCO, COLOR_RESET);
    printf("%s•%s Tienes 6 intentos para adivinar%s\n", COLOR_AZUL, COLOR_BLANCO, COLOR_RESET);
    printf("%s•%s Por cada letra incorrecta, se dibuja una parte del ahorcado%s\n", COLOR_AZUL, COLOR_BLANCO, COLOR_RESET);
    printf("%s•%s Ganas si adivinas la palabra antes de completar el dibujo%s\n", COLOR_AZUL, COLOR_BLANCO, COLOR_RESET);
    printf("%s•%s Se cargan %d palabras del archivo '%s'%s\n", COLOR_AZUL, COLOR_BLANCO, total_palabras, ARCHIVO_PALABRAS, COLOR_RESET);
    printf("%s\n==============================================================%s\n\n", COLOR_CYAN, COLOR_RESET);
}

void jugar_partida(char banco_palabras[][MAX_PALABRA], int total_palabras) // para manejar una partida completa
{
    int indice = rand() % total_palabras; // Generar índice aleatorio
    char *palabra = banco_palabras[indice];

    char adivinadas[50] = {0};      // para almacenar las letras adivinadas
    char letra;                     // para la letra ingresada
    int errores = 0;                // para contar los errores
    int longitud = strlen(palabra); // para la longitud de la palabra
    int aciertos = 0;

    while (errores < 6 && aciertos < longitud)
    {
        system("cls");
        printf("%s===== %s JUEGO DEL AHORCADO =====%s\n\n", COLOR_CYAN, EMOJI_JUEGO, COLOR_RESET);

        dibujar_ahorcado(errores);
        printf("\n");
        printf("%s%s Palabra: %s", COLOR_CYAN, EMOJI_PALABRA, COLOR_RESET);
        mostrar_palabra(palabra, adivinadas);
        printf("\n");
        printf("\n%s%s Intentos restantes: %s%d%s\n", COLOR_AMARILLO, EMOJI_INTENTOS, COLOR_VERDE, 6 - errores, COLOR_RESET);

        printf("\n%s%s Ingresa una letra: %s", COLOR_BLANCO, EMOJI_LETRA, COLOR_RESET);
        scanf(" %c", &letra);
        letra = toupper(letra);

        if (strchr(adivinadas, letra))
        {
            printf("\n%s%s ¡Ya probaste esa letra!%s\n\n", COLOR_ROJO, EMOJI_ERROR, COLOR_RESET);
            Sleep(1000);
            continue;
        }

        int len = strlen(adivinadas);
        adivinadas[len] = letra;
        adivinadas[len + 1] = '\0';

        int encontrada = 0;
        for (int i = 0; i < longitud; i++)
        {
            if (palabra[i] == letra)
            {
                encontrada = 1;
                aciertos++;
            }
        }

        if (encontrada)
        {
            printf("\n%s%s ¡Correcto!%s", COLOR_VERDE, EMOJI_CORRECTO, COLOR_RESET);
        }
        else
        {
            printf("\n%s%s ¡Incorrecto!%s", COLOR_ROJO, EMOJI_ERROR, COLOR_RESET);
            errores++;
        }

        Sleep(1000);
    }

    system("cls");
    printf("%s===== %s JUEGO DEL AHORCADO =====%s\n\n", COLOR_CYAN, EMOJI_JUEGO, COLOR_RESET);
    dibujar_ahorcado(errores);
    printf("\n");
    printf("%s%s Palabra: %s", COLOR_CYAN, EMOJI_PALABRA, COLOR_RESET);
    mostrar_palabra(palabra, adivinadas);
    printf("\n\n");

    if (aciertos == longitud)
    {
        printf("%s%s ¡GANASTE! %s%s\n", COLOR_VERDE, EMOJI_VICTORIA, EMOJI_VICTORIA, COLOR_RESET);
        Sleep(1000);
        victoria(palabra);
    }
    else
    {
        printf("%s%s ¡PERDISTE! %s%s\n", COLOR_ROJO, EMOJI_DERROTA, EMOJI_DERROTA, COLOR_RESET);
        printf("\n%sLa palabra era: %s%s%s\n", COLOR_AMARILLO, COLOR_VERDE, palabra, COLOR_RESET);
        Sleep(1000);
        derrota(palabra);
    }
    printf("\n%sPresiona una tecla para volver al menú...%s", COLOR_AMARILLO, COLOR_RESET);
    getchar(); // para consumir el Enter anterior
    getchar(); // espera tecla
}

void dibujar_ahorcado(int errores) // para dibujar el ahorcado con marco ASCII doble
{
    printf("\n\n%s", COLOR_BLANCO); // Color marrón para el ahorcado

    // Base del ahorcado (siempre visible)
    printf("   ╔═══╗\n");
    printf("   ║   ║\n");

    switch (errores)
    {
    case 0:
        // Ahorcado vacío
        printf("   ║      \n");
        printf("   ║      \n");
        printf("   ║      \n");
        printf("   ║       \n");
        break;

    case 1:
        // Cabeza
        printf("   ║  😐  \n");
        printf("   ║      \n");
        printf("   ║      \n");
        printf("   ║       \n");
        break;

    case 2:
        // Cabeza + cuello
        printf("   ║  😐  \n");
        printf("   ║   |  \n");
        printf("   ║      \n");
        printf("   ║       \n");
        break;

    case 3:
        // Cabeza + cuello + brazo izquierdo
        printf("   ║  😐  \n");
        printf("   ║  /|  \n");
        printf("   ║      \n");
        printf("   ║       \n");
        break;

    case 4:
        // Cabeza + cuello + brazos
        printf("   ║  😐  \n");
        printf("   ║  /|\\ \n");
        printf("   ║      \n");
        printf("   ║       \n");
        break;

    case 5:
        // Cabeza + cuello + brazos + pierna izquierda
        printf("   ║  😐  \n");
        printf("   ║  /|\\ \n");
        printf("   ║  /   \n");
        printf("   ║       \n");
        break;

    case 6:
        // Ahorcado completo
        printf("   ║  😐  \n");
        printf("   ║  /|\\ \n");
        printf("   ║  / \\ \n");
        printf("   ║       \n");
        break;
    }

    // Base del ahorcado
    printf("   ║       \n");
    printf("%s═══╩═══════════════════════════%s\n", COLOR_VERDE, COLOR_RESET);
    printf("%s", COLOR_RESET); // Resetear color
}

int cargar_palabras_archivo(char banco_palabras[][MAX_PALABRA], int *total_palabras) // para cargar las palabras del archivo
{
    if (banco_palabras == NULL || total_palabras == NULL)
    {
        return -1; // Error: punteros nulos
    }

    FILE *archivo = fopen(ARCHIVO_PALABRAS, "r");
    if (archivo == NULL)
    {
        return -1; // Error: no se pudo abrir el archivo
    }

    *total_palabras = 0;
    char palabra_temp[MAX_PALABRA];

    // Leer palabras línea por línea de forma más segura
    while (fgets(palabra_temp, sizeof(palabra_temp), archivo) && *total_palabras < MAX_PALABRAS)
    {
        // Eliminar el salto de línea si existe
        size_t len = strlen(palabra_temp);
        if (len > 0 && palabra_temp[len - 1] == '\n')
        {
            palabra_temp[len - 1] = '\0';
        }

        // Verificar que la palabra no esté vacía
        if (strlen(palabra_temp) > 0)
        {
            // Copiar la palabra al banco
            strncpy(banco_palabras[*total_palabras], palabra_temp, MAX_PALABRA - 1);
            banco_palabras[*total_palabras][MAX_PALABRA - 1] = '\0';

            // Convertir a mayúsculas
            for (int i = 0; banco_palabras[*total_palabras][i]; i++)
            {
                banco_palabras[*total_palabras][i] = toupper(banco_palabras[*total_palabras][i]);
            }
            (*total_palabras)++;
        }
    }

    fclose(archivo);

    if (*total_palabras > 0)
    {
        printf("%s%s Se cargaron %s%d%s palabras del archivo '%s'%s\n", COLOR_VERDE, EMOJI_CORRECTO, COLOR_AMARILLO, *total_palabras, COLOR_VERDE,
               ARCHIVO_PALABRAS, COLOR_RESET);
        return 0; // Éxito
    }
    else
    {
        return -1; // Error: no se cargaron palabras
    }
}

void mostrar_palabra(char palabra[], char adivinadas[]) // para mostrar la palabra
{
    int longitud = strlen(palabra);

    printf("%s", COLOR_BLANCO); // Color blanco para las letras adivinadas
    for (int i = 0; i < longitud; i++)
    {
        if (strchr(adivinadas, palabra[i]))
        {
            printf("%c ", palabra[i]);
        }
        else
        {
            printf("_ ");
        }
    }
    printf("%s", COLOR_RESET); // Resetear color
}

void victoria(char palabra[]) // para mostrar la victoria con marco ASCII doble
{
    int contador = 0;
    while (contador < 8) // While para generar el efecto de la victoria
    {
        system("cls");
        printf("%s===== %s JUEGO DEL AHORCADO =====%s\n", COLOR_CYAN, EMOJI_JUEGO, COLOR_RESET);
        printf("\n       %s%s Victoria %s%s\n", EMOJI_VICTORIA, EMOJI_VICTORIA, EMOJI_VICTORIA, EMOJI_VICTORIA);
        printf("\n   ╔═══╗             ");
        printf("\n   ║   ║         ");
        printf("\n   ║             ");
        printf("\n   ║          😀 ");
        printf("\n   ║          /|\\ ");
        printf("\n   ║          / \\ ");
        printf("\n%s═══╩═══════════════════════════%s\n\n", COLOR_VERDE, COLOR_RESET);
        Sleep(100);

        system("cls");
        printf("%s===== %s JUEGO DEL AHORCADO =====%s\n", COLOR_CYAN, EMOJI_JUEGO, COLOR_RESET);
        printf("\n       %s%s Victoria %s%s\n", EMOJI_VICTORIA, EMOJI_VICTORIA, EMOJI_VICTORIA, EMOJI_VICTORIA);
        printf("\n   ╔═══╗             ");
        printf("\n   ║   ║          ");
        printf("\n   ║              ");
        printf("\n   ║            ");
        printf("\n   ║          😀 ");
        printf("\n   ║          /|\\ ");
        printf("\n%s═══╩═══════════════════════════%s\n\n", COLOR_VERDE, COLOR_RESET);
        Sleep(100);

        system("cls");
        printf("%s===== %s JUEGO DEL AHORCADO =====%s\n", COLOR_CYAN, EMOJI_JUEGO, COLOR_RESET);
        printf("\n       %s%s Victoria %s%s\n", EMOJI_VICTORIA, EMOJI_VICTORIA, EMOJI_VICTORIA, EMOJI_VICTORIA);
        printf("\n   ╔═══╗              ");
        printf("\n   ║   ║           ");
        printf("\n   ║               ");
        printf("\n   ║         \\😀/");
        printf("\n   ║           | ");
        printf("\n   ║          / \\ ");
        printf("\n%s═══╩═══════════════════════════%s\n\n", COLOR_VERDE, COLOR_RESET);
        Sleep(100);

        system("cls");
        printf("%s===== %s JUEGO DEL AHORCADO =====%s\n", COLOR_CYAN, EMOJI_JUEGO, COLOR_RESET);
        printf("\n       %s%s Victoria %s%s\n", EMOJI_VICTORIA, EMOJI_VICTORIA, EMOJI_VICTORIA, EMOJI_VICTORIA);
        printf("\n   ╔═══╗              ");
        printf("\n   ║   ║           ");
        printf("\n   ║         |😀|");
        printf("\n   ║           | ");
        printf("\n   ║          / \\ ");
        printf("\n   ║               ");
        printf("\n%s═══╩═══════════════════════════%s\n\n", COLOR_VERDE, COLOR_RESET);
        Sleep(100);

        system("cls");
        printf("%s===== %s JUEGO DEL AHORCADO =====%s\n", COLOR_CYAN, EMOJI_JUEGO, COLOR_RESET);
        printf("\n       %s%s Victoria %s%s\n", EMOJI_VICTORIA, EMOJI_VICTORIA, EMOJI_VICTORIA, EMOJI_VICTORIA);
        printf("\n   ╔═══╗              ");
        printf("\n   ║   ║           ");
        printf("\n   ║               ");
        printf("\n   ║         \\😀/");
        printf("\n   ║           | ");
        printf("\n   ║          / \\ ");
        printf("\n%s═══╩═══════════════════════════%s\n\n", COLOR_VERDE, COLOR_RESET);
        Sleep(100);

        system("cls");
        printf("%s===== %s JUEGO DEL AHORCADO =====%s\n", COLOR_CYAN, EMOJI_JUEGO, COLOR_RESET);
        printf("\n       %s%s Victoria %s%s\n", EMOJI_VICTORIA, EMOJI_VICTORIA, EMOJI_VICTORIA, EMOJI_VICTORIA);
        printf("\n   ╔═══╗              ");
        printf("\n   ║   ║           ");
        printf("\n   ║               ");
        printf("\n   ║          😀 ");
        printf("\n   ║          /|\\ ");
        printf("\n   ║          / \\ ");
        printf("\n%s═══╩═══════════════════════════%s\n\n", COLOR_VERDE, COLOR_RESET);
        contador++;
    }
    printf("%sLa palabra era: %s%s%s\n", COLOR_BLANCO, COLOR_VERDE, palabra, COLOR_RESET);
}

void derrota(char palabra[]) // para mostrar la derrota con marco ASCII doble
{
    int contador = 0;
    while (contador < 4) // While para generar el efecto de la derrota
    {
        system("cls");
        printf("%s===== %s JUEGO DEL AHORCADO =====%s\n", COLOR_CYAN, EMOJI_JUEGO, COLOR_RESET);
        printf("\n       %s%s Perdiste %s%s\n", EMOJI_DERROTA, EMOJI_DERROTA, EMOJI_DERROTA, EMOJI_DERROTA);
        printf("\n   ╔═══╗           ");
        printf("\n   ║   ║           ");
        printf("\n   ║ |😓|          ");
        printf("\n   ║   |           ");
        printf("\n   ║  / \\         ");
        printf("\n   ║               ");
        printf("\n%s═══╩═══════════════════════════%s\n\n", COLOR_VERDE, COLOR_RESET);
        Sleep(250);

        system("cls");
        printf("%s===== %s JUEGO DEL AHORCADO =====%s\n", COLOR_CYAN, EMOJI_JUEGO, COLOR_RESET);
        printf("\n       %s%s Perdiste %s%s\n", EMOJI_DERROTA, EMOJI_DERROTA, EMOJI_DERROTA, EMOJI_DERROTA);
        printf("\n   ╔═══╗           ");
        printf("\n   ║   ║           ");
        printf("\n   ║  😥           ");
        printf("\n   ║  /|\\         ");
        printf("\n   ║  / \\         ");
        printf("\n   ║               ");
        printf("\n%s═══╩═══════════════════════════%s\n\n", COLOR_VERDE, COLOR_RESET);
        Sleep(250);

        system("cls");
        printf("%s===== %s JUEGO DEL AHORCADO =====%s\n", COLOR_CYAN, EMOJI_JUEGO, COLOR_RESET);
        printf("\n       %s%s Perdiste %s%s\n", EMOJI_DERROTA, EMOJI_DERROTA, EMOJI_DERROTA, EMOJI_DERROTA);
        printf("\n   ╔═══╗           ");
        printf("\n   ║   ║           ");
        printf("\n   ║ |😓|          ");
        printf("\n   ║   |           ");
        printf("\n   ║  / \\         ");
        printf("\n   ║               ");
        printf("\n%s═══╩═══════════════════════════%s\n\n", COLOR_VERDE, COLOR_RESET);
        Sleep(250);

        system("cls");
        printf("%s===== %s JUEGO DEL AHORCADO =====%s\n", COLOR_CYAN, EMOJI_JUEGO, COLOR_RESET);
        printf("\n       %s%s Perdiste %s%s\n", EMOJI_DERROTA, EMOJI_DERROTA, EMOJI_DERROTA, EMOJI_DERROTA);
        printf("\n   ╔═══╗           ");
        printf("\n   ║   ║           ");
        printf("\n   ║  😥           ");
        printf("\n   ║  /|\\         ");
        printf("\n   ║  / \\         ");
        printf("\n   ║               ");
        printf("\n%s═══╩═══════════════════════════%s\n\n", COLOR_VERDE, COLOR_RESET);
        Sleep(250);

        system("cls");
        printf("%s===== %s JUEGO DEL AHORCADO =====%s\n", COLOR_CYAN, EMOJI_JUEGO, COLOR_RESET);
        printf("\n       %s%s Perdiste %s%s\n", EMOJI_DERROTA, EMOJI_DERROTA, EMOJI_DERROTA, EMOJI_DERROTA);
        printf("\n   ╔═══╗           ");
        printf("\n   ║   ║           ");
        printf("\n   ║ |😓|          ");
        printf("\n   ║   |           ");
        printf("\n   ║  / \\         ");
        printf("\n   ║               ");
        printf("\n%s═══╩═══════════════════════════%s\n\n", COLOR_VERDE, COLOR_RESET);
        Sleep(250);

        system("cls");
        printf("%s===== %s JUEGO DEL AHORCADO =====%s\n", COLOR_CYAN, EMOJI_JUEGO, COLOR_RESET);
        printf("\n       %s%s Perdiste %s%s\n", EMOJI_DERROTA, EMOJI_DERROTA, EMOJI_DERROTA, EMOJI_DERROTA);
        printf("\n   ╔═══╗           ");
        printf("\n   ║   ║           ");
        printf("\n   ║  😥           ");
        printf("\n   ║  /|\\         ");
        printf("\n   ║  / \\         ");
        printf("\n   ║               ");
        printf("\n%s═══╩═══════════════════════════%s\n\n", COLOR_VERDE, COLOR_RESET);
        Sleep(250);
        contador++;
    }
    printf("%sLa palabra era: %s%s%s\n", COLOR_BLANCO, COLOR_VERDE, palabra, COLOR_RESET);
}
