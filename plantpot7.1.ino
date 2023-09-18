#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#define UTC_OFFSET_IN_SECONDS 19800

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

#define WIFI_SSID "DiLuK_4G"
#define WIFI_PASSWORD "DiLuKK123"
#define API_KEY "AIzaSyCptd3O9gN-vffcr5qVIvsj1jAsv3mtscQ"
#define DATABASE_URL "smart-home-gardening-56498-default-rtdb.firebaseio.com" 

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", UTC_OFFSET_IN_SECONDS);

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

//unsigned long sendDataPrevMillis = 0;
unsigned long count;
String optaPWRstatus;
int fc28PWRstatus;
int s12sdPWRstatus;
bool signupOK = false;


const int optacouplarPWR = 32;
const int fc28PWR = 33;
const int s12sdPWR = 14;
const int optasens = 36;
const int btrsens = 39;
const int fc28sens = 34;
const int s12sdsens = 35;

int optaval;
int opta;
int fc28val;
int soilLevel;
int btrval;
int btrLevel;
int s12sdval;
int uvIntensity;
int btrAvg;
int s12sdAvg;
int fc28Avg;
int optaSum;
unsigned long btrSum;
unsigned long s12sdSum;
unsigned long fc28Sum;
long randNum;
int hour;
int mini;
int sec;
int day;
int dayCount;
int terns;
int dayNum;
int soilTotal;
int soilTotal2;
int uvTotal;
int uvTotal2;
int soilAve;
int soilAve2;
int uvAve;
int uvAve2;
int reportSnailCount;
int z=0;
String p1= ":";

void setup() {
  pinMode(optacouplarPWR, OUTPUT);
  pinMode(fc28PWR, OUTPUT);
  pinMode(s12sdPWR, OUTPUT);
  Serial.begin(115200);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;
  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;
  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  timeClient.begin();

}


void loop() {

  timeClient.update();
  //Serial.println(timeClient.getDay()+timeClient.getHours()+timeClient.getMinutes()+timeClient.getSeconds());
  day= timeClient.getDay();
  hour= timeClient.getHours();
  mini= timeClient.getMinutes();
  sec= timeClient.getSeconds();
  Serial.println(day+p1+hour+p1+mini+p1+sec);///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  

  int snailCount;
  
  if(Firebase.RTDB.getString(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/optacouplarPWR")){
      optaPWRstatus=fbdo.intData();
  }else{
    Serial.println(fbdo.errorReason());
  }
  if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/fc28PWR")){
      fc28PWRstatus=fbdo.intData();
  }else{
    Serial.println(fbdo.errorReason());
  }
  if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/s12sdPWR")){
      s12sdPWRstatus=fbdo.intData();
  }else{
    Serial.println(fbdo.errorReason());
  }
  if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Snail_Detector_Count")){
      snailCount=fbdo.intData();
  }else{
    Serial.println(fbdo.errorReason());
  }


  if(optaPWRstatus=="1"){
    digitalWrite(optacouplarPWR, HIGH);
  }else{
    digitalWrite(optacouplarPWR, LOW);
  }
  if(fc28PWRstatus==1){
    digitalWrite(fc28PWR, HIGH);
  }else{
    digitalWrite(fc28PWR, LOW);
  }
  if(s12sdPWRstatus==1){
    digitalWrite(s12sdPWR, HIGH);
  }else{
    digitalWrite(s12sdPWR, LOW);
  }
  
btrSum=0;
btrAvg=0;
fc28Sum=0;
fc28Avg=0;
s12sdSum=0;
s12sdAvg=0;
optaval=0;
optaSum=0;

//////////////////////////////////////////////#########################################////////////////////////////////////////////
  for(count=1; count<=10000; count++){
  
    btrSum=btrSum+(analogRead(btrsens)/40);
    btrAvg=(btrSum/count); 
  
    fc28Sum=fc28Sum+(analogRead(fc28sens));
    fc28Avg=(fc28Sum/count);
  
    s12sdSum=s12sdSum+(analogRead(s12sdsens));
    s12sdAvg=(s12sdSum/count);
  
    optaval=analogRead(optasens);
        if (optaval >= 1800){
          opta=1;
          Serial.print(" Opta=");
          Serial.print(optaval);
        }else{
          opta=0;
        }
      optaSum=optaSum+opta; 
  }
