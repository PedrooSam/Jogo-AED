#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct Node {
	int n;
	struct Node *prox;
	struct Node *ant;
	struct Node *mapaSecundario;
} Node;

typedef struct {
    int x;
    int y; 
    int mapa;
    bool vivo;
    int vida;
} Objeto;

typedef struct {
    Vector2 posicao;
    Vector2 velocidade;
    bool ativo;
} Projetil;

// ############################
// 	VARIAVEÍS GLOBAIS	
// ############################

//Objetos
Objeto player;
Objeto lacaio;
Objeto boss;

//Projeteis
Projetil projetil;

// lacaio
bool lacaioParado = true;
bool lacaioAtingido = false;
bool lacaioAtacando = false;
bool lacaio2Adicionado = false;
bool lacaio3Adicionado = false;

// Jogador
int playerOffSet = 140; //hitbox do boneco
int velocidade = 7;     //velocidade do boneco
float scale = 3.5f;     // Fator de escala para o personagem

bool pausa = false;     //variavel que verifica se o jogo esta ou nao pausado
bool pegando = false;   // Variável para verificar se o personagem está pegando algo
bool levandoDanoFlag = false;    

//Lacaio

float distanciaX = 0; //Distancia entre lacaio e player
float distanciaY = 0;

float intervalo = 150.0f; //Intervalo de ataque do lacaio
float timerAtaque = 0.0f;

//Cooldown do tiro 
float intervaloTiro = 25.0f;
float timerTiro = 0.0f;

// Chave
Objeto chave;
bool chaveSpawn = true;

// Puzzle
bool puzzle1Resolvido = false;
bool puzzle2Resolvido = false;

//mapas
bool mapaBloqueado = false;

// Pontacao 
int pontuacao = 0;

//potion
bool curou = false;

float timerBalao = 0.0f;
bool colidiu = false;
bool colidiu1 = false;

// ############################
// FLAGS	
// ############################

bool andandoDireita = true;             // Direção inicial
bool chavePegandoFlag = false;          // Verifica se tá pegando a chave
bool puzzleDesbloqueado = false;        // Verifica se pode acessar a sala do puzzle 

bool chaveTesouroPegandoFlag = false;   //Verifica se tá pegando a chave do tesouro
bool chaveTesouroSpawn = true;          //Verifica se a chave do tesouro pode spawnar
bool chaveTesouroNoBau = false;         //Verifica se a chave do tesouro está no baú

bool espadaTesouroPegandoFlag = false;  //Verifica se tá pegando a espada
bool espadaTesouroSpawn = true;         //Verifica se a espada pode spawnar
bool espadaTesouroNoBau = false;        //Verifica se a espada está no baú 

bool diamanteTesouroPegandoFlag = false;//Verifica se está pegando o diamante
bool diamanteTesouroSpawn = true;       //Verifica se o diamante pode spawnar
bool diamanteTesouroNoBau = false;      //Verifica se o diamante está no baú

bool botao1Pressionado = false;
bool botao2Pressionado = false;

bool segurandoItem = false;
bool personagemParado = true;

bool glockDourada = false;
bool glockDouradaSpawn = true;

bool deathFlag = false;
bool deathSoundPlayed = false;

int delayBala = 0;

bool bossAtack1Flag = false;
bool bossAtack2Flag = false;
bool bossIndoDireita = true;
bool lacaioIndoDireita = true;
int timerAtaqueB = 400;
float distanciaXB = 0.0f;
float distanciaYB = 0.0f;
bool acertou = false;

//delay do som de dano
float delayUrh = 0.0f;

//Colisão Universal
bool CollisionObject(Rectangle playerCollision, Rectangle objeto) {
    if (CheckCollisionRecs(playerCollision, objeto)) {
        float penetracaoEsquerda = (playerCollision.x + playerCollision.width) - objeto.x;
        float penetracaoDireita = (playerCollision.x + playerCollision.width) - playerCollision.x;
        float penetracaoCima = (playerCollision.y + playerCollision.height) - objeto.y;
        float penetracaoBaixo = (objeto.y + objeto.height) - playerCollision.y;

        if (penetracaoEsquerda < penetracaoDireita && penetracaoEsquerda < penetracaoCima && penetracaoEsquerda < penetracaoBaixo) {
            player.x -= velocidade;
            return true;
        } else if (penetracaoDireita < penetracaoEsquerda && penetracaoDireita < penetracaoCima && penetracaoDireita < penetracaoBaixo) {
            player.x += velocidade;
            return true;
        } else if (penetracaoCima < penetracaoBaixo && penetracaoCima < penetracaoEsquerda && penetracaoCima < penetracaoDireita) {
            player.y -= velocidade;
            return true;
        } else if (penetracaoBaixo < penetracaoCima && penetracaoBaixo < penetracaoEsquerda && penetracaoBaixo < penetracaoDireita) {
            player.y += velocidade;
            return true;
        }
    }
    return false;
}

void inserirMapa(Node **head, Node **tail, int n, Node *mapaSec) {
  Node *novo = (Node*)malloc(sizeof(Node));  
  if (novo != NULL){
    novo->n = n;    
    novo->prox = NULL;
    if(mapaSec != NULL) mapaSec->ant = novo;
    novo->mapaSecundario = mapaSec;
    

    if(*head == NULL){
      novo->ant=NULL;
      *head=novo;
      *tail=novo;
    }else{        
      (*tail)->prox=novo;
      novo->ant =*tail;
      *tail=novo;
    }

  }
}

void configurarMapaSecundario(Node **mapa, int n) {
    Node *novo = (Node*)malloc(sizeof(Node));  
    novo->n = n;
    novo->prox = NULL;
    novo->ant = NULL;
    novo->mapaSecundario = NULL;
    *mapa = novo;
}

int getNextMapaPrincipal(Node *head) {
    Node *aux = head;
    while(aux->prox != NULL && aux->n != player.mapa) {
        aux = aux->prox;
    }
    if(aux->prox != NULL) {
        return aux->prox->n;
    }
    
    return aux->n;
}

int getAntMapaPrincipal(Node *head) {
    Node *aux = head;
    while(aux->prox != NULL && aux->n != player.mapa) {
        aux = aux->prox;
    }
    if(aux->ant != NULL) {
        return aux->ant->n;
    }
    
    return aux->n;
}

int getMapaSecundario(Node *head) {
    Node *aux = head;
    while(aux->prox != NULL && aux->n != player.mapa) {
        aux = aux->prox;
    }
    if(aux->mapaSecundario != NULL) {
        return aux->mapaSecundario->n;
    }
    
    return aux->n;
}

int getAntMapaSecundario(Node *head) {
    Node *aux = head;
    
    while(aux->prox != NULL) {
        if(aux->mapaSecundario != NULL && aux->mapaSecundario->n == player.mapa) {
            return aux->mapaSecundario->ant->n;
        }
      
        aux = aux->prox;
    }
    
    return aux->n;
}

// Função do Menu
void menu(Texture2D backgroundMenu) {
    Rectangle botaoIniciar = {540, 250, 200, 100};
    Rectangle botaoSair = {540, 400, 200, 100};
    
    Sound confirm = LoadSound("audios/confirm.mp3");
    
    while (1) {
        BeginDrawing();

        DrawTexture(backgroundMenu,0,0,WHITE);
        
        EndDrawing();
            
        // Verificar clique no botão
        if (CheckCollisionPointRec(GetMousePosition(), botaoIniciar) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            pausa = false;
            PlaySound(confirm);
                  
            // Após pressionar Enter, redefine as variáveis e o estado do jogo
            player.vivo = true;
            player.vida = 5;
            player.mapa = 0;
            player.x = 800;
            player.y = 200;

            lacaio.x = 100;
            lacaio.y = 200;
            lacaio.mapa = -1;
            lacaio.vivo = true;
            lacaio.vida = 10;
            
            boss.x = 100;
            boss.y = 200;
            boss.mapa = 4;
            boss.vivo = true;
            boss.vida = 50;

            lacaio2Adicionado = false;
            lacaio3Adicionado = false;

            chaveSpawn = true;
            chaveTesouroNoBau = false;
            chaveTesouroSpawn = true;
            espadaTesouroNoBau = false;
            espadaTesouroSpawn = true;
            diamanteTesouroNoBau = false;
            diamanteTesouroSpawn = true;

            glockDourada = false;
            glockDouradaSpawn = true;

            puzzle1Resolvido = false;
            puzzle2Resolvido = false;
            puzzleDesbloqueado = false;
            curou = false;
            pontuacao = 0;
            mapaBloqueado = false;
            levandoDanoFlag = false;
            
            deathFlag = false;
            deathSoundPlayed = false;
            break;  
        }
        else if (CheckCollisionPointRec(GetMousePosition(), botaoSair) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            PlaySound(confirm);
            CloseWindow();
            break;
        }           
    }
}

