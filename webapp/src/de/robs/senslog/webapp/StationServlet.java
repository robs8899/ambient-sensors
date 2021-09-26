package de.robs.senslog.webapp;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import javax.servlet.ServletConfig;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import de.robs.senslog.OutputRenderer;

public final class StationServlet extends HttpServlet {

  private static final long serialVersionUID = 1789878746313564971L;
  private static final Log LOG = LogFactory.getLog(StationServlet.class);

  public void init(ServletConfig config) throws ServletException {
    super.init(config);
  }

  protected void doGet(HttpServletRequest req, HttpServletResponse rsp)
      throws ServletException, IOException {

    try {

      LOG.debug("request from " + req.getRemoteAddr() +
          ", uri: " + req.getRequestURI());

      String target = req.getParameter("target");
      if (target == null) target = "index";
      
      String sensorId = req.getParameter("id");
      if (sensorId == null) sensorId = "00000000-0000-0000-0000-000000000000";

      Object renderModel = null; // The model to render

      if (target.equals("index")) {

        renderModel = getServletContext().getAttribute("stationModel");

      } else if (target.equals("charts")) {

        renderModel = getServletContext().getAttribute("stationModel");

      } else if (target.equals("sensor")) {

        Object sensorSeriesMap = getServletContext().getAttribute("sensorSeriesMap");

        if (sensorSeriesMap == null) {
          LOG.error("series map not found in context attributes.");
          rsp.setStatus(HttpServletResponse.SC_NOT_FOUND);
          return;
        }

        renderModel = ((Map<?, ?>)sensorSeriesMap).get(sensorId);

      }

      if (renderModel == null) {
        LOG.error("no render model for target: " + target);
        rsp.setStatus(HttpServletResponse.SC_NOT_FOUND);
        return;
      }

      //      if (LOG.isDebugEnabled()) {
      //
      //        ByteArrayOutputStream bos = new ByteArrayOutputStream(0x100);
      //        OutputRenderer.getInstance().render(renderModel, "identity", renderParam, bos);
      //        LOG.debug("model identity transform:\r\n" + bos.toString("utf-8"));
      //
      //      }

      // -- create parameters for the XSL transformation
      
      Map<String, String> renderParam = new HashMap<String, String>();
      renderParam.put("contextPath", req.getContextPath());
      renderParam.put("requestURI", req.getRequestURI());
      
      // -- render the otuput
      
      rsp.setContentType("text/xml");
      OutputRenderer.getInstance().render(renderModel, target, renderParam, rsp.getOutputStream());

    } catch (Exception e) {

      throw new ServletException(e.toString(), e);

    }

  }

}
