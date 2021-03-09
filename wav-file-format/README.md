# wav-file-format

A C++ program to create a sine tone within a WAV file. The program is designed to help understand the details of how the WAV file format is internally organized. It is not meant to be used as a library for generating .wav files. 

Accompanying YouTube video: https://www.youtube.com/watch?v=qqjvB_VxMRM

Running the C++ code: 
```
g++ -o wave-file-format main.cpp 
./wave-file-format.exe
```

Extras: 

- `WaveFileFormat.java` is a tiny program which is a Java alternative to C++. It pretty much does the same thing, but had to force the byte ordering to little endian. This might be a problem when running this program to generate a wav file in an OS which has big endian byte ordering.

Running the Java code:
```
java WaveFileFormat.java
```