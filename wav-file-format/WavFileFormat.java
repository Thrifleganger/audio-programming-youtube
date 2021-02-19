/*
  A tiny Java program to create a sine tone within a WAV file. The program is designed to help understand the details
  of how the WAV file format is internally organized. It is not meant to be used as a library for generating .wav files.

  To run: java <filename.java>
  Written by: Akash Murthy
 */

import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.file.Files;
import java.nio.file.Paths;

public class WavFileFormat {

  public static int SAMPLE_RATE = 44100;
  public static int BIT_DEPTH = 16;

  private static class SineOscillator {
    private float frequency, amplitude, angle = 0.0f, offset = 0.0f;

    public SineOscillator(float frequency, float amplitude) {
      this.frequency = frequency;
      this.amplitude = amplitude;
      this.offset = 2 * (float) Math.PI * this.frequency / WavFileFormat.SAMPLE_RATE;
    }

    public float process() {
      float sample = amplitude * (float) Math.sin(angle);
      angle += offset;
      return sample;
    }
  }

  public static void main(String[] args) throws IOException {

    int duration = 2;
    SineOscillator sineOscillator = new SineOscillator(440, 0.5f);
    Files.deleteIfExists(Paths.get("waveform.wav"));
    RandomAccessFile audioFile = new RandomAccessFile("waveform.wav", "rw");

    //Header chunk
    audioFile.write("RIFF".getBytes());
    audioFile.write("----".getBytes());
    audioFile.write("WAVE".getBytes());

    // Format chunk
    audioFile.write("fmt ".getBytes());
    writeToFile(audioFile, 16, 4); // Size
    writeToFile(audioFile, 1, 2); // Compression code
    writeToFile(audioFile, 1, 2); // Number of channels
    writeToFile(audioFile, SAMPLE_RATE, 4); // Sample rate
    writeToFile(audioFile, SAMPLE_RATE * BIT_DEPTH / 8, 4); // Byte rate
    writeToFile(audioFile, BIT_DEPTH / 8, 2); // Block align
    writeToFile(audioFile, BIT_DEPTH, 2); // Bit depth

    // Data chunk
    audioFile.write("data".getBytes());
    audioFile.write("----".getBytes());

    long preAudioPosition = audioFile.getFilePointer();

    float maxAmplitude = (float) Math.pow(2, BIT_DEPTH - 1) - 1;
    for (int i = 0; i < SAMPLE_RATE * duration; i++) {
      float sample = sineOscillator.process();
      int intSample = (int) (sample * maxAmplitude);
      writeToFile(audioFile, intSample, 2);
    }
    long postAudioPosition = audioFile.getFilePointer();

    audioFile.seek(preAudioPosition - 4);
    writeToFile(audioFile, (int) (postAudioPosition - preAudioPosition), 4);

    audioFile.seek(4L);
    writeToFile(audioFile, (int) (postAudioPosition - 8), 4);

    audioFile.close();
  }

  /*
  The JVM or the Java Virtual Machine stores data in Big Endian format. When writing binary data to file using the
  RandomAccessFile class, the data is written in Big Endian as well. Which is a huge problem, since the native OS could
  have a different ordering. In my case, the ordering for Windows OS is little endian, and the files written directly
  would be unreadable my any application outside the JVM. Hence, we have to do this ordering manually before writing
  to the file.
   */
  static void writeToFile(RandomAccessFile file, int value, int size) throws IOException {
    ByteBuffer byteBuffer = ByteBuffer.allocate(size);
    byteBuffer.order(ByteOrder.LITTLE_ENDIAN);
    if (size == 2) {
      byteBuffer.putShort((short)value);
    } else if (size == 4) {
      byteBuffer.putInt(value);
    }
    file.write(byteBuffer.array());
  }
}
