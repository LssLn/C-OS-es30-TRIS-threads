# C-OS-es30-TRIS-threads
Realizzazione di una versione elettronica del famoso gioco "Tris" tramite l'uso di thread.

Il programma prevede che il main thread: 
  inizializzi il campo da gioco costituito da una matrice 3x3 di caratteri; 
  crei due thread "Giocatore_1" e "Giocatore_2";
  assegni il turno ai giocatori che saranno sincronizzati tramite mutex (sbloccando quindi il mutex di ciascun giocatore).

Inoltre, ogni volta che un giocatore fa la sua mossa, il main thread:
   - stampa a video il campo da gioco
   - controlla se qualcuno ha vinto e, in caso affermativo, stampa un messaggio  sul video, cancella i thread, rimuove i mutex e termina.

Ogni thread secondario:
  - richiede all'utente di inserire la colonna e la riga da spuntare.
  - assegna alla matrice il valore 'X' o 'O' corrispondente
  - attende il successivo turno.
