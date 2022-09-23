#include <Arduino.h>
int board[ 8 ][ 8 ];
int posiPomme[2];
int display[8];
int score = 0;
int DIN_pin = 11;
int CS_pin = 10;
int CLK_pin = 12; 
int snakeTail[5][2] = { {1,7}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}};

int lastX;
int lastY;

// Allume la  nouvelle led lors d'un déplacement
void GetNewTabDisplay(){
    for(int j = 0; j<8;j++){
        int couterforint = 0;
        for(int i = 0; i<8;i++){
            if(board[j][i] ==1) {
            if(i==7)couterforint++;
            else if(i==6)couterforint+=2;
            else
            couterforint+=pow(2,7-i)+1;
            }
        }    
        display[j] = couterforint;
    }
}

// Récupère les valeurs du joystick en X et en Y
void GetValJoystick(){
    int valX = analogRead(A0)- 512;//0-1023
    int valY = analogRead(A1) - 512;//-512-512
    if(valX< 100 && valX>-100 && valY< 100 && valY>-100 ) return;
    
    for(int i = score; i>=1; i--) {
                snakeTail[i][0]=snakeTail[i-1][0];
                snakeTail[i][1]=snakeTail[i-1][1];
            }
    
    if(abs(valX)>abs(valY)){
        if(valX>0){
            snakeTail[0][0]--;
            if(snakeTail[0][0]<0)snakeTail[0][0] = 0;
        } 
        else{
            snakeTail[0][0]++;
            if(snakeTail[0][0]>7)snakeTail[0][0] = 7;
        }
    }
    else{
        if(valY>0){
            snakeTail[0][1]++;
            if(snakeTail[0][1]>7)snakeTail[0][1] = 7;
        } 
        else{
            snakeTail[0][1]--;
            if(snakeTail[0][1]<0)snakeTail[0][1] = 0;
        }
    }
    
}

// Replace la pomme sur la matrice
void ReplacePomme(){
    posiPomme[0]= random(8);
    posiPomme[1]= random(8);
}

// Augmente le score et replace la pomme lors de la collision
void Collision(){
    if(snakeTail[0][0]==posiPomme[0]&&snakeTail[0][1]==posiPomme[1]){
        score ++;
        // snakeTail[score][0] = lastX;
        // snakeTail[score][1] = lastY;
        ReplacePomme();
    }
}

void write_pix(int data) {
    digitalWrite(CS_pin, LOW);
    for (int i = 0; i < 8; i++)
    {
        digitalWrite(CLK_pin, LOW);
        digitalWrite(DIN_pin, data & 0x80); // masquage de donnée
        data = data << 1; // on décale les bits vers la gauche
        digitalWrite(CLK_pin, HIGH);
    }
}

void write_line(int adress, int data) {
    digitalWrite(CS_pin, LOW);
    write_pix(adress);
    write_pix(data);
    //Serial.println(data);
    digitalWrite(CS_pin, HIGH);
}

void write_matrix(int *tab) {
    for (int i = 0; i < 8; i++) write_line(i + 1, tab[i]);
}

void init_MAX7219(void) {
    write_line(0x09, 0x00); //decoding BCD
    write_line(0X0A, 0x01); //brightness
    write_line(0X0B, 0x07); //scanlimit 8leds
    write_line(0X0C, 0x01); //power-down mode 0, normalmode1;
    write_line(0X0F, 0x00);
}

void clear_matrix(void) {
    int clean[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    write_matrix(clean);
}

int intToHex(int x) {
    switch (x) {
        case 0: return 0x01; break; //LED sur la première case
        case 1: return 0x02; break; //LED sur 2 case
        case 2: return 0x04; break; //LED sur 3 case
        case 3: return 0x08; break; //LED sur 4 case
        case 4: return 0x10; break; //LED sur 5 case
        case 5: return 0x20; break; //LED sur 6 case
        case 6: return 0x40; break; //LED sur 7 case
        case 7: return 0x80; break; //LED sur 8 case
    }
}

void setup() {
    Serial.begin(9600);
    // Serial.println(pow(2,i));
    ReplacePomme();
    pinMode(CS_pin, OUTPUT);
    pinMode(DIN_pin, OUTPUT);
    pinMode(CLK_pin, OUTPUT);
    init_MAX7219();
    clear_matrix();
}

void loop() {
    lastX = snakeTail[score][0];
    lastY = snakeTail[score][1];
    GetValJoystick();
    Collision();

    // Eteint toutes les led hors position du snake et de la pomme
    for(int j = 0; j<8;j++){
        for(int i = 0; i<8;i++){
            board[j][i] = 0;
        }    
    }

    // Affiche la pomme et le snake
    for (int i =0;i<=score;i++)
        board[snakeTail[i][0]][snakeTail[i][1]] = 1;
    board[posiPomme[0]][posiPomme[1]] = 1;
    GetNewTabDisplay();
    write_matrix(display);
    delay(100);
}
