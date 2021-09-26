package de.robs.senslog.webapp;

import java.io.IOException;
import java.util.Map;

import javax.servlet.ServletConfig;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public final class ChartServlet extends HttpServlet {

  private static final long serialVersionUID = 1711058732018514609L;
  private static final Log LOG = LogFactory.getLog(ChartServlet.class);

  public void init(ServletConfig config) throws ServletException {
    super.init(config);
  }

  protected void doGet(HttpServletRequest req, HttpServletResponse rsp)
      throws ServletException, IOException {

    try {

      LOG.debug("request from " + req.getRemoteAddr() +
          ", uri: " + req.getRequestURI());

      String sensorId = req.getParameter("id");
      String path = req.getPathInfo();

      if (sensorId == null) {
        LOG.error("sensor id not found in request parameters.");
        rsp.setStatus(HttpServletResponse.SC_NOT_FOUND);
        return;
      }

      Object sensorChartMap = getServletContext().getAttribute("sensorChartMap");

      if (sensorChartMap == null) {
        LOG.error("chart map not found in context attributes.");
        rsp.setStatus(HttpServletResponse.SC_NOT_FOUND);
        return;
      }

      byte[] chartBytes = (byte[])((Map<?, ?>)sensorChartMap).get(sensorId);

      if (chartBytes == null) {
        LOG.error("chart not found in map: sensorId=" + sensorId);
        rsp.setStatus(HttpServletResponse.SC_NOT_FOUND);
        return;
      }

      rsp.setContentType("image/png");
      rsp.getOutputStream().write(chartBytes);

    } catch (Exception e) {

      throw new ServletException(e.toString(), e);

    }

  }

}
