#include "raylib.h"

// Variáveis globais
int bola_x;
int bola_y;
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
void iniciarJogo() {
    while (!WindowShouldClose()){
        if (IsKeyPressed(KEY_P)) {
            
            pausa = !pausa;
       }
        if(!pausa){
            // Movimento da bola
            if (IsKeyDown(KEY_UP)) {
                bola_y -= velocidadeBola;
            }
            if (IsKeyDown(KEY_DOWN)) {
                bola_y += velocidadeBola;
            }
            if (IsKeyDown(KEY_RIGHT)) {
                bola_x += velocidadeBola;
            }
            if (IsKeyDown(KEY_LEFT)) {
                bola_x -= velocidadeBola;
            }
            
            // Restringir a bola para não sair da tela
            if (bola_x < 0){
                bola_x = 0;
            }
                    
            if (bola_y < 0){
                bola_y = 0;
            }
                    
            if (bola_x >= GetScreenWidth()){
                bola_x -=3;
            } 
                    
            if (bola_y >= GetScreenHeight()) {
                bola_y -=3;
            }
                    
            // Voltar ao menu se a tecla P for pressionada
            
            Rectangle retangulo = {100, 100, 500, 300};
            Vector2 vetor = {bola_x, bola_y};
            
            
            
            if(CheckCollisionCircleRec(vetor, 30, retangulo)){
                
            }
            // Desenho do jogo
            BeginDrawing();
            ClearBackground(RAYWHITE);
            
            DrawRectangleRec(retangulo, BLACK);
        
            DrawRectangle(bola_x, bola_y,60,100, BLUE);
            
            EndDrawing();
        }
      
       if(pausa){
           BeginDrawing();
            
           DrawText("PAUSE", 500, 290, 70, GOLD);
                
           EndDrawing();
        }
    }
}
int main(void) {
    // Configuração inicial da janela e posição inicial da bola
    int larguraTela = 1280, alturaTela = 720;
    bola_x = larguraTela / 2;
    bola_y = alturaTela / 2;
    
    InitWindow(larguraTela, alturaTela, "Uma Noite no Castelo");
    SetTargetFPS(60);

    // Carregar recursos (ex.: imagens, texturas) se necessário
    
    // Texture2D backgroundImage = LoadTexture("caminho/para/imagem.png");

    // Exibir menu e iniciar o jogo
    menu();
    iniciarJogo();
    
    // Apartir daqui tem que descarregar recursos e fechar a janela
    
    // UnloadTexture(backgroundImage);
    CloseWindow();
    
    return 0;
}
