package de.robs.senslog.webapp;

import java.io.IOException;
import java.text.DateFormat;
import java.text.DateFormatSymbols;
import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Locale;

import de.robs.senslog.config.SampleValueConfig;
import de.robs.senslog.config.SensorConfig;
import de.robs.senslog.config.StationConfig;
import de.robs.senslog.model.SampleModel;
import de.robs.senslog.model.ValueModel;

public class SampleBuilder {

  // display formatters
  private final DateFormat displayFormatDateLong;
  private final DateFormat displayFormatDateShort;
  private final DecimalFormat displayFormatSamples;

  public SampleBuilder(StationConfig config) {

    // --- create display formatter instances ---

    Locale displayLocale = new Locale(config.getDspLocale());

    this.displayFormatDateLong = new SimpleDateFormat(
        config.getDspFmtTsLong(), new DateFormatSymbols(displayLocale));

    this.displayFormatDateShort = new SimpleDateFormat(
        config.getDspFmtTsShort(), new DateFormatSymbols(displayLocale));

    this.displayFormatSamples = new DecimalFormat(
        config.getDspFmtSamples(), new DecimalFormatSymbols(displayLocale));

  }

  public SampleModel create(LogEntry entry, SensorConfig scfg)
      throws IOException, ParseException {

    if (entry == null) return null;

    SampleModel sample = new SampleModel(
        displayFormatDateLong.format(new Date(entry.getTimeStamp())));
    List<SampleValueConfig> svcl = scfg.getSampleValueConfig();

    for (int i=0; i<svcl.size(); i++) {

      SampleValueConfig svc = svcl.get(i);
      if (svc.isIgnored()) continue;
      sample.getValues().add(new ValueModel(svc.getName(), svc.getUnit(),
          displayFormatSamples.format(entry.getValues()[i])));

    }

    return sample;

  }

  public List<SampleModel> create(List<LogEntry> entries, SensorConfig scfg)
      throws IOException, ParseException {

    if (entries == null) return null;

    List<SampleModel> samples = new ArrayList<SampleModel>();

    for (LogEntry entry : entries) {

      SampleModel sample = new SampleModel(
          displayFormatDateShort.format(new Date(entry.getTimeStamp())));
      List<SampleValueConfig> svcl = scfg.getSampleValueConfig();

      for (int i=0; i<svcl.size(); i++) {

        SampleValueConfig svc = svcl.get(i);
        if (svc.isIgnored()) continue;
        sample.getValues().add(new ValueModel(svc.getName(), svc.getUnit(),
            displayFormatSamples.format(entry.getValues()[i])));

      }

      samples.add(sample);

    }

    return samples;

  }

}
