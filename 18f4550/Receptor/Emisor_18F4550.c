#include <18F4550.h>
#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL2,CPUDIV1,VREGEN,NOBROWNOUT     // 8MHZ
//#fuses HSPLL,USBDIV,PLL5,CPUDIV1,VREGEN,NOWDT,NOPROTECT,NOLVP,NODEBUG,NOBROWNOUT   // 20 MHZ
#use delay(clock=48000000)


#include "lib_rf2gh4_10_4550.h" // Librería modificada para el el nRF24L01 con el PIC 18F4550.


/*********************************/
#define servo_00     pin_d4
#define servo_01    pin_d5
#define servo_02    pin_d6
#define servo_03    pin_d7

#define n 3                      // Servo Motor sayisini tutacak olan degisken
           

#define max_theCount 200// 200*100us=20ms
#define derecha  25// 25*100us=2,5ms
#define centro   15// 15*100us=1,5ms
#define izquierda 5// 5*100us =0,5ms

#define val_timer0 127

int8    theCount=1;
int8    duty[n];

#INT_TIMER0
void isr_timer0(){
   delay_cycles(5);
   if (duty[0] >= theCount)
      {
      output_high(servo_00); 
      }
      else
      {
      
          output_low(servo_00); 
         #asm
            nop
         #endasm
      }
      
      if (duty[1] >= theCount)
      {
       output_high(servo_01); 
      }
      else
      {
         output_low(servo_01); 
         #asm
            nop
         #endasm
      }
      
       if (duty[2] >= theCount)
      {
       output_high(servo_02); 
      }
      else
      {
         output_low(servo_02); 
         #asm
            nop
         #endasm
      }
      
       if (duty[3] >= theCount)
      {
       output_high(servo_03); 
      }
      else
      {
         output_low(servo_03); 
         #asm
            nop
         #endasm
      }
      
      theCount=theCount+1;
      if (theCount>max_theCount)
      {
         theCount=1;
      }
      else
      {
         delay_us(1);
         #asm
            nop
         #endasm
      }
      set_timer0(val_timer0);
}

/*************************************/

#byte porta=0xF80               // Dirección de los puertos A, B, C, D y E.
#byte portb=0xF81
#byte portc=0xF82
#byte portd=0xF83
#byte porte=0xF84

//******
//#bit  a0=porta.0           // Un truco para más tarde hacer intercambios de bits.

int8 ret1;
int8 pot_deger[7], out_deger;
//#bit  b0=data.0            // Un truco para más tarde hacer intercambios de bits.
//******

unsigned int8 pwm_valA=0,pwm_valB=0,pwm_dutyA=0,pwm_dutyB=0;

#int_timer1
void timer1_kesmesi()
{
     
      set_timer1(65185);//20mhz kristai ile 70usn
      if(++pwm_valA<pwm_dutyA)output_high(pin_a1);
      else output_low(pin_a1);
      if(++pwm_valB<pwm_dutyB)output_high(pin_a2);
      else output_low(pin_a2);      
} 

