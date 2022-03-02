#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile
#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h> 
#include <AES.h>
#define Password_length 8

//for the encryption part 
AES aes;
unsigned long long int my_iv = 01234567;
byte *key = (unsigned char*)"0123456789101112";


RH_ASK driver;
const byte coded_max_payload = 60; 
String text ="";
const byte coded_max_char =coded_max_payload/N_BLOCK*N_BLOCK;
//encrypted;

//For the keypad password

char Data[Password_length];
int Master[Password_length] = {1,2,3,4,5,6,7};
byte data_count =0 ,master_count =0 ;
bool Pass_is_good;
char custom_key; 

const byte ROWS =4; //its a 4x3 keypads hence 4 rows and 3 columns 
const byte COLS =3;

char keys[ROWS][COLS]={
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS]={2,3,4,5}; //connecting the row pinouts of the keypad
byte colPins[COLS]={6,7,8};   //connecting the cols pinouts

Keypad keypad=Keypad(makeKeymap(keys) ,rowPins , colPins, ROWS ,COLS);

LiquidCrystal_I2C lcd(0x27, 16, 2);
void clearData();

//Function for the password
int password()
{
  lcd.setCursor(0,0);
  lcd.print("New Message");
  delay(2000);

  for(int i=0; i<4; i++)
   {
    lcd.backlight();
    delay(250);
    lcd.noBacklight();
    delay(250);
   }
  //Turn backlight back on 
  lcd.backlight();
  lcd.clear();
  delay(1000);
  
  lcd.setCursor(0,0);
  lcd.print("Enter Password:");
  delay(2000);

  int pass_word[7];
  int index=0;
  while(index<7)
  {
     custom_key = keypad.getKey();
    if(custom_key)
    {
      lcd.setCursor(index,1);
      pass_word[index]=custom_key;
      lcd.print("*");
      index++;
      Serial.print(custom_key);
    }
  }
    lcd.clear();
    int check=1;
    for(int i=0;i<=5;i++)
    {
      if(pass_word[i]-48==Master[i]){
        check*1;}
      
       else{
      check=check*0; 
      Serial.print(pass_word[i]-48);
      Serial.print(" ");
      Serial.print(Master[i]);
      Serial.print(" ");
      Serial.println("Wrong");}
    }

    if(check == 1)
    {
      lcd.print("Correct");
      delay(2000);
      lcd.clear();
      clearData();
      return (1);
    }
    else{
      lcd.print("Incorrect");
      delay(2000);
      lcd.clear();
      clearData();
      //return 0;
      password();
    }
 } 
    void clearData(){
      while(data_count !=0){
        Data[data_count--]=0;
      }
      return;
    }


 //function for decryption 
 
void decryptNow(char*data,byte bits,byte*final_result)
{
  int plainLength =coded_max_char;
  int paddedLength = plainLength + N_BLOCK - plainLength % N_BLOCK;
  aes.iv_inc();
  byte iv [N_BLOCK] = "";

  Serial.print(F("MaxSizevar: "));
  Serial.print(paddedLength);
  Serial.println("");
  Serial.print(F("MaxSizemsg: "));
  Serial.print(plainLength);
  Serial.println("");
  Serial.print(F("Ciphertext padded: "));
  Serial.print(data);
  char buffer; 
 #define printable data 
 for (byte i = 0; i < paddedLength; i++){
 buffer = printable[i];
 }
 Serial.println();
 
 aes.set_IV(my_iv); //setting initial vector
 aes.get_IV(iv); //geeting the initial vector

 aes.do_aes_decrypt(data,paddedLength,final_result,key,bits,iv);
}


void setup()
{
  lcd.begin(16,2);
    Serial.begin(9600);  // Debugging only
    if (!driver.init())
    Serial.println("init failed"); 
    byte max_payload =0;   
    max_payload = (driver.maxMessageLength()/N_BLOCK)*N_BLOCK;
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0,0);    
    
    
 }

void loop()
{
    uint8_t buf[coded_max_payload];
    uint8_t buflen = sizeof(buf);
    if (driver.recv(buf, &buflen)) // Non-blocking
    {
     void decryptNow(char*data ,byte bits ,byte*final_result);
      decryptNow(buf,128,buf);
      Serial.print(F("Message Decoded:"));
      Serial.print(F(""));
      
      byte i=0;
      char buffer;
      #define printable buf 
      buffer = printable[i];
      
      if(buffer !=0){
      
      text=(char*)buf;
      Serial.println(text);
      lcd.clear();
      //lcd.print(text);
      
      char result = password();
      if( result == 1)
      {
        lcd.print(text);
        while (true)
        {
        custom_key = keypad.getKey();
        Serial.println(custom_key);
          if (custom_key == '*')
            {
              break;
            }
         }
        lcd.clear();
      }
   }
 }
      delay(8000);
      lcd.clear();
      //lcd.write((char*)buf);        
    }
    
