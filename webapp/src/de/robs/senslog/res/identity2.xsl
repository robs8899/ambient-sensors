<?xml version="1.0"?>
<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xslt="http://xml.apache.org/xslt"
  xmlns:model="http://senslog.robs.de/model"
  xmlns="http://www.w3.org/1999/xhtml">

  <xsl:output omit-xml-declaration="yes" method="xml"
    encoding="utf-8" indent="yes" xslt:indent-amount="2" />

  <xsl:template match="@*|*|processing-instruction()|comment()">
    <xsl:copy>
      <xsl:apply-templates select="*|@*|text()|processing-instruction()|comment()" />
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
