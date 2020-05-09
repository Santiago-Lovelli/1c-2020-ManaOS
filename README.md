# tp-2020-1c-ManaOS-

## Compilacio: 
### GameCard (29-04): 
gcc -L"/home/utnso/workspace/tp-2020-1c-ManaOS-/ComunParaTodos/Debug" -o "GameCard"  ./src/GameCard.o   -lComunParaTodos -lpthread -lcommons

### ComunParaTodos (29-04):
gcc -shared -o "libComunParaTodos.so"  ./Time/Time.o  ./Serializacion/Serializa
ion.o  ./ManejoDePunterosDobles/ManejoDePunterosDobles.o  ./Logger/Logger.o  ./Lista/lista.o  ./Conexiones/Conexiones.o  ./Bitmap/Bitmap.o  ./Archivos/archivos.o
