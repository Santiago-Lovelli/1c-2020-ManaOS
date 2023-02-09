# tp-2020-1c-ManaOS-
```
                                  ,'\
    _.----.        ____         ,'  _\   ___    ___     ____
_,-'       `.     |    |  /`.   \,-'    |   \  /   |   |    \  |`.
\      __    \    '-.  | /   `.  ___    |    \/    |   '-.   \ |  |
 \.    \ \   |  __  |  |/    ,','_  `.  |          | __  |    \|  |
   \    \/   /,' _`.|      ,' / / / /   |          ,' _`.|     |  |
    \     ,-'/  /   \    ,'   | \/ / ,`.|         /  /   \  |     |
     \    \ |   \_/  |   `-.  \    `'  /|  |    ||   \_/  | |\    |
      \    \ \      /       `-.`.___,-' |  |\  /| \      /  | |   |
       \    \ `.__,'|  |`-._    `|      |__| \/ |  `.__,'|  | |   |
        \_.-'       |__|    `-._ |              '-.|     '-.| |   |
                                `'                            '-._|
```
## Compilacio: 
### GameCard (26-05): 
gcc -L"/home/utnso/workspace/tp-2020-1c-ManaOS-/ComunParaTodos/Debug" -o "GameCard"  ./src/GameCard.o   -lComunParaTodos -lpthread -lcommons

por las dudas:
gcc -I"/home/utnso/workspace/tp-2020-1c-ManaOS-/ComunParaTodos" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/GameCard.d" -MT"src/GameCard.o" -o "src/GameCard.o" "../src/GameCard.c"

### ComunParaTodos (29-04):
gcc -shared -o "libComunParaTodos.so"  ./Time/Time.o  ./Serializacion/Serializa
ion.o  ./ManejoDePunterosDobles/ManejoDePunterosDobles.o  ./Logger/Logger.o  ./Lista/lista.o  ./Conexiones/Conexiones.o  ./Bitmap/Bitmap.o  ./Archivos/archivos.o

### Pruebas: 
casos de pruebas