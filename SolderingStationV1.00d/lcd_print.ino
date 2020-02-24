
void UpdateScreen(){
  //Print Channel States
  if (RefreshScreen>0){ //Update screen when channel is selected /set temperatures
    PrintSetTemperature(Colour1,ChannelXOffset[0],1);
    PrintSetTemperature(Colour2,ChannelXOffset[1],2);
    RefreshScreen = 0;
  }

  if (RefreshScreenTemperature==1){ 
    //Print Measured Temperatures
    PrintMeasuredTemperature(Colour1,ChannelXOffset[0],1);
    PrintMeasuredTemperature(Colour2,ChannelXOffset[1],2);
	PrintInputVoltage();
    RefreshScreenTemperature = 0;
  }

  //Print stars to indicate long press of buttons
  if (SelectedChannel==1){
    if((InterruptCounterS1>ButtonsVeryLongPress)||(InterruptCounterS2>ButtonsVeryLongPress)){
      //Print 2 stars for Channel 1
      PrintStarsChannel(Colour1,ChannelXOffset[0],2);
    } else if((InterruptCounterS1>ButtonsLongPress)||(InterruptCounterS2>ButtonsLongPress)){
      //Print 1 star for Channel 1
      PrintStarsChannel(Colour1,ChannelXOffset[0],1);
    } else {
      PrintStarsChannel(Colour1,ChannelXOffset[0],0);
    }
  } else if (SelectedChannel==2){
    if((InterruptCounterS1>ButtonsVeryLongPress)||(InterruptCounterS2>ButtonsVeryLongPress)){
      //Print 2 stars for Channel 2
      PrintStarsChannel(Colour2,ChannelXOffset[1],2);
    } else if((InterruptCounterS1>ButtonsLongPress)||(InterruptCounterS2>ButtonsLongPress)){
      //Print 1 star for Channel 2
      PrintStarsChannel(Colour2,ChannelXOffset[1],1);
    } else {
        PrintStarsChannel(Colour2,ChannelXOffset[1],0);
    }
  } else {

  }
  
}

void PrintChannel(int Colour[3], int Offset, int Ch){ 
  TFTscreen.setTextSize(1);
  TFTscreen.stroke(Colour[0], Colour[1], Colour[2]);//red,green,blue
  TFTscreen.setCursor(5+Offset+XOffset,5+YOffset);
  TFTscreen.print("Status: ");
  PrintChannelState(Colour,Offset,Ch);
  TFTscreen.setTextSize(1);
  TFTscreen.setCursor(5+Offset+XOffset,29+YOffset);
  TFTscreen.print("Set: ");
  PrintSetTemperature(Colour,Offset,Ch);
  TFTscreen.stroke(Colour[0], Colour[1], Colour[2]);
  TFTscreen.setTextSize(1);
  TFTscreen.setCursor(5+Offset+XOffset,61+YOffset);
  TFTscreen.print("Measured: ");
  PrintMeasuredTemperature(Colour,Offset,Ch);
}

void PrintStarsChannel(int Colour[3], int Offset, int stars){ 
  TFTscreen.setTextSize(1);
  TFTscreen.stroke(Colour[0], Colour[1], Colour[2]);
  TFTscreen.setCursor(35+Offset+XOffset,29+YOffset);
  if (stars==1){
    //Print one star
    TFTscreen.print("*");
  } else if (stars==2){
    //Print two stars
    TFTscreen.print("**");
  } else {
    //Clear the stars area
    TFTscreen.noStroke();
    TFTscreen.fill(0,0,0);
    TFTscreen.rect(35+Offset+XOffset,29+YOffset,12,8);
  }
}

void PrintChannelState(int Colour[3],int Offset,int Ch){
  TFTscreen.setTextSize(2);
  TFTscreen.noStroke();
  TFTscreen.fill(0,0,0);
  TFTscreen.rect(23+Offset+XOffset,13+YOffset,37,16);
  TFTscreen.stroke(Colour[0], Colour[1], Colour[2]);
  TFTscreen.setCursor(23+Offset+XOffset,14+YOffset);
  if (Ch==1){
    if (ChannelState[0]==1)  {
      TFTscreen.print("ON ");
    } else if ((ChannelState[0]==0)&(MeasuredTemperature[0]<TemperatureDisconnectSensing)) {
      TFTscreen.print("OFF");
      } else {
        TFTscreen.print("NC");
        ChannelState[0]=2;
      }
  }
  if (Ch==2){
    if (ChannelState[1]==1)  {
      TFTscreen.print("ON ");
    } else if ((ChannelState[1]==0)&(MeasuredTemperature[1]<TemperatureDisconnectSensing)) {
      TFTscreen.print("OFF");
      } else {
        TFTscreen.print("NC");
        ChannelState[1]=2;
      }
  }
}

