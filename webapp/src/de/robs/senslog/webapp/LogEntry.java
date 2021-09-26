package de.robs.senslog.webapp;

import java.util.Arrays;
import java.util.Date;

public final class LogEntry {

  private final long timeStamp;
  private final double[] values;

  public LogEntry(long timeStamp, double[] values) {
    this.timeStamp = timeStamp;
    this.values = values;
  }

  public long getTimeStamp() {
    return timeStamp;
  }

  public double[] getValues() {
    return values;
  }

  public String toString() {
    return new Date(timeStamp).toString() +
        " - " + Arrays.toString(values);
  }
  
}
