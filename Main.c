#include "raylib.h"

// Variáveis globais
int retangulo_x;
int retangulo_y;
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
void iniciarJogo(Texture2D backgroundImage, Texture2D personagemDireita, Texture2D personagemEsquerda, Texture2D personagemPegando, Texture2D personagemPegandoEsquerda) {
    bool andandoDireita = true; // Direção inicial
    while (!WindowShouldClose()) {
        Rectangle retangulo = {retangulo_x, retangulo_y, 60, 100};
        
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
            if (retangulo_x < 0) retangulo_x = 0;
            if (retangulo_y < 300) retangulo_y = 300;
            if (retangulo_x > GetScreenWidth() - 83) retangulo_x = GetScreenWidth() - 83;
            if (retangulo_y > GetScreenHeight() - 220) retangulo_y = GetScreenHeight() - 220;
            
            Rectangle retanguloEstatico = {100, 100, 500, 300};
            
            // Colisão com o retângulo estático
            if (CheckCollisionRecs(retangulo, retanguloEstatico)) {
                float penetracaoEsquerda = (retangulo.x + retangulo.width) - retanguloEstatico.x;
                float penetracaoDireita = (retanguloEstatico.x + retanguloEstatico.width) - retangulo.x;
                float penetracaoCima = (retangulo.y + retangulo.height) - retanguloEstatico.y;
                float penetracaoBaixo = (retanguloEstatico.y + retanguloEstatico.height) - retangulo.y;

                if (penetracaoEsquerda < penetracaoDireita && penetracaoEsquerda < penetracaoCima && penetracaoEsquerda < penetracaoBaixo) {
                    retangulo_x = retanguloEstatico.x - retangulo.width;
                } else if (penetracaoDireita < penetracaoEsquerda && penetracaoDireita < penetracaoCima && penetracaoDireita < penetracaoBaixo) {
                    retangulo_x = retanguloEstatico.x + retanguloEstatico.width;
                } else if (penetracaoCima < penetracaoBaixo && penetracaoCima < penetracaoEsquerda && penetracaoCima < penetracaoDireita) {
                    retangulo_y = retanguloEstatico.y - retangulo.height;
                } else if (penetracaoBaixo < penetracaoCima && penetracaoBaixo < penetracaoEsquerda && penetracaoBaixo < penetracaoDireita) {
                    retangulo_y = retanguloEstatico.y + retanguloEstatico.height;
                }
            }
            
            // Desenho do jogo
            BeginDrawing();
            ClearBackground(RAYWHITE);
            
            DrawTexture(backgroundImage, 0, 0, WHITE);
            DrawRectangleRec(retanguloEstatico, BLACK);
            
            // Fator de escala para o personagem
            float scale = 3.5f;

            // Verifica se o personagem está no estado de "pegando" e a direção em que ele está olhando
            if (pegando) {
                if (andandoDireita) {
                    DrawTextureEx(personagemPegando, (Vector2){retangulo_x, retangulo_y}, 0.0f, scale, WHITE);
                }else {
                DrawTextureEx(personagemPegandoEsquerda, (Vector2){retangulo_x, retangulo_y}, 0.0f, scale, WHITE);
            }
            } else {
            // Desenha o personagem normal, dependendo da direção
                if (andandoDireita) {
                    DrawTextureEx(personagemDireita, (Vector2){retangulo_x, retangulo_y}, 0.0f, scale, WHITE);
            }   else {
                    DrawTextureEx(personagemEsquerda, (Vector2){retangulo_x, retangulo_y}, 0.0f, scale, WHITE);
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
    retangulo_x = larguraTela / 2;
    retangulo_y = alturaTela / 2;
        
    InitWindow(larguraTela, alturaTela, "Uma Noite no Castelo");
    SetTargetFPS(60);
    
    // Carregar recursos (ex.: imagens, texturas)
    Texture2D backgroundImage = LoadTexture("Imagens/background.png");
    Texture2D personagemDireita = LoadTexture("Imagens/personagemDireita.png");
    Texture2D personagemEsquerda = LoadTexture("Imagens/personagemEsquerda.png");
    Texture2D personagemPegando = LoadTexture("Imagens/personagemPegando.png"); // Nova imagem de pegando
    Texture2D personagemPegandoEsquerda = LoadTexture("Imagens/personagemPegandoEsquerda.png");
    
    menu();
    iniciarJogo(backgroundImage, personagemDireita, personagemEsquerda, personagemPegando, personagemPegandoEsquerda);
    
    UnloadTexture(backgroundImage);
    UnloadTexture(personagemDireita);
    UnloadTexture(personagemEsquerda);
    UnloadTexture(personagemPegando);
    UnloadTexture(personagemPegandoEsquerda);
    CloseWindow();
    
    return 0;
}
