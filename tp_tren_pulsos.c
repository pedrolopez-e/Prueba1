//Este programa imprime un tren de pulsos de 4Hz.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>
#define P 0x70 

unsigned char in(unsigned char reg){
  outb(reg, P);
  return inb(P + 1);
}

void out(unsigned char valor, unsigned char reg){
  outb(reg, P);
  outb(valor, P + 1);
}

int main(){
  unsigned char a, c, i, frec, pf; // a: registro a, c: registro c, i: contador, frec: frecuencia de pulso

  /* Doy permisos a los ports 70 y 71 */
  if (ioperm(P, 2, 1)){
      perror("ioperm"); 
      exit(1);
  }

  /*Cambio la frecuencia del SQW a 4hz usando los 4 primeros bits del registro A*/
  a = in(0x0A); //Leo el registro A
  frec = a | 0x0E; //Establezlo la frecuencia en 4Hz (0x0E equivale a 00001110)   
  out(frec, 0x0A);

  printf("TREN DE PULSOS CON FRECUENCIA 4HZ\n\n");
  printf("0---+---1---+---2---+---3---+---4---+---5---+---6---+---7---+---8---+---9---+---10\n\n"); 

  c = in(0x0C);//Borro flags anteriores
  for (i = 0; i <= 80; i++){ // Espera 10 segundos en total
    c = in(0x0C);
    pf = c & 0x40; //Veo si el bit pf del registro C está activado
    if (pf == 0){
        printf("_");
        fflush(stdout);
    };   
    if (pf != 0){
        printf("|"); //Imprimo de esta forma porque por alguna razon, no imprime _ en la otra condicion, como si PF fuera siempre 1
        fflush(stdout);
    };

    usleep(125000);   /* Esperar 125ms */ 
  }

  //Retiro permisos
  if (ioperm(P, 2, 0)){
        perror("ioperm");
        exit(1);
  }

  return 0;
}
