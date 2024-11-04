#include "raylib.h"

// Variáveis globais
Vector2 player;
int velocidade = 5;

Vector2 chave;

bool pausa = false;
bool pegando = false; // Variável para verificar se o personagem está pegando algo

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
            break;  
        }
        else if (CheckCollisionPointRec(GetMousePosition(), botaoSair) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            CloseWindow();
            break;
        }           
    }
}

// Função principal do jogo
void iniciarJogo(Texture2D backgroundImage, Texture2D personagemDireita, Texture2D personagemEsquerda, Texture2D personagemPegando, Texture2D personagemPegandoEsquerda, Texture2D chaveCenario, Texture2D personagemPegandoChaveEsquerda, Texture2D personagemPegandoChaveDireita) {
    bool andandoDireita = true; // Direção inicial
    bool chavePegandoFlag = false;
    
    //posição jogador
    player.x = 200;
    player.y = 200;
    
    //posição de chave
    chave.x = 500;
    chave.y = 550;
    
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_P)) {
            pausa = !pausa;
        }
        
        if (!pausa) {
            // Verificar se a tecla 'E' foi pressionada para pegar algo
            if (IsKeyPressed(KEY_E)) {
                pegando = !pegando; // Alternar o estado de pegando
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
            if (player.x < 0) {     // Canto esquerdo
                player.x = 0;
            }
                
            if (player.y < 380) {     // Sobe até onde tem chão
                player.y = 380;
            }
                
            if (player.x > GetScreenWidth() - 83) {  // Canto direito
                player.x = GetScreenWidth() - 83;
            }
                
            if (player.y > GetScreenHeight() - 140) { // Canto inferior
                player.y = GetScreenHeight() - 140;
            }
            
            Rectangle retanguloCollision = {100, 100, 500, 300};
            Rectangle playerCollision = {player.x, player.y, 83, 83};
            Rectangle chaveCollision = {chave.x, chave.y, 28, 18};
            
            // Colisão com o retângulo estático
            //CollisionObject(playerCollision, retanguloCollision);
            
            if(pegando && CollisionObject(playerCollision, chaveCollision)) {
                chavePegandoFlag = true;
            }
            
            // Desenho do jogo
            BeginDrawing();
            ClearBackground(RAYWHITE);
            
            DrawTexture(backgroundImage, 0, 0, WHITE);
            if(chavePegandoFlag == false) {
                DrawTextureEx(chaveCenario, chave, 0.0f, 1.0f, WHITE);
            }
            // Fator de escala para o personagem
            float scale = 3.5f;

            // Verifica se o personagem está no estado de "pegando" e a direção em que ele está olhando
            if (pegando) {
                if(chavePegandoFlag && andandoDireita) {
                    DrawTextureEx(personagemPegandoChaveDireita, (Vector2){player.x, player.y-80}, 0.0f, scale, WHITE); 
                } else if(chavePegandoFlag) {
                    DrawTextureEx(personagemPegandoChaveEsquerda, (Vector2){player.x, player.y-80}, 0.0f, scale, WHITE); 
                } else if (andandoDireita) {
                    DrawTextureEx(personagemPegando, (Vector2){player.x, player.y-80}, 0.0f, scale, WHITE); 
                } else {
                    DrawTextureEx(personagemPegandoEsquerda, (Vector2){player.x, player.y-80}, 0.0f, scale, WHITE);
                }
            } else {
                if (andandoDireita) {
                    DrawTextureEx(personagemDireita, (Vector2){player.x, player.y-80}, 0.0f, scale, WHITE);
                } else {
                    DrawTextureEx(personagemEsquerda, (Vector2){player.x, player.y-80}, 0.0f, scale, WHITE);
                }
            }
            
            EndDrawing();
        }
        
        if (pausa) {
            BeginDrawing();
            DrawText("PAUSE", 500, 290, 70, GOLD);
            EndDrawing();
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
    
    menu();
    iniciarJogo(backgroundImage, personagemDireita, personagemEsquerda, personagemPegando, personagemPegandoEsquerda, chaveCenario, personagemPegandoChaveDireita, personagemPegandoChaveEsquerda);
    
    UnloadTexture(backgroundImage);
    UnloadTexture(personagemDireita);
    UnloadTexture(personagemEsquerda);
    UnloadTexture(personagemPegando);
    UnloadTexture(personagemPegandoEsquerda);
    UnloadTexture(chaveCenario);
    UnloadTexture(personagemPegandoChaveDireita);
    UnloadTexture(personagemPegandoChaveEsquerda);
    
    CloseWindow();
    
    return 0;
}

