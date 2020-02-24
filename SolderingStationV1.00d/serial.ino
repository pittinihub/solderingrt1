void ExecuteSerialCommunication() {
  if(Serial.available()) 
  { 
    char c = (char)Serial.read();
    switch (c){
      case '\n':
        IncomingCmd[CmdIndex] = 0;
        ExecuteSerialCmd();  // execute the command
        CmdIndex = 0; // reset the cmdIndex
        //PrintStatus();
        break;
      default:
            IncomingCmd[CmdIndex] = c;
            if(CmdIndex<99) CmdIndex++;
        break;
      }
  }
}

void ExecuteSerialCmd() {
  if(IncomingCmd[0]=='a' && IncomingCmd[1]==' ') {
    SetTemperature[0] = atoi(IncomingCmd+2);
    Serial.println("SET T1");
  }
  if(IncomingCmd[0]=='b' && IncomingCmd[1]==' ') {
    SetTemperature[1] = atoi(IncomingCmd+2);
    Serial.println("SET T2");
  }
  if(IncomingCmd[0]=='d' && IncomingCmd[1]==' ') {
    DebugMode = atoi(IncomingCmd+2);
    Serial.print("DebugMode ");
    Serial.println(DebugMode);
  } 
  if(IncomingCmd[0]=='w' && IncomingCmd[1]==' ') {
    MaxDutyCycle = atoi(IncomingCmd+2);
    Serial.println("SET Max Duty Cycle");
  }
  if(IncomingCmd[0]=='u' && IncomingCmd[1]==' ') {
    MaxDutyCycle = atoi(IncomingCmd+2);
    Serial.println("SET Vin UVLO");
  }
  if(IncomingCmd[0]=='P' && IncomingCmd[1]==' ') {
    KP = (float) atoi(IncomingCmd+2) / 1000.0;
    Serial.println("SET P");
    Serial.println(KP);
    PIDch1.setGains(KP,KI,KD);
    PIDch2.setGains(KP,KI,KD);
  }
  if(IncomingCmd[0]=='I' && IncomingCmd[1]==' ') {
    KI = (float) atoi(IncomingCmd+2) / 1000.0;
    Serial.println("SET I");
    Serial.println(KI);
    PIDch1.setGains(KP,KI,KD);
    PIDch2.setGains(KP,KI,KD);
  }
  if(IncomingCmd[0]=='D' && IncomingCmd[1]==' ') {
    KD = (float) atoi(IncomingCmd+2) / 1000.0;
    Serial.println("SET D");
    Serial.println(KD);
    PIDch1.setGains(KP,KI,KD);
    PIDch2.setGains(KP,KI,KD);
  }
  if(IncomingCmd[0]=='h') {
    SerialMenu();
  }
  if(IncomingCmd[0]=='s') {
    SaveParameters();
    Serial.println("Parameters Saved");
  }  
}

void SerialMenu() {
  Serial.println("***************************************************************************");
  Serial.println("Soldering Station serial interface:");
  Serial.println("serial speed 9600baud");
  Serial.println("by Riccardo Pittini");
  Serial.println("***************************************************************************");
  Serial.println("Menu: ");
  Serial.println("h: Print help menu");
  Serial.println("a XXX: Set ch.1 temperature, XXX temperature in degC");
  Serial.println("b XXX: Set ch.2 temperature, XXX temperature in degC");
  Serial.println("d X: Disable/Enable debug serial mode, X 0 or 1");
  Serial.println("w XXX: Set Maximum duty cycle /Input Power (read below, default: 150)");
  //Serial.println("u XXX: Set Vin UVLO protection (80 = 8V, default: 80)");
  //Serial.println("P XXX: Set P regulator component");
  //Serial.println("I XXX: Set I regulator component");
  //Serial.println("D XXX: Set D regulator component");
  Serial.println("s: Save parameters");
  Serial.println("***************************************************************************");
  Serial.println("NOTE on input power limitation: ");
  Serial.println("Feauture is implemented as duty cycle limitation");
  Serial.println("Calculate the desired duty cyckle as: Irms = Ipeak*sqrt(duty/255), Ipeak = Vin/TIP_Resistance, TIP_resistance = 2ohm for single and 1ohm for dual");
  Serial.println("Duty value is from 0 to 255, Exmaple: w 181");
  Serial.println("***************************************************************************");
}

void SerialDebug() {
  Serial.print("Tint(C): ");
  Serial.println(InternalTemperature);
  Serial.print("T1adc:"); Serial.print(MeasuredTemperatureADCvalue[0]);
  Serial.print(" T2adc:"); Serial.println(MeasuredTemperatureADCvalue[1]);
  Serial.print("T1:"); Serial.print(MeasuredTemperature[0]);
  Serial.print(" T2:"); Serial.println(MeasuredTemperature[1]);
  Serial.print("Int. cnt S1: ");
  Serial.println(InterruptCounterS1);
  Serial.print("Int. cnt S2: ");
  Serial.println(InterruptCounterS2);
  Serial.print("Ch1 & Ch2 State: "); Serial.print(ChannelState[0]); Serial.print(" / ");  Serial.println(ChannelState[1]); 
  Serial.print("Duty1/Duty2:  "); Serial.print(OutputVal1); Serial.print(" / "); Serial.println(OutputVal2);
  Serial.print("MAX Duty1/Duty2:  "); Serial.print(MaxDutyCycle); Serial.print(" / "); Serial.println(MaxDutyCycle);
  Serial.print("Vin: ");
  Serial.println(InputVoltage);
  Serial.print("VinUVLO(V): ");
  Serial.println(VinUVLO*10);
  Serial.print("Tint(C): ");
  Serial.println(InternalTemperature);
}
