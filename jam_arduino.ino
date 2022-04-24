#include <Time.h>
#include <TimeLib.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <PrayerTimes.h>

char daysOfTheWeek[7][12] = {"Ahad", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};
char months[12][5] = {"Jan", "Feb", "Mar", "Apr", "Mei", "Jun", "Jul", "Ags", "Sep", "Okt", "Nov", "Des"};

double waktu[sizeof(TimeName)/sizeof(char*)];

static const char* NamaWaktu[] ={
  "Imsak",
  "Subuh",
  "Syuruq",
  "Dzuhur",
  "Ashar",
  "Terbenam",
  "Maghrib",
  "Isya'",
  "Count"
};

const byte JUMLAH_WAKTU_DEFAULT = sizeof(waktu)/sizeof(double);
const byte JUMLAH_WAKTU = sizeof(NamaWaktu)/sizeof(char*);
const byte DATA_WAKTU = 2; // jam dan menit
byte WaktuPenting[JUMLAH_WAKTU][DATA_WAKTU];

// pengaturan bujur lintang (bogor) -6.595038,106.816635
float _lat = -6.595038;    // lintang
float _lng = 106.816635;   // bujur

// timezone
int _timezone = 7; // zona waktu WIB=7, WITA=8, WIT=9

// sudut subuh dan maghrib Indonesia
int _sudut_subuh = 20;
int _sudut_isyak = 18;

// Uncomment according to your hardware type
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
//#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW

// Defining size, and output pins
#define MAX_DEVICES 4
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10


// Create a new instance of the MD_Parola class with hardware SPI connection
MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

void setup()
{
  Serial.begin(9600);
  // setTime(jam,menit,detik,hari,bulan,tahun);
  setTime(21,12,0,24,4,22);  // sesuaikan jam sekarang
  myDisplay.begin();
  
  // Set the intensity (brightness) of the display (0-15)
  myDisplay.setIntensity(0);
  
  // Clear the display
  myDisplay.displayClear();
  set_calc_method(ISNA);  // metode perhitungan
  set_asr_method(Shafii); // madzhab?
  set_high_lats_adjust_method(AngleBased);
  set_fajr_angle(_sudut_subuh);     // sudut waktu subuh
  set_isha_angle(_sudut_isyak);     // sudut waktu isyak
 
}

void loop()
{
  for(byte i=0; i<5; i++){
    displayDateTime();
  }
  WaktuShalat(WaktuPenting);
  CetakWaktuShalat();
}

void displayDateTime() {
  String jam = String(hour());
  String menit = printDigits(minute());
  String detik = printDigits(second());
  String waktuskrg = jam + ":" + menit;
  String tanggal = String(day());
  String bulan = String(months[month()-1]);
  String tahun = String(year());
  myDisplay.setTextAlignment(PA_CENTER);
  myDisplay.print(daysOfTheWeek[weekday()-1]);
  delay(1000);
  for(byte i=1; i<=10; i++){
    if(i % 2 == 0) {
      waktuskrg = jam + " " + menit;
    } else {
      waktuskrg = jam + ":" + menit;
    }
    myDisplay.setTextAlignment(PA_CENTER);
    myDisplay.print(waktuskrg);
    delay(1000);
  }
  myDisplay.setTextAlignment(PA_CENTER);
  myDisplay.print(tanggal + " " + bulan);
  delay(1000);
  myDisplay.setTextAlignment(PA_CENTER);
  myDisplay.print(tahun);
  delay(1000);
}

String printDigits(int digits)
{
  String digit = String(digits);
  if(digits < 10)
    digit = "0" + digit; 
  return digit;
}


void CetakWaktuShalat(){
    int _tahun = year();
    int _bulan = month();
    int _hari = day();
    for(byte i=0; i<JUMLAH_WAKTU-1; i++){
        if( i == 5 ) continue;
        String namawaktu =  NamaWaktu[i];
        myDisplay.setTextAlignment(PA_CENTER);
        myDisplay.print(namawaktu);
        delay(1000);
        String jamsholat =  String(DuaDigit(WaktuPenting[i][0])) + ":" +  String(DuaDigit(WaktuPenting[i][1]));
        myDisplay.setTextAlignment(PA_CENTER);
        myDisplay.print(jamsholat);
        delay(1000);
    }
}
  
void WaktuShalat(byte WaktuShalat[JUMLAH_WAKTU][DATA_WAKTU]){
  // pengaturan jam dan tanggal
  int _tahun = year();
  int _bulan = month();
  int _hari = day(); // tanggal
    get_prayer_times(_tahun, _bulan, _hari, _lat, _lng, _timezone, waktu);
    byte waktu_custom = 0;
    for(byte i=0; i<JUMLAH_WAKTU_DEFAULT-1; i++){
        int jam, menit;
        get_float_time_parts(waktu[i], jam, menit);
        if( i == 0 ){
            WaktuPenting[waktu_custom][0] = jam;
            WaktuPenting[waktu_custom][1] = menit-10;
            waktu_custom++;
            WaktuPenting[waktu_custom][0] = jam;
            WaktuPenting[waktu_custom][1] = menit;
        } else {
            WaktuPenting[waktu_custom][0] = jam;
            WaktuPenting[waktu_custom][1] = menit;
        }
        waktu_custom++;
    }
}
  
String DuaDigit(byte angka){
    if( angka > 9 ){
        return String(angka);
    } else {
        String tmp = "0";
        tmp.concat(angka);
        return tmp;
    }
}
