# Um dia no museu

Este é um jogo envolvendo diversos temas como RPG,puzzle,mistério e ação e referenciando o ponto turístico de Pernambuco o Instituto Ricardo Brennand,também conhecido como Castelo de Brennand, o jogo foi desenvolvido em C usando a biblioteca [Raylib](https://www.raylib.com/). O jogo pode ser executado diretamente no Notepad++ for raylib utilizando a tecla **F6**.

## Pré-requisitos
### Opção 1. Notepad++ for raylib
1. Instale a biblioteca Raylib.
   - [Guia de instalação da Raylib](https://www.raylib.com/)
2. Tenha o compilador GCC instalado.
   - [MinGW para Windows](http://www.mingw.org/) ou [GCC para Linux](https://gcc.gnu.org/)
3. Quando o Raylib estiver instalado, um aplicativo chamado "Notepad++ for raylib" será instalado automaticamente. Use essa IDE para rodar o jogo.

#### Como Rodar o Jogo no Notepad++ for raylib

1. Abra o arquivo `Jogo-AED` em abrir pasta como área de trabalho na aba de ficheiro no **Notepad++ for raylib**.
2. Abra o Main.c e pressione **F6** para abrir a janela de execução de scripts.
3. Escolha a opção raylib_compile_execute e rode o jogo.

### Opção 2. Makefile no Linux/WSL
1. Instalar o  GCC:
```
sudo apt install build-essential git
```
2. Instalar as libs necessárias:
```
sudo apt install libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev
```
3. Baixe o código-fonte do Raylib diretamente do repositorio em uma pasta no linux e entre na pasta 
```
git clone --depth 1 https://github.com/raysan5/raylib.git raylib
cd raylib/src/
```
4. Compile a raylib:
```
make PLATFORM=PLATFORM_DESKTOP
```
5. Instale a raylib:
```
sudo make install
cd ~
```
6. Clone o repositório do jogo:
```
git clone https://github.com/Taverna-Hub/Recifishing.git
cd Jogo-AED
```
7. Compile e rode o jogo:
```
make run
```

### Opção 3. Compilar e Rodar no Visual Studio Code
1. Instale a biblioteca Raylib.
   - [Guia de instalação da Raylib](https://www.raylib.com/)
2. Tenha o compilador GCC instalado.
   - [MinGW para Windows](http://www.mingw.org/) ou [GCC para Linux](https://gcc.gnu.org/)
3. Aperte F5 no arquivo Main.c
   - Obs: Caso esse aviso de erro apareça, aperte em "Debug Anyway". Marque "Remember my choice for this task"
