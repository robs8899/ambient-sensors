package de.robs.senslog.config;

import java.util.ArrayList;
import java.util.List;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlRootElement;

@XmlAccessorType(XmlAccessType.FIELD)
@XmlRootElement(name="sensor")
public class SensorConfig {

  @XmlAttribute(name="id") private String sensorId;
  @XmlAttribute(name="name") private String sensorName;
  @XmlAttribute(name="log-file") private String logFile;
  @XmlAttribute(name="log-path") private String logPath;
  @XmlAttribute(name="log-locale") private String logLocale;
  @XmlAttribute(name="log-fmt-tstmp") private String logFmtTs;
  @XmlAttribute(name="log-fmt-smpls") private String logFmtSamples;
  @XmlElement(name="sample-value") private List<SampleValueConfig> sampleValueConfig;

  public SensorConfig() {}

  public String getSensorId() {
    return sensorId;
  }

  public String getSensorName() {
    return sensorName;
  }

  public String getLogFile() {
    return logFile;
  }

  public String getLogPath() {
    return logPath;
  }

  public String getLogLocale() {
    return logLocale;
  }

  public String getLogFmtTs() {
    return logFmtTs;
  }

  public String getLogFmtSamples() {
    return logFmtSamples;
  }

  public List<SampleValueConfig> getSampleValueConfig() {
    if (sampleValueConfig == null) sampleValueConfig = new ArrayList<SampleValueConfig>();
    return sampleValueConfig;
  }

  public void setSensorId(String sensorId) {
    this.sensorId = sensorId;
  }

  public void setSensorName(String sensorName) {
    this.sensorName = sensorName;
  }

  public void setLogFile(String logFile) {
    this.logFile = logFile;
  }

  public void setLogPath(String logPath) {
    this.logPath = logPath;
  }

  public void setLogLocale(String logLocale) {
    this.logLocale = logLocale;
  }

  public void setLogFmtTs(String logFmtTs) {
    this.logFmtTs = logFmtTs;
  }

  public void setLogFmtSamples(String logFmtSamples) {
    this.logFmtSamples = logFmtSamples;
  }

}
