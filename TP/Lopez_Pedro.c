#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>

#define P 0x70

/* Esta funcion devuelve el valor guardado en el registro pasado por parametro */
unsigned char in(unsigned char registro){
    outb(registro, P); //se accede al valor a traves del puerto 0x71 (se posiciona en ese registro)
    return inb(P + 1); //se toma/devuelve/lee ese valor a traves del puerto 0x71 (puerto P + 1)
}

/* Funcion que escribe sobre el registro pasado por parametro*/
void out(unsigned char valor, unsigned char reg){
    outb(reg, P);
    outb(valor, P + 1);
}

/* Funcion que convierte a binario */
void convertir_b(unsigned char numero){
  int i, binario[8] = {0,0,0,0,0,0,0,0};

  for (i = 0; numero > 0; i++){
    binario[i] = numero % 2;
    numero = numero / 2;
  }

  for (i = 7; i >= 0; i--){
    printf("|  ");                
    printf("%d  ", binario[i]);
  }
  return;
}

/* Funcion que verifica que el reloj no se esté actualizando */
void uip(){
    if ((in(0x0A) & (0x80)) != 0){ //Veo si el reloj se está actualizando usando el UIP del registro A
        usleep(1984);
    }
}

/* Funcion que imprime tabla con el estado de los registros A, B y C del rtc */
void tabla(){
    unsigned char a, b, c;

    /* Leo registros A, B y C */
    uip();//Veo si hay actualizaciones
    a = in(0x0A);
    b = in(0x0B);
    c = in(0x0C);

    /* Imprimo tabla */
    printf("|===============================================|\n");
    printf("|__________________REGISTRO A___________________|\n");
    printf("| UIP | DV2 | DV1 | DV0 | RS3 | RS2 | RS1 | RS0 |\n");
    convertir_b(a);
    printf("|\n");
    printf("|===============================================|\n");
    printf("|__________________REGISTRO B___________________|\n");
    printf("| SET | PIE | AIE | UIE | SQWE|  DM |24/12| DSE |\n");
    convertir_b(b);
    printf("|\n");
    printf("|===============================================|\n");
    printf("|__________________REGISTRO C___________________|\n");
    printf("| IRQF|  PF |  AF |  UF |  0  |  0  |  0  |  0  |\n");
    convertir_b(c);
    printf("|\n");
    printf("|===============================================|\n");
    
    return;
}

/* Funcion alarma */
void alarma(){
    unsigned char seg, a, c, i, b = 1;

    printf("Ingrese el segundo en el que quiere la alarma (0-59): ");
    scanf("%hhx", &seg);

    uip();

    /* Seteo segundos con el dato del usuario y minutos y hora alarma con codigos indiferentes para que sea cada minuto */
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
    return;
}

/* Funcion tren de pulsos */
void pulsos(){
    unsigned char a, c, i, frec, pf; // a: registro a, c: registro c, i: contador, frec: frecuencia de pulso

    uip();

    /*Cambio la frecuencia del SQW a 4hz usando los 4 primeros bits del registro A*/
    a = in(0x0A); //Leo el registro A
    frec = a | 0x0E; //Establezlo la frecuencia en 4Hz (0x0E equivale a 00001110)   
    out(frec, 0x0A);

    printf("\nTREN DE PULSOS CON FRECUENCIA 4HZ\n\n");
    printf("0---+---1---+---2---+---3---+---4---+---5---+---6---+---7---+---8---+---9---+---10\n\n"); 

    c = in(0x0C);//Borro flags anteriores
    for (i = 0; i <= 80; i++){ // Espera 10 segundos en total
      c = in(0x0C);
      pf = c & 0x40; //Veo si el bit pf del registro C está activado

      if (pf != 0){
          printf("|");
          fflush(stdout);
      };

      if (pf == 0){
          printf("_");
          fflush(stdout);
      };

      usleep(125000); //Esperar 125ms 
    }
      return;
}

int main(){
    int opcion;

    /* Establezco permisos de los ports 70 y 71 */
    if (ioperm(P, 2, 1)){
        perror("ioperm");
        exit(1);
    };

    printf("\nTP PORTS\n");
    printf("Alumno: Pedro López\nGrupo: 1\n\n");
    /* Menú */
    printf("Elija una de las siguientes opciones:\n");
    printf("1. Tabla de registros\n");
    printf("2. Alarma\n");
    printf("3. Tren de pulsos\n");
    scanf("%d", &opcion);
    switch(opcion){
        case 1:
               tabla();
               break;
        case 2:
               alarma();
               break;
        case 3:
               pulsos();
               break;
        default:
               printf("Opción no disponible\n");
    }
    
    /* Retiro permisos */
    if (ioperm(P, 2, 0)){
        perror("ioperm");
        exit(1);
    }

    return 0;    
}