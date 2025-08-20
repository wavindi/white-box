#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial mySoftwareSerial(2, 3); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

// Button pins
const int playPauseButton = 7;
const int prevButton = 8;
const int nextButton = 9;

// Button states
bool playPauseState = false;
bool prevState = false;
bool nextState = false;
bool lastPlayPauseState = false;
bool lastPrevState = false;
bool lastNextState = false;

// Player state
bool isPlaying = false;
int currentTrack = 1;
int currentFolder = 1;
int maxFolders = 4;
int volume = 20;

void setup() {
  // Initialize serial communications
  mySoftwareSerial.begin(9600);
  Serial.begin(9600);
  
  // Initialize buttons with internal pull-up resistors (NO EXTERNAL RESISTORS NEEDED)
  pinMode(playPauseButton, INPUT_PULLUP);
  pinMode(prevButton, INPUT_PULLUP);
  pinMode(nextButton, INPUT_PULLUP);
  
  // Initialize DFPlayer
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println("Unable to begin DFPlayer");
    while(true);
  }
  
  Serial.println("DFPlayer Mini online");
  
  // Set initial volume and EQ
  myDFPlayer.volume(volume);
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  
  // Start with first track in first folder
  myDFPlayer.playFolder(currentFolder, currentTrack);
  isPlaying = true;
  
  delay(100);
}

void loop() {
  // Read button states (NOTE: INPUT_PULLUP makes buttons read LOW when pressed)
  playPauseState = (digitalRead(playPauseButton) == LOW);
  prevState = (digitalRead(prevButton) == LOW);
  nextState = (digitalRead(nextButton) == LOW);
  
  // Handle Play/Pause button
  if (playPauseState && !lastPlayPauseState) {
    if (isPlaying) {
      myDFPlayer.pause();
      Serial.println("Paused");
      isPlaying = false;
    } else {
      myDFPlayer.start();
      Serial.println("Playing");
      isPlaying = true;
    }
    delay(200); // Debounce
  }
  
  // Handle Previous button (previous folder)
  if (prevState && !lastPrevState) {
    currentFolder--;
    if (currentFolder < 1) {
      currentFolder = maxFolders;
    }
    currentTrack = 1;
    myDFPlayer.playFolder(currentFolder, currentTrack);
    Serial.print("Playing folder: ");
    Serial.println(currentFolder);
    isPlaying = true;
    delay(200); // Debounce
  }
  
  // Handle Next button (next folder)
  if (nextState && !lastNextState) {
    currentFolder++;
    if (currentFolder > maxFolders) {
      currentFolder = 1;
    }
    currentTrack = 1;
    myDFPlayer.playFolder(currentFolder, currentTrack);
    Serial.print("Playing folder: ");
    Serial.println(currentFolder);
    isPlaying = true;
    delay(200); // Debounce
  }
  
  // Store last button states
  lastPlayPauseState = playPauseState;
  lastPrevState = prevState;
  lastNextState = nextState;
  
  // Check if track finished and loop it
  if (myDFPlayer.available()) {
    if (myDFPlayer.readType() == DFPlayerPlayFinished) {
      // Loop current track
      myDFPlayer.playFolder(currentFolder, currentTrack);
    }
  }
  
  delay(50);
}
