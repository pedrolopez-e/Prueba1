/* Este programa pide al usuario que ingrese un numero del 0 al 59 y utiliza al rtc para indicar cuando salta
la alarma en el segundo ingresado, cada minuto */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>

#define P 0x70

unsigned char in(unsigned char registro){
    outb(registro, P);
    return inb(P + 1);
}

void out(unsigned char valor, unsigned char reg){
    outb(reg, P);
    outb(valor, P + 1);
}

int main(){
    unsigned char seg, a, c, i, b = 1;

    printf("Ingrese el segundo en el que quiere la alarma (0-59): ");
    scanf("%hhx", &seg);

    /* Doy permiso a los ports 70 y 71 */
    if (ioperm(P, 2, 1)) {
        perror("ioperm"); 
        exit(1);
    }

  /* Seteo segundos con el dato del usuario y minutos y hora alarma con codigos indiferentes */
  out(seg, 0x01); 
  out(0xFF, 0x03);  
  out(0xFF, 0x05); 

  c = in(0x0C); /* Borro flags anteriores */
  printf("Esperando las alarmas...\n");

  while (b){
      c = in(0x0C);
      /* Chequeo el bit 5 (AF) del registro C */
      if (c & 0x20){
	      printf("Alarma\n"); //Cuando el AF indica alarma
      }
      usleep(500000);   /* Esperar 500ms */ 
  }

  /* Retiro permisos */
  if (ioperm(P, 2, 0)){
      perror("ioperm"); 
      exit(1);
  }
  return 0;
}