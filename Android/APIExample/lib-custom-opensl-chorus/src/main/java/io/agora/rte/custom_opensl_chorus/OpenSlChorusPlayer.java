package io.agora.rte.custom_opensl_chorus;

public class OpenSlChorusPlayer {
  static {
    System.loadLibrary("opensl-chorus");
  }

  public native void initOpenslDemo(long handle);

  public native void startOpenslDemo();

  public native void stopOpenslDemo();

  public native void startEchoTest();

  public native void stopEchoTest();
}