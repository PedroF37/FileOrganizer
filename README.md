# FileOrganizer
## Projeto em C de organizador de arquivos por extensão

Projeto é chamado: `$./fo <diretório>`
#
Projeto organiza os arquivos que têm extensão em pastas nomeadas com a extensão.

Exemplo, arquivos .txt para a pasta Txt, .pdf para pasta Pdf etc

Projeto faz a recursão em pastas
#
Projeto usa as bibliotecas:
* `libfileutils` -> https://github.com/PedroF37/FileUtils
* `libdirutils` -> https://github.com/PedroF37/DirUtils
* `libstrutils` -> https://github.com/PedroF37/StrUtils
#
Para compilar: `$ clang -Wall -Wextra --pedantic -lfileutils -ldirutils -lstrutils -o fo fo.c`
