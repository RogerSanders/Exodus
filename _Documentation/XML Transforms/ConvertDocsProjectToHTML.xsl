<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:doc="http://www.exodusemulator.com/schema/XMLDocSchema.xsd">

  <!-- Specify the output document format settings -->
  <xsl:output method="html" indent="yes" encoding="UTF-8" doctype-system="about:legacy-compat"/>
  <xsl:strip-space elements="*"/>

  <!-- Process the root "XMLDocProject" element -->
  <xsl:template match="/doc:XMLDocProject">
    <meta http-equiv="refresh" content="0; url=html/{@InitialPage}.html" />
  </xsl:template>

</xsl:stylesheet>
