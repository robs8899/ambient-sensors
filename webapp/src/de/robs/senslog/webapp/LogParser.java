package de.robs.senslog.webapp;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.text.DateFormat;
import java.text.DateFormatSymbols;
import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Locale;
import java.util.regex.Pattern;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public final class LogParser {

  private static final Log LOG = LogFactory.getLog(LogParser.class);

  private static final Pattern CSV_SPLITTER = Pattern.compile(",(?=(?:[^\"]*\"[^\"]*\")*[^\"]*$)");
  private static final Pattern KEY_VAL_SPLITTER = Pattern.compile("=");
  private static final String CSV_ENCODING = "US-ASCII";
  private static final int CSV_BUFFER = 8192;

  private final String logFilePath;
  private final DateFormat logNameFormat;
  private final DateFormat logTsFormat;
  private final DecimalFormat logNumFormat;
  private final List<LogEntry> logEntries;

  public LogParser(String lfPath, String lfFmt, String tsFmt, String numFmt, String locale) {

    logFilePath = lfPath;
    Locale logLocale = new Locale(locale);
    logNameFormat = new SimpleDateFormat(lfFmt, new DateFormatSymbols(logLocale));
    logTsFormat = new SimpleDateFormat(tsFmt, new DateFormatSymbols(logLocale));
    logNumFormat = new DecimalFormat(numFmt, new DecimalFormatSymbols(logLocale));
    logEntries = new ArrayList<LogEntry>();

  }

  public void parse(Date date) throws UnsupportedEncodingException {

    logEntries.clear();

    String filePath = logFilePath + "/" + logNameFormat.format(date);
    LOG.debug("parsing: " + filePath);

    FileInputStream fis = null;
    try { fis = new FileInputStream(filePath);
    } catch (Exception e) {
      LOG.error(e.toString());
      return;
    }

    InputStreamReader isr = new InputStreamReader(fis, CSV_ENCODING);
    BufferedReader br = new BufferedReader(isr, CSV_BUFFER);
    String brl = null;

    try {

      while ((brl = br.readLine()) != null) {

        String tokens[] = CSV_SPLITTER.split(brl);
        for (int i=0; i<tokens.length; i++) {
          tokens[i] = trim(tokens[i], 0x0020); // trim spaces
          tokens[i] = trim(tokens[i], 0x0022); // trim quotes
        }

        long ts = logTsFormat.parse(tokens[0].trim()).getTime();

        // create a log entry with number of tokens
        LogEntry entry = new LogEntry(ts, new double[tokens.length-1]);

        for (int i=1; i<tokens.length; i++) {
          
          // split key/value pairs if present
          String kv[] = KEY_VAL_SPLITTER.split(tokens[i]);
          String val = kv[kv.length-1];
          
          val = trim(val, 0x0020); // trim spaces
          val = trim(val, 0x0022); // trim quotes
          val = trim(val, 0x002B); // trim plus
          
          entry.getValues()[i-1] = logNumFormat.parse(val).doubleValue();
          
        }
        
        logEntries.add(entry);

      }

    } catch (Exception e) {

      LOG.error(e, e);

    } finally {

      try { br.close();
      } catch (Exception e) {
        LOG.error(e, e);
      }

    }

  }

  public List<LogEntry> getAll() {

    return logEntries;

  }

  public LogEntry getAt(int index) {

    if (index >= 0 && logEntries.size() > index)
      return logEntries.get(index);
    return null;

  }

  public LogEntry getLast() {
    
    return getAt(logEntries.size()-1);
    
  }
  
  public LogEntry getMax(int valueIndex) {

    double max = Double.NEGATIVE_INFINITY;
    int idx = -1;

    for (int i=0; i<logEntries.size(); i++) {
      double val = logEntries.get(i).getValues()[valueIndex];
      if (val > max) {
        max = val;
        idx = i;
      }
    }

    return logEntries.get(idx);

  }

  public LogEntry getMin(int valueIndex) {

    double min = Double.POSITIVE_INFINITY;
    int idx = -1;

    for (int i=0; i<logEntries.size(); i++) {
      double val = logEntries.get(i).getValues()[valueIndex];
      if (val < min) {
        min = val;
        idx = i;
      }
    }

    return logEntries.get(idx);

  }

  public LogEntry getMean() {

    LogEntry mean = new LogEntry(logEntries.get(0).getTimeStamp(),
        new double[logEntries.get(0).getValues().length]);

    for (int i=0; i<logEntries.size(); i++) {
      for (int k=0; k<logEntries.get(i).getValues().length; k++) {
        mean.getValues()[k] += logEntries.get(i).getValues()[k];
      }
    }

    for (int k=0; k<logEntries.get(0).getValues().length; k++) {
      mean.getValues()[k] /= logEntries.size();
    }

    return mean;

  }

  public int getCount() {
    
    return logEntries.size();
    
  }
  
  // ------------------- private stuff ----------------------

  /** Warning: Problem at the end of daylight saving time! */
  @SuppressWarnings("unused")
  private List<String> createLogFileCollection(Date from, Date to) {

    List<String> resultList = new ArrayList<String>();
    long interval = 24 * 60 * 60 * 1000; // 24 hour in millis
    long current = from.getTime();
    long end = to.getTime();

    while (current <= end) {
      resultList.add(logNameFormat.format(new Date(current)));
      current += interval;
    }

    return resultList;

  }


  /** Check for a time between 06:00 and 18:00 */
  @SuppressWarnings("unused")
  private static boolean isDayTime(long ts) {

    int t0 = (int)(ts % (24*60*60*1000L));
    int t1 = 60 * 60 * 1000 * 6;
    int t2 = 60 * 60 * 1000 * 18;
    return t0 >= t1 && t0 < t2;

  }

  private static String trim(String s, int c) {

    int st = 0;
    int len = s.length();

    while ((st < len) && (s.codePointAt(st) == c)) st++;
    while ((st < len) && (s.codePointAt(len - 1) == c)) len--;
    return ((st > 0) || (len < s.length())) ? s.substring(st, len) : s;

  }

}
