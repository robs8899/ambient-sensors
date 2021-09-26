package de.robs.senslog.webapp;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Font;
import java.awt.Shape;
import java.awt.Stroke;
import java.awt.geom.Ellipse2D;
import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.text.NumberFormat;
import java.util.Locale;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.Axis;
import org.jfree.chart.axis.AxisLocation;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.block.BlockBorder;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.XYItemRenderer;
import org.jfree.chart.renderer.xy.XYSplineRenderer;
import org.jfree.chart.title.LegendTitle;
import org.jfree.data.time.FixedMillisecond;
import org.jfree.data.time.TimeSeries;
import org.jfree.data.time.TimeSeriesCollection;
import org.jfree.data.xy.XYDataset;
import org.jfree.ui.RectangleEdge;

import de.robs.senslog.config.StationConfig;

public class ChartBuilder {

  private static final Color CHART_BKGR_COLOR = new Color(0x282828);
  private static final Color CHART_AXIS_COLOR = new Color(0xE8E8E8);
  private static final Font CHART_AXIS_FONT = new Font(Font.SANS_SERIF, Font.PLAIN, 14);
  private static final Stroke CHART_AXIS_STROKE = new BasicStroke(1.0f);
  private static final Stroke CHART_SERIES_STROKE = new BasicStroke(2.0f);
  private static final Shape CHART_SERIES_SHAPE = new Ellipse2D.Float(-2.0f, -2.0f, 4.0f, 4.0f);
  
  private final JFreeChart chart;
  private final NumberFormat format;
  private TimeSeries series;
  private int index = -1;

  public ChartBuilder(StationConfig config) {

    this.chart = ChartFactory.createTimeSeriesChart(null, null, null, null);
    chart.setBackgroundPaint(CHART_BKGR_COLOR);

    XYPlot plot = (XYPlot)chart.getPlot();
    plot.setBackgroundPaint(CHART_BKGR_COLOR);
    plot.setDomainGridlinePaint(CHART_AXIS_COLOR);
    plot.setRangeGridlinePaint(CHART_AXIS_COLOR);
    plot.setOutlineVisible(false);

    Axis axis = plot.getDomainAxis();
    axis.setAxisLinePaint(CHART_AXIS_COLOR);
    axis.setAxisLineStroke(CHART_AXIS_STROKE);
    axis.setLabelFont(CHART_AXIS_FONT);
    axis.setLabelPaint(CHART_AXIS_COLOR);
    axis.setTickLabelFont(CHART_AXIS_FONT);
    axis.setTickLabelPaint(CHART_AXIS_COLOR);
    axis.setTickMarkPaint(CHART_AXIS_COLOR);
    axis.setTickMarkStroke(CHART_AXIS_STROKE);

    LegendTitle legend = chart.getLegend();
    legend.setBackgroundPaint(CHART_BKGR_COLOR);
    legend.setItemPaint(CHART_AXIS_COLOR);
    legend.setFrame(BlockBorder.NONE);
    legend.setItemFont(CHART_AXIS_FONT);
    legend.setPosition(RectangleEdge.BOTTOM);
    legend.setVisible(true);

    Locale locale = new Locale(config.getDspLocale());
    this.format = new DecimalFormat(config.getDspFmtSamples(),
        new DecimalFormatSymbols(locale));
    
  }

  public void addValueAxis(String label, String name) {

    index++;
    series = new TimeSeries(name);
    
    XYDataset dataset = new TimeSeriesCollection(series);
    
    NumberAxis axis = new NumberAxis(label);

    axis.setAxisLinePaint(CHART_AXIS_COLOR);
    axis.setAxisLineStroke(CHART_AXIS_STROKE);
    axis.setLabelFont(CHART_AXIS_FONT);
    axis.setLabelPaint(CHART_AXIS_COLOR);
    axis.setTickLabelFont(CHART_AXIS_FONT);
    axis.setTickLabelPaint(CHART_AXIS_COLOR);
    axis.setTickMarkPaint(CHART_AXIS_COLOR);
    axis.setTickMarkStroke(CHART_AXIS_STROKE);

    axis.setNumberFormatOverride(format);
    axis.setAutoRangeIncludesZero(false);
    axis.setAutoRangeMinimumSize(2.00);

    XYItemRenderer renderer = new XYSplineRenderer(10);
    // XYItemRenderer renderer = new XYLineAndShapeRenderer(true, true);
    renderer.setSeriesStroke(0, CHART_SERIES_STROKE);
    renderer.setSeriesShape(0, CHART_SERIES_SHAPE);

    XYPlot plot = (XYPlot)chart.getPlot();
    plot.setDataset(index, dataset);
    plot.setRangeAxis(index, axis);
    plot.mapDatasetToRangeAxis(index, index);
    plot.setRangeAxisLocation(index, AxisLocation.BOTTOM_OR_LEFT);
    plot.setRenderer(index, renderer); 

  }

  public void addValue(long timeStamp, double value) {
    series.add(new FixedMillisecond(timeStamp), value);
  }
  
  public JFreeChart getChart() {
    return chart;
  }
  
}
