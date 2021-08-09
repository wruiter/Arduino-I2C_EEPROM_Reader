#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("I2C Tool Available, please enter commands:");
}
void loop() {
  while (Serial.available() > 0 ) {
    int size=16;
    int BusAddres=80;
    String input = Serial.readString();
    int loc=input.indexOf(" ");
    String partA = input.substring(0, loc);
    String partB = input.substring(loc+1);
    if (partA == "dump"){
      int loc=partB.indexOf(" ");
      String model = partB.substring(0, loc);
      int BusAddress = partB.substring(loc+1).toInt();
      if (model=="24c02") size=256;
      if (model=="24c04") size=512;
      if (model=="24c08") size=1024;
      if (model=="24c16") size=2048;
      if (model=="24c32") size=4096;
      edump(size, BusAddress); 
      }
    if (partA == "scan"){
      scan();
    }
    if (partA == "read"){
      int loc=partB.indexOf(" ");
      int BusAddress = partB.substring(0, loc).toInt();
      int Address = partB.substring(loc+1).toInt();
     Serial.println(read(BusAddress, Address));
     Serial.println("\n## Done! ##\n");
    }
    
    if (partA == "help"){
      Serial.println("Possible commands:");
      Serial.println("");
      Serial.println("dump 'type' 'address'");
      Serial.println("  type can be 24c02, 24c04, 24c08, 24c16, 24c32");
      Serial.println("  address can be 1 to 127 decimal.");
      Serial.println("");
      Serial.println("scan");
      Serial.println("  this wil run a scan on the i2c bus and report the address found.");
      Serial.println("");
      Serial.println("read 'bus address' 'address'");
      Serial.println("  bus address can be 1 to 127 decimal.");
      Serial.println("  address can be 1 to 255 decimal.");
    }
  }
}
  
void eeprom_i2c_write(byte address, byte from_addr, byte data) {
  Wire.beginTransmission(address);
  Wire.write(from_addr);
  Wire.write(data);
  Wire.endTransmission();
}

byte eeprom_i2c_read(int address, int from_addr) {
  Wire.beginTransmission(address);
  Wire.write(from_addr);
  Wire.endTransmission();
  Wire.requestFrom(address, 1);
  if(Wire.available())
    return Wire.read();
  else
    return 0xFF;
}
int read(int BusAddress, int address) {
  Serial.println("");
  Serial.print("## Reading data from bus address ");
  Serial.print(BusAddress);
  Serial.print(" & data adress ");
  Serial.print(address);
  Serial.println(" ##\n");
  Wire.beginTransmission(BusAddress);
  Wire.write(address);
  Wire.endTransmission();
  Wire.requestFrom(BusAddress, 1);
  if(Wire.available())
    return Wire.read();
  else
    return 0xFF;
}


void ewrite(int size) {
  int value=0;
  for(int i = 0; i < size; i++) {
    eeprom_i2c_write(0x50, i, i);
    value++;
    if (value==11)
      value=0;
    delay(10);
  }
  Serial.println("Writen to memory!");
}

void edump(int size, int addres) {
  Serial.println("");
  Serial.print("## Starting ");
  Serial.print(size);
  Serial.print(" byte dump from adress ");
  Serial.print(addres);
  Serial.println(" ##\n");
  for(int y = 0; y < (size/16); y++) {
    int c;
    int x = 0;
    char start[5];
    sprintf(start, "0x%05x",(y*16)+x);
    Serial.print(start);
    Serial.print(" -");
    char buff[3];
    char txt[20];
    for(int x = 0; x < 16; x++) {
      txt[x]=""; 
      byte r = eeprom_i2c_read(addres, (y*16)+x);
      if (r>0x21 and r<0x80)
        txt[x]=r;
      else
        txt[x]=46;      
      if (x==8)
        Serial.print(" ");
      Serial.print(" ");
      sprintf(buff, "%02x",r);
      Serial.print(buff);
    }
    Serial.print(" ");
    for (c=0; c<16; c++) {
      Serial.write(txt[c]);
    }
    Serial.println("");
  }
  Serial.println("\n## Done! ##\n");
}

void scan()
{
  Serial.println("");
  Serial.println("\n## Scanning fer devices ##\n");
  byte error, address;
  int nDevices;
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
      Serial.print("0");
      Serial.print(address,HEX);
      Serial.print(" - ");
      Serial.print(address);
      Serial.print(" - ");
      if (address<64) Serial.print("0");
      if (address<32) Serial.print("0");
      if (address<16) Serial.print("0");
      if (address<8) Serial.print("0");
      if (address<4) Serial.print("0");
      if (address<2) Serial.print("0");
      Serial.print(address, BIN);
      Serial.println("  !");
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("\n## Done ##\n");
}
