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
// Variáveis globais

typedef struct {
    Vector2 posicao;
    Vector2 velocidade;
    bool ativo;
} Projetil;

//Objetos
Objeto player;
Objeto lacaio;

//Projeteis
Projetil projetil;

// lacaio
bool lacaioParado = true;
bool lacaioAtingido = false;

// Jogador
int playerOffSet = 140; //hitbox do boneco
int velocidade = 7;     //velocidade do boneco
float scale = 3.5f;     // Fator de escala para o personagem

bool pausa = false;     //variavel que verifica se o jogo esta ou nao pausado
bool pegando = false;   // Variável para verificar se o personagem está pegando algo

//Distancia entre lacaio e player
float distanciaX = 0;
float distanciaY = 0;
float intervalo = 200.0f;
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

//Colisão Universal, 
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
    
    while (1) {
        BeginDrawing();

        DrawTexture(backgroundMenu,0,0,WHITE);
        
        EndDrawing();
            
        // Verificar clique no botão
        if (CheckCollisionPointRec(GetMousePosition(), botaoIniciar) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            pausa = false;
            break;  
        }
        else if (CheckCollisionPointRec(GetMousePosition(), botaoSair) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            CloseWindow();
            break;
        }           
    }
}

// Função puzzle 1
bool puzzleOrdenar(Texture2D puzzle1) {
    bool resultado = true;
    int lista[4] = {-1, -1, -1, -1};
    int a = rand() % 32766, b = rand() % 32766, c = rand() % 32766, d = rand() % 32766;
    char texto1[6], texto2[6], texto3[6], texto4[6];
    
    // Converte os números para strings
    sprintf(texto1, "%d", a);
    sprintf(texto2, "%d", b);
    sprintf(texto3, "%d", c);
    sprintf(texto4, "%d", d);
    
    
    // Define as áreas de colisão para os botões
    Rectangle alternativa1Collision = {520, 310, 200, 60};
    Rectangle alternativa2Collision = {520, 390, 200, 60};
    Rectangle alternativa3Collision = {520, 480, 200, 60};
    Rectangle alternativa4Collision = {520, 570, 200, 60};
    
    int preenchidos = 0; // Contador de posições preenchidas em `lista`
    
    while (preenchidos < 4) {
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
        }
        if (CheckCollisionPointRec(GetMousePosition(), alternativa2Collision) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { 
            lista[preenchidos] = b;
            preenchidos++;
        }
        if (CheckCollisionPointRec(GetMousePosition(), alternativa3Collision) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            lista[preenchidos] = c;
            preenchidos++;
        }
        if (CheckCollisionPointRec(GetMousePosition(), alternativa4Collision) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { 
            lista[preenchidos] = d;
            preenchidos++; 
        }
        if(IsKeyPressed(KEY_ESCAPE))//player nao quer mais fazer o puzzle
            return false;
        
        EndDrawing();
    }
    
    // Bubble Sort para ordenar a lista
    for (int i = 0; i < 4 - 1; i++) {
        bool trocou = false;
        for (int j = 0; j < 4 - i - 1; j++) {
            if (lista[j] > lista[j + 1]) {
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
        return puzzleOrdenar(puzzle1); //recursao para que o puzzle seja reiniciado
    }
}

// Função principal do jogo
void iniciarJogo(Texture2D backgroundImage, Texture2D personagemPegando, Texture2D personagemPegandoEsquerda, Texture2D chaveCenario, Texture2D pegandoChaveEsquerda, Texture2D pegandoChaveDireita, Texture2D mapa1, Texture2D mapa2, Texture2D arena, Texture2D mensagem1, Texture2D menuBack, Texture2D espadaTesouro, Texture2D chaveTesouro, Texture2D bau, Texture2D bauPreenchido, Texture2D botao1Off, Texture2D botao1On, Texture2D botao2Off, Texture2D botao2On, Texture2D pegandoChaveTesouroDireita, Texture2D pegandoChaveTesouroEsquerda, Texture2D pegandoEspadaEsquerda, Texture2D pegandoEspadaDireita, Texture2D bauPreenchido2, Texture2D diamante, Texture2D pegandoDiamanteEsquerda,Texture2D  pegandoDiamanteDireita, Texture2D bauPreenchido3, Texture2D puzzle1, Texture2D spritesheet, Texture2D spritesheetRight, Texture2D spriteWalkLeft, Texture2D spriteWalkRight,Texture2D pegandoEsquerdaIdle, Texture2D pegandoDireitaIdle, Texture2D backgroundMenu,Texture2D pegandoChaveEsquerdaIdle,Texture2D pegandoChaveDireitaIdle, Texture2D pegandoChaveTesouroEsquerdaIdle, Texture2D pegandoChaveTesouroDireitaIdle, Texture2D pegandoEspadaEsquerdaIdle, Texture2D pegandoEspadaDireitaIdle, Texture2D pegandoDiamanteEsquerdaIdle, Texture2D pegandoDiamanteDireitaIdle, Texture2D lacaioIdleDireita, Texture2D bala, Texture2D danoLacaioDireita, Texture2D diamanteFree, Texture2D atirandoEsquerda, Texture2D atirandoDireita) {
    
    bool andandoDireita = true;     // Direção inicial
    bool chavePegandoFlag = false;  // Verifica se tá pegando a chave
    bool puzzleDesbloqueado = false;// Verifica se pode acessar a sala do puzzle 
    
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
    //bool botao2Pressionado = false;
    
    //float vetorPuzzleOrdenar[4] = {0};
    
    //configuração do mapa
    Node *head = NULL; 
    Node *tail = NULL;
    Node *mapaSec0 = NULL;
    
    configurarMapaSecundario(&mapaSec0, -1);
    
    inserirMapa(&head, &tail, 0, NULL);
    inserirMapa(&head, &tail, 1, NULL);
    inserirMapa(&head, &tail, 2, mapaSec0);
    
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
    lacaio.x = 300;
    lacaio.y = 400;
    lacaio.mapa = -1;
    lacaio.vivo = true;
    lacaio.vida = 10;
    
    //posição de chave
    chave.x = 500;
    chave.y = 550;
    chave.mapa = 0;
    
    //animação idle
    int totalFrames = 4;
    int frameAtual = 0;
    float tempoFrame = 0.3f;
    float timer = 0.0f;
    int larguraFrame = spritesheet.width / totalFrames;
    int alturaFrame = spritesheet.height;
    bool personagemParado = true;
    
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
    
    // (lacaio) animacao idle
    int totalFrameslacaioIdle = 4;
    int frameAtuallacaioIdle = 0;
    float tempoFramelacaioIdle = 0.2f;
    float timerlacaioIdle = 0.0f;
    int larguraFramelacaioIdle = lacaioIdleDireita.width / totalFrameslacaioIdle;
    int alturaFramelacaioIdle = lacaioIdleDireita.height;
    
   // (lacaio) animacao dano
    int totalFrameslacaioDano = 4;
    int frameAtuallacaioDano = 0;
    float tempoFramelacaioDano = 0.2f;
    float timerlacaioDano = 0.0f;
    int larguraFramelacaioDano = danoLacaioDireita.width / totalFrameslacaioDano;
    int alturaFramelacaioDano = danoLacaioDireita.height;
    
    // Animação de atirar
    int totalFramesTiro = 4;
    int frameAtualTiro = 0;
    float tempoFrameTiro = 0.05f;
    float timerTiro = 0.0f;
    int larguraFrameTiro = atirandoEsquerda.width / totalFramesTiro;
    int alturaFrameTiro = atirandoEsquerda.height;
    
    //segurando item
    bool segurandoItem = false;

    while (!WindowShouldClose()) {
            if (!pausa) {
                if (IsKeyPressed(KEY_P)) {
                pausa = !pausa;
            }
            
            timerTiro += 1;
            if (IsKeyPressed(KEY_SPACE) && !projetil.ativo && personagemParado && !segurandoItem && timerTiro >= intervaloTiro) {       // Só pode atirar quando tiver parado 
                projetil.ativo = true;
                atirou = true;
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
            if(IsKeyDown(KEY_W) && IsKeyDown(KEY_D)){ // Diagonal direita
                player.y -= velocidade;
                player.x += velocidade;
                personagemParado = false;
                andandoDireita = true;
            }
            else if(IsKeyDown(KEY_W) && IsKeyDown(KEY_A)){ // Diagonal esquerda
                player.y -= velocidade;
                player.x -= velocidade;
                personagemParado = false;
                andandoDireita = false;
            }
            else if(IsKeyDown(KEY_S) && IsKeyDown(KEY_D)){ // Diagonal pra baixo direita
                player.y += velocidade;
                player.x += velocidade;
                personagemParado = false;
                andandoDireita = true;
            }
            else if(IsKeyDown(KEY_S) && IsKeyDown(KEY_A)){ // Diagonal pra baixo esquerda
                player.y += velocidade;
                player.x -= velocidade;
                personagemParado = false;
                andandoDireita = false;
            }
            else if (IsKeyDown(KEY_W)) {
                player.y -= velocidade;
                personagemParado = false;
            }
            else if (IsKeyDown(KEY_S)) {
                player.y += velocidade;
                personagemParado = false;
            }
            else if (IsKeyDown(KEY_D)) {
                player.x += velocidade;
                andandoDireita = true; // Personagem está indo para a direita
                personagemParado = false;
            }
            else if (IsKeyDown(KEY_A)) {
                player.x -= velocidade;
                andandoDireita = false; // Personagem está indo para a esquerda
                personagemParado = false;
            }else{
                personagemParado = true;
            }
            
            // Movimento do lacaio
            float velocidadeLacaio = 150.0f * GetFrameTime();
            // Atualize o timer do ataque
            timerAtaque += 1;
            if (lacaio.vivo && lacaio.mapa == player.mapa) {
                // Movimenta no eixo X em direção ao jogador
                if (lacaio.x < player.x -200) {
                    lacaio.x += velocidadeLacaio;
                } else if (lacaio.x > player.x - 100) {
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
                if (distanciaX <= 100 && distanciaY <= 50 && timerAtaque >= intervalo) {
                    player.vida -= 1;   // Reduz a vida do jogador
                    timerAtaque = 0.0f; // Reinicia o tempo de recarga do ataque
                }
            }    
            
            if(player.vida <= 0){
                player.vivo = false;
            }
            
            //verifica se tá segurando algo
            if(!chavePegandoFlag && !chaveTesouroPegandoFlag && !espadaTesouroPegandoFlag && !diamanteTesouroPegandoFlag){
                segurandoItem = false;
            }else{
                segurandoItem = true;
            }
 
            // Atualização da animação idle
            if (personagemParado) {
                timer += GetFrameTime();
                
                if (timer >= tempoFrame) {
                    timer = 0.0f;
                    frameAtual++;
                    
                    if (frameAtual >= totalFrames) frameAtual = 0;
                }
            }
            
            //Atualização da animação walk
            if (!personagemParado) {  // Personagem andando
                timerWalk += GetFrameTime();
                
                if (timerWalk >= tempoFrameWalk) {
                    timerWalk = 0.0f;
                    frameAtualWalk++;
                    
                    if (frameAtualWalk >= totalFramesWalk) frameAtualWalk = 0;
                }
            }
            
            //Atualização da animação pegando andando
            if (!personagemParado) {  // Personagem andando
                timerKey += GetFrameTime();
                
                if (timerKey >= tempoFrameKey) {
                    timerKey = 0.0f;
                    frameAtualKey++;
                    
                    if (frameAtualKey >= totalFramesKey){ 
                    frameAtualKey = 0;
                    }
                }
            }
            
            //Atualização da animação pegando parado
            if (personagemParado) { 
                timerIdle += GetFrameTime();
                
                if (timerIdle >= tempoFrameIdle) {
                    timerIdle = 0.0f;
                    frameAtualIdle++;
                    
                    if (frameAtualIdle >= totalFramesIdle) {
                        frameAtualIdle = 0;
                        }
                }
            }
            // (lacaio)Atualização do lacaio parado
            if (lacaioParado) { 
                timerlacaioIdle += GetFrameTime();
                
                if (timerlacaioIdle >= tempoFramelacaioIdle) {
                    timerlacaioIdle = 0.0f;
                    frameAtuallacaioIdle++;
                    
                    if (frameAtuallacaioIdle >= totalFrameslacaioIdle) frameAtuallacaioIdle = 0;
                }
            }
            
            //Lacaio levando chumbo
            if (lacaioAtingido) {
                timerlacaioDano += GetFrameTime();
                
                if (timerlacaioDano >= tempoFramelacaioDano) {
                    timerlacaioDano = 0.0f; //reseta o temporizador
                    frameAtuallacaioDano++;
                    
                    if (frameAtuallacaioDano >= totalFrameslacaioDano)  {
                        frameAtuallacaioDano = 0;
                        lacaioAtingido = false;}
                }
            }
    
            //Atualização da animação de tiro 
            if (atirou) { 
                timerTiro += GetFrameTime();
                
                if (timerTiro >= tempoFrameTiro) {
                    timerTiro = 0.0f;
                    frameAtualTiro++;
                    
                    if (frameAtualTiro >= totalFramesTiro) {
                        frameAtualTiro = 0;
                        atirou = false;
                        }
                }
            }
            
            // Restringir o jogador para não sair da tela
            if (player.x < 0) {     // Lado esquerdo
                player.x = 0;
            }
            
            if(player.mapa != -1){
                
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
            
            //colisões
            Rectangle playerCollision = {player.x, player.y, 83, 83};
            Rectangle chaveCollision = {chave.x, chave.y, 28, 18};
            
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
            Rectangle diamanteTesouroCollision = {900, 460, 150, 100};
            
            
            Rectangle botao1Collision = {140,350,50,50};
            //Rectangle botao2Collision = {360,350,50,50};  
            
            Rectangle lacaioCollision = {lacaio.x, lacaio.y, 200, 150};
            
            //Verifica as colisões 
            if(player.mapa == 0) {
                if(chaveSpawn && pegando && CollisionObject(playerCollision, chaveCollision)) chavePegandoFlag = true;
               
                CollisionObject(playerCollision, pilarMapa0);
                CollisionObject(playerCollision, pilarEsqMapa0);
                
                if(CollisionObject(playerCollision, doorCollision)) {
                    player.mapa = getNextMapaPrincipal(head);
                    player.x = 200;
                    player.y = 540;
                }
                
                
            } else if(player.mapa == 1) {
                
                CollisionObject(playerCollision, mesaMapa1);
                
                if(CollisionObject(playerCollision, doorCollisionEsquerda)) {
                    player.mapa = getAntMapaPrincipal(head);
                    player.x = 920;
                    player.y = 540;
                }
                else if(CollisionObject(playerCollision, doorCollision)){
                    player.mapa = getNextMapaPrincipal(head);
                    player.x = 200;
                    player.y = 540;
                    
                
            }
            }
            else if(player.mapa == 2){
                CollisionObject(playerCollision, mesaMapa2);
                
                if(CollisionObject(playerCollision, doorCollisionEsquerda)) {
                    player.mapa = getAntMapaPrincipal(head);
                    player.x = 920;
                    player.y = 540;
                }
                
                if(chavePegandoFlag && CollisionObject(playerCollision, arenaCollision)) {
                    chaveSpawn = false;
                    puzzleDesbloqueado = true;
                    pegando = false;
                    chavePegandoFlag = false;
                }
                
                if(puzzleDesbloqueado && CollisionObject(playerCollision, arenaCollision)){
                    player.mapa = getMapaSecundario(head);
                    player.x = 400;
                    player.y = 540;
                }
            }
            else if(player.mapa == -1){
                CollisionObject(playerCollision, pilarEsqMapa3);
                CollisionObject(playerCollision, pilarMapa3);
                CollisionObject(playerCollision, pilarDirMapa3);
                
                if(CollisionObject(playerCollision, portaMapa3)){
                    player.mapa = getAntMapaSecundario(head);
                    player.x = 400;
                    player.y = 540;
                }
                
            }
            
            // Desenho do jogo
            BeginDrawing();
            ClearBackground(RAYWHITE);
            
            // Só vai desenhar o item se estiver no mapa dele
            if(player.mapa == 0) {
                DrawTexture(backgroundImage, 0, 0, WHITE);
                
                if(chaveSpawn && chavePegandoFlag == false) {
                    DrawTextureEx(chaveCenario, (Vector2){chave.x, chave.y}, 0.0f, 1.0f, WHITE);
                }
                
                //Diamante                 
                if(!diamanteTesouroNoBau){
                    if(CollisionObject(playerCollision, diamanteTesouroCollision) && !diamanteTesouroPegandoFlag && pegando && !diamanteTesouroNoBau && puzzle2Resolvido){  //Verifica se pode pegar
                        diamanteTesouroPegandoFlag = true;
                        diamanteTesouroSpawn = false;
                    }
                    
                    if(CollisionObject(playerCollision, bauTesouroCollision) && diamanteTesouroPegandoFlag){    // Verifica se colocou o diamante no bau
                        diamanteTesouroSpawn = false;
                        diamanteTesouroNoBau = true;
                        diamanteTesouroPegandoFlag = false;
                        pegando = false;
                    }
                    
                    if(diamanteTesouroSpawn && !diamanteTesouroPegandoFlag){                             // Verifica se pode desenhar o diamante
                        if(!puzzle2Resolvido){
                        DrawTextureEx(diamante, (Vector2){900, 460},0.0f, 3.0f,WHITE);
                        }
                        else{
                        DrawTextureEx(diamanteFree, (Vector2){900, 460},0.0f, 3.0f,WHITE);    
                        }
                    }
                }
            }
            else if(player.mapa == 1){
                DrawTexture(mapa1, 0, 0, WHITE);
                
            }
            else if(player.mapa == 2){
                DrawTexture(mapa2,0,0,WHITE);
                
                if(CollisionObject(playerCollision, arenaCollision) && !chavePegandoFlag){
                    DrawTextureEx(mensagem1, (Vector2){player.x + 30, player.y - 300}, 0.0f, 3.0f, WHITE);
                }
            
            }
            else if(player.mapa == -1){
                DrawTexture(arena,0,0,WHITE);
                //Chave do tesouro 
                
                if(!chaveTesouroNoBau){
                    if(CollisionObject(playerCollision, chaveTesouroCollision) && !chaveTesouroPegandoFlag && pegando && !chaveTesouroNoBau){  //Verifica se pode pegar
                        chaveTesouroPegandoFlag = true;
                        chaveTesouroSpawn = false;
                    }
                    
                    if(CollisionObject(playerCollision, bauTesouroCollision) && chaveTesouroPegandoFlag){                                      // Verifica se colocou a chave no bau
                        chaveTesouroSpawn = false;
                        chaveTesouroNoBau = true;
                        chaveTesouroPegandoFlag = false;
                        pegando = false;
                    }
                    
                    if(chaveTesouroSpawn && !chavePegandoFlag){                                                                                 // Verifica se pode desenhar a chave
                        DrawTexture(chaveTesouro, 300, 400, WHITE);
                    }
                }
                // Espada
                
                if(!espadaTesouroNoBau){
                    if(CollisionObject(playerCollision, espadaTesouroCollision) && !espadaTesouroPegandoFlag && pegando && !espadaTesouroNoBau && chaveTesouroNoBau && puzzle1Resolvido){  //Verifica se pode pegar
                        espadaTesouroPegandoFlag = true;
                        espadaTesouroSpawn = false;
                    }
                    
                    if(CollisionObject(playerCollision, bauTesouroCollision) && espadaTesouroPegandoFlag){                                      // Verifica se colocou a espada no bau
                        espadaTesouroSpawn = false;
                        espadaTesouroNoBau = true;
                        espadaTesouroPegandoFlag = false;
                        pegando = false;
                    }
                    
                    if(espadaTesouroSpawn && !espadaTesouroPegandoFlag && chaveTesouroNoBau && puzzle1Resolvido){                                                   // Verifica se pode desenhar a espada
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
                    if(!botao1Pressionado){
                        DrawTextureEx(botao1Off, (Vector2){130,350},0.0f, 4.0f,WHITE);
                       
                    }else if(botao1Pressionado){
                        DrawTextureEx(botao1On, (Vector2){130,350},0.0f, 4.0f,WHITE);
                        puzzle1Resolvido = puzzleOrdenar(puzzle1);
                        botao1Pressionado = false;
                    }
                }
                        
                //botao 2 verde
                
                if(espadaTesouroNoBau){
                   DrawTextureEx(botao2Off, (Vector2){200, 350}, 0.0f,4.0f, WHITE);
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
            
                
            //DrawRectangle(lacaio.x, lacaio.y, 200, 150, BLUE);

            char text[10];
            sprintf(text, "X:%d Y:%d Mapa: %d Vidas: %d DistanciaX: %f DistanciaY %f", player.x, player.y, player.mapa, player.vida, distanciaX, distanciaY);
            DrawText(text, 20, 20, 20, WHITE);
       
            
            // Atualiza a posição do projétil em cada frame se ele estiver ativo
            if (projetil.ativo) {
                projetil.posicao.x += projetil.velocidade.x;
                projetil.posicao.y += projetil.velocidade.y;

                // Verifique se o projétil saiu da tela e desativa
                if (projetil.posicao.x > GetScreenWidth() || projetil.posicao.x < 0 ||
                    projetil.posicao.y > GetScreenHeight() || projetil.posicao.y < 0) {
                    projetil.ativo = false;
                }
                
                if ((projetil.posicao.x > lacaio.x && projetil.posicao.x < lacaio.x + 200) && (projetil.posicao.y > lacaio.y && projetil.posicao.y < lacaio.y + 150) && lacaio.vivo) {
                    projetil.ativo = false;
                    lacaio.vida -= 1;
                    lacaioAtingido = true;
                        
                        if(lacaio.vida == 0){
                            lacaio.vivo = false;
                        }
                }
                
                // Desenhe o projétil na posição atual
                DrawTextureEx(bala, projetil.posicao, 0.0f, 0.25f, WHITE);  
            }

            //lacaio
            if(lacaio.mapa == player.mapa && lacaio.vivo){
                if(lacaioAtingido){
                    Rectangle sourceReclacaioDano = { frameAtuallacaioDano * larguraFramelacaioDano, 0, larguraFramelacaioDano, alturaFramelacaioDano};
                    DrawTextureRec(danoLacaioDireita, sourceReclacaioDano, (Vector2){lacaio.x, lacaio.y}, WHITE);
                }
                else if(lacaioParado){
                    Rectangle sourceReclacaioIdle = { frameAtuallacaioIdle * larguraFramelacaioIdle, 0, larguraFramelacaioIdle, alturaFramelacaioIdle};
                    DrawTextureRec(lacaioIdleDireita, sourceReclacaioIdle, (Vector2){lacaio.x, lacaio.y}, WHITE);
                }
            }
            
            if(player.vivo){
                // Verifica se o personagem está no estado de "pegando" e a direção em que ele está olhando
                if (pegando) {
                    
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
                            Rectangle sourceRecKey = { frameAtualTiro * larguraFrameTiro, 0, larguraFrameTiro, alturaFrameTiro };
                            DrawTextureRec(atirandoDireita, sourceRecKey, (Vector2){player.x, player.y - playerOffSet}, WHITE);
                        }else{
                            Rectangle sourceRec = { frameAtual * larguraFrame, 0, larguraFrame, alturaFrame };
                            DrawTextureRec(spritesheetRight, sourceRec, (Vector2){player.x, player.y-playerOffSet}, WHITE);
                        }
                    }else{
                        if(atirou && !segurandoItem && !pegando){
                            Rectangle sourceRecKey = { frameAtualTiro * larguraFrameTiro, 0, larguraFrameTiro, alturaFrameTiro };
                            DrawTextureRec(atirandoEsquerda, sourceRecKey, (Vector2){player.x, player.y - playerOffSet}, WHITE);
                        }else{
                            Rectangle sourceRec = { frameAtual * larguraFrame, 0, larguraFrame, alturaFrame };
                            DrawTextureRec(spritesheet, sourceRec, (Vector2){player.x, player.y-playerOffSet}, WHITE);
                        }
                    }
                }
            }    
            EndDrawing();
        }
                
        if (pausa) {
            Rectangle botaoVoltarInicio = {500, 400, 250, 50};
            BeginDrawing();
            DrawTexture(menuBack, 10, 50 , WHITE );
            DrawText("Voltar ao início", 510, 400, 30, GOLD);
            DrawText("PAUSE", 500, 290, 70, GOLD);
            EndDrawing();
            
            if (CheckCollisionPointRec(GetMousePosition(), botaoVoltarInicio)) {
                BeginDrawing();
                DrawText("Votar ao início", 510, 400, 30, RED);
                EndDrawing();
                if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    menu(backgroundMenu);
                }
            }
        }
    }
}

int main(void) {
    int larguraTela = 1280, alturaTela = 720;
        
    InitWindow(larguraTela, alturaTela, "Um dia no Castelo");
    SetTargetFPS(60);
    
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
    
    Texture2D lacaioIdleDireita = LoadTexture("lacaio/Idle.png");
    
    Texture2D bala = LoadTexture("Imagens/bala.png");
    
    Texture2D danoLacaioDireita = LoadTexture("lacaio/danoDireita.png");
    
    Texture2D diamanteFree = LoadTexture("tesouro/diamanteFree.png");
    
    Texture2D atirandoDireita = LoadTexture("Imagens/atirandoDireita.png");
    Texture2D atirandoEsquerda = LoadTexture("Imagens/atirandoEsquerda.png");
    
    menu(backgroundMenu);
    
    iniciarJogo(backgroundImage, personagemPegando, personagemPegandoEsquerda, chaveCenario, pegandoChaveEsquerda, pegandoChaveDireita, mapa1, mapa2, arena, mensagem1, menuBack, espadaTesouro, chaveTesouro , bau, bauPreenchido, botao1Off, botao1On, botao2Off, botao2On, pegandoChaveTesouroDireita, pegandoChaveTesouroEsquerda, pegandoEspadaEsquerda, pegandoEspadaDireita, bauPreenchido2, diamante, pegandoDiamanteEsquerda, pegandoDiamanteDireita, bauPreenchido3, puzzle1, spritesheet, spritesheetRight, spriteWalkLeft, spriteWalkRight, pegandoEsquerdaIdle, pegandoDireitaIdle, backgroundMenu, pegandoChaveEsquerdaIdle, pegandoChaveDireitaIdle, pegandoChaveTesouroEsquerdaIdle, pegandoChaveTesouroDireitaIdle, pegandoEspadaEsquerdaIdle, pegandoEspadaDireitaIdle, pegandoDiamanteEsquerdaIdle, pegandoDiamanteDireitaIdle, lacaioIdleDireita, bala, danoLacaioDireita, diamanteFree, atirandoEsquerda, atirandoDireita);
    
    CloseWindow();
    return 0;
}
