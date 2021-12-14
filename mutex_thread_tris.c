/**
 * Realizzazione di una versione elettronica del famoso gioco "Tris" tramite 
 * l'uso di thread.
 * 
 * Il programma prevede che il main thread:
 * - inizializzi il campo da gioco costituito da una matrice 3x3 di caratteri;
 * - crei due thread "Giocatore_1" e "Giocatore_2";
 * - assegni il turno ai giocatori che saranno sincronizzati tramite mutex
 *   (sbloccando quindi il mutex di ciascun giocatore).
 * 
 * Inoltre, ogni volta che un giocatore fa la sua mossa, il main thread:
 * - stampa a video il campo da gioco
 * - controlla se qualcuno ha vinto e, in caso affermativo, stampa un messaggio
 *   sul video, cancella i thread, rimuove i mutex e termina.
 * 
 * Ogni thread secondario:
 * - richiede all'utente di inserire la colonna e la riga da spuntare.
 * - assegna alla matrice il valore 'X' o 'O' corrispondente
 * - attende il successivo turno.
 * 
 * Suggerimenti:
 * Passare come argomento ad ogni thread un intero per indicare al thread se si
 * riferisce al giocatore 1 o 2.
 * 
 * Il main thread per capire quale thread sbloccare pu√≤ utilizzare una variabile turno.
 * 
 * Utilizzare 3 mutex per sincronizzare i tre tread (un mutex per ogni thread).
 * Il flusso di esecuzione deve essere il seguente:
 * - Main thread
 * - Thread 1
 * - Main thread
 * - Thread 2
 * - Main thread
 * - Thread 1
 * - Main thread
 * ...
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

void *giocatori(void *args);
void stampa_campo();
int controlla_vittoria();

char campo[3][3];

pthread_mutex_t mut_mt;
pthread_mutex_t mut_g1;
pthread_mutex_t mut_g2;

int main() {
    pthread_t g1, g2;
    int time_to_exit = 0, turno = 1, vitt;
    int r, c;

    /* Inizializzazione dei mutex */
    pthread_mutex_init(&mut_mt, NULL);
    pthread_mutex_init(&mut_g1, NULL);
    pthread_mutex_init(&mut_g2, NULL);
    
    /* lock dei mutex dei giocatori in modo che non partano se non sbloccati
     * dal main thread */
    pthread_mutex_lock(&mut_g1);
    pthread_mutex_lock(&mut_g2);
    
    /* Inizializzazione campo con carattere space (' ') che indica che la
     * casella √® vuota */
    for(r = 0; r < 3; r++) {
        for(c = 0; c < 3; c++) {
            campo[r][c] = ' ';
        }
    }
    
    /* Creazione dei due thread */
    pthread_create(&g1, NULL, giocatori, (void *)1);
    pthread_create(&g2, NULL, giocatori, (void *)2);
    
    /* Inizio flusso di gioco del main thread */
    while(!time_to_exit) {
        pthread_mutex_lock(&mut_mt);
        
        stampa_campo();
        
        /* la funzione controlla vittoria restituisce:
         * -1 se la partita deve continuare,
         * 0 in caso di parit√  (partita finita),
         * 1 e 2 vittoria di un giocatore (partita finita
         */
        vitt = controlla_vittoria();
        if(vitt == -1) {
            if(turno == 1) {
                /* Sblocca il thread del giocatore 1 */
                pthread_mutex_unlock(&mut_g1);
                turno = 2;
            } else {
                /* Sblocca il thread del giocatore 2 */
                pthread_mutex_unlock(&mut_g2);
                turno = 1;
            }
        } else if(vitt == 0) {
            printf("Parita'\n");
            time_to_exit = 1;
        } else {
            printf("Ha vinto il Giocatore %d\n", vitt);
            time_to_exit = 1;
        }
    }
    
    /* L'unlock dei due mutex serve a far si che possano essere cancellati */
    pthread_mutex_unlock(&mut_g1);
    pthread_mutex_unlock(&mut_g2);
    
    /* Cancellazione e attesa di terminazione dei thread */
    pthread_cancel(g1);
    pthread_join(g1, NULL);
    pthread_cancel(g2);
    pthread_join(g2, NULL);
    
    /* Distruzione dei mutex */
    pthread_mutex_destroy(&mut_mt);
    pthread_mutex_destroy(&mut_g1);
    pthread_mutex_destroy(&mut_g2);
    
    printf("Bye!\n");
}

int controlla_vittoria() {
    int i, j;
    
    /* Controlla righe */
    for(i=0; i<3; i++) {
        if(campo[i][0] == campo[i][1] && campo[i][0] == campo[i][2] && campo[i][0] != ' ') {
            if(campo[i][0] == 'X')
                return 1;
            else
                return 2;
        }
    }
    
    /* Controlla colonne */
    for(i=0; i<3; i++) {
        if(campo[0][i] == campo[1][i] && campo[0][i] == campo[2][i] && campo[0][i] != ' ') {
            if(campo[0][i] == 'X')
                return 1;
            else
                return 2;
        }
    }
    
    /* Controlla diagonali */
    if(campo[0][0] == campo[1][1] && campo[0][0] == campo[2][2] && campo[0][0] != ' ') {
        if(campo[0][0] == 'X')
            return 1;
        else
            return 2;
    }
    
    if(campo[0][2] == campo[1][1] && campo[0][2] == campo[2][0] && campo[0][2] != ' ') {
        if(campo[0][2] == 'X')
            return 1;
        else
            return 2;
    }
    
    /* Controlla che NON vi sia parit√  */
    for(i=0; i<3; i++) {
        for(j=0; j<3; j++) {
            if(campo[i][j] == ' ')
                return -1;
        }
    }
    
    /* Se si arriva qui allora √® parit√  */
    return 0;
    
    
}

void stampa_campo() {
    printf("%c | %c | %c\n", campo[0][0], campo[0][1], campo[0][2]);
    printf("---------\n");
    printf("%c | %c | %c\n", campo[1][0], campo[1][1], campo[1][2]);
    printf("---------\n");
    printf("%c | %c | %c\n", campo[2][0], campo[2][1], campo[2][2]);
}

void *giocatori(void *args) {
    int player = (int)args;
    
    /* Assegnazione ad ogni giocaore di un simbolo e mutex diverso */
    char simbolo = 'X';
    int riga, colonna;
    pthread_mutex_t *mut = &mut_g1;
    
    if(player == 2) {
        simbolo = 'O';
        mut = &mut_g2;
    }
    
    while(1) {
        pthread_mutex_lock(mut);
        
        printf("Inserisci riga: ");
        scanf("%d%*c", &riga);
        printf("Inserisci colonna: ");
        scanf("%d%*c", &colonna);
        
        campo[riga][colonna] = simbolo;
        
        /* Sblocca il mutex del main thread in modo da passargli il turno */
        pthread_mutex_unlock(&mut_mt);
    }
}
