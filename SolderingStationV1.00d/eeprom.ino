
void EEPROMWritelong(int address, long value) {
  //Decomposition from a long to 4 bytes by using bitshift.
  //One = Most significant -> Four = Least significant byte
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);
  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}

long EEPROMReadlong(long address) {
  //Read the 4 bytes from the eeprom memory.
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);
  //Return the recomposed long by using bitshift.
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void LoadParameters() {
  PresetTemperature[0] = EEPROMReadlong(EEPROM_address+4);
  PresetTemperature[1] = EEPROMReadlong(EEPROM_address+8);
  MaxDutyCycle = EEPROMReadlong(EEPROM_address+12);
  VinUVLO = EEPROMReadlong(EEPROM_address+16);
}

void SaveParameters() {
  EEPROMWritelong(EEPROM_address+4, PresetTemperature[0]);
  EEPROMWritelong(EEPROM_address+8, PresetTemperature[1]);
  EEPROMWritelong(EEPROM_address+12, MaxDutyCycle);
  EEPROMWritelong(EEPROM_address+16, VinUVLO);
}

void InitializeEEPROM() {
  PresetTemperature[0] = PresetTemperature1;
  PresetTemperature[1] = PresetTemperature2;
  MaxDutyCycle = PresetMaxDutyCycle;
  EEPROMWritelong(EEPROM_address+4, PresetTemperature[0]);
  EEPROMWritelong(EEPROM_address+8, PresetTemperature[1]);
  EEPROMWritelong(EEPROM_address+12, PresetMaxDutyCycle);
  EEPROMWritelong(EEPROM_address+16, PresetVinUVLO);
  //Reset to default and first boot completed
  FirstBoot = 0;
  EEPROM.write(EEPROM_address, FirstBoot);
}
