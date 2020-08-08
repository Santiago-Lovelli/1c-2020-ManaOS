
echo " 
::: Comienzo compilacion ComunParaTodos ::: 
"
cd /home/utnso/workspace/tp-2020-1c-ManaOS-/ComunParaTodos/
mkdir Debug
cd Debug

mkdir Archivos
mkdir Bitmap
mkdir Conexiones
mkdir Lista
mkdir Logger
mkdir ManejoDePunterosDobles
mkdir Serializacion
mkdir Time

gcc -Icommons -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"Time/Time.d" -MT"Time/Time.o" -o "Time/Time.o" "../Time/Time.c"

gcc -Icommons -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"Serializacion/Serializacion.d" -MT"Serializacion/Serializacion.o" -o "Serializacion/Serializacion.o" "../Serializacion/Serializacion.c"

gcc -Icommons -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"ManejoDePunterosDobles/ManejoDePunterosDobles.d" -MT"ManejoDePunterosDobles/ManejoDePunterosDobles.o" -o "ManejoDePunterosDobles/ManejoDePunterosDobles.o" "../ManejoDePunterosDobles/ManejoDePunterosDobles.c"

gcc -Icommons -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"Logger/Logger.d" -MT"Logger/Logger.o" -o "Logger/Logger.o" "../Logger/Logger.c"

gcc -Icommons -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"Lista/lista.d" -MT"Lista/lista.o" -o "Lista/lista.o" "../Lista/lista.c"

gcc -Icommons -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"Conexiones/Conexiones.d" -MT"Conexiones/Conexiones.o" -o "Conexiones/Conexiones.o" "../Conexiones/Conexiones.c"

gcc -Icommons -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"Bitmap/Bitmap.d" -MT"Bitmap/Bitmap.o" -o "Bitmap/Bitmap.o" "../Bitmap/Bitmap.c"

gcc -Icommons -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"Archivos/archivos.d" -MT"Archivos/archivos.o" -o "Archivos/archivos.o" "../Archivos/archivos.c"

gcc -shared -o "libComunParaTodos.so"  ./Time/Time.o  ./Serializacion/Serializacion.o  ./ManejoDePunterosDobles/ManejoDePunterosDobles.o  ./Logger/Logger.o  ./Lista/lista.o  ./Conexiones/Conexiones.o  ./Bitmap/Bitmap.o  ./Archivos/archivos.o   -lcommons

echo "
::: Termine compilacion ComunParaTodos ::: 
" 