void PrintSetTemperature(int Colour[3],int Offset,int Ch){
  TFTscreen.setTextSize(2);
  if (SelectedChannel==Ch){
    //Channel is highlited (print inverted)
    TFTscreen.stroke(Colour[0], Colour[1], Colour[2]);
    TFTscreen.fill(Colour[0], Colour[1], Colour[2]);
    TFTscreen.rect(23+Offset+XOffset,40+YOffset,37,16);
    TFTscreen.stroke(0,0,0);
    TFTscreen.setCursor(24+Offset+XOffset,41+YOffset);
    TFTscreen.print(SetTemperature[Ch-1]);
  }
  else {
    //Channel is NOT highlited (print normally)
    TFTscreen.stroke(Colour[0], Colour[1], Colour[2]);
    TFTscreen.noStroke();
    TFTscreen.fill(0,0,0);
    TFTscreen.rect(23+Offset+XOffset,40+YOffset,37,16);
    TFTscreen.setCursor(24+Offset+XOffset,41+YOffset);
    TFTscreen.print(SetTemperature[Ch-1]);
  }
}

void PrintMeasuredTemperature(int Colour[3],int Offset,int Ch){
  TFTscreen.stroke(Colour[0], Colour[1], Colour[2]);
  TFTscreen.setTextSize(3);
  TFTscreen.noStroke();
  TFTscreen.fill(0,0,0);
  TFTscreen.rect(10+Offset+XOffset,73+YOffset,60,24);
  TFTscreen.setCursor(10+Offset+XOffset,73+YOffset);
  if (MeasuredTemperature[Ch-1]>=TemperatureDisconnectSensing){
    TFTscreen.print(" -");
  }
  else if (MeasuredTemperature[Ch-1]<=LowTemperatureLimit){
    TFTscreen.print("LOW");}
    else {
    TFTscreen.print(MeasuredTemperature[Ch-1]);
  }
}

void PrintPresetTemperatures(){
  TFTscreen.noStroke();
  TFTscreen.fill(0,0,0);
  TFTscreen.rect(0,105,160,20);
  TFTscreen.stroke(Colour3[0], Colour3[1], Colour3[2]);//red,green,blue
  TFTscreen.line(0+XOffset, 105+YOffset, 160+XOffset, 105+YOffset); //Line
  TFTscreen.setTextColor(WHITE, BLACK);
  TFTscreen.setTextSize(1);
  TFTscreen.setCursor(59+XOffset,109);
  TFTscreen.print("PreSets");
  TFTscreen.setTextSize(2);
  TFTscreen.setCursor(5+XOffset,109);
  TFTscreen.print(PresetTemperature[0]);
  TFTscreen.setCursor(120+XOffset,109+YOffset);
  TFTscreen.print(PresetTemperature[1]);
}

void PrintInputVoltage(){
  TFTscreen.setTextColor(WHITE, BLACK);
  TFTscreen.setCursor(59+XOffset,118);
  TFTscreen.setTextSize(1);
  TFTscreen.print("Vin ");
  TFTscreen.print(InputVoltage,1);
//  TFTscreen.print("[V]");
}

void PrintPower(int Colour[3],int Offset,int Ch){
  TFTscreen.stroke(Colour3[0], Colour3[1], Colour3[2]);
  TFTscreen.fill(0,0,0);
  TFTscreen.rect(10,1,70,5);
  TFTscreen.noStroke();
  TFTscreen.fill(Colour3[0], Colour3[1], Colour3[2]);
  TFTscreen.rect(10+80+XOffset,2,22+80,4);
  TFTscreen.rect(22+80+XOffset,2,34+80,4);
  TFTscreen.rect(34+80+XOffset,2,46+80,4);
  TFTscreen.rect(46+80+XOffset,2,58+80,4);
  //TFTscreen.rect(58+80,2,70+80,4);
  // Compute 
  int PowerPercentage = OutputVal1Old/MaxDutyCycle;
  if (OutputVal1Old>OutputVal1){
    //Write the black part of the screen
    TFTscreen.noStroke();
    TFTscreen.fill(0,0,0);
    TFTscreen.rect(10+59*(1-PowerPercentage),2,10+60,4);
  }
  else {
    //Write the light part of the screen
    TFTscreen.noStroke();
    TFTscreen.fill(Colour3[0], Colour3[1], Colour3[2]);
    TFTscreen.rect(10,2,10+59*PowerPercentage,4);
  }

}
