#include "raylib.h"

// Variáveis globais
int player_x;
int player_y;
int velocidadeBola = 5;
bool pausa = false;
bool pegando = false; // Variável para verificar se o personagem está pegando algo

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
void iniciarJogo(Texture2D backgroundImage, Texture2D personagemDireita, Texture2D personagemEsquerda, Texture2D personagemPegando, Texture2D personagemPegandoEsquerda, Texture2D chaveCenario) {
    bool andandoDireita = true; // Direção inicial
    while (!WindowShouldClose()) {
        Rectangle retangulo = {player_x, player_y, 60, 100};
        
        if (IsKeyPressed(KEY_P)) {
            pausa = !pausa;
        }
        
        if (!pausa) {
            // Verificar se a tecla 'E' foi pressionada para pegar algo
            if (IsKeyPressed(KEY_E)) {
                if(pegando){
                    pegando = false;
                }
                else{
                    pegando = true;
                }
            }   
            
            // Movimento da bola
            if (IsKeyDown(KEY_UP)){
                player_y -= velocidadeBola;
                }
            if (IsKeyDown(KEY_DOWN)){
                player_y += velocidadeBola;
                }
            if (IsKeyDown(KEY_RIGHT)){
                player_x += velocidadeBola;
                andandoDireita = true; // Personagem está indo para a direita
            }
            if (IsKeyDown(KEY_LEFT)){
                player_x -= velocidadeBola;
                andandoDireita = false; // Personagem está indo para a esquerda
            }
            
            // Restringir a bola para não sair da tela
            // Os valores são únicos pra ficar visualmente real com o personagem 
            if (player_x < 0){     // Canto esquerdo
                player_x = 0;
                }
                
            if (player_y < 300){     // Sobe até onde tem chão
                player_y = 300;
                }
                
            if (player_x > GetScreenWidth() - 83){  // Canto direito
                player_x = GetScreenWidth() - 83;
                }
                
            if (player_y > GetScreenHeight() - 220){ // Canto inferior
                player_y = GetScreenHeight() - 220;
                }
            
            // Rectangle retanguloEstatico = {100, 100, 500, 300};
            
            // Colisão com o retângulo estático
            /*if (CheckCollisionRecs(retangulo, retanguloEstatico)) {
                float penetracaoEsquerda = (retangulo.x + retangulo.width) - retanguloEstatico.x;
                float penetracaoDireita = (retanguloEstatico.x + retanguloEstatico.width) - retangulo.x;
                float penetracaoCima = (retangulo.y + retangulo.height) - retanguloEstatico.y;
                float penetracaoBaixo = (retanguloEstatico.y + retanguloEstatico.height) - retangulo.y;

                if (penetracaoEsquerda < penetracaoDireita && penetracaoEsquerda < penetracaoCima && penetracaoEsquerda < penetracaoBaixo) {
                    // Colisão pela esquerda
                    player_x = retanguloEstatico.x - retangulo.width;
                } else if (penetracaoDireita < penetracaoEsquerda && penetracaoDireita < penetracaoCima && penetracaoDireita < penetracaoBaixo) {
                    // Colisão pela direita
                    player_x = retanguloEstatico.x + retanguloEstatico.width;
                } else if (penetracaoCima < penetracaoBaixo && penetracaoCima < penetracaoEsquerda && penetracaoCima < penetracaoDireita) {
                    // Colisão por cima
                    player_y = retanguloEstatico.y - retangulo.height;
                } else if (penetracaoBaixo < penetracaoCima && penetracaoBaixo < penetracaoEsquerda && penetracaoBaixo < penetracaoDireita) {
                    // Colisão por baixo
                    player_y = retanguloEstatico.y + retanguloEstatico.height;
                }
            }*/ // COLISAO
            
            // Desenho do jogo
            BeginDrawing();
            ClearBackground(RAYWHITE);
            
            DrawTexture(backgroundImage, 0, 0, WHITE);
            
            // DrawRectangleRec(retanguloEstatico, BLACK);  RETANGULO PRETO
            
            DrawTextureEx(chaveCenario, (Vector2){500, 550}, 0.0f, 1, WHITE);
            
            // Fator de escala para o personagem
            float scale = 3.5f; // Aumenta o tamanho do personagem em 350%

            // Verifica se o personagem está no estado de "pegando" e a direção em que ele está olhando
            if (pegando) {
                if (andandoDireita) {
                    DrawTextureEx(personagemPegando, (Vector2){player_x, player_y}, 0.0f, scale, WHITE);
                }else {
                DrawTextureEx(personagemPegandoEsquerda, (Vector2){player_x, player_y}, 0.0f, scale, WHITE);
            }
            } else {
            // Desenha o personagem normal, dependendo da direção
                if (andandoDireita) {
                    DrawTextureEx(personagemDireita, (Vector2){player_x, player_y}, 0.0f, scale, WHITE);
            }   else {
                    DrawTextureEx(personagemEsquerda, (Vector2){player_x, player_y}, 0.0f, scale, WHITE);
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
    player_x = larguraTela / 2;
    player_y = alturaTela / 2;
        
    InitWindow(larguraTela, alturaTela, "Uma Noite no Castelo");
    SetTargetFPS(60);
    
    // Carregar recursos (ex.: imagens, texturas)
    Texture2D backgroundImage = LoadTexture("cenario/background.png");
    Texture2D personagemDireita = LoadTexture("Imagens/personagemDireita.png");
    Texture2D personagemEsquerda = LoadTexture("Imagens/personagemEsquerda.png");
    Texture2D personagemPegando = LoadTexture("Imagens/personagemPegando.png"); // Nova imagem de pegando
    Texture2D personagemPegandoEsquerda = LoadTexture("Imagens/personagemPegandoEsquerda.png");
    Texture2D chaveCenario = LoadTexture("cenario/chaveCenario.png");
    
    menu();
    iniciarJogo(backgroundImage, personagemDireita, personagemEsquerda, personagemPegando, personagemPegandoEsquerda, chaveCenario);
    
    UnloadTexture(backgroundImage);
    UnloadTexture(personagemDireita);
    UnloadTexture(personagemEsquerda);
    UnloadTexture(personagemPegando);
    UnloadTexture(personagemPegandoEsquerda);
    UnloadTexture(chaveCenario);
    CloseWindow();
    
    return 0;
}
