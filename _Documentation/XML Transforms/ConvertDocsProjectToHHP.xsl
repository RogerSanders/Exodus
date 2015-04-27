<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:doc="http://www.exodusemulator.com/schema/XMLDocSchema.xsd">

  <!-- Specify the output document format settings -->
  <xsl:output method="text" indent="no" encoding="UTF-8"/>
  <xsl:preserve-space elements="*"/>
  
  <!-- Specify input parameters for the transform -->
  <xsl:param name="OutputFilePath" />
  <xsl:param name="ContentsFilePath" />
  <xsl:param name="IndexFilePath" />
  <xsl:param name="HTMLContentFileList" />

  <!-- Load a new line character into a variable for use when outputting the target document -->
  <xsl:variable name='newLine'><xsl:text xml:space="preserve">
</xsl:text></xsl:variable>

  <!-- Process the root "XMLDocProject" element -->
  <xsl:template match="/doc:XMLDocProject">
    <xsl:variable name='initialPagePath'>
      <xsl:if test="string-length(@InitialPage)">
        <xsl:text>_CHMContent\html\</xsl:text><xsl:value-of select="@InitialPage"/><xsl:text>.html</xsl:text>
      </xsl:if>
    </xsl:variable>

    <!-- Note that we need this newline entry here, as xslt 1.0 always inserts a byte order mark for UTF-8,
    and hhc.exe tries to process this as text, preventing the first line from being parsed correctly. -->
    <xsl:value-of select="$newLine"/>

    <xsl:text>[OPTIONS]</xsl:text><xsl:value-of select="$newLine"/>
    <xsl:text>Compatibility=1.1 or later</xsl:text><xsl:value-of select="$newLine"/>
    <xsl:text>Display compile progress=No</xsl:text><xsl:value-of select="$newLine"/>
    <xsl:text>Language=0x409 English (US)</xsl:text><xsl:value-of select="$newLine"/>
    <xsl:text>Full-text search=Yes</xsl:text><xsl:value-of select="$newLine"/>
    <xsl:text>Default Window=Help</xsl:text><xsl:value-of select="$newLine"/>
    <xsl:if test="string-length(@Title)">
      <xsl:text>Title=</xsl:text><xsl:value-of select="@Title"/><xsl:value-of select="$newLine"/>
    </xsl:if>
    <xsl:if test="string-length($initialPagePath)">
      <xsl:text>Default topic=</xsl:text><xsl:value-of select="$initialPagePath"/><xsl:value-of select="$newLine"/>
    </xsl:if>
    <xsl:if test="string-length($ContentsFilePath)">
      <xsl:text>Contents file=</xsl:text><xsl:value-of select="$ContentsFilePath"/><xsl:value-of select="$newLine"/>
    </xsl:if>
    <xsl:if test="string-length($IndexFilePath)">
      <xsl:text>Index file=</xsl:text><xsl:value-of select="$IndexFilePath"/><xsl:value-of select="$newLine"/>
    </xsl:if>
    <xsl:text>Compiled file=</xsl:text><xsl:value-of select="$OutputFilePath"/><xsl:value-of select="$newLine"/>
    <xsl:value-of select="$newLine"/>

    <xsl:text>[WINDOWS]</xsl:text><xsl:value-of select="$newLine"/>
    <xsl:text>Help=</xsl:text>
    <xsl:text>"</xsl:text><xsl:value-of select="@Title"/><xsl:text>"</xsl:text><xsl:text>,</xsl:text>
    <xsl:text>"</xsl:text><xsl:value-of select="$ContentsFilePath"/><xsl:text>"</xsl:text><xsl:text>,</xsl:text>
    <xsl:text>"</xsl:text><xsl:value-of select="$IndexFilePath"/><xsl:text>"</xsl:text><xsl:text>,</xsl:text>
    <xsl:text>"</xsl:text><xsl:value-of select="$initialPagePath"/><xsl:text>"</xsl:text><xsl:text>,</xsl:text>
    <xsl:text>"</xsl:text><xsl:value-of select="$initialPagePath"/><xsl:text>"</xsl:text><xsl:text>,</xsl:text>
    <xsl:text>,</xsl:text>
    <xsl:text>,</xsl:text>
    <xsl:text>,</xsl:text>
    <xsl:text>,</xsl:text>
    <xsl:text>0x10062520</xsl:text><xsl:text>,</xsl:text>
    <xsl:text>,</xsl:text>
    <xsl:text>0x0010387E</xsl:text><xsl:text>,</xsl:text>
    <xsl:text>,</xsl:text>
    <xsl:text>,</xsl:text>
    <xsl:text>,</xsl:text>
    <xsl:text>,</xsl:text>
    <xsl:text>0,</xsl:text>
    <xsl:text>0,</xsl:text>
    <xsl:text>,</xsl:text>
    <xsl:text>0</xsl:text><xsl:value-of select="$newLine"/>
    <xsl:value-of select="$newLine"/>

    <xsl:text>[FILES]</xsl:text><xsl:value-of select="$newLine"/>
    <xsl:call-template name="OutputFileList">
      <xsl:with-param name="remainingListEntries" select="$HTMLContentFileList"/>
    </xsl:call-template>
  </xsl:template>

  <!-- Add each file in the list of content files -->
  <xsl:template name="OutputFileList">
    <xsl:param name="remainingListEntries"/>
    <xsl:if test="string-length($remainingListEntries)">
      <xsl:variable name="nextListEntry"><xsl:value-of select="substring-before(concat($remainingListEntries,';'),';')"/></xsl:variable>

      <xsl:value-of select="$nextListEntry"/><xsl:value-of select="$newLine"/>

      <xsl:call-template name="OutputFileList">
        <xsl:with-param name="remainingListEntries" select="substring-after($remainingListEntries, ';')"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:template>

</xsl:stylesheet>
