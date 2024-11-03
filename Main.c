#include "raylib.h"

// Variáveis globais
int retangulo_x;
int retangulo_y;
int velocidadeBola = 3;
bool pausa = false;

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
void iniciarJogo(Texture2D backgroundImage, Texture2D personagemDireita, Texture2D personagemEsquerda) {
    bool andandoDireita = true; // Direção inicial
    while (!WindowShouldClose()) {
        Rectangle retangulo = {retangulo_x, retangulo_y, 60, 100};
        
        if (IsKeyPressed(KEY_P)) {
            pausa = !pausa;
        }
        
        if (!pausa) {
            // Movimento da bola
            if (IsKeyDown(KEY_UP)){
                retangulo_y -= velocidadeBola;
                }
            if (IsKeyDown(KEY_DOWN)){
                retangulo_y += velocidadeBola;
                }
            if (IsKeyDown(KEY_RIGHT)){
                retangulo_x += velocidadeBola;
                andandoDireita = true; // Personagem está indo para a direita
            }
            if (IsKeyDown(KEY_LEFT)){
                retangulo_x -= velocidadeBola;
                andandoDireita = false; // Personagem está indo para a esquerda
            }
            
            // Restringir a bola para não sair da tela
            if (retangulo_x < 0){
                retangulo_x = 0;
                }
                
            if (retangulo_y < 0){
                retangulo_y = 0;
                }
                
            if (retangulo_x > GetScreenWidth() - 60){
                retangulo_x = GetScreenWidth() - 60;
                }
                
            if (retangulo_y > GetScreenHeight() - 100){
                retangulo_y = GetScreenHeight() - 100;
                }
            
            Rectangle retanguloEstatico = {100, 100, 500, 300};
            
            // Colisão com o retângulo estático
            if (CheckCollisionRecs(retangulo, retanguloEstatico)) {
                float penetracaoEsquerda = (retangulo.x + retangulo.width) - retanguloEstatico.x;
                float penetracaoDireita = (retanguloEstatico.x + retanguloEstatico.width) - retangulo.x;
                float penetracaoCima = (retangulo.y + retangulo.height) - retanguloEstatico.y;
                float penetracaoBaixo = (retanguloEstatico.y + retanguloEstatico.height) - retangulo.y;

                if (penetracaoEsquerda < penetracaoDireita && penetracaoEsquerda < penetracaoCima && penetracaoEsquerda < penetracaoBaixo) {
                    // Colisão pela esquerda
                    retangulo_x = retanguloEstatico.x - retangulo.width;
                } else if (penetracaoDireita < penetracaoEsquerda && penetracaoDireita < penetracaoCima && penetracaoDireita < penetracaoBaixo) {
                    // Colisão pela direita
                    retangulo_x = retanguloEstatico.x + retanguloEstatico.width;
                } else if (penetracaoCima < penetracaoBaixo && penetracaoCima < penetracaoEsquerda && penetracaoCima < penetracaoDireita) {
                    // Colisão por cima
                    retangulo_y = retanguloEstatico.y - retangulo.height;
                } else if (penetracaoBaixo < penetracaoCima && penetracaoBaixo < penetracaoEsquerda && penetracaoBaixo < penetracaoDireita) {
                    // Colisão por baixo
                    retangulo_y = retanguloEstatico.y + retanguloEstatico.height;
                }
            }
            
            // Desenho do jogo
            BeginDrawing();
            ClearBackground(RAYWHITE);
            
            // Desenha a imagem de fundo
            DrawTexture(backgroundImage, 0, 0, WHITE);
            
            DrawRectangleRec(retanguloEstatico, BLACK);
            
            // Fator de escala para o personagem
            float scale = 3.5f; // Aumenta o tamanho do personagem em 350%
  
            // Desenha o personagem dependendo da direção
            if (andandoDireita) {
                DrawTextureEx(personagemDireita, (Vector2){retangulo_x, retangulo_y}, 0.0f, scale, WHITE); 
            } 
            if(!andandoDireita){
                DrawTextureEx(personagemEsquerda, (Vector2){retangulo_x, retangulo_y}, 0.0f, scale, WHITE); 
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
    // Configuração inicial da janela e posição inicial da bola
    int larguraTela = 1280, alturaTela = 720;
    retangulo_x = larguraTela / 2;
    retangulo_y = alturaTela / 2;
        
    InitWindow(larguraTela, alturaTela, "Uma Noite no Castelo");
    SetTargetFPS(60);
    
    // Carregar recursos (ex.: imagens, texturas)
    Texture2D backgroundImage = LoadTexture("Imagens/background.png");
    Texture2D personagemDireita = LoadTexture("Imagens/personagemDireita.png"); // Carregar a textura do personagem olhando para a direita
    Texture2D personagemEsquerda = LoadTexture("Imagens/personagemEsquerda.png"); // Carregar a textura do personagem olhando para a esquerda

    // Exibir menu e iniciar o jogo
    menu();
    iniciarJogo(backgroundImage, personagemDireita, personagemEsquerda);
    
    // Descarregar as texturas e fechar a janela
    UnloadTexture(backgroundImage);
    UnloadTexture(personagemDireita);
    UnloadTexture(personagemEsquerda); // Descarregar a textura do personagem
    CloseWindow();
    
    return 0;
}
