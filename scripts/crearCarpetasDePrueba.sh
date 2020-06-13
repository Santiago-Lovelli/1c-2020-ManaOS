cd /home/utnso/Escritorio;mkdir tall-grass;
cd tall-grass;
mkdir Metadata;
mkdir Files;
mkdir Blocks;
cd Metadata;echo "BLOCK_SIZE=10
BLOCKS=5192
MAGIC_NUMBER=TALL_GRASS
" >> Metadata.bin;
echo "" >> Bitmap.bin;
cd ../Files;
mkdir PIKACHU;
cd PIKACHU;
echo "DIRECTORY=N
SIZE=250
BLOCKS=[40,21,82,3]
OPEN=N
" >> Metadata.bin;
echo "Lista las carpetas base"