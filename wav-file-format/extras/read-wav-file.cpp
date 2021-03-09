/*
  A tiny program to read a WAV file, display it's metadata, modify the audio sample data (reduce the volume by half)
  and write the audio data back into a new file. The program is designed to help understand the details
  of how the WAV file format is read to a data structure. It is not meant to be used as a library for reading .wav files.

  To run: g++ <filename.cpp>
  Written by: Akash Murthy
 */

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

/*
 * A structure to hold a Wav file's metadata.
 * Note that this will not support all types of wave files!!!
 * Wave files could have a variable length format chunks, they could have chunks organized differently, etc.
 * This is only meant as a example on how to read binary data to a struct.
*/
struct WavHeader {
    char riffId[4];
    unsigned long headerChunkSize;
    char waveId[4];
    char formatChunkId[4];
    unsigned long formatChunkSize;
    unsigned short compressionCode;
    unsigned short numberOfChannels;
    unsigned long samplesRate;
    unsigned long byteRate;
    unsigned short blockAlign;
    unsigned short bitsPerSample;
    char dataChunkId[4];
    unsigned long dataChunkSize;
};

// Utility function to just print out some metadata.
void displayWavFileInfo(WavHeader& file) {
    cout << "Header chunk ID: " << string (file.riffId) << endl;
    cout << "Header chunk size: " << file.headerChunkSize << endl;
    cout << "Wave ID: " << string (file.waveId) << endl;

    cout << "Format chunk ID: " << string (file.formatChunkId) << endl;
    cout << "Format chunk size: " << file.formatChunkSize << endl;
    cout << "Compression code: " << file.compressionCode << endl;
    cout << "Number of channels: " << file.numberOfChannels << endl;
    cout << "Sample rate: " << file.samplesRate << endl;
    cout << "Byte rate: " << file.byteRate << endl;
    cout << "Block align: " << file.blockAlign << endl;
    cout << "Bits per sample: " << file.bitsPerSample << endl;

    cout << "Data chunk ID: " << string (file.dataChunkId) << endl;
    cout << "Data chunk size: " << (file.dataChunkSize) << endl;
}

// For this simple example, we only support Linear PCM, with 16 bit integer format
bool isSupported(WavHeader& header) {
    if (header.compressionCode == 1 && header.bitsPerSample == 16) {
        return true;
    }
    return false;
}

int main() {
    ifstream audioFile;
    audioFile.open("waveform.wav", ios::binary);
    audioFile.seekg(0, ios::beg);

    // Read and display the wave header alone
    WavHeader wavHeader{};
    audioFile.read((char*)&wavHeader, sizeof(WavHeader));
    displayWavFileInfo(wavHeader);

    // Decide if the wav file chosen is supported or not
    if (!isSupported(wavHeader)) {
        cout << "Wav file is not supported" << endl;
        audioFile.close();
        return 1;
    }

    /* dataChunkSize is the total number of bytes of audio data. Each byte is 8 bits.
       Since we need 16 bit integer (short) for storing the audio sample value, the total number of samples
       are dataChunkSize / 2
    */
    const auto totalNumberOfSamples = wavHeader.dataChunkSize/2;

    // Seek to the start of the audio data
    audioFile.seekg(sizeof(WavHeader), ios::beg);

    // Initialize a vector container for storing all the audio samples and read data to it.
    vector<short> wavData;
    wavData.resize(totalNumberOfSamples);
    audioFile.read((char*)wavData.data(), wavHeader.dataChunkSize);

    // Do some processing on the data
    for (int i = 0; i < totalNumberOfSamples; i++) {
        // Half the amplitude level of each sample.
        // Same as reducing the gain by 6dBFS
        wavData[i] *= 0.5;
    }

    // Write the modified data into a new file
    ofstream outputFile;
    outputFile.open("waveform-modified.wav", ios::binary);
    outputFile.write((char*)&wavHeader, sizeof(WavHeader));
    outputFile.write((char*)wavData.data(), wavHeader.dataChunkSize);

    audioFile.close();
    outputFile.close();
    return 0;
}