/////////////////////////////////////////////#############################################/////////////////////////////////////////////

    if(optaSum>=1){
      snailCount++;
      Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Snail_Detector_Value", optaSum);
      Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Snail_Detector_Count", snailCount);

      if(hour==23){
        if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record23/Snail_Count")){
          reportSnailCount=fbdo.intData();
        }else{
        Serial.println(fbdo.errorReason());
        }
        reportSnailCount++;
        Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record23/Snail_Count", reportSnailCount);
      }else if(hour==22){
        if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record22/Snail_Count")){
          reportSnailCount=fbdo.intData();
        }else{
        Serial.println(fbdo.errorReason());
        }
        reportSnailCount++;
        Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record22/Snail_Count", reportSnailCount);
      }else if(hour==21){
        if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record21/Snail_Count")){
          reportSnailCount=fbdo.intData();
        }else{
        Serial.println(fbdo.errorReason());
        }
        reportSnailCount++;
        Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record21/Snail_Count", reportSnailCount);
      }else if(hour==20){
        if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record20/Snail_Count")){
          reportSnailCount=fbdo.intData();
        }else{
        Serial.println(fbdo.errorReason());
        }
        reportSnailCount++;
        Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record20/Snail_Count", reportSnailCount);
      }else if(hour==19){
        if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record19/Snail_Count")){
          reportSnailCount=fbdo.intData();
        }else{
        Serial.println(fbdo.errorReason());
        }
        reportSnailCount++;
        Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record19/Snail_Count", reportSnailCount);
      }else if(hour==18){
        if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record18/Snail_Count")){
          reportSnailCount=fbdo.intData();
        }else{
        Serial.println(fbdo.errorReason());
        }
        reportSnailCount++;
        Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record18/Snail_Count", reportSnailCount);
      }else if(hour==17){
        if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record17/Snail_Count")){
          reportSnailCount=fbdo.intData();
        }else{
        Serial.println(fbdo.errorReason());
        }
        reportSnailCount++;
        Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record17/Snail_Count", reportSnailCount);
      }else if(hour==16){
        if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record16/Snail_Count")){
          reportSnailCount=fbdo.intData();
        }else{
        Serial.println(fbdo.errorReason());
        }
        reportSnailCount++;
        Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record16/Snail_Count", reportSnailCount);
      }else if(hour==15){
        if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record15/Snail_Count")){
          reportSnailCount=fbdo.intData();
        }else{
        Serial.println(fbdo.errorReason());
        }
        reportSnailCount++;
        Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record15/Snail_Count", reportSnailCount);
      }else if(hour==14){
        if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record14/Snail_Count")){
          reportSnailCount=fbdo.intData();
        }else{
        Serial.println(fbdo.errorReason());
        }
        reportSnailCount++;
        Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record14/Snail_Count", reportSnailCount);
      }else if(hour==13){
        if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record13/Snail_Count")){
          reportSnailCount=fbdo.intData();
        }else{
        Serial.println(fbdo.errorReason());
        }
        reportSnailCount++;
        Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record13/Snail_Count", reportSnailCount);
      }else if(hour==12){
        if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record12/Snail_Count")){
          reportSnailCount=fbdo.intData();
        }else{
        Serial.println(fbdo.errorReason());
        }
        reportSnailCount++;
        Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record12/Snail_Count", reportSnailCount);
      }else if(hour==11){
        if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record11/Snail_Count")){
          reportSnailCount=fbdo.intData();
        }else{
        Serial.println(fbdo.errorReason());
        }
        reportSnailCount++;
        Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record11/Snail_Count", reportSnailCount);
      }else if(hour==10){
        if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record10/Snail_Count")){
          reportSnailCount=fbdo.intData();
        }else{
        Serial.println(fbdo.errorReason());
        }
        reportSnailCount++;
        Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record10/Snail_Count", reportSnailCount);
      }else if(hour==9){
        if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record9/Snail_Count")){
          reportSnailCount=fbdo.intData();
        }else{
        Serial.println(fbdo.errorReason());
        }
        reportSnailCount++;
        Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record9/Snail_Count", reportSnailCount);
      }else if(hour==8){
        if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record8/Snail_Count")){
          reportSnailCount=fbdo.intData();
        }else{
        Serial.println(fbdo.errorReason());
        }
        reportSnailCount++;
        Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record8/Snail_Count", reportSnailCount);
      }else if(hour==7){
        if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record7/Snail_Count")){
          reportSnailCount=fbdo.intData();
        }else{
        Serial.println(fbdo.errorReason());
        }
        reportSnailCount++;
        Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record7/Snail_Count", reportSnailCount);
      }else if(hour==6){
        if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record6/Snail_Count")){
          reportSnailCount=fbdo.intData();
        }else{
        Serial.println(fbdo.errorReason());
        }
        reportSnailCount++;
        Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record6/Snail_Count", reportSnailCount);
      }else if(hour==5){
        if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record5/Snail_Count")){
          reportSnailCount=fbdo.intData();
        }else{
        Serial.println(fbdo.errorReason());
        }
        reportSnailCount++;
        Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record5/Snail_Count", reportSnailCount);
      }else if(hour==4){
        if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record4/Snail_Count")){
          reportSnailCount=fbdo.intData();
        }else{
        Serial.println(fbdo.errorReason());
        }
        reportSnailCount++;
        Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record4/Snail_Count", reportSnailCount);
      }else if(hour==3){
        if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record3/Snail_Count")){
          reportSnailCount=fbdo.intData();
        }else{
        Serial.println(fbdo.errorReason());
        }
        reportSnailCount++;
        Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record3/Snail_Count", reportSnailCount);
      }else if(hour==2){
        if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record2/Snail_Count")){
          reportSnailCount=fbdo.intData();
        }else{
        Serial.println(fbdo.errorReason());
        }
        reportSnailCount++;
        Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record2/Snail_Count", reportSnailCount);
      }else if(hour==1){
        if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record1/Snail_Count")){
          reportSnailCount=fbdo.intData();
        }else{
        Serial.println(fbdo.errorReason());
        }
        reportSnailCount++;
        Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record1/Snail_Count", reportSnailCount);
      }else if(hour==0){
        if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record0/Snail_Count")){
          reportSnailCount=fbdo.intData();
        }else{
        Serial.println(fbdo.errorReason());
        }
        reportSnailCount++;
        Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record0/Snail_Count", reportSnailCount);
      }

    }else{
      Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Snail_Detector_Value", optaSum);
    }

