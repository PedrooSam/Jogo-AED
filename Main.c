#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

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
} Objeto;
// Variáveis globais

//Objetos

    // Jogador
Objeto player;
int playerOffSet = 140; //hitbox do boneco
int velocidade = 10; //velocidade do boneco
float scale = 3.5f; // Fator de escala para o personagem
bool pausa = false; //variavel que verifica se o jogo esta ou nao pausado
bool pegando = false; // Variável para verificar se o personagem está pegando algo

// Chave
Objeto chave;
bool chaveSpawn = true;


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
void menu() {
    Rectangle botaoIniciar = {540, 250, 200, 100};
    Rectangle botaoSair = {540, 400, 200, 100};
    
    while (1) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        DrawText("UMA NOITE NO CASTELO", 329, 100, 50, SKYBLUE);
        DrawRectangleRec(botaoIniciar, GRAY);
        DrawText("Iniciar", botaoIniciar.x + 50, botaoIniciar.y + 35, 18, BLUE);
        DrawRectangleRec(botaoSair, GRAY);
        DrawText("Sair", botaoSair.x + 50, botaoSair.y + 35, 18, BLUE);
        
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

bool puzzleOrdenar(Texture2D puzzle1) {
    bool resultado = true;
    int lista[4] = {-1, -1, -1, -1};
    long a = rand()%32766, b = rand()%32766, c = rand()%32766, d = rand()%32766;
    char texto1[5], texto2[5], texto3[5], texto4[5];
    resultado = true;
    sprintf(texto1, "%d", a);
    sprintf(texto2, "%d", b);
    sprintf(texto3, "%d", c);
    sprintf(texto4, "%d", d);
    
    Rectangle alternativa1Collision = {520, 310, 200, 60};
    Rectangle alternativa2Collision = {520, 390, 200, 60};
    Rectangle alternativa3Collision = {520, 480, 200, 60};
    Rectangle alternativa4Collision = {520, 570, 200, 60};
    
   
    int preenchidos = 0; // Contador de posições preenchidas em `lista`
    
    while (preenchidos < 4) {
        BeginDrawing();
        DrawTexture(puzzle1, 370, 100, WHITE);
        
        DrawRectangleRec(alternativa1Collision, BLUE);
        DrawRectangleRec(alternativa2Collision, BLACK);
        DrawRectangleRec(alternativa3Collision, LIME);
        DrawRectangleRec(alternativa4Collision, YELLOW);
        DrawText(texto1, 550, 330, 20, GOLD);
        DrawText(texto2, 550, 415, 20, GOLD);
        DrawText(texto3, 550, 510, 20, GOLD);
        DrawText(texto4, 550, 590, 20, GOLD);
        
        
        if (CheckCollisionPointRec(GetMousePosition(), alternativa1Collision) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (lista[preenchidos] == -1) {
                BeginDrawing();
                DrawText("SEXO", 550, 590, 20, GOLD);
                EndDrawing();
                lista[preenchidos] = a;
                preenchidos++;
            }
        }
        if (CheckCollisionPointRec(GetMousePosition(), alternativa2Collision) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (lista[preenchidos] == -1) {
                lista[preenchidos] = b;
                preenchidos++;
            }
        }
        if (CheckCollisionPointRec(GetMousePosition(), alternativa3Collision) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (lista[preenchidos] == -1) {
                lista[preenchidos] = c;
                preenchidos++;
            }
        }
        if (CheckCollisionPointRec(GetMousePosition(), alternativa4Collision) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (lista[preenchidos] == -1) {
                lista[preenchidos] = d;
                preenchidos++;
                
            }
        }

        EndDrawing();
    }
    
    // Versão v2 do bubble sort
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
    }
    
    return resultado; 
}
// Função principal do jogo
void iniciarJogo(Texture2D backgroundImage, Texture2D personagemDireita, Texture2D personagemEsquerda, Texture2D personagemPegando, Texture2D personagemPegandoEsquerda, Texture2D chaveCenario, Texture2D personagemPegandoChaveEsquerda, Texture2D personagemPegandoChaveDireita, Texture2D mapa1, Texture2D mapa2, Texture2D arena, Texture2D mensagem1, Texture2D menuBack, Texture2D espadaTesouro, Texture2D chaveTesouro, Texture2D bau, Texture2D bauPreenchido, Texture2D botao1Off, Texture2D botao1On, Texture2D botao2Off, Texture2D botao2On, Texture2D pegandoChaveTesouroDireita, Texture2D pegandoChaveTesouroEsquerda, Texture2D pegandoEspadaEsquerda, Texture2D pegandoEspadaDireita, Texture2D bauPreenchido2, Texture2D diamante, Texture2D pegandoDiamanteEsquerda,Texture2D  pegandoDiamanteDireita, Texture2D bauPreenchido3, Texture2D puzzle1) {
    
    bool andandoDireita = true; // Direção inicial
    bool chavePegandoFlag = false;
    bool puzzleDesbloqueado = false;
    
    bool chaveTesouroPegandoFlag = false;
    bool chaveTesouroSpawn = true;
    bool chaveTesouroNoBau = false;
    
    bool espadaTesouroPegandoFlag = false;
    bool espadaTesouroSpawn = true;
    bool espadaTesouroNoBau = false;
    
    bool diamanteTesouroPegandoFlag = false;
    bool diamanteTesouroSpawn = true;
    bool diamanteTesouroNoBau = false;
    
    bool puzzle1Resolvido = false;
    //bool puzzle2Resolvido = false;
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
    
    
    
    //posição jogador
    player.x = 800;
    player.y = 200;
    player.mapa = 0;
    

    
    //posição de chave
    chave.x = 500;
    chave.y = 550;
    chave.mapa = 0;
    
    
    
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_P)) {
            pausa = !pausa;
        }
        
        if (!pausa) {
            // Verificar se a tecla 'E' foi pressionada para pegar algo
            if (IsKeyPressed(KEY_E)) {
                pegando = !pegando; // Alternar o estado de pegando
                if(chavePegandoFlag) chavePegandoFlag = !chavePegandoFlag;
            }   
            
            //Teleporte de mapas
            if (IsKeyPressed(KEY_R)) {
               player.mapa = 1;
            }   
            
            if (IsKeyPressed(KEY_T)) {
               player.mapa = 0;
            }
            if (IsKeyPressed(KEY_M)) {
               player.mapa = -1;
            }
                
            
            // Movimento do jogador
            if (IsKeyDown(KEY_UP)) {
                player.y -= velocidade;
            }
            if (IsKeyDown(KEY_DOWN)) {
                player.y += velocidade;
            }
            if (IsKeyDown(KEY_RIGHT)) {
                player.x += velocidade;
                andandoDireita = true; // Personagem está indo para a direita
            }
            if (IsKeyDown(KEY_LEFT)) {
                player.x -= velocidade;
                andandoDireita = false; // Personagem está indo para a esquerda
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
            Rectangle diamanteTesouroCollision = {300, 400, 50, 50};
            
            
            Rectangle botao1Collision = {140,350,50,50};
            //Rectangle botao2Collision = {360,350,50,50};
            
            
            
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
                    if(CollisionObject(playerCollision, espadaTesouroCollision) && !espadaTesouroPegandoFlag && pegando && !espadaTesouroNoBau && chaveTesouroNoBau){  //Verifica se pode pegar
                        espadaTesouroPegandoFlag = true;
                        espadaTesouroSpawn = false;
                    }
                    
                    if(CollisionObject(playerCollision, bauTesouroCollision) && espadaTesouroPegandoFlag){                                      // Verifica se colocou a espada no bau
                        espadaTesouroSpawn = false;
                        espadaTesouroNoBau = true;
                        espadaTesouroPegandoFlag = false;
                        pegando = false;
                    }
                    
                    if(espadaTesouroSpawn && !espadaTesouroPegandoFlag && chaveTesouroNoBau){                                                   // Verifica se pode desenhar a espada
                        DrawTexture(espadaTesouro, 200, 200,WHITE);
                    }
                }
                //Diamante 
                
                if(!diamanteTesouroNoBau && espadaTesouroNoBau){
                    if(CollisionObject(playerCollision, diamanteTesouroCollision) && !diamanteTesouroPegandoFlag && pegando && !diamanteTesouroNoBau && espadaTesouroNoBau){  //Verifica se pode pegar
                        diamanteTesouroPegandoFlag = true;
                        diamanteTesouroSpawn = false;
                    }
                    
                    if(CollisionObject(playerCollision, bauTesouroCollision) && diamanteTesouroPegandoFlag){                                      // Verifica se colocou o diamante no bau
                        diamanteTesouroSpawn = false;
                        diamanteTesouroNoBau = true;
                        diamanteTesouroPegandoFlag = false;
                        pegando = false;
                    }
                    
                    if(diamanteTesouroSpawn && !diamanteTesouroPegandoFlag && espadaTesouroNoBau){                                                   // Verifica se pode desenhar o diamante
                        DrawTextureEx(diamante, (Vector2){300, 400},0.0f, 3.0f,WHITE);
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
                
                // Botoes
                if (CheckCollisionPointRec(GetMousePosition(), botao1Collision) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            botao1Pressionado = !botao1Pressionado;
                    }
                if(!botao1Pressionado){
                    DrawTextureEx(botao1Off, (Vector2){130,350},0.0f, 4.0f,WHITE);
                   
                }else if(botao1Pressionado){
                    DrawTextureEx(botao1On, (Vector2){130,350},0.0f, 4.0f,WHITE);
                    puzzleOrdenar(puzzle1);
                    botao1Pressionado = false;
                }
                
                //botao 
                
                
                
                
                
                
                
            }
            //Verificacao chave
            if(chaveTesouroPegandoFlag && !pegando && !chaveTesouroNoBau){                                         //Verifica se deixou a chave cair 
                    chaveTesouroPegandoFlag = false;
                    chaveTesouroSpawn = true;
                }
            //Verificacao espada
            if(espadaTesouroPegandoFlag && !pegando && !espadaTesouroNoBau){                                         //Verifica se deixou a chave cair 
                    espadaTesouroPegandoFlag = false;
                    espadaTesouroSpawn = true;
                }
            //Verificacao do diamante
            if(diamanteTesouroPegandoFlag && !pegando && !diamanteTesouroNoBau){                                         //Verifica se deixou a chave cair 
                    diamanteTesouroPegandoFlag = false;
                    diamanteTesouroSpawn = true;
                }
            
            //DrawRectangle(botao1Collision.x, botao1Collision.y, botao1Collision.width, botao1Collision.height, BLUE);
            
            char text[10];
            sprintf(text, "X:%d Y:%d Mapa: %d", player.x, player.y, player.mapa);
            DrawText(text, 20, 20, 20, WHITE);
            

            // Verifica se o personagem está no estado de "pegando" e a direção em que ele está olhando
            if (pegando) {
                if(chavePegandoFlag && andandoDireita) { // primeira chave
                    DrawTextureEx(personagemPegandoChaveEsquerda, (Vector2){player.x, player.y-playerOffSet}, 0.0f, scale, WHITE); 
                } else if(chavePegandoFlag) {
                    DrawTextureEx(personagemPegandoChaveDireita, (Vector2){player.x, player.y-playerOffSet}, 0.0f, scale, WHITE); 
                }
 
                else if(chaveTesouroPegandoFlag && andandoDireita) {
                    DrawTextureEx(pegandoChaveTesouroDireita, (Vector2){player.x, player.y-playerOffSet}, 0.0f, scale, WHITE);  //Chave tesouro
                } else if(chaveTesouroPegandoFlag) {
                    DrawTextureEx(pegandoChaveTesouroEsquerda, (Vector2){player.x, player.y-playerOffSet}, 0.0f, scale, WHITE); 
                }
                else if(espadaTesouroPegandoFlag && andandoDireita) {
                    DrawTextureEx(pegandoEspadaDireita, (Vector2){player.x, player.y-playerOffSet}, 0.0f, scale, WHITE);  //Espada tesouro
                } else if(espadaTesouroPegandoFlag) {
                    DrawTextureEx(pegandoEspadaEsquerda, (Vector2){player.x, player.y-playerOffSet}, 0.0f, scale, WHITE); 
                } 
                else if(diamanteTesouroPegandoFlag && andandoDireita) {
                    DrawTextureEx(pegandoDiamanteDireita, (Vector2){player.x, player.y-playerOffSet}, 0.0f, scale, WHITE);  //Diamante tesouro
                } else if(diamanteTesouroPegandoFlag) {
                    DrawTextureEx(pegandoDiamanteEsquerda, (Vector2){player.x, player.y-playerOffSet}, 0.0f, scale, WHITE); 
                } else if (andandoDireita) {
                    DrawTextureEx(personagemPegando, (Vector2){player.x, player.y-playerOffSet}, 0.0f, scale, WHITE); 
                } else {
                    DrawTextureEx(personagemPegandoEsquerda, (Vector2){player.x, player.y-playerOffSet}, 0.0f, scale, WHITE);
                }
                
                
            } else {
                if (andandoDireita) {
                    DrawTextureEx(personagemDireita, (Vector2){player.x, player.y-playerOffSet}, 0.0f, scale, WHITE);
                } else {
                    DrawTextureEx(personagemEsquerda, (Vector2){player.x, player.y-playerOffSet}, 0.0f, scale, WHITE);
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
                    menu();
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
    
    Texture2D personagemDireita = LoadTexture("Imagens/personagemDireita.png");
    Texture2D personagemEsquerda = LoadTexture("Imagens/personagemEsquerda.png");
    
    Texture2D personagemPegando = LoadTexture("Imagens/personagemPegando.png");
    Texture2D personagemPegandoEsquerda = LoadTexture("Imagens/personagemPegandoEsquerda.png");
    
    Texture2D chaveCenario = LoadTexture("cenario/chaveCenario.png");
    
    Texture2D personagemPegandoChaveDireita = LoadTexture("pegandoItens/personagemPegandoChaveDireita.png");
    Texture2D personagemPegandoChaveEsquerda = LoadTexture("pegandoItens/personagemPegandoChaveEsquerda.png");
    
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
    
    menu();
    iniciarJogo(backgroundImage, personagemDireita, personagemEsquerda, personagemPegando, personagemPegandoEsquerda, chaveCenario, personagemPegandoChaveDireita, personagemPegandoChaveEsquerda, mapa1, mapa2, arena, mensagem1, menuBack, espadaTesouro, chaveTesouro , bau, bauPreenchido, botao1Off, botao1On, botao2Off, botao2On, pegandoChaveTesouroDireita, pegandoChaveTesouroEsquerda, pegandoEspadaEsquerda, pegandoEspadaDireita, bauPreenchido2, diamante, pegandoDiamanteEsquerda, pegandoDiamanteDireita, bauPreenchido3, puzzle1 );
    
    UnloadTexture(backgroundImage);
    
    UnloadTexture(personagemDireita);
    UnloadTexture(personagemEsquerda);
    
    UnloadTexture(personagemPegando);
    UnloadTexture(personagemPegandoEsquerda);
    
    UnloadTexture(chaveCenario);
    
    UnloadTexture(personagemPegandoChaveDireita);
    UnloadTexture(personagemPegandoChaveEsquerda);
    
    UnloadTexture(mapa1);
    UnloadTexture(mapa2);
    
    UnloadTexture(arena);
    
    UnloadTexture(mensagem1);
    
    UnloadTexture(menuBack);
    
    UnloadTexture(espadaTesouro);
    UnloadTexture(chaveTesouro);
    UnloadTexture(bau);
    UnloadTexture(bauPreenchido);
    
    UnloadTexture(botao1Off);
    UnloadTexture(botao1On);
    UnloadTexture(botao2Off);
    UnloadTexture(botao2On);

    UnloadTexture(pegandoChaveTesouroEsquerda);
    UnloadTexture(pegandoChaveTesouroDireita);
    
    UnloadTexture(pegandoEspadaEsquerda);
    UnloadTexture(pegandoEspadaDireita);
    UnloadTexture(bauPreenchido2);
    UnloadTexture(diamante);
    
    UnloadTexture(pegandoDiamanteDireita);
    UnloadTexture(pegandoChaveTesouroEsquerda);
    UnloadTexture(bauPreenchido3);
    
    UnloadTexture(puzzle1);
    
    CloseWindow();
    return 0;
}