//função puzzle 2
bool puzzleIcons(Texture puzzle2, Music musica){
    bool resultado = true;
    int lista[3] = {-1, -1, -1};
    
     // Toca a música (começa a tocar apenas uma vez)
    PlayMusicStream(musica);
    
    // Define as áreas de colisão para os botões
    Rectangle alternativa1Collision = {420, 390, 125, 125};
    Rectangle alternativa2Collision = {560, 390, 125, 125};
    Rectangle alternativa3Collision = {700, 390, 125, 125};
    int preenchidos = 0; // Contador de posições preenchidas em `lista`
    
    Sound confirm = LoadSound("audios/confirm.mp3");
    
    while (preenchidos < 3) {
        // Atualiza o fluxo da música em cada loop para que continue tocando
        UpdateMusicStream(musica);
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(puzzle2, 370, 100, WHITE);
        
        // Desenha os botões e os textos dos números gerados
        //DrawRectangleRec(alternativa1Collision, BLUE);
        //DrawRectangleRec(alternativa2Collision, BLACK);
        //DrawRectangleRec(alternativa3Collision, LIME);
        
        // Verifica as colisões com o mouse para cada alternativa e armazena o número na lista
        if (CheckCollisionPointRec(GetMousePosition(), alternativa1Collision) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            lista[preenchidos] = 1;
            preenchidos++;
            PlaySound(confirm);
        }
        if (CheckCollisionPointRec(GetMousePosition(), alternativa2Collision) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { 
            lista[preenchidos] = 3;
            preenchidos++;
            PlaySound(confirm);
        }
        if (CheckCollisionPointRec(GetMousePosition(), alternativa3Collision) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            lista[preenchidos] = 2;
            preenchidos++;
            PlaySound(confirm);
        }
        if(IsKeyPressed(KEY_ESCAPE))//player nao quer mais fazer o puzzle
            return false;

        EndDrawing();
    }
    
    // Bubble Sort para ordenar e verificar o resultado, se tiver troca, ta errado!'
    for (int i = 0; i < 3 - 1; i++) {
        bool trocou = false;
        for (int j = 0; j < 3 - i - 1; j++) {
            if (lista[j] >= lista[j + 1]) {
                int temp = lista[j];
                lista[j] = lista[j + 1];
                lista[j + 1] = temp;
                trocou = true;
                resultado = false;
            }
        }
        // Se não houve troca, a lista já está ordenada
        if (resultado) break;
    }
    
    //verifica se a lista ta ordenada pq se tiver ai o puzzle acaba
    if(resultado){
        while(!IsKeyPressed(KEY_ENTER)){//esse while basicamente pra ter a mensagem de sucesso e ele so para se o usuario apertar enter, mesma coisa no proximo, mas ele mostra a de erro
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("PARABENS! Puzzle concluido! Diamante desbloqueado!",200, 310, 30, GREEN);
            DrawText("Agora, aperte ENTER para sair!",450, 350, 20, WHITE);
            EndDrawing();
            puzzle2Resolvido = true;
        }
        return resultado;
    }
    
    //se a lista que o usuario mandou nao tiver ordenada manda ele fazer o puzzle dnv
    else{
        while(!IsKeyPressed(KEY_ENTER)){
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("ERRO! Puzzle não concluido!",445, 310, 30, RED);
            DrawText("Aperte ENTER para tentar novamente!",460, 350, 20, WHITE);
            EndDrawing();
        }
        return puzzleIcons(puzzle2, musica); //recursao para que o puzzle seja reiniciado
    }
}

// Função puzzle 1
bool puzzleOrdenar(Texture2D puzzle1, Music musica) {
    bool resultado = true;
    int lista[4] = {-1, -1, -1, -1};
    int a = rand() % 32766, b = rand() % 32766, c = rand() % 32766, d = rand() % 32766;
    char texto1[6], texto2[6], texto3[6], texto4[6];

    // Converte os números para strings
    sprintf(texto1, "%d", a);
    sprintf(texto2, "%d", b);
    sprintf(texto3, "%d", c);
    sprintf(texto4, "%d", d);
    
    Sound confirm = LoadSound("audios/confirm.mp3");
    
    // Define as áreas de colisão para os botões
    Rectangle alternativa1Collision = {520, 310, 200, 60};
    Rectangle alternativa2Collision = {520, 390, 200, 60};
    Rectangle alternativa3Collision = {520, 480, 200, 60};
    Rectangle alternativa4Collision = {520, 570, 200, 60};
    
    int preenchidos = 0; // Contador de posições preenchidas em `lista`
    
     // Toca a música (começa a tocar apenas uma vez)
    PlayMusicStream(musica);

    while (preenchidos < 4) {
        // Atualiza o fluxo da música em cada loop para que continue tocando
        UpdateMusicStream(musica);
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(puzzle1, 370, 100, WHITE);
        
        // Desenha os botões e os textos dos números gerados
        //DrawRectangleRec(alternativa1Collision, BLUE);
        //DrawRectangleRec(alternativa2Collision, BLACK);
        //DrawRectangleRec(alternativa3Collision, LIME);
        //DrawRectangleRec(alternativa4Collision, YELLOW);
        
        DrawText(texto1, 550, 330, 20, GOLD);
        DrawText(texto2, 550, 415, 20, GOLD);
        DrawText(texto3, 550, 510, 20, GOLD);
        DrawText(texto4, 550, 590, 20, GOLD);
        
        // Verifica as colisões com o mouse para cada alternativa e armazena o número na lista
        if (CheckCollisionPointRec(GetMousePosition(), alternativa1Collision) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            lista[preenchidos] = a;
            preenchidos++;
            PlaySound(confirm);
        }
        if (CheckCollisionPointRec(GetMousePosition(), alternativa2Collision) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { 
            lista[preenchidos] = b;
            preenchidos++;
            PlaySound(confirm);
        }
        if (CheckCollisionPointRec(GetMousePosition(), alternativa3Collision) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            lista[preenchidos] = c;
            preenchidos++;
            PlaySound(confirm);
        }
        if (CheckCollisionPointRec(GetMousePosition(), alternativa4Collision) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { 
            lista[preenchidos] = d;
            preenchidos++; 
            PlaySound(confirm);
        }
        if(IsKeyPressed(KEY_ESCAPE))//player nao quer mais fazer o puzzle
            return false;
        
        EndDrawing();
    }
    
    // Bubble Sort para ordenar a lista
    for (int i = 0; i < 4 - 1; i++) {
        bool trocou = false;
        for (int j = 0; j < 4 - i - 1; j++) {
            if (lista[j] >= lista[j + 1]) {
                int temp = lista[j];
                lista[j] = lista[j + 1];
                lista[j + 1] = temp;
                trocou = true;
                resultado = false;
            }
        }
        // Se não houve troca, a lista já está ordenada
        if (resultado) break;
    }
    
    //verifica se a lista ta ordenada pq se tiver ai o puzzle acaba
    if(resultado){
        while(!IsKeyPressed(KEY_ENTER)){//esse while basicamente pra ter a mensagem de sucesso e ele so para se o usuario apertar enter, mesma coisa no proximo, mas ele mostra a de erro
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("PARABENS! Puzzle concluido!",400, 310, 30, GREEN);
            DrawText("Agora, aperte ENTER para sair!",450, 350, 20, WHITE);
            EndDrawing();
            puzzle1Resolvido = true;
        }
        return resultado;
        
    }
    //se a lista que o usuario mandou nao tiver ordenada manda ele fazer o puzzle dnv
    else{
        while(!IsKeyPressed(KEY_ENTER)){
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("ERRO! Puzzle não concluido!",445, 310, 30, RED);
            DrawText("Aperte ENTER para tentar novamente!",460, 350, 20, WHITE);
            EndDrawing();
        }
        return puzzleOrdenar(puzzle1, musica); //recursao para que o puzzle seja reiniciado
    }
}

// Função de pegar o frame certo
int getFrame(float timer, float tempoFrame, int totalFrames, int frameAtual) {
    if (timer >= tempoFrame) {
        frameAtual++; // Incrementa o frame
        
        if (frameAtual >= totalFrames) {
            frameAtual = 0; // Reinicia o frame se exceder o total
        }
    }
    return frameAtual; // Retorna o frame atual (ou o incrementado)
}

// Função para gerenciar o timer de cada frame
float getTime(float timer) {
    timer += GetFrameTime();  // Atualiza o timer com o tempo decorrido desde o último quadro
    return timer;  // Retorna o timer atualizado
}
// Função da intro
void ShowIntro() {
    int currentScene = 0;

    Texture2D images[2];    
    images[0] = LoadTexture("ui/lore.png");
    images[1] = LoadTexture("ui/move.png"); // Imagem com os controles
    
    Sound confirm = LoadSound("audios/confirm.mp3");
    
    while (!WindowShouldClose() && currentScene < 2) {
        // Atualizar para próxima cena quando ENTER for pressionado
        if (IsKeyPressed(KEY_ENTER)) {
            currentScene++;
            PlaySound(confirm);
        }

        BeginDrawing();
        ClearBackground(BLACK);

        // Exibir imagem e texto da cena atual
        if (currentScene < 2) {
            DrawTexture(images[currentScene], 0, 0, WHITE);
        }

        EndDrawing();
    }

    // Limpar as texturas das imagens
    for (int i = 0; i < 2; i++) {
        UnloadTexture(images[i]);
    }
}

void ShowLoading(){
    Texture2D loading = LoadTexture("ui/loading.png");
    
    int totalFrames = 4;
    int frameAtual = 0;
    float tempoFrame = 0.3f;
    float timer = 0.0f;
    int larguraFrame = loading.width / totalFrames;
    int alturaFrame = loading.height;
    int contagem = 0;
    
    while (!WindowShouldClose() && contagem < 8) { 
        timer += GetFrameTime(); 

        // Muda para o próximo frame quando o tempo do frame é atingido
        if (timer >= tempoFrame) {
            timer = 0.0f;               // Reinicia o temporizador
            frameAtual = (frameAtual + 1) % totalFrames; // Avança para o próximo frame, loopando após o último
            contagem++;
        }

        // Define a área do frame atual a ser desenhada
        Rectangle sourceRec = { larguraFrame * frameAtual, 0, larguraFrame, alturaFrame };

        // Define onde o frame será desenhado na tela (ajuste as coordenadas conforme necessário)
        Vector2 posicao = {0, 0};

        // Inicia o desenho da tela
        BeginDrawing();
        ClearBackground(RAYWHITE); // Limpa a tela com uma cor de fundo

        // Desenha o frame atual da animação de loading
        DrawTextureRec(loading, sourceRec, posicao, WHITE);

        // Finaliza o desenho
        EndDrawing();
    }

    // Unload da textura após a animação
    UnloadTexture(loading);
}

// Função principal do jogo
void iniciarJogo(Texture2D backgroundImage, Texture2D personagemPegando, Texture2D personagemPegandoEsquerda, Texture2D chaveCenario, Texture2D pegandoChaveEsquerda, Texture2D pegandoChaveDireita, Texture2D mapa1, Texture2D mapa2, Texture2D arena, Texture2D mensagem1, Texture2D menuBack, Texture2D espadaTesouro, Texture2D chaveTesouro, Texture2D bau, Texture2D bauPreenchido, Texture2D botao1Off, Texture2D botao1On, Texture2D botao2Off, Texture2D botao2On, Texture2D pegandoChaveTesouroDireita, Texture2D pegandoChaveTesouroEsquerda, Texture2D pegandoEspadaEsquerda, Texture2D pegandoEspadaDireita, Texture2D bauPreenchido2, Texture2D diamante, Texture2D pegandoDiamanteEsquerda,Texture2D  pegandoDiamanteDireita, Texture2D bauPreenchido3, Texture2D puzzle1, Texture2D spritesheet, Texture2D spritesheetRight, Texture2D spriteWalkLeft, Texture2D spriteWalkRight,Texture2D pegandoEsquerdaIdle, Texture2D pegandoDireitaIdle, Texture2D backgroundMenu,Texture2D pegandoChaveEsquerdaIdle,Texture2D pegandoChaveDireitaIdle, Texture2D pegandoChaveTesouroEsquerdaIdle, Texture2D pegandoChaveTesouroDireitaIdle, Texture2D pegandoEspadaEsquerdaIdle, Texture2D pegandoEspadaDireitaIdle, Texture2D pegandoDiamanteEsquerdaIdle, Texture2D pegandoDiamanteDireitaIdle, Texture2D bala,Texture2D danoLacaioEsquerda ,Texture2D danoLacaioDireita, Texture2D diamanteFree, Texture2D atirandoEsquerda, Texture2D atirandoDireita, Texture2D lacaioDireita, Texture2D lacaioEsquerda, Texture2D lacaioAtaqueEsquerda, Texture2D lacaioAtaqueDireita, Texture2D levandoDano, Texture2D mensagem2, Texture2D puzzle2, Texture2D vida1,Texture2D vida2,Texture2D vida3,Texture2D vida4,Texture2D vida5,   Texture2D lacaioVida1,Texture2D lacaioVida2,Texture2D lacaioVida3,Texture2D lacaioVida4,Texture2D lacaioVida5,Texture2D lacaioVida6,Texture2D lacaioVida7,Texture2D lacaioVida8, Texture2D lacaioVida9,Texture2D lacaioVida10, Texture2D fogo,Texture2D procuraChave,Texture2D procuraEspadas,Texture2D procuraDiamante,Texture2D achouChave,Texture2D achouEspadas,Texture2D achouDiamante, Texture2D potion, Texture2D secret,Texture2D atirandoEsquerdaDourado,Texture2D atirandoDireitaDourado, Texture2D glock,Texture2D death, Texture2D deathLeft, Texture2D shadow,Texture2D bossIdle,Texture2D bossIdleLeft,Texture2D bossAtack1,Texture2D bossAtack1Left,Texture2D bossAtack2 , Texture2D bossAtack2Left, Texture2D mapa4, Sound andando,Sound tiro, Sound uhr, Sound lacaioSom, Sound abrindoPorta,Sound destrancandoPorta,Sound portaTrancada,Sound pegandoItem, Sound end, Sound heal, Sound getGlock, Music musica, Sound desert, Sound deathSound) {
    
    
    // ############################
    // 	CONFIG DE MAPA	
    // ############################
    Node *head = NULL; 
    Node *tail = NULL;
    Node *mapaSec0 = NULL;
    
    configurarMapaSecundario(&mapaSec0, -1);
    
    inserirMapa(&head, &tail, 0, NULL);
    inserirMapa(&head, &tail, 1, NULL);
    inserirMapa(&head, &tail, 2, mapaSec0);
    inserirMapa(&head, &tail, 3, NULL);
    inserirMapa(&head, &tail, 4, NULL);
    
    //posicao bala 
    projetil.ativo = false;
    bool atirou = false;
    
    //posição jogador
    player.x = 800;
    player.y = 200;
    player.mapa = 0; 
    player.vivo = true;
    player.vida = 5;
    
    //posição lacaio
    lacaio.x = 100;
    lacaio.y = 200;
    lacaio.mapa = -1;
    lacaio.vivo = true;
    lacaio.vida = 10; //VIDA LACAIO
    
    //posição boss
    boss.x = 100;
    boss.y = 200;
    boss.mapa = 4;
    boss.vivo = true;
    boss.vida = 50;
    
    //posição de chave
    chave.x = 500;
    chave.y = 550;
    chave.mapa = 0;

    // ############################
    // 	CONFIG DE ANIMAÇÃO	
    // ############################
	
    //animação idle
    int totalFrames = 4;
    int frameAtual = 0;
    float tempoFrame = 0.15f;
    float timer = 0.0f;
    int larguraFrame = spritesheet.width / totalFrames;
    int alturaFrame = spritesheet.height;
    
    //animação walk
    int totalFramesWalk = 5;
    int frameAtualWalk = 0;
    float tempoFrameWalk = 0.1f;
    float timerWalk = 0.0f;
    int larguraFrameWalk = spriteWalkLeft.width / totalFramesWalk;
    int alturaFrameWalk = spriteWalkLeft.height;
    
    //animação pegando chave primária {Conta pra maioria das animações};
    int totalFramesKey = 5;
    int frameAtualKey = 0;
    float tempoFrameKey = 0.1f;
    float timerKey = 0.0f;
    int larguraFrameKey = pegandoChaveDireita.width / totalFramesKey;
    int alturaFrameKey = pegandoChaveDireita.height;
    
    //animação pegando idle {Também conta pra boa parte das animações};
    int totalFramesIdle = 4;
    int frameAtualIdle = 0;
    float tempoFrameIdle = 0.15f;
    float timerIdle = 0.0f;
    int larguraFrameIdle = pegandoDireitaIdle.width / totalFramesIdle;
    int alturaFrameIdle = pegandoDireitaIdle.height;

   // (lacaio) animacao dano
    int totalFrameslacaioDano = 4;
    int frameAtuallacaioDano = 0;
    float tempoFramelacaioDano = 0.05f;
    float timerlacaioDano = 0.0f;
    int larguraFramelacaioDano = danoLacaioDireita.width / totalFrameslacaioDano;
    int alturaFramelacaioDano = danoLacaioDireita.height;
    
    // Animação de atirar
    int totalFramesTiro = 4;
    int frameAtualTiro = 0;
    float tempoFrameTiro = 5.0f;
    float timerTiro = 0.0f;
    int larguraFrameTiro = atirandoEsquerda.width / totalFramesTiro;
    int alturaFrameTiro = atirandoEsquerda.height;
    
    // Lacaio andando 
    int totalFrameslacaioWalk = 4;
    int frameAtuallacaioWalk = 0;
    float tempoFramelacaioWalk = 0.05f;
    float timerlacaioWalk = 0.0f;
    int larguraFramelacaioWalk = lacaioDireita.width / totalFrameslacaioWalk;
    int alturaFramelacaioWalk = lacaioDireita.height;
    
    // Lacaio atacando 
    int totalFrameslacaioAtaque = 4;
    int frameAtuallacaioAtaque = 0;
    float tempoFramelacaioAtaque = 0.1f;
    float timerlacaioAtaque = 0.0f;
    int larguraFramelacaioAtaque = lacaioAtaqueDireita.width / totalFrameslacaioAtaque;
    int alturaFramelacaioAtaque = lacaioAtaqueDireita.height;
    
    // Personagem levando dano
    int totalFramesLevandoDano = 4;
    int frameAtualLevandoDano = 0;
    float tempoFrameLevandoDano = 0.05f;
    float timerLevandoDano = 0.0f;
    int larguraFrameLevandoDano = levandoDano.width / totalFramesLevandoDano;
    int alturaFrameLevandoDano = levandoDano.height;
    
    // Animação fogo
    int totalFramesFogo = 4;
    int frameAtualFogo = 0;
    float tempoFrameFogo = 0.1f;
    float timerFogo = 0.0f;
    int larguraFogo = fogo.width / totalFramesFogo;
    int alturaFrameFogo = fogo.height;

    // Animação potion
    int totalFramesPotion = 8;
    int frameAtualPotion = 0;
    float tempoFramePotion = 0.1f;
    float timerPotion = 0.0f;
    int larguraPotion = potion.width / totalFramesPotion;
    int alturaFramePotion = potion.height;
    
    //Animação secret room
    int totalFramesSecret = 4;
    int frameAtualSecret = 0;
    float tempoFrameSecret = 0.1f;
    float timerSecret = 0.0f;
    int larguraSecret = secret.width / totalFramesSecret;
    int alturaFrameSecret = secret.height;
    
    //Animação death
    int totalFramesDeath = 9;
    int frameAtualDeath = 0;
    float tempoFrameDeath = 0.2f;
    float timerDeath = 0.0f;
    int larguraDeath = death.width / totalFramesDeath;
    int alturaFrameDeath = death.height;
    
    //#######BOSS###################
    //Idle
    int totalBossIdle = 8;
    int atualBossIdle = 0;
    float tempoBossIdle = 0.2f;
    float timerBossIdle = 0.0f;
    int larguraBossIdle = bossIdle.width / totalBossIdle;
    int alturaBossIdle = bossIdle.height;
    
    //Atack1
    int totalBossAtack1 = 11;
    int atualBossAtack1 = 0;
    float tempoBossAtack1 = 0.1f;
    float timerBossAtack1 = 0.0f;
    int larguraBossAtack1 = bossAtack1.width / totalBossAtack1;
    int alturaBossAtack1 = bossAtack1.height;
    
    //Atack2
    int totalBossAtack2 = 5;
    int atualBossAtack2 = 0;
    float tempoBossAtack2 = 0.1f;
    float timerBossAtack2 = 0.0f;
    int larguraBossAtack2 = bossAtack2.width / totalBossAtack2;
    int alturaBossAtack2 = bossAtack2.height;
    
    ShowIntro();
    ShowLoading();
    while (!WindowShouldClose()) {
            pontuacao += 1;
            
            UpdateMusicStream(musica);
    
            // Verifica se a música terminou e reinicia se necessário
            if (IsMusicStreamPlaying(musica) == false) {
                PlayMusicStream(musica);
            }
            
            if (!pausa) {
                if (IsKeyPressed(KEY_P)) {
                pausa = !pausa;
            }

            // ############################
            // 	JOGADOR
            // ############################
            
            timerTiro += 1;
            if (IsKeyPressed(KEY_SPACE) && !projetil.ativo && personagemParado && !segurandoItem && timerTiro >= intervaloTiro ) {       // Só pode atirar quando tiver parado 

                projetil.ativo = true;
                
                if(glockDourada){
                    PlaySound(desert);
                }else{
                    PlaySound(tiro);
                }
                atirou = true;
                pegando = false;
                projetil.posicao = (Vector2){ player.x, player.y - playerOffSet + 60 };
                
                if(andandoDireita) {
                    projetil.velocidade = (Vector2){ 30.0f, 0.0f };  // Velocidade para a direita
                }else{
                    projetil.velocidade = (Vector2){ -30.0f, 0.0f };
                }
            }

            // Verificar se a tecla 'E' foi pressionada para pegar algo
            if (IsKeyPressed(KEY_E)) {
                pegando = !pegando; // Alternar o estado de pegando
                if(chavePegandoFlag) chavePegandoFlag = !chavePegandoFlag;
            }   
            
            // Movimento do jogador            
            if (IsKeyDown(KEY_W) && IsKeyDown(KEY_D)) { // Diagonal direita
                if (!IsSoundPlaying(andando)) PlaySound(andando);
                player.y -= velocidade;
                player.x += velocidade;
                personagemParado = false;
                andandoDireita = true;
            }
            else if (IsKeyDown(KEY_W) && IsKeyDown(KEY_A)) { // Diagonal esquerda
                if (!IsSoundPlaying(andando)) PlaySound(andando);
                player.y -= velocidade;
                player.x -= velocidade;
                personagemParado = false;
                andandoDireita = false;
            }
            else if (IsKeyDown(KEY_S) && IsKeyDown(KEY_D)) { // Diagonal pra baixo direita
                if (!IsSoundPlaying(andando)) PlaySound(andando);
                player.y += velocidade;
                player.x += velocidade;
                personagemParado = false;
                andandoDireita = true;
            }
            else if (IsKeyDown(KEY_S) && IsKeyDown(KEY_A)) { // Diagonal pra baixo esquerda
                if (!IsSoundPlaying(andando)) PlaySound(andando);
                player.y += velocidade;
                player.x -= velocidade;
                personagemParado = false;
                andandoDireita = false;
            }
            else if (IsKeyDown(KEY_W)) {
                if (!IsSoundPlaying(andando)) PlaySound(andando);
                player.y -= velocidade;
                personagemParado = false;
            }
            else if (IsKeyDown(KEY_S)) {
                if (!IsSoundPlaying(andando)) PlaySound(andando);
                player.y += velocidade;
                personagemParado = false;
            }
            else if (IsKeyDown(KEY_D)) {
                if (!IsSoundPlaying(andando)) PlaySound(andando);
                player.x += velocidade;
                andandoDireita = true;
                personagemParado = false;
            }
            else if (IsKeyDown(KEY_A)) {
                if (!IsSoundPlaying(andando)) PlaySound(andando);
                player.x -= velocidade;
                andandoDireita = false;
                personagemParado = false;
            }
            else {
                personagemParado = true;
                StopSound(andando);  // Parar o som quando o jogador não estiver se movendo
            }
     
            // ############################
            // 	LACAIO
            // ############################
            
            float velocidadeLacaio = 200.0f * GetFrameTime();
            // Atualize o timer do ataque
            timerAtaque += 1;
            delayUrh += 1;
            if (lacaio.vivo && lacaio.mapa == player.mapa) {
                
                Sound sword = LoadSound("audios/swordSound.mp3");
                
                // Movimenta no eixo X em direção ao jogador
                if (lacaio.x < player.x -200) {
                    lacaio.x += velocidadeLacaio;
                    lacaioIndoDireita = true;
                } else if (lacaio.x > player.x) {
                    lacaioIndoDireita = false;
                    lacaio.x -= velocidadeLacaio;
                }

                // Movimenta no eixo Y em direção ao jogador
                if (lacaio.y < player.y - playerOffSet) {
                    lacaio.y += velocidadeLacaio;
                } else if (lacaio.y > player.y - playerOffSet) {
                    lacaio.y -= velocidadeLacaio;
                }

                // Calcula a distância absoluta entre o lacaio e o jogador em cada eixo
                distanciaX = fabsf((player.x - 200) - lacaio.x);
                distanciaY = fabsf((player.y - playerOffSet) - lacaio.y);

                // Verifica se o lacaio está dentro do alcance de ataque
                if (distanciaY <= 50 && timerAtaque >= intervalo && (distanciaX <= 100 || (distanciaX <= 200 && !lacaioIndoDireita))) {
                    PlaySound(sword);

                    if(delayUrh >= 20){
                        PlaySound(uhr);   
                        delayUrh = 0.0f;
                    }
                    lacaioAtacando = true;
                    levandoDanoFlag = true;
                    player.vida -= 1;   // Reduz a vida do jogador
                    timerAtaque = 0.0f; // Reinicia o tempo de recarga do ataque
                }
            }    
            
            // ############################
            // 	BOSS
            // ############################
            timerAtaqueB += 1;
            float velocidadeBoss = 300.0f * GetFrameTime();
            if (boss.vivo && boss.mapa == player.mapa) {
                
                if(boss.x < player.x - 50) bossIndoDireita = true;
                else if(boss.x > player.x + 50) bossIndoDireita = false;
                
                if(bossAtack1Flag){
                    // Movimenta no eixo X em direção ao jogador
                    if (boss.x < player.x - 200) {
                        boss.x += velocidadeBoss;
                    } else if (boss.x > player.x) {
                        boss.x -= velocidadeBoss;
                    }
        
                    // Movimenta no eixo Y em direção ao jogador
                    if (boss.y < player.y - playerOffSet) {
                        boss.y += velocidadeBoss;
                    } else if (boss.y > player.y - playerOffSet) {
                        boss.y -= velocidadeBoss;
                    }
                }

                // Calcula a distância absoluta entre o lacaio e o jogador em cada eixo
                distanciaXB = fabsf((player.x - 200 ) - boss.x);
                distanciaYB = fabsf((player.y - playerOffSet) - boss.y);
                
                if(timerAtaqueB >= 400) {
                    acertou = false;
                    timerAtaqueB = 0;
                    bossAtack1Flag = true;
                }
                
                // Verifica se o lacaio está dentro do alcance de ataque
                if (distanciaYB <= 50 && !acertou && (distanciaXB < 20 || (distanciaXB <= 200 && !bossIndoDireita))) {
                    acertou = true;
                    levandoDanoFlag = true;
                    player.vida -= 1;   // Reduz a vida do jogador
                }
            }  
            
            //Verifica se o player morreu
            if(player.vida <= 0){
                player.vivo = false;
            }
            
            //verifica se tá segurando algo
            if(!chavePegandoFlag && !chaveTesouroPegandoFlag && !espadaTesouroPegandoFlag && !diamanteTesouroPegandoFlag){
                segurandoItem = false;
            }else{
                segurandoItem = true;
            }

            // ############################
            // 	ANIMAÇÃO
            // ############################

            // Atualizacao idle 
            if(personagemParado){ 
            timer = getTime(timer);            
            frameAtual = getFrame(timer, tempoFrame, totalFrames, frameAtual);
            if(timer >= tempoFrame) timer = 0.0f;
            }
            
            if (!personagemParado) {
            timerWalk = getTime(timerWalk);  // Incrementa timerWalk com o tempo do frame atual
            frameAtualWalk = getFrame(timerWalk, tempoFrameWalk, totalFramesWalk, frameAtualWalk);
            if (timerWalk >= tempoFrameWalk) timerWalk = 0.0f;  // Reseta timerWalk se exceder tempoFrameWalk
            }
            
            // Atualização pegando andando
            if (!personagemParado) {
                timerKey = getTime(timerKey);  // Incrementa timerKey
                frameAtualKey = getFrame(timerKey, tempoFrameKey, totalFramesKey, frameAtualKey);
                if (timerKey >= tempoFrameKey) timerKey = 0.0f;  // Reseta timerKey
            }

            // Atualização pegando parado
            if (personagemParado) {
                timerIdle = getTime(timerIdle);  // Incrementa timerIdle
                frameAtualIdle = getFrame(timerIdle, tempoFrameIdle, totalFramesIdle, frameAtualIdle);
                if (timerIdle >= tempoFrameIdle) timerIdle = 0.0f;  // Reseta timerIdle
            }

            // Atualização lacaio levando dano
            if (lacaioAtingido) {
                timerlacaioDano += GetFrameTime();  // Incrementa timerlacaioDano
                if (timerlacaioDano >= tempoFramelacaioDano) {
                    frameAtuallacaioDano++;
                    timerlacaioDano = 0.0f;  // Reseta timerlacaioDano
                    
                    if(frameAtuallacaioDano >= totalFramesLevandoDano){
                        frameAtuallacaioDano = 0;
                        lacaioAtingido = false;
                    }
                }
            }

            // Atualização de tiro
            if (atirou) {
                timerTiro += GetFrameTime();  // Incrementa timerTiro diretamente com o tempo do frame
                if (timerTiro >= tempoFrameTiro) {
                    frameAtualTiro++; // Passa para o próximo frame
                    timerTiro = 0.0f; // Reseta o timer para o próximo frame
                    
                    if (frameAtualTiro >= totalFramesTiro) {
                        frameAtualTiro = 0;  // Reseta o frame de tiro após completar a animação
                        atirou = false;  // Desativa 'atirou' após concluir todos os frames
                    }
                }
            }

            // Atualização lacaio andando
            if (lacaio.vivo && lacaio.mapa == player.mapa) {
                timerlacaioWalk = getTime(timerlacaioWalk);  // Incrementa timerlacaioWalk
                frameAtuallacaioWalk = getFrame(timerlacaioWalk, tempoFramelacaioWalk, totalFrameslacaioWalk, frameAtuallacaioWalk);
                if (timerlacaioWalk >= tempoFramelacaioWalk) timerlacaioWalk = 0.0f;  // Reseta timerlacaioWalk
            }

            // Atualização lacaio atacando
            if (lacaio.vivo && lacaio.mapa == player.mapa && lacaioAtacando) {
                timerlacaioAtaque += GetFrameTime();  // Incrementa timerlacaioAtaque
                if (timerlacaioAtaque >= tempoFramelacaioAtaque) {
                    frameAtuallacaioAtaque++;
                    timerlacaioAtaque = 0.0f;  // Reseta timerlacaioAtaque
                    
                    if(frameAtuallacaioAtaque > totalFrameslacaioAtaque){
                        frameAtuallacaioAtaque = 0;
                        lacaioAtacando = false;
                    }
                }
            }

            // Atualização personagem levando dano
            if (levandoDanoFlag) {
                timerLevandoDano += GetFrameTime();  // Incrementa timerLevandoDano
                if (timerLevandoDano >= tempoFrameLevandoDano) {
                    frameAtualLevandoDano++;
                    timerLevandoDano = 0.0f;  // Reseta timerLevandoDano
                    
                    if(frameAtualLevandoDano > totalFramesLevandoDano){
                        frameAtualLevandoDano = 0;
                        levandoDanoFlag = false;
                    }
                }
            }
            
            // Atualização secret room
            if(player.mapa == 3){
                timerSecret = getTime(timerSecret);
                frameAtualSecret = getFrame(timerSecret, tempoFrameSecret, totalFramesSecret, frameAtualSecret);
                if(timerSecret > tempoFrameSecret){
                    timerSecret = 0.0f;
                }
            }

            
            // Atualização fogo 
            if(mapaBloqueado){
                timerFogo = getTime(timerFogo);
                frameAtualFogo = getFrame(timerFogo, tempoFrameFogo, totalFramesFogo, frameAtualFogo);
                if(timerFogo > tempoFrameFogo){
                    timerFogo = 0.0f;
                }
            }
            
            // Atualização potion
            if(!curou){
                timerPotion = getTime(timerPotion);
                frameAtualPotion = getFrame(timerPotion, tempoFramePotion, totalFramesPotion, frameAtualPotion);
                if(timerPotion > tempoFramePotion){
                    timerPotion = 0.0f;
                }
            }
            
            // Atualização lacaio death
            if (deathFlag) {
                timerDeath += GetFrameTime();  // Incrementa timerLevandoDano
                if (timerDeath >= tempoFrameDeath) {
                    frameAtualDeath++;
                    timerDeath = 0.0f;  // Reseta timerLevandoDano
                    
                    if(!deathSoundPlayed){
                        PlaySound(deathSound);
                        deathSoundPlayed = true;
                    }
                        
                    if(frameAtualDeath >= totalFramesDeath){
                        frameAtualDeath = 0;
                        deathFlag = false;
                    }
                }
            }
            
            //###############BOSS###################
            // Atualização boss idle
            if (!bossAtack1Flag  && !bossAtack2Flag ) {
                timerBossIdle = getTime(timerBossIdle);  // Incrementa timerlacaioWalk
                atualBossIdle = getFrame(timerBossIdle, tempoBossIdle, totalBossIdle, atualBossIdle);
                if (timerBossIdle >= tempoBossIdle) timerBossIdle = 0.0f;  // Reseta timerlacaioWalk
            }
            
           // Atualização do ataque do boss (bossAtack1)
            if (bossAtack1Flag && player.mapa == 4) {
                timerBossAtack1 += GetFrameTime();  // Incrementa o timer do ataque

                if (timerBossAtack1 >= tempoBossAtack1) {
                    // Verifica a distância entre o boss e o jogador para definir se ele corre ou ataca
                    if (distanciaYB > 50 || (distanciaXB > 20 && bossIndoDireita) || (distanciaXB > 100 && !bossIndoDireita)) {
                        // Enquanto o boss estiver longe, ele continua "correndo"
                        if (atualBossAtack1 < 5 || atualBossAtack1 > 6) {
                            atualBossAtack1 = 5; // Define o frame de corrida inicial
                        }
                        atualBossAtack1 = (atualBossAtack1 == 5) ? 6 : 5; // Alterna entre os frames de corrida
                    } else {
                        // Quando ele está perto o suficiente, passa para os frames de ataque
                        atualBossAtack1++;
                        if (atualBossAtack1 < totalBossAtack1) {
                            atualBossAtack1 = 0;
                            bossAtack1Flag = false; // Termina o ataque
                        }
                    }

                    timerBossAtack1 = 0.0f;  // Reseta o timer do ataque
                }
            }

            
             // Atualização boss atack2
            if (bossAtack2Flag ) {
                timerBossAtack2 += GetFrameTime();  // Incrementa timerlacaioAtaque
                if (timerBossAtack2 >= tempoBossAtack2) {
                    atualBossAtack2++;
                    timerBossAtack2 = 0.0f;  // Reseta timerlacaioAtaque
                    
                    if(atualBossAtack2 > totalBossAtack2){
                        atualBossAtack2 = 0;
                        bossAtack2Flag = false;
                    }
                }
            }
            
            // Restringir o jogador para não sair da tela
            if (player.x < 0) {     // Lado esquerdo
                player.x = 0;
            }
            
            if(player.mapa != -1 && player.mapa != 4){    
                if (player.y < 325 + playerOffSet) {     // Limite teto
                    player.y = 325 + playerOffSet;
                }
            }
            else{
                if(player.y < 150 + playerOffSet){
                    player.y = 150 + playerOffSet;
                }
            }
            
                
            if (player.x > GetScreenWidth() - 83) {  // Lado direito
                player.x = GetScreenWidth() - 83;
            }
                
            if (player.y > GetScreenHeight() - playerOffSet + 60) { //Limite chão
                player.y = GetScreenHeight() - playerOffSet + 60;
            }
            
            // ############################
            // 	COLISÕES
            // ############################
            
            Rectangle playerCollision = {player.x, player.y, 83, 83};
            Rectangle chaveCollision = {chave.x, chave.y, 50, 50};
            
            Rectangle arenaCollision = {225 , 200, 200, 300};
            Rectangle doorCollision = {1190, 500, 160, 200};
            Rectangle doorCollisionEsquerda = {0, 500, 100, 200};
            
            Rectangle pilarMapa0 = {500, 50, 230, 500};
            Rectangle pilarEsqMapa0 = {0, 50, 100, 500};
            
            Rectangle mesaMapa1 = {200, 400, 200, 100};
            
            Rectangle mesaMapa2 = {850, 400, 400, 100};
            
            Rectangle pilarEsqMapa3 = {20, 115, 50, 400};
            Rectangle pilarMapa3 = {600, 115, 120, 400};
            Rectangle pilarDirMapa3 = {1190, 115, 120, 400};
            Rectangle portaMapa3 = {500, 700, 300, 50};
            
            Rectangle chaveTesouroCollision = {300, 400, 50, 50};
            Rectangle bauTesouroCollision = {925, 250, 100, 150};

            Rectangle espadaTesouroCollision = {200, 200, 60 , 100};
            Rectangle diamanteTesouroCollision = {50, 500, 100, 50};
            
            Rectangle botao1Collision = {140,350,50,50};
            Rectangle botao2Collision = {400,350,50,50};  
            
            Rectangle glockDouradaCollision = {700, 575, 50, 50};
            
            //Verifica as colisões 
            if(player.mapa == 4){
                if(CollisionObject(playerCollision, doorCollision)) {
                    player.x = 200;
                    player.y = 540;
                    player.mapa = 0;
                }
            }
            
            else if(player.mapa == 0) {
                if(chaveSpawn && pegando && CollisionObject(playerCollision, chaveCollision)){ 
                    PlaySound(pegandoItem);
                    chavePegandoFlag = true;
                }
               
                CollisionObject(playerCollision, pilarMapa0);
                CollisionObject(playerCollision, pilarEsqMapa0);
                
                if(CollisionObject(playerCollision, doorCollision)) {
                    player.x = 200;
                    player.y = 540;
                    player.mapa = getNextMapaPrincipal(head);
                }
                
                if(CollisionObject(playerCollision, doorCollisionEsquerda)) {
                    player.x = 900;
                    player.y = 540;
                    player.mapa = 4;
                }

                if(CollisionObject(playerCollision, diamanteTesouroCollision) && !diamanteTesouroPegandoFlag && pegando && !diamanteTesouroNoBau && puzzle2Resolvido){  //Verifica se pode pegar
                        PlaySound(pegandoItem);
                        diamanteTesouroPegandoFlag = true;
                        diamanteTesouroSpawn = false;
                }
    
            } else if(player.mapa == 1) {
                
                CollisionObject(playerCollision, mesaMapa1);
                
                // Parte pra fazer mais lacaios aparecerem 
                if(lacaio.mapa == 1 && lacaio.vivo){
                    mapaBloqueado = true;
                }else{
                    mapaBloqueado = false;
                }
     
                if(puzzle2Resolvido && !lacaio3Adicionado){   //Vai aparecer o lacaio no mapa 1
                    PlaySound(lacaioSom);
                    lacaio.x = 0;
                    lacaio.mapa = 1;
                    lacaio.vida = 10;
                    lacaio.vivo = true;
                    lacaio3Adicionado = true;
                    deathSoundPlayed = false;
                }
                
                if(!mapaBloqueado){     // Não vai poder sar do mapa enquanto não matar o monstro
                    if(CollisionObject(playerCollision, doorCollisionEsquerda)) {
                        player.x = 920;
                        player.y = 540;
                        player.mapa = getAntMapaPrincipal(head);
                    }
                    else if(CollisionObject(playerCollision, doorCollision)){                     
                        player.x = 200;
                        player.y = 540;
                        player.mapa = getNextMapaPrincipal(head);
                    }
                }
            }
            else if(player.mapa == 2){
                CollisionObject(playerCollision, mesaMapa2);
                
                // Parte pra fazer mais lacaios aparecerem 
                if(lacaio.mapa == 2 && lacaio.vivo){
                    mapaBloqueado = true;
                }else{
                    mapaBloqueado = false;
                }
                
                if(!mapaBloqueado){     // Não vai poder sar do mapa enquanto não matar o monstro
                    if(CollisionObject(playerCollision, doorCollisionEsquerda)) {
                        player.x = 920;
                        player.y = 540;
                        player.mapa = getAntMapaPrincipal(head);
                    }

                }
                
                if(puzzle1Resolvido && !puzzle2Resolvido && CollisionObject(playerCollision, doorCollision)){
                    player.x = 100;
                    player.mapa = getNextMapaPrincipal(head);
                }
                               
                if(CollisionObject(playerCollision, arenaCollision)){
                     if(chavePegandoFlag) {     //Destrancou
                        PlaySound(destrancandoPorta);
                        chaveSpawn = false;
                        puzzleDesbloqueado = true;
                        pegando = false;
                        chavePegandoFlag = false;
                    }                                   
                    else if(puzzleDesbloqueado && !mapaBloqueado){
                        PlaySound(abrindoPorta);
                        if(lacaio.vivo)PlaySound(lacaioSom);
                        
                        player.x = 400;
                        player.y = 540;
                        player.mapa = getMapaSecundario(head);
                    }else if(mapaBloqueado){
                        PlaySound(portaTrancada);
                    }else{
                        PlaySound(portaTrancada);
                    }
                }

                if(puzzle2Resolvido && !lacaio2Adicionado){   //Vai aparecer o lacaio no mapa 2
                    PlaySound(lacaioSom);
                    lacaio.x = 1000;
                    lacaio.mapa = 2;
                    lacaio.vida = 10;
                    lacaio.vivo = true;
                    lacaio2Adicionado = true;
                    deathSoundPlayed = false;
                }
            } else if(player.mapa == 3) {
                if(CollisionObject(playerCollision, doorCollisionEsquerda)) {
                    player.x = 920;
                    player.y = 540;
                    player.mapa = getAntMapaPrincipal(head);
                }
                
                if(!glockDourada && glockDouradaSpawn && CollisionObject(playerCollision, glockDouradaCollision)){
                    PlaySound(getGlock);
                    glockDourada = true;
                    glockDouradaSpawn = false;
                }
            } else if(player.mapa == -1){
                CollisionObject(playerCollision, pilarEsqMapa3);
                CollisionObject(playerCollision, pilarMapa3);
                CollisionObject(playerCollision, pilarDirMapa3);
                
                if(CollisionObject(playerCollision, portaMapa3)){
                    player.mapa = getAntMapaSecundario(head);
                    player.x = 400;
                    player.y = 540;
                }
                
                if(!chaveTesouroNoBau){
                    if(!chaveTesouroPegandoFlag && pegando && !chaveTesouroNoBau && CollisionObject(playerCollision, chaveTesouroCollision)){  //Verifica se pode pegar
                        PlaySound(pegandoItem);
                        chaveTesouroPegandoFlag = true;
                        chaveTesouroSpawn = false;
                    }
                    
                    if(CollisionObject(playerCollision, bauTesouroCollision) && chaveTesouroPegandoFlag){                                      // Verifica se colocou a chave no bau
                        PlaySound(pegandoItem);
                        chaveTesouroSpawn = false;
                        chaveTesouroNoBau = true;
                        chaveTesouroPegandoFlag = false;
                        pegando = false;
                    }
                }
                // Espada
                
                if(!espadaTesouroNoBau){
                    if(CollisionObject(playerCollision, espadaTesouroCollision) && !espadaTesouroPegandoFlag && pegando && !espadaTesouroNoBau && chaveTesouroNoBau && puzzle1Resolvido){  //Verifica se pode pegar
                        PlaySound(pegandoItem);
                        espadaTesouroPegandoFlag = true;
                        espadaTesouroSpawn = false;
                    }
                    
                    if(CollisionObject(playerCollision, bauTesouroCollision) && espadaTesouroPegandoFlag){                                      // Verifica se colocou a espada no bau
                        PlaySound(pegandoItem);
                        espadaTesouroSpawn = false;
                        espadaTesouroNoBau = true;
                        espadaTesouroPegandoFlag = false;
                        pegando = false;
                    }
                }
                
                if(CollisionObject(playerCollision, bauTesouroCollision) && diamanteTesouroPegandoFlag && puzzle2Resolvido){    // Verifica se colocou o diamante no bau
                    StopSound(andando);
                    diamanteTesouroSpawn = false;
                    diamanteTesouroNoBau = true;
                    diamanteTesouroPegandoFlag = false;
                    pegando = false;
               }
                
               if(CollisionObject(playerCollision, bauTesouroCollision) && diamanteTesouroPegandoFlag && puzzle2Resolvido){    // Verifica se colocou o diamante no bau
                    StopSound(andando);
                    diamanteTesouroSpawn = false;
                    diamanteTesouroNoBau = true;
                    diamanteTesouroPegandoFlag = false;
                    pegando = false;
               }
                
            
                
            } 
            
            // ############################
            //  DESENHO DAS TEXTURAS/IMAGENS
            // ############################
            
            BeginDrawing();
            ClearBackground(RAYWHITE);
            
            // Só vai desenhar o item se estiver no mapa dele
            if(player.mapa == 4){
                //DrawTexture(mapa4, 0, 0, WHITE);
            }
            else if(player.mapa == 0) {
                DrawTexture(backgroundImage, 0, 0, WHITE);
                
                if(chaveSpawn && chavePegandoFlag == false) {
                    DrawTextureEx(chaveCenario, (Vector2){chave.x, chave.y}, 0.0f, 1.0f, WHITE);
                }
                
                //Diamante                 
                if(!diamanteTesouroNoBau){
                    
                    if(diamanteTesouroSpawn && !diamanteTesouroPegandoFlag){                             // Verifica se pode desenhar o diamante
                        if(!puzzle2Resolvido){
                        DrawTextureEx(diamante, (Vector2){60, 500},0.0f, 3.0f,WHITE);
                        }
                        else{
                        DrawTextureEx(diamanteFree, (Vector2){100, 500},0.0f, 3.0f,WHITE);    
                        }
                    }
                    
                }
                
                                    
                                    
                if(CollisionObject(playerCollision, diamanteTesouroCollision) && !puzzle2Resolvido && player.x < 200 ){
                        colidiu = true;
                }
                
                if(colidiu){
                     timerBalao += GetFrameTime();
                     DrawTextureEx(mensagem2, (Vector2){player.x + 30, player.y - 300}, 0.0f, 1.0f, WHITE);
                     
                     if(timerBalao >= 3){
                         timerBalao = 0.0f;
                         colidiu = false;
                     }
                }
            }
            else if(player.mapa == 1){
                DrawTexture(mapa1, 0, 0, WHITE);          
            }
            else if(player.mapa == 2){
                DrawTexture(mapa2, 0, 0, WHITE);
                
                if(CollisionObject(playerCollision, arenaCollision) && !chavePegandoFlag && !puzzleDesbloqueado ){
                    colidiu1 = true;
                }
                
                if(colidiu1){
                     timerBalao += GetFrameTime();
                     DrawTextureEx(mensagem1, (Vector2){player.x + 30, player.y - 300}, 0.0f, 3.0f, WHITE);
                     
                     if(timerBalao >= 3){
                         timerBalao = 0.0f;
                         colidiu1 = false;
                     }
                }
            
            } else if(player.mapa == 3){
                //DrawTexture(secret, 0,0, WHITE);
                Rectangle secretRec = { frameAtualSecret * larguraSecret, 0, larguraSecret, alturaFrameSecret};
                DrawTextureRec(secret, secretRec, (Vector2){0,0}, WHITE);
                                
                if(glockDouradaSpawn){
                    DrawTextureEx(glock, (Vector2){700, 570}, 0.0f, 1.0f, WHITE);
                }
              
            } else if(player.mapa == -1){
                DrawTexture(arena,0,0,WHITE);
                //Chave do tesouro 
                
                if(!chaveTesouroNoBau){
                    
                    if(chaveTesouroSpawn && !chavePegandoFlag){                                                                                 // Verifica se pode desenhar a chave
                        DrawTexture(chaveTesouro, 300, 400, WHITE);
                    }
                }
                // Espada
                
                if(!espadaTesouroNoBau){
                    if(espadaTesouroSpawn && !espadaTesouroPegandoFlag && chaveTesouroNoBau && puzzle1Resolvido){                               // Verifica se pode desenhar a espada
                        DrawTexture(espadaTesouro, 200, 200,WHITE);
                    }
                }
                
                //Bau
                if(!chaveTesouroNoBau){
                    DrawTextureEx(bau, (Vector2){900,270},0.0f, 4.0f,WHITE);
                }else if(chaveTesouroNoBau){
                    DrawTextureEx(bauPreenchido, (Vector2){900,270},0.0f, 4.0f,WHITE);
                }
                
                if(espadaTesouroNoBau){
                    DrawTextureEx(bauPreenchido2, (Vector2){900,270},0.0f, 4.0f,WHITE);
                }
                
                if(diamanteTesouroNoBau){
                    DrawTextureEx(bauPreenchido3, (Vector2){900,270},0.0f, 4.0f,WHITE);
                }
                
                // Botao 1 vermelho
                if(chaveTesouroNoBau){
                    if (CheckCollisionPointRec(GetMousePosition(), botao1Collision) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        botao1Pressionado = !botao1Pressionado;
                    }
                    if(!botao1Pressionado && !puzzle1Resolvido){
                        DrawTextureEx(botao1Off, (Vector2){130,350},0.0f, 4.0f,WHITE);
                       
                    }else if(botao1Pressionado){
                        DrawTextureEx(botao1On, (Vector2){130,350},0.0f, 4.0f,WHITE);
                        puzzle1Resolvido = puzzleOrdenar(puzzle1, musica);
                        botao1Pressionado = false;
                    }
                }
                 
                //botao verde
                if (espadaTesouroNoBau) {
                    if (CheckCollisionPointRec(GetMousePosition(), botao2Collision) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        botao2Pressionado = !botao2Pressionado; 
                    }
                    if (!botao2Pressionado && !puzzle2Resolvido) {
                        DrawTextureEx(botao2Off, (Vector2){400, 350}, 0.0f, 4.0f, WHITE);
                    } else if (botao2Pressionado) {
                        DrawTextureEx(botao2On, (Vector2){400, 350}, 0.0f, 4.0f, WHITE);
                        puzzle2Resolvido = puzzleIcons(puzzle2, musica); //tem que implementar a funçaõ do puzzle dois
                        botao2Pressionado = false;
                    }
                }
                             
            }
            
            //Verificacao chave
            if(chaveTesouroPegandoFlag && !pegando && !chaveTesouroNoBau){                                         //Verifica se deixou a chave cair 
                    chaveTesouroPegandoFlag = false;
                    chaveTesouroSpawn = true;
                }
            //Verificacao espada
            if(espadaTesouroPegandoFlag && !pegando && !espadaTesouroNoBau){                                         //Verifica se deixou a espada cair 
                    espadaTesouroPegandoFlag = false;
                    espadaTesouroSpawn = true;
                }
            //Verificacao do diamante
            if(diamanteTesouroPegandoFlag && !pegando && !diamanteTesouroNoBau){                                         //Verifica se deixou o diamante cair 
                    diamanteTesouroPegandoFlag = false;
                    diamanteTesouroSpawn = true;
                }
            
                            
            delayBala += 1;
            // Atualiza a posição do projétil em cada frame se ele estiver ativo
            if (projetil.ativo) {
                projetil.posicao.x += projetil.velocidade.x;
                projetil.posicao.y += projetil.velocidade.y;

                // Verifique se o projétil saiu da tela e desativa
                if (projetil.posicao.x > GetScreenWidth() || projetil.posicao.x < 0 ||
                    projetil.posicao.y > GetScreenHeight() || projetil.posicao.y < 0) {
                    projetil.ativo = false;
                }
                
                if ((projetil.posicao.x > lacaio.x && projetil.posicao.x < lacaio.x + 200) && (projetil.posicao.y > lacaio.y && projetil.posicao.y < lacaio.y + 150) && lacaio.vivo && lacaio.mapa == player.mapa) {
                    
                    
                    projetil.ativo = false;
                    lacaioAtingido = true;
                    
                    if(glockDourada){
                        lacaio.vida -= 3;
                    }else{
                        lacaio.vida -= 1;
                    }
                        
                    if(lacaio.vida <= 0){
                        lacaio.vivo = false;
                        deathFlag = true;
                    }
                }
                

                // Desenhe o projétil na posição atual
                if(glockDourada){
                    DrawTextureEx(bala, projetil.posicao, 0.0f, 0.5f, WHITE); 
                }else{
                    DrawTextureEx(bala, projetil.posicao, 0.0f, 0.25f, WHITE);  
                }

                if(delayBala >= 5){
                    delayBala = 0;
                    if(lacaioIndoDireita){
                        lacaio.x -= 15;
                    }else{
                        lacaio.x += 15;
                    }
                }
            }
            
            //###########################################
            //################BOSS#######################
            //###########################################
            
            if(boss.vivo && boss.mapa == player.mapa){
                if(bossAtack1Flag ){
                    if(bossIndoDireita){
                        Rectangle sourceRec1 = { atualBossAtack1 * larguraBossAtack1, 0, larguraBossAtack1, alturaBossAtack1};
                        DrawTextureRec(bossAtack1, sourceRec1, (Vector2){boss.x,boss.y}, WHITE);
                    }else{
                        Rectangle sourceRec1 = { atualBossAtack1 * larguraBossAtack1, 0, larguraBossAtack1, alturaBossAtack1};
                        DrawTextureRec(bossAtack1Left, sourceRec1, (Vector2){boss.x,boss.y}, WHITE);
                    }
                }else if(bossAtack2Flag ){
                    if(bossIndoDireita){
                        Rectangle sourceRec2 = { atualBossAtack2 * larguraBossAtack2, 0, larguraBossAtack2, alturaBossAtack2};
                        DrawTextureRec(bossAtack2, sourceRec2, (Vector2){boss.x,boss.y}, WHITE);
                    }else{
                        Rectangle sourceRec2 = { atualBossAtack2 * larguraBossAtack2, 0, larguraBossAtack2, alturaBossAtack2};
                        DrawTextureRec(bossAtack2Left, sourceRec2, (Vector2){boss.x,boss.y}, WHITE);
                    }
                }else{
                    if(bossIndoDireita){
                        Rectangle sourceRec = { atualBossIdle * larguraBossIdle, 0, larguraBossIdle, alturaBossIdle};
                        DrawTextureRec(bossIdle, sourceRec, (Vector2){boss.x,boss.y}, WHITE);
                    }
                    else{
                        Rectangle sourceRec = { atualBossIdle * larguraBossIdle, 0, larguraBossIdle, alturaBossIdle};
                        DrawTextureRec(bossIdleLeft, sourceRec, (Vector2){boss.x,boss.y}, WHITE);
                    }
                    
                }
            }
            //lacaio
            if(lacaio.mapa == player.mapa && lacaio.vivo){
                
                //Sombra lacaio
                if(!lacaioIndoDireita) DrawTextureEx(shadow, (Vector2){lacaio.x + 70, lacaio.y + 210}, 0.0f, 1.4f, WHITE);
                else if(lacaioIndoDireita)DrawTextureEx(shadow, (Vector2){lacaio.x + 30, lacaio.y + 210}, 0.0f, 1.4f, WHITE);
                //#####
                
                if(lacaioAtingido){
                    if(lacaioIndoDireita){
                        Rectangle sourceReclacaioDano = { frameAtuallacaioDano * larguraFramelacaioDano, 0, larguraFramelacaioDano, alturaFramelacaioDano};
                        DrawTextureRec(danoLacaioDireita, sourceReclacaioDano, (Vector2){lacaio.x, lacaio.y}, WHITE);
                    }
                    else{
                        Rectangle sourceReclacaioDano = { frameAtuallacaioDano * larguraFramelacaioDano, 0, larguraFramelacaioDano, alturaFramelacaioDano};
                        DrawTextureRec(danoLacaioEsquerda, sourceReclacaioDano, (Vector2){lacaio.x, lacaio.y}, WHITE);
                    }
                }
                 else if(lacaioIndoDireita){
                     if(lacaioAtacando){
                        Rectangle sourceReclacaioAtaque = { frameAtuallacaioAtaque * larguraFramelacaioAtaque, 0, larguraFramelacaioAtaque, alturaFramelacaioAtaque};
                        DrawTextureRec(lacaioAtaqueDireita, sourceReclacaioAtaque, (Vector2){lacaio.x, lacaio.y}, WHITE);   
                     }else{
                        Rectangle sourceReclacaioWalk = { frameAtuallacaioWalk * larguraFramelacaioWalk, 0, larguraFramelacaioWalk, alturaFramelacaioWalk};
                        DrawTextureRec(lacaioDireita, sourceReclacaioWalk, (Vector2){lacaio.x, lacaio.y}, WHITE);
                     }
                } else if(!lacaioIndoDireita){
                    if(lacaioAtacando){
                        Rectangle sourceReclacaioAtaque = { frameAtuallacaioAtaque * larguraFramelacaioAtaque, 0, larguraFramelacaioAtaque, alturaFramelacaioAtaque};
                        DrawTextureRec(lacaioAtaqueEsquerda, sourceReclacaioAtaque, (Vector2){lacaio.x, lacaio.y}, WHITE);    
                     }else{
                        Rectangle sourceReclacaioWalk = { frameAtuallacaioWalk * larguraFramelacaioWalk, 0, larguraFramelacaioWalk, alturaFramelacaioWalk};
                        DrawTextureRec(lacaioEsquerda, sourceReclacaioWalk, (Vector2){lacaio.x, lacaio.y}, WHITE);
                   }
                }
            }else if(lacaio.mapa == player.mapa && deathFlag){
                if(lacaioIndoDireita){
                        DrawTextureEx(shadow, (Vector2){lacaio.x + 30, lacaio.y + 210}, 0.0f, 1.4f, WHITE);                    
                        Rectangle sourceDeath = { frameAtualDeath * larguraDeath, 0, larguraDeath, alturaFrameDeath};
                        DrawTextureRec(death, sourceDeath, (Vector2){lacaio.x, lacaio.y}, WHITE);
                }else{
                        DrawTextureEx(shadow, (Vector2){lacaio.x + 70, lacaio.y + 210}, 0.0f, 1.4f, WHITE);  
                        Rectangle sourceDeath = { frameAtualDeath * larguraDeath, 0, larguraDeath, alturaFrameDeath};
                        DrawTextureRec(deathLeft, sourceDeath, (Vector2){lacaio.x, lacaio.y}, WHITE);
                }
            }
                   
            //Potion
            
            Rectangle potionCollision = {1000, 600, 70, 70};
            if(lacaio2Adicionado && !lacaio.vivo && player.mapa == 2 && !curou){
                Rectangle sourcePotion = { frameAtualPotion * larguraPotion, 0, larguraPotion, alturaFramePotion};
                DrawTextureRec(potion, sourcePotion, (Vector2){1000, 600}, WHITE);
                
                if(CollisionObject(playerCollision, potionCollision)) {
                    PlaySound(heal);
                    curou = true;
                    player.vida ++;}
            }
              
              
            if(player.vivo){
                //Sombra
                if(personagemParado && pegando && !andandoDireita) DrawTextureEx(shadow, (Vector2){player.x + 85, player.y + 70}, 0.0f, 1.0f, WHITE);
                
                else if(!personagemParado && pegando && !andandoDireita) DrawTextureEx(shadow, (Vector2){player.x + 70, player.y + 75}, 0.0f, 1.0f, WHITE);
                
                else if(atirou && !andandoDireita) DrawTextureEx(shadow, (Vector2){player.x + 85, player.y + 70}, 0.0f, 1.0f, WHITE);
                
                else if(personagemParado) DrawTextureEx(shadow, (Vector2){player.x - 15, player.y + 70}, 0.0f, 1.0f, WHITE);
                
                else if(!personagemParado)DrawTextureEx(shadow, (Vector2){player.x + 10, player.y + 75}, 0.0f, 1.0f, WHITE); 
 
                // Verifica se o personagem está no estado de "pegando" e a direção em que ele está olhando
                if(levandoDanoFlag){
                    Rectangle sourceRecLevandoDano = { frameAtualLevandoDano * larguraFrameLevandoDano, 0, larguraFrameLevandoDano, alturaFrameLevandoDano};
                    DrawTextureRec(levandoDano, sourceRecLevandoDano, (Vector2){player.x, player.y - playerOffSet}, WHITE);
                }
                else if (pegando) {
                    
                    if(personagemParado && andandoDireita && !segurandoItem){   // Personagem totalmente parado
                        Rectangle sourceRecIdle = { frameAtualIdle * larguraFrameIdle, 0, larguraFrameIdle, alturaFrameIdle};
                        DrawTextureRec(pegandoDireitaIdle, sourceRecIdle, (Vector2){player.x, player.y - playerOffSet}, WHITE);
                    }else if(personagemParado && !segurandoItem){
                        Rectangle sourceRecIdle = { frameAtualIdle * larguraFrameIdle, 0, larguraFrameIdle, alturaFrameIdle};
                        DrawTextureRec(pegandoEsquerdaIdle, sourceRecIdle, (Vector2){player.x, player.y - playerOffSet}, WHITE);
                    }
                    
                    else if(chavePegandoFlag && andandoDireita) { // primeira chave
                        if(personagemParado){
                            Rectangle sourceRecIdle = { frameAtualIdle * larguraFrameIdle, 0, larguraFrameIdle, alturaFrameIdle};
                            DrawTextureRec(pegandoChaveDireitaIdle, sourceRecIdle, (Vector2){player.x, player.y - playerOffSet}, WHITE);
                        }else{
                            Rectangle sourceRecKey = { frameAtualKey * larguraFrameKey, 0, larguraFrameKey, alturaFrameKey };
                            DrawTextureRec(pegandoChaveDireita, sourceRecKey, (Vector2){player.x, player.y - playerOffSet}, WHITE);
                        }
                    } else if(chavePegandoFlag) {               
                        if(personagemParado){
                            Rectangle sourceRecIdle = { frameAtualIdle * larguraFrameIdle, 0, larguraFrameIdle, alturaFrameIdle};
                            DrawTextureRec(pegandoChaveEsquerdaIdle, sourceRecIdle, (Vector2){player.x, player.y - playerOffSet}, WHITE);
                        }else{
                            Rectangle sourceRecKey = {  frameAtualKey * larguraFrameKey, 0, larguraFrameKey, alturaFrameKey};
                            DrawTextureRec(pegandoChaveEsquerda, sourceRecKey, (Vector2){player.x, player.y - playerOffSet}, WHITE);
                        }
                    }
     
                    else if(chaveTesouroPegandoFlag && andandoDireita) {    // Chave do tesouro
                        if(personagemParado){
                            Rectangle sourceRecIdle = { frameAtualIdle * larguraFrameIdle, 0, larguraFrameIdle, alturaFrameIdle};
                            DrawTextureRec(pegandoChaveTesouroDireitaIdle, sourceRecIdle, (Vector2){player.x, player.y - playerOffSet}, WHITE);
                        }else{
                            Rectangle sourceRecKey = { frameAtualKey * larguraFrameKey, 0, larguraFrameKey, alturaFrameKey };
                            DrawTextureRec(pegandoChaveTesouroDireita, sourceRecKey, (Vector2){player.x, player.y - playerOffSet}, WHITE);
                        }
                    } else if(chaveTesouroPegandoFlag) {
                        if(personagemParado){
                            Rectangle sourceRecIdle = { frameAtualIdle * larguraFrameIdle, 0, larguraFrameIdle, alturaFrameIdle};
                            DrawTextureRec(pegandoChaveTesouroEsquerdaIdle, sourceRecIdle, (Vector2){player.x, player.y - playerOffSet}, WHITE);
                        }else{
                            Rectangle sourceRecKey = { frameAtualKey * larguraFrameKey, 0, larguraFrameKey, alturaFrameKey };
                            DrawTextureRec(pegandoChaveTesouroEsquerda, sourceRecKey, (Vector2){player.x, player.y - playerOffSet}, WHITE);
                        }
                    }
                    
                    else if(espadaTesouroPegandoFlag && andandoDireita) {  // Espada
                        if(personagemParado){
                            Rectangle sourceRecIdle = { frameAtualIdle * larguraFrameIdle, 0, larguraFrameIdle, alturaFrameIdle};
                            DrawTextureRec(pegandoEspadaDireitaIdle, sourceRecIdle, (Vector2){player.x, player.y - playerOffSet}, WHITE);
                        }else{
                            Rectangle sourceRecKey = { frameAtualKey * larguraFrameKey, 0, larguraFrameKey, alturaFrameKey };
                            DrawTextureRec(pegandoEspadaDireita, sourceRecKey, (Vector2){player.x, player.y - playerOffSet}, WHITE);
                        }
                    } else if(espadaTesouroPegandoFlag) {
                         if(personagemParado){
                            Rectangle sourceRecIdle = { frameAtualIdle * larguraFrameIdle, 0, larguraFrameIdle, alturaFrameIdle};
                            DrawTextureRec(pegandoEspadaEsquerdaIdle, sourceRecIdle, (Vector2){player.x, player.y - playerOffSet}, WHITE);
                        }else{
                            Rectangle sourceRecKey = { frameAtualKey * larguraFrameKey, 0, larguraFrameKey, alturaFrameKey };
                            DrawTextureRec(pegandoEspadaEsquerda, sourceRecKey, (Vector2){player.x, player.y - playerOffSet}, WHITE);
                        }
                    } 
                    
                    else if(diamanteTesouroPegandoFlag && andandoDireita) { 
                        if(personagemParado){
                            Rectangle sourceRecIdle = { frameAtualIdle * larguraFrameIdle, 0, larguraFrameIdle, alturaFrameIdle};
                            DrawTextureRec(pegandoDiamanteDireitaIdle, sourceRecIdle, (Vector2){player.x, player.y - playerOffSet}, WHITE);
                        }else{
                            Rectangle sourceRecKey = { frameAtualKey * larguraFrameKey, 0, larguraFrameKey, alturaFrameKey };
                            DrawTextureRec(pegandoDiamanteDireita, sourceRecKey, (Vector2){player.x, player.y - playerOffSet}, WHITE);  //Diamante tesouro
                        }
                    } else if(diamanteTesouroPegandoFlag) {
                        if(personagemParado){
                            Rectangle sourceRecIdle = { frameAtualIdle * larguraFrameIdle, 0, larguraFrameIdle, alturaFrameIdle};
                            DrawTextureRec(pegandoDiamanteEsquerdaIdle, sourceRecIdle, (Vector2){player.x, player.y - playerOffSet}, WHITE);
                        }else{
                            Rectangle sourceRecKey = { frameAtualKey * larguraFrameKey, 0, larguraFrameKey, alturaFrameKey };
                            DrawTextureRec(pegandoDiamanteEsquerda, sourceRecKey, (Vector2){player.x, player.y - playerOffSet}, WHITE); 
                        }
                    }
                    
                    else if (andandoDireita) {  // Andando e pegando normal 
                        Rectangle sourceRecKey = { frameAtualKey * larguraFrameKey, 0, larguraFrameKey, alturaFrameKey };
                        DrawTextureRec(personagemPegando, sourceRecKey, (Vector2){player.x, player.y - playerOffSet}, WHITE); 
                    } else {
                        Rectangle sourceRecKey = { frameAtualKey * larguraFrameKey, 0, larguraFrameKey, alturaFrameKey };
                        DrawTextureRec(personagemPegandoEsquerda, sourceRecKey, (Vector2){player.x, player.y - playerOffSet}, WHITE); 
                    }
                    
                    
                } else if(!personagemParado) {  //Personagem andando normal
                    if (andandoDireita) {
                        Rectangle sourceRecWalk = { frameAtualWalk * larguraFrameWalk, 0, larguraFrameWalk, alturaFrameWalk };
                        DrawTextureRec(spriteWalkRight, sourceRecWalk, (Vector2){player.x, player.y - playerOffSet}, WHITE);
                    } else {
                        Rectangle sourceRecWalk = { frameAtualWalk * larguraFrameWalk, 0, larguraFrameWalk, alturaFrameWalk };
                        DrawTextureRec(spriteWalkLeft, sourceRecWalk, (Vector2){player.x, player.y - playerOffSet}, WHITE);
                    }
                } else {                        // Personagem parado 
                    if(andandoDireita){      
                        if(atirou && !segurandoItem && !pegando){
                            if(glockDourada){
                                Rectangle sourceRecKey = { frameAtualTiro * larguraFrameTiro, 0, larguraFrameTiro, alturaFrameTiro };
                                DrawTextureRec(atirandoDireitaDourado, sourceRecKey, (Vector2){player.x, player.y - playerOffSet}, WHITE);
                            }else{
                                Rectangle sourceRecKey = { frameAtualTiro * larguraFrameTiro, 0, larguraFrameTiro, alturaFrameTiro };
                                DrawTextureRec(atirandoDireita, sourceRecKey, (Vector2){player.x, player.y - playerOffSet}, WHITE);
                            }
                        }else{
                            Rectangle sourceRec = { frameAtual * larguraFrame, 0, larguraFrame, alturaFrame };
                            DrawTextureRec(spritesheetRight, sourceRec, (Vector2){player.x, player.y-playerOffSet}, WHITE);
                        }
                    }else{
                        if(atirou && !segurandoItem && !pegando){
                            if(glockDourada){
                                Rectangle sourceRecKey = { frameAtualTiro * larguraFrameTiro, 0, larguraFrameTiro, alturaFrameTiro };
                                DrawTextureRec(atirandoEsquerdaDourado, sourceRecKey, (Vector2){player.x, player.y - playerOffSet}, WHITE);
                            }else{
                                Rectangle sourceRecKey = { frameAtualTiro * larguraFrameTiro, 0, larguraFrameTiro, alturaFrameTiro };
                                DrawTextureRec(atirandoEsquerda, sourceRecKey, (Vector2){player.x, player.y - playerOffSet}, WHITE);
                            }
                        }else{
                            Rectangle sourceRec = { frameAtual * larguraFrame, 0, larguraFrame, alturaFrame };
                            DrawTextureRec(spritesheet, sourceRec, (Vector2){player.x, player.y-playerOffSet}, WHITE);
                        }
                    }
                }
            }    
  
            //Fogo
            if(mapaBloqueado){
                Rectangle sourceFogo = { frameAtualFogo * larguraFogo, 0, larguraFogo, alturaFrameFogo};
                DrawTextureRec(fogo, sourceFogo, (Vector2){-50, 480}, WHITE);
                DrawTextureRec(fogo, sourceFogo, (Vector2){1180, 480}, WHITE);
            }
  
            // ############################
            //  UI/Interface
            // ############################
            if(player.vida == 5) DrawTextureEx(vida5, (Vector2){20,20}, 0.0f, 5.0f, WHITE);
            else if(player.vida == 4) DrawTextureEx(vida4, (Vector2){20,20}, 0.0f, 5.0f, WHITE);
            else if(player.vida == 3) DrawTextureEx(vida3, (Vector2){20,20}, 0.0f, 5.0f, WHITE);
            else if(player.vida == 2) DrawTextureEx(vida2, (Vector2){20,20}, 0.0f, 5.0f, WHITE);
            else if(player.vida == 1) DrawTextureEx(vida1, (Vector2){20,20}, 0.0f, 5.0f, WHITE);
            
            //UI lacaio
            if(lacaio.mapa == player.mapa){
            if(lacaio.vida == 10) DrawTextureEx(lacaioVida10, (Vector2){700, 20}, 0.0f, 5.0f, WHITE);
            else if(lacaio.vida == 9) DrawTextureEx(lacaioVida9, (Vector2){700, 20}, 0.0f, 5.0f, WHITE);
            else if(lacaio.vida == 8) DrawTextureEx(lacaioVida8, (Vector2){700, 20}, 0.0f, 5.0f, WHITE);
            else if(lacaio.vida == 7) DrawTextureEx(lacaioVida7, (Vector2){700, 20}, 0.0f, 5.0f, WHITE);
            else if(lacaio.vida == 6) DrawTextureEx(lacaioVida6, (Vector2){700, 20}, 0.0f, 5.0f, WHITE);
            else if(lacaio.vida == 5) DrawTextureEx(lacaioVida5, (Vector2){700, 20}, 0.0f, 5.0f, WHITE);
            else if(lacaio.vida == 4) DrawTextureEx(lacaioVida4, (Vector2){700, 20}, 0.0f, 5.0f, WHITE);
            else if(lacaio.vida == 3) DrawTextureEx(lacaioVida3, (Vector2){700, 20}, 0.0f, 5.0f, WHITE);
            else if(lacaio.vida == 2) DrawTextureEx(lacaioVida2, (Vector2){700, 20}, 0.0f, 5.0f, WHITE);
            else if(lacaio.vida == 1) DrawTextureEx(lacaioVida1, (Vector2){700, 20}, 0.0f, 5.0f, WHITE);
            }
            
           if(espadaTesouroNoBau){
                DrawText("Recupere os tesouros escondidos",30, 80, 20, GREEN);
                DrawTextureEx(achouChave, (Vector2){10, 100}, 0.0f, 1.0f, WHITE );
                DrawTextureEx(achouEspadas, (Vector2){80, 100}, 0.0f, 1.0f, WHITE );
                DrawTextureEx(procuraDiamante, (Vector2){150, 100}, 0.0f, 1.0f, WHITE );
           }else if(chaveTesouroNoBau){
                DrawText("Recupere os tesouros escondidos",30, 80, 20, GREEN);
                DrawTextureEx(achouChave, (Vector2){10, 100}, 0.0f, 1.0f, WHITE );
                DrawTextureEx(procuraEspadas, (Vector2){80, 100}, 0.0f, 1.0f, WHITE );
                DrawTextureEx(procuraDiamante, (Vector2){150, 100}, 0.0f, 1.0f, WHITE );
           }else{
                DrawText("Recupere os tesouros escondidos",30, 80, 20, GREEN);
                DrawTextureEx(procuraChave, (Vector2){10, 100}, 0.0f, 1.0f, WHITE );
                DrawTextureEx(procuraEspadas, (Vector2){80, 100}, 0.0f, 1.0f, WHITE );
                DrawTextureEx(procuraDiamante, (Vector2){150, 100}, 0.0f, 1.0f, WHITE );
           }
            
            char pontuacaoTexto[100];
            if(bossAtack1Flag){
                sprintf(pontuacaoTexto, "TRUE atual: %d ", atualBossAtack1);
            }
            else {sprintf(pontuacaoTexto, "FALSE atual: %d ", atualBossAtack1);
            }
            DrawText(pontuacaoTexto, 500,500,40,WHITE);
            EndDrawing();
        }

        //tela abse de vitoria do jogo
        if(diamanteTesouroNoBau){ 
            
            PlaySound(end);
            while(!IsKeyPressed(KEY_ENTER)){
                 BeginDrawing();
                ClearBackground(BLACK);
                DrawText("OBRIGADO POR JOGAR!",445, 40, 30, GREEN);
                
                char pontuacaoTexto[100];
                sprintf(pontuacaoTexto, "TEMPO: %d SERKS", pontuacao);

                DrawText(pontuacaoTexto,1050, 40, 20, GREEN);
                
                DrawText("PROGRAMAÇÃO E DESIGN:",200, 100, 20, GREEN);
                DrawText("FELIPE MATIAS: ",320, 150, 20, YELLOW);
                DrawText("PERSONAGENS, ANIMAÇÕES, CENÁRIOS, ITENS, UI ",515, 150, 20, GREEN);
                DrawText("SISTEMA DE LUTA E INTELIGENCIA DOS INIMIGOS ",515, 200, 20, GREEN);
                DrawText("IMPLEMENTAÇÃO DE EFEITOS SONOROS E MÚSICAS",515, 250, 20, GREEN);
                
                DrawText("IMPLEMENTAÇÃO DE FUNCIONALIDADES E COLISÃO: ",200, 310, 20, GREEN);
                
                DrawText("LUCAS FERREIRA: ",320, 360, 20, YELLOW);
                DrawText("LÓGICA DOS MAPAS COM LISTA DUPLAMENTE ENCADEADA",515, 360, 20, GREEN);
                
                DrawText("PEDRO SAMPAIO: ",320, 410 , 20, YELLOW);   
                DrawText("DESENVOLVIMENTO DOS PUZZLES E MENU",515, 410, 20, GREEN);
                
                DrawText("GABRIEL LANDIM: ",320, 460, 20, YELLOW);
                DrawText("DESENVOLVIMENTO DOS PUZZLES E MENU",515, 460, 20, GREEN); 
                
                DrawText("DESIGN DE ÁUDIO: ",200, 510, 20, GREEN);
                
                DrawText("FELIPE FRANÇA: ",320, 560, 20, YELLOW);
                DrawText("EFEITOS SONOROS E MÚSICA: ",515, 560, 20, GREEN);
                
                DrawText("APOIO E COLABORAÇÃO",200, 610, 20, GREEN);
                
                DrawText("LUIS GUSTAVO: ",320, 660, 20, YELLOW);
                DrawText("APOIO AO TIME E FEEDBACK DURANTE O DESENVOLVIMENTO",515, 660, 20, GREEN);
                EndDrawing();
            }
            
            //Chama o menu e mostra a introdução novamente
            StopSound(end);
            menu(backgroundMenu);
            ShowIntro();
            ShowLoading();
        }//tela abse de gameover
        if (!player.vivo) {
            // Exibe a tela de "Game Over" e aguarda o jogador pressionar Enter
            
            Texture2D gameOver = LoadTexture("ui/gameOver.png");
            Sound gameOverSound = LoadSound("audios/gameOver.mp3");
            PlaySound(gameOverSound);
            while (!IsKeyPressed(KEY_ENTER)) {
                BeginDrawing();
                DrawTexture(gameOver, 0, 0, WHITE);
                DrawText("GAME OVER!", 520, 310, 30, RED);
                DrawText("Pressione ENTER para reiniciar", 450, 350, 20, WHITE);
                EndDrawing();
            }
            
            StopSound(gameOverSound);
            //Chama o menu novamente
            menu(backgroundMenu);
            ShowIntro();
            ShowLoading();
    }
                
        if (pausa) {
            Rectangle retomar = {500, 400, 250, 50};
            Rectangle sair = {500, 450, 250, 50};
            BeginDrawing();
            DrawTexture(menuBack, 10, 50 , WHITE );
            DrawText("Retomar", 510, 400, 30, GOLD);
            DrawText("Sair do jogo", 510, 450, 30, GOLD);
            DrawText("PAUSE", 500, 290, 70, GOLD);
            EndDrawing();
            
            if (CheckCollisionPointRec(GetMousePosition(), retomar) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                  pausa = false;
            }
            
            if (CheckCollisionPointRec(GetMousePosition(), sair) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                  CloseWindow();
            }
        }
    }
}

int main(void) {
    int larguraTela = 1280, alturaTela = 720;
        
    InitWindow(larguraTela, alturaTela, "Um dia no Castelo");
    SetTargetFPS(60);
    
    ToggleFullscreen();

    InitAudioDevice();
    
    Texture2D backgroundImage = LoadTexture("cenario/background.png");

    Texture2D personagemPegando = LoadTexture("Imagens/personagemPegando.png");
    Texture2D personagemPegandoEsquerda = LoadTexture("Imagens/personagemPegandoEsquerda.png");
    
    Texture2D chaveCenario = LoadTexture("cenario/chaveCenario.png");
    
    Texture2D pegandoChaveDireita = LoadTexture("pegandoItens/personagemPegandoChaveDireita.png");
    Texture2D pegandoChaveEsquerda = LoadTexture("pegandoItens/personagemPegandoChaveEsquerda.png");
    
    Texture2D mapa1 = LoadTexture("cenario/mapa1.png");
    Texture2D mapa2 = LoadTexture("cenario/mapa2.png");
    
    Texture2D arena = LoadTexture("cenario/arena.png");
    
    Texture2D mensagem1 = LoadTexture("mensagens/mensagem1.png");
    
    Texture2D menuBack = LoadTexture("cenario/menuBack.png");
    
    Texture2D espadaTesouro = LoadTexture("tesouro/espada.png");
    Texture2D chaveTesouro = LoadTexture("tesouro/chaveTesouro.png");
    Texture2D bau = LoadTexture("tesouro/bau.png");
    Texture2D bauPreenchido = LoadTexture("tesouro/bauPreenchido.png");
    
    Texture2D botao1Off = LoadTexture("botoes/botao1Off.png");
    Texture2D botao1On = LoadTexture("botoes/botao1On.png");
    Texture2D botao2Off = LoadTexture("botoes/botao2Off.png");
    Texture2D botao2On = LoadTexture("botoes/botao2On.png");
    
    Texture2D pegandoChaveTesouroEsquerda = LoadTexture("pegandoItens/personagemPegandoChaveTesouroEsquerda.png");
    Texture2D pegandoChaveTesouroDireita = LoadTexture("pegandoItens/personagemPegandoChaveTesouroDireita.png");
    
    Texture2D pegandoEspadaEsquerda = LoadTexture("pegandoItens/personagemPegandoEspadaEsquerda.png");
    Texture2D pegandoEspadaDireita = LoadTexture("pegandoItens/personagemPegandoEspadaDireita.png");
    
    Texture2D bauPreenchido2 = LoadTexture("tesouro/bauPreenchido2.png");
    
    Texture2D diamante = LoadTexture("tesouro/diamante.png");
    Texture2D pegandoDiamanteEsquerda = LoadTexture("pegandoItens/personagemPegandoDiamanteEsquerda.png");
    Texture2D pegandoDiamanteDireita = LoadTexture("pegandoItens/personagemPegandoDiamanteDireita.png");
    Texture2D bauPreenchido3 = LoadTexture("tesouro/bauPreenchido3.png");
    
    Texture2D puzzle1 = LoadTexture("botoes/puzzle1.png");
    Texture2D puzzle2 = LoadTexture("botoes/puzzle2.png");
    
    Texture2D spritesheet = LoadTexture("Imagens/spritesheet.png");
    Texture2D spritesheetRight = LoadTexture("Imagens/spritesheetRight.png");
    
    Texture2D spriteWalkLeft = LoadTexture("Imagens/spriteWalkLeft.png");
    Texture2D spriteWalkRight = LoadTexture("Imagens/spriteWalkRight.png");
    
    Texture2D pegandoEsquerdaIdle = LoadTexture("Imagens/personagemPegandoIdleEsquerda.png");
    Texture2D pegandoDireitaIdle = LoadTexture("Imagens/personagemPegandoIdleDireita.png");
    
    Texture2D backgroundMenu = LoadTexture("Imagens/menu.png");
    
    Texture2D pegandoChaveEsquerdaIdle = LoadTexture("pegandoItens/personagemPegandoChaveEsquerdaIdle.png");
    Texture2D pegandoChaveDireitaIdle = LoadTexture("pegandoItens/personagemPegandoChaveDireitaIdle.png");
    Texture2D pegandoChaveTesouroEsquerdaIdle = LoadTexture("pegandoItens/personagemPegandoChaveTesouroEsquerdaIdle.png");
    Texture2D pegandoChaveTesouroDireitaIdle = LoadTexture("pegandoItens/personagemPegandoChaveTesouroDireitaIdle.png");
    Texture2D pegandoEspadaEsquerdaIdle = LoadTexture("pegandoItens/personagemPegandoEspadaEsquerdaIdle.png");
    Texture2D pegandoEspadaDireitaIdle = LoadTexture("pegandoItens/personagemPegandoEspadaDireitaIdle.png");
    Texture2D pegandoDiamanteEsquerdaIdle = LoadTexture("pegandoItens/personagemPegandoDiamanteEsquerdaIdle.png");
    Texture2D pegandoDiamanteDireitaIdle = LoadTexture("pegandoItens/personagemPegandoDiamanteDireitaIdle.png");

    Texture2D bala = LoadTexture("Imagens/bala.png");
    
    Texture2D danoLacaioEsquerda =LoadTexture("lacaio/DanoEsquerda.png");
    Texture2D danoLacaioDireita = LoadTexture("lacaio/DanoDireita.png");
    
    Texture2D diamanteFree = LoadTexture("tesouro/diamanteFree.png");
    
    Texture2D atirandoDireita = LoadTexture("Imagens/atirandoDireita.png");
    Texture2D atirandoEsquerda = LoadTexture("Imagens/atirandoEsquerda.png");
    
    Texture2D lacaioDireita = LoadTexture("lacaio/LacaioWalkRight.png");
    Texture2D lacaioEsquerda = LoadTexture("lacaio/LacaioWalkLeft.png");
    
    Texture2D lacaioAtaqueDireita = LoadTexture("lacaio/AtaqueDireita.png");
    Texture2D lacaioAtaqueEsquerda = LoadTexture("lacaio/AtaqueEsquerda.png");
    
    Texture2D levandoDano = LoadTexture("Imagens/personagemLevandoDano.png");
    
    Texture2D mensagem2 = LoadTexture("mensagens/mensagem2.png");
    
    Texture2D vida1 = LoadTexture("ui/vida1.png");
    Texture2D vida2 = LoadTexture("ui/vida2.png");
    Texture2D vida3 = LoadTexture("ui/vida3.png");
    Texture2D vida4 = LoadTexture("ui/vida4.png");
    Texture2D vida5 = LoadTexture("ui/vida5.png");
    
    Texture2D lacaioVida1 = LoadTexture("ui/lacaioVida1.png");
    Texture2D lacaioVida2 = LoadTexture("ui/lacaioVida2.png");
    Texture2D lacaioVida3 = LoadTexture("ui/lacaioVida3.png");
    Texture2D lacaioVida4 = LoadTexture("ui/lacaioVida4.png");
    Texture2D lacaioVida5 = LoadTexture("ui/lacaioVida5.png");
    Texture2D lacaioVida6 = LoadTexture("ui/lacaioVida6.png");
    Texture2D lacaioVida7 = LoadTexture("ui/lacaioVida7.png");
    Texture2D lacaioVida8 = LoadTexture("ui/lacaioVida8.png");
    Texture2D lacaioVida9 = LoadTexture("ui/lacaioVida9.png");
    Texture2D lacaioVida10 = LoadTexture("ui/lacaioVida10.png");
    
    Texture2D fogo = LoadTexture("Imagens/fogo.png");
    
    Texture2D procuraChave = LoadTexture("ui/procuraChave.png");
    Texture2D procuraEspadas = LoadTexture("ui/procuraEspadas.png");
    Texture2D procuraDiamante = LoadTexture("ui/procuraDiamante.png");
    Texture2D achouChave = LoadTexture("ui/achouChave.png");
    Texture2D achouEspadas = LoadTexture("ui/achouEspadas.png");
    Texture2D achouDiamante = LoadTexture("ui/achouDiamante.png");
    
    Texture2D potion = LoadTexture("cenario/potion.png");
    
    Texture2D secret = LoadTexture("cenario/secret.png");
    
    Texture2D atirandoEsquerdaDourado = LoadTexture("Imagens/atirandoSuperEsquerda.png");
    Texture2D atirandoDireitaDourado = LoadTexture("Imagens/atirandoSuperDireita.png");
    Texture2D glock = LoadTexture("Imagens/glockDourada.png");
    
    Texture2D death = LoadTexture("lacaio/death.png");
    Texture2D deathLeft = LoadTexture("lacaio/deathLeft.png");
    
    Texture2D shadow = LoadTexture("Imagens/shadow.png");
    
    Texture2D bossIdle = LoadTexture("boss/Idle.png");
    Texture2D bossIdleLeft = LoadTexture("boss/IdleLeft.png");
    Texture2D bossAtack1 = LoadTexture("boss/Atack1.png");
    Texture2D bossAtack1Left = LoadTexture("boss/Atack1Left.png");
    Texture2D bossAtack2 = LoadTexture("boss/Atack2.png");
    Texture2D bossAtack2Left = LoadTexture("boss/Atack2Left.png");
    Texture2D mapa4 = LoadTexture("cenario/mapa4.png");
    // SONS
    
    Sound andando = LoadSound("audios/andando.mp3");
    Sound tiro = LoadSound("audios/tiro.mp3");
    Sound uhr = LoadSound("audios/uhr.mp3");
    Sound lacaioSom = LoadSound("audios/lacaio.mp3");
    Sound abrindoPorta = LoadSound("audios/abrindoPorta.mp3");
    Sound destrancandoPorta = LoadSound("audios/destrancandoPorta.mp3");
    Sound portaTrancada = LoadSound("audios/portaTrancada.mp3");
    Sound pegandoItem = LoadSound("audios/pegandoItem.mp3");
    Sound end = LoadSound("audios/linkinPark.mp3");
    Sound heal = LoadSound("audios/heal.mp3");
    Sound getGlock = LoadSound("audios/getGlock.mp3");
    
    Music musica = LoadMusicStream("audios/music.mp3"); 
    PlayMusicStream(musica);
    
    Sound desert = LoadSound("audios/desert.mp3");
    Sound deathSound = LoadSound("audios/deathSound.mp3");
    
    menu(backgroundMenu);
    
    iniciarJogo(backgroundImage, personagemPegando, personagemPegandoEsquerda, chaveCenario, pegandoChaveEsquerda, pegandoChaveDireita, mapa1, mapa2, arena, mensagem1, menuBack, espadaTesouro, chaveTesouro , bau, bauPreenchido, botao1Off, botao1On, botao2Off, botao2On, pegandoChaveTesouroDireita, pegandoChaveTesouroEsquerda, pegandoEspadaEsquerda, pegandoEspadaDireita, bauPreenchido2, diamante, pegandoDiamanteEsquerda, pegandoDiamanteDireita, bauPreenchido3, puzzle1, spritesheet, spritesheetRight, spriteWalkLeft, spriteWalkRight, pegandoEsquerdaIdle, pegandoDireitaIdle, backgroundMenu, pegandoChaveEsquerdaIdle, pegandoChaveDireitaIdle, pegandoChaveTesouroEsquerdaIdle, pegandoChaveTesouroDireitaIdle, pegandoEspadaEsquerdaIdle, pegandoEspadaDireitaIdle, pegandoDiamanteEsquerdaIdle, pegandoDiamanteDireitaIdle, bala,danoLacaioEsquerda, danoLacaioDireita, diamanteFree, atirandoEsquerda, atirandoDireita, lacaioDireita, lacaioEsquerda, lacaioAtaqueEsquerda, lacaioAtaqueDireita, levandoDano, mensagem2, puzzle2, vida1, vida2, vida3, vida4, vida5, lacaioVida1, lacaioVida2, lacaioVida3, lacaioVida4, lacaioVida5, lacaioVida6, lacaioVida7, lacaioVida8, lacaioVida9, lacaioVida10, fogo, procuraChave, procuraEspadas, procuraDiamante, achouChave, achouEspadas, achouDiamante,potion,secret, atirandoEsquerdaDourado, atirandoDireitaDourado, glock, death,deathLeft, shadow,bossIdle,bossIdleLeft,bossAtack1,bossAtack1Left,bossAtack2 ,bossAtack2Left,mapa4, andando, tiro, uhr, lacaioSom, abrindoPorta, destrancandoPorta, portaTrancada, pegandoItem, end, heal, getGlock, musica, desert, deathSound);
    
    CloseWindow();
    return 0;
}
