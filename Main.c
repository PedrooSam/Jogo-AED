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
int playerOffSet = 140;
int velocidade = 9;
float scale = 3.5f; // Fator de escala para o personagem
bool pausa = false;
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

// Função principal do jogo
void iniciarJogo(Texture2D backgroundImage, Texture2D personagemDireita, Texture2D personagemEsquerda, Texture2D personagemPegando, Texture2D personagemPegandoEsquerda, Texture2D chaveCenario, Texture2D personagemPegandoChaveEsquerda, Texture2D personagemPegandoChaveDireita, Texture2D mapa1, Texture2D mapa2, Texture2D arena, Texture2D mensagem1, Texture2D menuBack) {
    bool andandoDireita = true; // Direção inicial
    bool chavePegandoFlag = false;
    bool puzzleDesbloqueado = false;
    
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
            
            }
            else if(player.mapa == -1){
                DrawTexture(arena,0,0,WHITE);
            
            }
            
            //DrawRectangle(portaMapa3.x, portaMapa3.y, portaMapa3.width, portaMapa3.height, BLUE);
            
            char text[10];
            sprintf(text, "X:%d Y:%d Mapa: %d", player.x, player.y, player.mapa);
            DrawText(text, 20, 20, 20, WHITE);
            

            // Verifica se o personagem está no estado de "pegando" e a direção em que ele está olhando
            if (pegando) {
                if(chavePegandoFlag && andandoDireita) {
                    DrawTextureEx(personagemPegandoChaveEsquerda, (Vector2){player.x, player.y-playerOffSet}, 0.0f, scale, WHITE); 
                } else if(chavePegandoFlag) {
                    DrawTextureEx(personagemPegandoChaveDireita, (Vector2){player.x, player.y-playerOffSet}, 0.0f, scale, WHITE); 
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
            
            if(CollisionObject(playerCollision, arenaCollision) && !chavePegandoFlag && player.mapa == 2){
                    DrawTextureEx(mensagem1, (Vector2){player.x + 30, player.y - 300}, 0.0f, 3.0f, WHITE);
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
        
    InitWindow(larguraTela, alturaTela, "Uma Noite no Castelo");
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
    
    menu();
    iniciarJogo(backgroundImage, personagemDireita, personagemEsquerda, personagemPegando, personagemPegandoEsquerda, chaveCenario, personagemPegandoChaveDireita, personagemPegandoChaveEsquerda, mapa1, mapa2, arena, mensagem1, menuBack);
    
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
    CloseWindow();
    
    return 0;
}

