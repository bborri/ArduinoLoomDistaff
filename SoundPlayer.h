#pragma once

#include "assert.h"
#include "WaveUtil.h"
#include "WaveHC.h"


class SoundPlayer
{
public:
  SoundPlayer()
  {
    // Set the output pins for the DAC control. These pins are defined in the library
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);

    // Play with 8 MHz spi (default faster!)
    if (!m_card.init())
    {
      sdErrorCheck();  // Something went wrong, let's print out why
      while(1);        // then 'halt' - do nothing!
    }

    // enable optimize read - some cards may timeout. Disable if you're having problems
    m_card.partialBlockRead(true);

    // Now we will look for a FAT partition!
    uint8_t part;
    // we have up to 5 slots to look in
    for (part = 0; part < 5; part++)
    {
      if (m_volume.init(m_card, part)) 
        break;                             // we found one, let's bail
    }
    // if we ended up not finding one  :(
    if (part == 5)
    {
      putstring_nl("No valid FAT partition!");
      sdErrorCheck();      // Something went wrong, let's print out why
      while(1);            // then 'halt' - do nothing!
    }

    // Let's tell the user about what we found
    putstring("Using partition ");
    Serial.print(part, DEC);
    putstring(", type is FAT");
    Serial.println(m_volume.fatType(), DEC);     // FAT16 or FAT32?

    // Try to open the root directory
    if (!m_root.openRoot(m_volume))
    {
      putstring_nl("Can't open root dir!"); // Something went wrong,
      while(1);                             // then 'halt' - do nothing!
    }
  
    // Whew! We got past the tough parts.
    //putstring_nl("Files found:");
    // Print out all of the files in all the directories.
    //listDirectory(m_root);
  }

  void playLoomNote(const char* const note)
  {
    // Prepare the buffer
    char buffer[13];
    ::memset(buffer, 0, 13);
    // Copy the note name
    auto noteSize = ::strlen(note);
    assert(noteSize >= 1 && noteSize <= 8);
    ::strcpy(buffer, note);
    // Copy extension
    ::strncpy(buffer + noteSize, ".WAV", 12 - noteSize);
    Serial.println(note);
    //playSoundAsync(buffer);
    playSoundWait(buffer);
  }

  // Plays a full file from beginning to end synchronously
  void playSoundWait(const char* const name)
  {
    // call our helper to find and play this name
    playSoundAsync(name);
    while (m_wave.isplaying)
    {
      // do nothing while its playing
    }
    // now its done playing
  }

  // Start playing a sound in background and give control back
  void playSoundAsync(const char* const name)
  {
    stop();

    FatReader file;
    // look in the root directory and open the file
    if (!file.open(m_root, (char*)name))
    {
      putstring("Couldn't open file ");
      Serial.println(name);
      return;
    }
    // OK read the file and turn it into a wave object
    if (!m_wave.create(file))
    {
      putstring_nl("Not a valid WAV");
      return;
    }
      
    // ok time to play! start playback
    m_wave.play();
  }

  // Stop playing if you were. Do nothing if you weren't.
  void stop()
  {
    // see if the wave object is currently doing something
    if (m_wave.isplaying)
    {
      // already playing something, so stop it!
      m_wave.stop();
    }
  }

private:
  // list recursively - possible stack overflow if subdirectories too nested
  void listDirectory(FatReader& dir, int8_t dirLevel = 0, bool recursive = true)
  {
    // buffer for directory reads
    dir_t dirBuf;
    // read the next file in the directory 
    int8_t recur;
    while ((recur = dir.readDir(dirBuf)) > 0)
    {
      // skip subdirs . and ..
      if (dirBuf.name[0] == '.') 
        continue;
        
      for (uint8_t i = 0; i < dirLevel; i++) 
        Serial.print(' ');        // this is for prettyprinting, put spaces in front
      printEntryName(dirBuf);           // print the name of the file we just found
      Serial.println();           // and a new line
        
      if (recursive && DIR_IS_SUBDIR(dirBuf)) {   // we will recurse on any direcory
        FatReader nextDir;           // make a new directory object to hold information
        if (nextDir.open(m_volume, dirBuf)) 
          listDirectory(nextDir, dirLevel + 2, true); // list all the files in this directory now!
      }
    }
    // are we doing OK?
    //sdError();
  }

  bool sdErrorCheck()
  {
    if (!m_card.errorCode())
      return true;
    putstring_nl("SD error");
    putstring("  errorCode: ");
    Serial.println(m_card.errorCode(), HEX);
    putstring("  errorData: ");
    Serial.println(m_card.errorData(), HEX);
    return false;
  }

private:
  // SD Card
  SdReader m_card;
  // Partition volume
  FatVolume m_volume;
  // Filesystem root
  FatReader m_root;

  // Wave player
  WaveHC m_wave;
};
