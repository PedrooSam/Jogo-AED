#include "raylib.h"

void menu(){
    
    Rectangle botaoIniciar = {640,360,200,100};
    BeginDrawing();
    DrawText("iniciar", 640, 360, 18, BLUE);
    DrawRectangleRec(botaoIniciar, GRAY);
    EndDrawing();
    while(1){
        if(CheckCollisionPointRec(GetMousePosition(), botaoIniciar) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            break;
        } 
    } 
}
void iniciarJogo(){
    while(! WindowShouldClose()){
        if(IsKeyDown(KEY_UP)){
            bola_y -=velocidadeBola;
        }
        else if(IsKeyDown(KEY_DOWN)){
            bola_y +=velocidadeBola;
        }
        else if(IsKeyDown(KEY_RIGHT)){
            bola_x+=velocidadeBola;
        }
        else if(IsKeyDown(KEY_LEFT)){
            bola_x-=velocidadeBola;
        }
        
        if(bola_x <= 0){
            bola_x += velocidadeBola;
        }
        if(bola_y <= 0){
            bola_y += velocidadeBola;
        }
        if(bola_x >= GetScreenWidth()){
            bola_x -=velocidadeBola;
        }
        if(bola_y >= GetScreenHeight()){
            bola_y -= velocidadeBola;
        }
        if(IsKeyDown(KEY_P)){
            menu();
        }
        
        
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircle(bola_x, bola_y, 30, BLUE);
        //DrawTexture(backgroundImage, 0,0, WHITE);
        EndDrawing();
    }
}
int main(void){
    int larguraTela = 1280, alturaTela = 720;
    int bola_x = larguraTela/2;
    int bola_y = alturaTela/2;
    int velocidadeBola = 3;
       
    
    InitWindow(larguraTela, alturaTela, "Uma Noite no Castelo");
    SetTargetFPS(60);
    
    //Texture2D backgroundImage = LoadTexture("jogo de AED/guarda.werb");

    //UnloadTexture(backgroundImage);
    CloseWindow();
    return 0;
}