#include<AES.h>
#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile
#include "./printf.h"

//aes encryption part 
AES aes;

unsigned long long int my_iv = 01234567;
//byte cipher;
//byte comfirm;
//int plainLength =sizeof(plain);

byte *key = (unsigned char*)"0123456789101112";
 
byte max_playload =0;

RH_ASK driver;
String serialdata; //Holds data coming in
//char msg[50]; //Char array to send data out 

void encryptNow(char*data,byte bits,byte length)
{
  byte padedLength =length +N_BLOCK -length% N_BLOCK;
  aes.iv_inc();
  byte iv[N_BLOCK];
  byte plain_paded[padedLength];
  byte cipher[padedLength];
  byte check[padedLength];
  unsigned long mirco_sec =micros();
  aes.set_IV(my_iv);
  aes.get_IV(iv);
  aes.do_aes_encrypt(data,length,cipher,key,bits,iv);


  //to check if encryption and decryption
   aes.set_IV(my_iv);
   aes.get_IV(iv);
   aes.do_aes_decrypt(cipher,padedLength,check,key,bits,iv);
   printf("\nciphertext: ");
   aes.printArray(cipher,(bool)false);


   //sending the ciphertext to the reciever
    driver.send((uint8_t *)cipher,sizeof(cipher));//Sending encrypted message
    driver.waitPacketSent();


   //printing the ciphered result
  char result;
 #define printable check
  for (byte i = 0; i < sizeof(printable); ++i){
 
    result = printable[i];
  }
  Serial.println("*********************************************************************");
}

byte max_payload=0;
void setup()
{
    Serial.begin(9600);    // Debugging only
    if (!driver.init()){
    
    Serial.println(F("init failed"));}

     printf_begin();
     max_payload =(driver.maxMessageLength()/N_BLOCK)*N_BLOCK;
     Serial.println(F("Enter Message: "));
     Serial.println("");
}

void loop()
{
  char data[128]=""; 
  byte bytes =0;
  if (Serial.available()){
    while(Serial.available()>0)
    {
      char buff;
      buff=Serial.read();
      if(buff!=0x0A && buff!=0x0D)
      {
        data[bytes]=buff;
        ++bytes;
      }
      delay(10);
    }
    data[bytes]=0x0;
    Serial.print("plaintext: ");
    Serial.println(data);
    byte data_len=strlen(data)+1;
    {
      if (data_len<max_payload)
      {
        encryptNow(data,128,data_len);
      }
      else
      {
        Serial.println(F("Exceeded the radiohead library maximum payload"));
      }
    }
    
    
    delay(1000);
  }
  }