////////////////////////////////////////////////##############################################//////////////////////////////////////////////////////////
    if(btrAvg>=90){
        btrLevel=6;
    }else if(btrAvg>=80){
        btrLevel=5;
    }else if(btrAvg>=70){
        btrLevel=4;
    }else if(btrAvg>=50){
        btrLevel=3;
    }else if(btrAvg>=35){
        btrLevel=2;
    }else{
        btrLevel=1;
    }

  Serial.print("  Btr Avg: ");
  Serial.print(btrAvg);
  Serial.print("  Btr Level: ");
  Serial.print(btrLevel);


    if(fc28Avg>=3800){
      soilLevel=1;
    }else if(fc28Avg>=3500){
      soilLevel=2;  
    }else if(fc28Avg>=2900){
      soilLevel=3;
    }else if(fc28Avg>=2300){
      soilLevel=4;
    }else if(fc28Avg>=1200){
      soilLevel=5;
    }else if(fc28Avg>=10){
      soilLevel=6;
    }else{
      soilLevel=1;
    }
    
    Serial.print("  fc28 Avg: ");
    Serial.print(fc28Avg);
    Serial.print("  soil Level: ");
    Serial.print(soilLevel);


    if(s12sdAvg>=3500){
      uvIntensity=6;
    }else if(s12sdAvg>=2400){
      uvIntensity=5;
    }else if(s12sdAvg>=1700){
      uvIntensity=4;
    }else if(s12sdAvg>=500){
      uvIntensity=3;
    }else if(s12sdAvg>=300){
      uvIntensity=2;
    }else{
      uvIntensity=1;
    }

    Serial.print("  s12sd Avg: ");
    Serial.print(s12sdAvg);
    Serial.print("  Intensity Level: ");
    Serial.println(uvIntensity);
 
    randNum= random(1000);
  (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/isOnline", randNum)); 
  (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Battery_Level", btrLevel));
  (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Soil_Moisture_Level", soilLevel));
  (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Sunlight_UV_Intensity", uvIntensity));

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if(hour==23){
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record23/Terns")){
      terns=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record23/SoilLevel_RealTotal")){
      soilTotal2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record23/UVlevel_RealTotal")){
      uvTotal2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record23/SoilLevel_Average")){
      soilAve2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record23/UVlevel_Average")){
      uvAve2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    terns++;
    if(fc28PWRstatus==1){
      soilTotal=soilTotal2+soilLevel;
      soilAve=soilTotal/terns;
    }else{
      soilTotal=soilTotal2;
      soilAve=soilAve2;
    }
    if(s12sdPWRstatus==1){
      uvTotal=uvTotal2+uvIntensity;
      uvAve=uvTotal/terns;
    }else{
      uvTotal=uvTotal2;
      uvAve=uvAve2;
    }
    
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record23/Terns", terns));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record23/SoilLevel_RealTotal", soilTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record23/UVlevel_RealTotal", uvTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record23/SoilLevel_Average", soilAve));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record23/UVlevel_Average", uvAve));
        
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record0/Terns", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record0/SoilLevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record0/UVlevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record0/SoilLevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record0/UVlevel_Average", z));  
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record0/Snail_Count", z)); 
  }else if(hour==22){
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record22/Terns")){
      terns=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record22/SoilLevel_RealTotal")){
      soilTotal2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record22/UVlevel_RealTotal")){
      uvTotal2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record22/SoilLevel_Average")){
      soilAve2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record22/UVlevel_Average")){
      uvAve2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    terns++;
    if(fc28PWRstatus==1){
      soilTotal=soilTotal2+soilLevel;
      soilAve=soilTotal/terns;
    }else{
      soilTotal=soilTotal2;
      soilAve=soilAve2;
    }
    if(s12sdPWRstatus==1){
      uvTotal=uvTotal2+uvIntensity;
      uvAve=uvTotal/terns;
    }else{
      uvTotal=uvTotal2;
      uvAve=uvAve2;
    }
    
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record22/Terns", terns));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record22/SoilLevel_RealTotal", soilTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record22/UVlevel_RealTotal", uvTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record22/SoilLevel_Average", soilAve));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record22/UVlevel_Average", uvAve));
        
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record23/Terns", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record23/SoilLevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record23/UVlevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record23/SoilLevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record23/UVlevel_Average", z));  
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record23/Snail_Count", z)); 
  }else if(hour==21){
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record21/Terns")){
      terns=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record21/SoilLevel_RealTotal")){
      soilTotal2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record21/UVlevel_RealTotal")){
      uvTotal2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record21/SoilLevel_Average")){
      soilAve2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record21/UVlevel_Average")){
      uvAve2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    terns++;
    if(fc28PWRstatus==1){
      soilTotal=soilTotal2+soilLevel;
      soilAve=soilTotal/terns;
    }else{
      soilTotal=soilTotal2;
      soilAve=soilAve2;
    }
    if(s12sdPWRstatus==1){
      uvTotal=uvTotal2+uvIntensity;
      uvAve=uvTotal/terns;
    }else{
      uvTotal=uvTotal2;
      uvAve=uvAve2;
    }
    
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record21/Terns", terns));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record21/SoilLevel_RealTotal", soilTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record21/UVlevel_RealTotal", uvTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record21/SoilLevel_Average", soilAve));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record21/UVlevel_Average", uvAve));
        
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record22/Terns", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record22/SoilLevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record22/UVlevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record22/SoilLevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record22/UVlevel_Average", z));  
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record22/Snail_Count", z)); 
  }else if(hour==20){
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record20/Terns")){
      terns=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record20/SoilLevel_RealTotal")){
      soilTotal2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record20/UVlevel_RealTotal")){
      uvTotal2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record20/SoilLevel_Average")){
      soilAve2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record20/UVlevel_Average")){
      uvAve2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    terns++;
    if(fc28PWRstatus==1){
      soilTotal=soilTotal2+soilLevel;
      soilAve=soilTotal/terns;
    }else{
      soilTotal=soilTotal2;
      soilAve=soilAve2;
    }
    if(s12sdPWRstatus==1){
      uvTotal=uvTotal2+uvIntensity;
      uvAve=uvTotal/terns;
    }else{
      uvTotal=uvTotal2;
      uvAve=uvAve2;
    }
    
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record20/Terns", terns));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record20/SoilLevel_RealTotal", soilTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record20/UVlevel_RealTotal", uvTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record20/SoilLevel_Average", soilAve));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record20/UVlevel_Average", uvAve));
        
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record21/Terns", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record21/SoilLevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record21/UVlevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record21/SoilLevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record21/UVlevel_Average", z));  
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record21/Snail_Count", z)); 
  }else if(hour==19){
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record19/Terns")){
      terns=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record19/SoilLevel_RealTotal")){
      soilTotal2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record19/UVlevel_RealTotal")){
      uvTotal2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record19/SoilLevel_Average")){
      soilAve2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record19/UVlevel_Average")){
      uvAve2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    terns++;
    if(fc28PWRstatus==1){
      soilTotal=soilTotal2+soilLevel;
      soilAve=soilTotal/terns;
    }else{
      soilTotal=soilTotal2;
      soilAve=soilAve2;
    }
    if(s12sdPWRstatus==1){
      uvTotal=uvTotal2+uvIntensity;
      uvAve=uvTotal/terns;
    }else{
      uvTotal=uvTotal2;
      uvAve=uvAve2;
    }
    
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record19/Terns", terns));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record19/SoilLevel_RealTotal", soilTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record19/UVlevel_RealTotal", uvTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record19/SoilLevel_Average", soilAve));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record19/UVlevel_Average", uvAve));
        
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record20/Terns", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record20/SoilLevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record20/UVlevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record20/SoilLevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record20/UVlevel_Average", z));  
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record20/Snail_Count", z)); 
  }else if(hour==18){
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record18/Terns")){
      terns=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record18/SoilLevel_RealTotal")){
      soilTotal2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record18/UVlevel_RealTotal")){
      uvTotal2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record18/SoilLevel_Average")){
      soilAve2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record18/UVlevel_Average")){
      uvAve2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    terns++;
    if(fc28PWRstatus==1){
      soilTotal=soilTotal2+soilLevel;
      soilAve=soilTotal/terns;
    }else{
      soilTotal=soilTotal2;
      soilAve=soilAve2;
    }
    if(s12sdPWRstatus==1){
      uvTotal=uvTotal2+uvIntensity;
      uvAve=uvTotal/terns;
    }else{
      uvTotal=uvTotal2;
      uvAve=uvAve2;
    }
    
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record18/Terns", terns));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record18/SoilLevel_RealTotal", soilTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record18/UVlevel_RealTotal", uvTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record18/SoilLevel_Average", soilAve));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record18/UVlevel_Average", uvAve));
        
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record19/Terns", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record19/SoilLevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record19/UVlevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record19/SoilLevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record19/UVlevel_Average", z));  
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record19/Snail_Count", z)); 
  }else if(hour==17){
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record17/Terns")){
      terns=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record17/SoilLevel_RealTotal")){
      soilTotal2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record17/UVlevel_RealTotal")){
      uvTotal2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record17/SoilLevel_Average")){
      soilAve2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record17/UVlevel_Average")){
      uvAve2=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    terns++;
    if(fc28PWRstatus==1){
      soilTotal=soilTotal2+soilLevel;
      soilAve=soilTotal/terns;
    }else{
      soilTotal=soilTotal2;
      soilAve=soilAve2;
    }
    if(s12sdPWRstatus==1){
      uvTotal=uvTotal2+uvIntensity;
      uvAve=uvTotal/terns;
    }else{
      uvTotal=uvTotal2;
      uvAve=uvAve2;
    }
    
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record17/Terns", terns));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record17/SoilLevel_RealTotal", soilTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record17/UVlevel_RealTotal", uvTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record17/SoilLevel_Average", soilAve));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record17/UVlevel_Average", uvAve));
        
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record18/Terns", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record18/SoilLevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record18/UVlevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record18/SoilLevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record18/UVlevel_Average", z));  
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record18/Snail_Count", z)); 
  }else if(hour==16){
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record16/Terns")){
      terns=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record16/SoilLevel_RealTotal")){
      soilTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record16/UVlevel_RealTotal")){
      uvTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    terns++;
    soilTotal=soilTotal+soilLevel;
    uvTotal=uvTotal+uvIntensity;
    soilAve=soilTotal/terns;
    uvAve=uvTotal/terns;
    
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record16/Terns", terns));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record16/SoilLevel_RealTotal", soilTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record16/UVlevel_RealTotal", uvTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record16/SoilLevel_Average", soilAve));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record16/UVlevel_Average", uvAve));
        
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record17/Terns", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record17/SoilLevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record17/UVlevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record17/SoilLevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record17/UVlevel_Average", z));  
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record17/Snail_Count", z)); 
  }else if(hour==15){
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record15/Terns")){
      terns=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record15/SoilLevel_RealTotal")){
      soilTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record15/UVlevel_RealTotal")){
      uvTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    terns++;
    soilTotal=soilTotal+soilLevel;
    uvTotal=uvTotal+uvIntensity;
    soilAve=soilTotal/terns;
    uvAve=uvTotal/terns;
    
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record15/Terns", terns));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record15/SoilLevel_RealTotal", soilTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record15/UVlevel_RealTotal", uvTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record15/SoilLevel_Average", soilAve));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record15/UVlevel_Average", uvAve));
        
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record16/Terns", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record16/SoilLevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record16/UVlevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record16/SoilLevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record16/UVlevel_Average", z));  
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record16/Snail_Count", z)); 
  }else if(hour==14){
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record14/Terns")){
      terns=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record14/SoilLevel_RealTotal")){
      soilTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record14/UVlevel_RealTotal")){
      uvTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    terns++;
    soilTotal=soilTotal+soilLevel;
    uvTotal=uvTotal+uvIntensity;
    soilAve=soilTotal/terns;
    uvAve=uvTotal/terns;
    
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record14/Terns", terns));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record14/SoilLevel_RealTotal", soilTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record14/UVlevel_RealTotal", uvTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record14/SoilLevel_Average", soilAve));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record14/UVlevel_Average", uvAve));
        
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record15/Terns", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record15/SoilLevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record15/UVlevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record15/SoilLevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record15/UVlevel_Average", z));  
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record15/Snail_Count", z)); 
  }else if(hour==13){
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record13/Terns")){
      terns=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record13/SoilLevel_RealTotal")){
      soilTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record13/UVlevel_RealTotal")){
      uvTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    terns++;
    soilTotal=soilTotal+soilLevel;
    uvTotal=uvTotal+uvIntensity;
    soilAve=soilTotal/terns;
    uvAve=uvTotal/terns;
    
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record13/Terns", terns));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record13/SoilLevel_RealTotal", soilTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record13/UVlevel_RealTotal", uvTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record13/SoilLevel_Average", soilAve));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record13/UVlevel_Average", uvAve));
        
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record14/Terns", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record14/SoilLevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record14/UVlevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record14/SoilLevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record14/UVlevel_Average", z));  
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record14/Snail_Count", z)); 
  }else if(hour==12){
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record12/Terns")){
      terns=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record12/SoilLevel_RealTotal")){
      soilTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record12/UVlevel_RealTotal")){
      uvTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    terns++;
    soilTotal=soilTotal+soilLevel;
    uvTotal=uvTotal+uvIntensity;
    soilAve=soilTotal/terns;
    uvAve=uvTotal/terns;
    
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record12/Terns", terns));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record12/SoilLevel_RealTotal", soilTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record12/UVlevel_RealTotal", uvTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record12/SoilLevel_Average", soilAve));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record12/UVlevel_Average", uvAve));
        
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record13/Terns", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record13/SoilLevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record13/UVlevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record13/SoilLevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record13/UVlevel_Average", z));  
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record13/Snail_Count", z)); 
  }else if(hour==11){
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record11/Terns")){
      terns=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record11/SoilLevel_RealTotal")){
      soilTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record11/UVlevel_RealTotal")){
      uvTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    terns++;
    soilTotal=soilTotal+soilLevel;
    uvTotal=uvTotal+uvIntensity;
    soilAve=soilTotal/terns;
    uvAve=uvTotal/terns;
    
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record11/Terns", terns));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record11/SoilLevel_RealTotal", soilTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record11/UVlevel_RealTotal", uvTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record11/SoilLevel_Average", soilAve));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record11/UVlevel_Average", uvAve));
        
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record12/Terns", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record12/SoilLevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record12/UVlevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record12/SoilLevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record12/UVlevel_Average", z));  
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record12/Snail_Count", z)); 
  }else if(hour==10){
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record10/Terns")){
      terns=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record10/SoilLevel_RealTotal")){
      soilTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record10/UVlevel_RealTotal")){
      uvTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    terns++;
    soilTotal=soilTotal+soilLevel;
    uvTotal=uvTotal+uvIntensity;
    soilAve=soilTotal/terns;
    uvAve=uvTotal/terns;
    
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record10/Terns", terns));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record10/SoilLevel_RealTotal", soilTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record10/UVlevel_RealTotal", uvTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record10/SoilLevel_Average", soilAve));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record10/UVlevel_Average", uvAve));
        
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record11/Terns", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record11/SoilLevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record11/UVlevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record11/SoilLevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record11/UVlevel_Average", z));  
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record11/Snail_Count", z)); 
  }else if(hour==9){
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record9/Terns")){
      terns=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record9/SoilLevel_RealTotal")){
      soilTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record9/UVlevel_RealTotal")){
      uvTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    terns++;
    soilTotal=soilTotal+soilLevel;
    uvTotal=uvTotal+uvIntensity;
    soilAve=soilTotal/terns;
    uvAve=uvTotal/terns;
    
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record9/Terns", terns));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record9/SoilLevel_RealTotal", soilTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record9/UVlevel_RealTotal", uvTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record9/SoilLevel_Average", soilAve));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record9/UVlevel_Average", uvAve));
        
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record10/Terns", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record10/SoilLevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record10/UVlevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record10/SoilLevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record10/UVlevel_Average", z));  
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record10/Snail_Count", z)); 
  }else if(hour==8){
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record8/Terns")){
      terns=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record8/SoilLevel_RealTotal")){
      soilTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record8/UVlevel_RealTotal")){
      uvTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    terns++;
    soilTotal=soilTotal+soilLevel;
    uvTotal=uvTotal+uvIntensity;
    soilAve=soilTotal/terns;
    uvAve=uvTotal/terns;
    
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record8/Terns", terns));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record8/SoilLevel_RealTotal", soilTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record8/UVlevel_RealTotal", uvTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record8/SoilLevel_Average", soilAve));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record8/UVlevel_Average", uvAve));
        
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record9/Terns", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record9/SoilLevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record9/UVlevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record9/SoilLevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record9/UVlevel_Average", z));  
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record9/Snail_Count", z)); 
  }else if(hour==7){
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record7/Terns")){
      terns=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record7/SoilLevel_RealTotal")){
      soilTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record7/UVlevel_RealTotal")){
      uvTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    terns++;
    soilTotal=soilTotal+soilLevel;
    uvTotal=uvTotal+uvIntensity;
    soilAve=soilTotal/terns;
    uvAve=uvTotal/terns;
    
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record7/Terns", terns));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record7/SoilLevel_RealTotal", soilTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record7/UVlevel_RealTotal", uvTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record7/SoilLevel_Average", soilAve));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record7/UVlevel_Average", uvAve));
        
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record8/Terns", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record8/SoilLevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record8/UVlevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record8/SoilLevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record8/UVlevel_Average", z));  
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record8/Snail_Count", z)); 
  }else if(hour==6){
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record6/Terns")){
      terns=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record6/SoilLevel_RealTotal")){
      soilTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record6/UVlevel_RealTotal")){
      uvTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    terns++;
    soilTotal=soilTotal+soilLevel;
    uvTotal=uvTotal+uvIntensity;
    soilAve=soilTotal/terns;
    uvAve=uvTotal/terns;
    
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record6/Terns", terns));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record6/SoilLevel_RealTotal", soilTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record6/UVlevel_RealTotal", uvTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record6/SoilLevel_Average", soilAve));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record6/UVlevel_Average", uvAve));
        
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record7/Terns", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record7/SoilLevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record7/UVlevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record7/SoilLevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record7/UVlevel_Average", z));  
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record7/Snail_Count", z));  
  }else if(hour==5){
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record5/Terns")){
      terns=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record5/SoilLevel_RealTotal")){
      soilTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record5/UVlevel_RealTotal")){
      uvTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    terns++;
    soilTotal=soilTotal+soilLevel;
    uvTotal=uvTotal+uvIntensity;
    soilAve=soilTotal/terns;
    uvAve=uvTotal/terns;
    
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record5/Terns", terns));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record5/SoilLevel_RealTotal", soilTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record5/UVlevel_RealTotal", uvTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record5/SoilLevel_Average", soilAve));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record5/UVlevel_Average", uvAve));
        
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record6/Terns", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record6/SoilLevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record6/UVlevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record6/SoilLevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record6/UVlevel_Average", z));  
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record6/Snail_Count", z));  
  }else if(hour==4){
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record4/Terns")){
      terns=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record4/SoilLevel_RealTotal")){
      soilTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record4/UVlevel_RealTotal")){
      uvTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    terns++;
    soilTotal=soilTotal+soilLevel;
    uvTotal=uvTotal+uvIntensity;
    soilAve=soilTotal/terns;
    uvAve=uvTotal/terns;
    
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record4/Terns", terns));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record4/SoilLevel_RealTotal", soilTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record4/UVlevel_RealTotal", uvTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record4/SoilLevel_Average", soilAve));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record4/UVlevel_Average", uvAve));
        
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record5/Terns", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record5/SoilLevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record5/UVlevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record5/SoilLevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record5/UVlevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record5/Snail_Count", z));    
  }else if(hour==3){
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record3/Terns")){
      terns=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record3/SoilLevel_RealTotal")){
      soilTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record3/UVlevel_RealTotal")){
      uvTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    terns++;
    soilTotal=soilTotal+soilLevel;
    uvTotal=uvTotal+uvIntensity;
    soilAve=soilTotal/terns;
    uvAve=uvTotal/terns;
    
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record3/Terns", terns));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record3/SoilLevel_RealTotal", soilTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record3/UVlevel_RealTotal", uvTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record3/SoilLevel_Average", soilAve));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record3/UVlevel_Average", uvAve));
        
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record4/Terns", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record4/SoilLevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record4/UVlevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record4/SoilLevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record4/UVlevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record4/Snail_Count", z));    
  }else if(hour==2){
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record2/Terns")){
      terns=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record2/SoilLevel_RealTotal")){
      soilTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record2/UVlevel_RealTotal")){
      uvTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    terns++;
    soilTotal=soilTotal+soilLevel;
    uvTotal=uvTotal+uvIntensity;
    soilAve=soilTotal/terns;
    uvAve=uvTotal/terns;
    
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record2/Terns", terns));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record2/SoilLevel_RealTotal", soilTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record2/UVlevel_RealTotal", uvTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record2/SoilLevel_Average", soilAve));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record2/UVlevel_Average", uvAve));
        
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record3/Terns", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record3/SoilLevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record3/UVlevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record3/SoilLevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record3/UVlevel_Average", z)); 
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record3/Snail_Count", z));   
  }else if(hour==1){
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record1/Terns")){
      terns=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record1/SoilLevel_RealTotal")){
      soilTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record1/UVlevel_RealTotal")){
      uvTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    terns++;
    soilTotal=soilTotal+soilLevel;
    uvTotal=uvTotal+uvIntensity;
    soilAve=soilTotal/terns;
    uvAve=uvTotal/terns;
    
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record1/Terns", terns));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record1/SoilLevel_RealTotal", soilTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record1/UVlevel_RealTotal", uvTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record1/SoilLevel_Average", soilAve));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record1/UVlevel_Average", uvAve));
        
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record2/Terns", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record2/SoilLevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record2/UVlevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record2/SoilLevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record2/UVlevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record2/Snail_Count", z));    
  }else if(hour==0){
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record0/Terns")){
      terns=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record0/SoilLevel_RealTotal")){
      soilTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    if(Firebase.RTDB.getInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record0/UVlevel_RealTotal")){
      uvTotal=fbdo.intData();
    }else{
    Serial.println(fbdo.errorReason());
    }
    terns++;
    soilTotal=soilTotal+soilLevel;
    uvTotal=uvTotal+uvIntensity;
    soilAve=soilTotal/terns;
    uvAve=uvTotal/terns;
    
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record0/Terns", terns));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record0/SoilLevel_RealTotal", soilTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record0/UVlevel_RealTotal", uvTotal));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record0/SoilLevel_Average", soilAve));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record0/UVlevel_Average", uvAve));
    
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record1/Terns", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record1/SoilLevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record1/UVlevel_RealTotal", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record1/SoilLevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record1/UVlevel_Average", z));
    (Firebase.RTDB.setInt(&fbdo, "PotDataFromID/1845697856/Plant_Pot_01/Report/Record1/Snail_Count", z));
    
  }

}