#int_ext2                  
void int_RB2()               // Esta rutina está para un futuro si haces comunicaciones bidireccionales.
{                            // No tiene efecto en el programa principal, ya que sólo emite.
output_high(pin_a0);
   ret1 = RF_RECEIVE();
   if ( (ret1 == 0) || (ret1 == 1) )
   {
      do
      {    
         pot_deger[0]=RF_DATA[0];  // Data contendrá el valor que le llege del emisor, a través de RF_DATA[0].
           pot_deger[1]=RF_DATA[1];  // Data contendrá el valor que le llege del emisor, a través de RF_DATA[0].
            pot_deger[2]=RF_DATA[2];  // Data contendrá el valor que le llege del emisor, a través de RF_DATA[0].
             pot_deger[3]=RF_DATA[3];  // Data contendrá el valor que le llege del emisor, a través de RF_DATA[0].
              pot_deger[4]=RF_DATA[4];  // Data contendrá el valor que le llege del emisor, a través de RF_DATA[0].
               pot_deger[5]=RF_DATA[5];  // Data contendrá el valor que le llege del emisor, a través de RF_DATA[0].
               out_deger=RF_DATA[6];
               pot_deger[7]=RF_DATA[7];
         //porta=data;       // Lo que haya en data lo refleja en los LEDs.
         //a0=b0;            // Un truco para que RB0 sirva de interrupción y RA0 para poner el LED correspondiente.
         ret1 = RF_RECEIVE(); 
      } while ( (ret1 == 0) || (ret1 == 1) ); // Tanto si existe entrada múltiple o simple de datos los lee. 
   }
   
         pwm_dutyA=pot_deger[4];// soft pwm
         pwm_dutyB=pot_deger[4];// soft pwm
   
    if(pot_deger[0]>150 && pot_deger[1]>90 && pot_deger[1]<140)
    {
      output_high(pin_d0); 
      output_low(pin_d1); 
      output_high(pin_d2); 
      output_low(pin_d3); 

    }
     if(pot_deger[0]<80 && pot_deger[1]>90 && pot_deger[1]<140)
    {
          output_low(pin_d0); 
          output_high(pin_d1); 
          output_low(pin_d2); 
          output_high(pin_d3); 
                 // pwm_dutyA=0;// soft pwm
    }
    
     if(pot_deger[1]>150 && pot_deger[0]>90 && pot_deger[0]<140)
    {
      output_high(pin_d0); 
      output_low(pin_d1); 
      output_low(pin_d2); 
      output_high(pin_d3); 

    }
     if(pot_deger[1]<80 && pot_deger[0]>90 && pot_deger[0]<140)
    {
          output_low(pin_d0); 
          output_high(pin_d1); 
          output_high(pin_d2); 
          output_low(pin_d3); 
                 // pwm_dutyA=0;// soft pwm
    }
   output_low(pin_a0);
}

void main()
{  
   RF_INT_EN();              // Habilitar interrupción RB0/INT.
   RF_CONFIG_SPI();          // Configurar módulos SPI del PIC.
   RF_CONFIG(0x40,0x08);     // Configurar módulo RF (canal y dirección).
   RF_ON();  // Activar el módulo RF.
   
      set_tris_a(0x00);
   output_a(0x00);
   
       int     k;
    int8    i=0;
    
    for(k=0;k<=(n-1);k++)
      duty[k]=centro;//
    
    setup_timer_0(RTCC_DIV_8 | RTCC_8_BIT);

    set_timer0(val_timer0);
    
    enable_interrupts(INT_TIMER0);
    enable_interrupts(GLOBAL);
    
     duty[0]=11;
     duty[1]=13;


     output_low(pin_d0); 
          output_low(pin_d1); 
          output_low(pin_d2); 
          output_low(pin_d3); 
          
    output_high(pin_a3);
                delay_ms(1000);
                output_low(pin_a3);
                

    set_timer1(15035);// soft pwm
    setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);// soft pwm
     enable_interrupts(int_timer1);// soft pwm
   enable_interrupts(GLOBAL);// soft pwm
   
   while(true)
   {   

if(pot_deger[2]>50)
{
  duty[0]= pot_deger[2]/10;
}
if(pot_deger[3]>50)
{
  duty[1]= pot_deger[3]/10;
}
if(pot_deger[5]>50)
{
  duty[2]= pot_deger[5]/10;
}
if(pot_deger[7]>50)
{
  duty[3]= pot_deger[7]/10;
}
if(out_deger==10)
{
  output_high(pin_a3);
                delay_ms(1000);
                output_low(pin_a3);
}


}// Bucle infinito.

   
  // int8 cont=0, ret2;
   
  // while(true)
  // {  
   //   RF_DATA[0] = cont;     // El contenido del contador lo cargo en RF_DATA[0] para ser enviado.
   //   RF_DIR=0x08;           // Dirección del receptor.
    //  ret2=RF_SEND();        // Envía el dato.
    //  cont++;                // Incrementa el contador.
   //   delay_ms(50);          // Una pausa en cada incremento.
 //  }
}
