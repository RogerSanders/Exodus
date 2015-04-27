<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:doc="http://www.exodusemulator.com/schema/XMLDocSchema.xsd">

  <!-- Specify the output document format settings -->
  <xsl:output method="html" indent="yes" encoding="UTF-8" doctype-system="about:legacy-compat"/>
  <xsl:strip-space elements="*"/>

  <!-- Specify input parameters for the transform -->
  <xsl:param name="TypeFilesPresent" />
  <xsl:param name="OutputHHCFile" select="false()" />
  <xsl:param name="BuildingCHMFiles" select="false()" />

  <!-- Load a new line character into a variable for use when outputting the target document -->
  <xsl:variable name='newLine'><xsl:text xml:space="preserve">
</xsl:text></xsl:variable>

  <!-- These elements allow formatted XHTML content to be copied directly to the output file -->
  <xsl:template match="*" mode="copy" priority="1">
    <xsl:element name="{name()}">
      <xsl:apply-templates select="node()|@*" mode="copy"/>
    </xsl:element>
  </xsl:template>
  <xsl:template match="node()|@*" mode="copy">
    <xsl:copy/>
  </xsl:template>

  <!-- Process the root "XMLDocContent" element -->
  <xsl:template match="/doc:XMLDocContent">
    <html>
      <head>
        <title>
          <xsl:value-of select="@Title"/>
        </title>
        <xsl:if test="not($OutputHHCFile)">
          <link href="../styles/DocStyle.css" rel="stylesheet" type="text/css"/>
        </xsl:if>
      </head>
      <xsl:choose>
        <xsl:when test="not($BuildingCHMFiles) and not(@TOCFile)">
          <body class="docBody">
            <div class="tocFrame">
              <iframe src="TableOfContents.html" class="tocTable"></iframe>
            </div>
            <div class="docFrame">
              <div class="mainSection">
                <xsl:apply-templates/>
              </div>
            </div>
          </body>
        </xsl:when>
        <xsl:otherwise>
          <body>
            <div class="mainSection">
              <xsl:apply-templates/>
            </div>
          </body>
        </xsl:otherwise>
      </xsl:choose>
    </html>
  </xsl:template>

  <!-- Process "Section" elements -->
  <xsl:template match="doc:Section">
    <h1 class="heading">
      <xsl:value-of select="@Title"/>
    </h1>
    <div class="section">
      <xsl:apply-templates/>
    </div>
  </xsl:template>
  <xsl:template match="doc:Section[@PageName]">
    <h1 class="heading">
      <xsl:choose>
        <xsl:when test="contains($TypeFilesPresent,concat('[',@PageName,']'))">
          <a href="{@PageName}.html"><xsl:value-of select="@Title"/></a>
        </xsl:when>
        <xsl:otherwise>
          <span class="missinglink"><xsl:value-of select="@Title"/></span>
        </xsl:otherwise>
      </xsl:choose>
    </h1>
    <div class="section">
      <xsl:apply-templates/>
    </div>
  </xsl:template>

  <!-- Process "SubSection" elements -->
  <xsl:template match="doc:SubSection">
    <h2 class="subHeading">
      <xsl:value-of select="@Title"/>
    </h2>
    <div class="subSection">
      <xsl:apply-templates/>
    </div>
  </xsl:template>

  <!-- Process "TOC" elements -->
  <xsl:template match="doc:TOC">
    <xsl:choose>
      <xsl:when test="$OutputHHCFile">
        <object type="text/site properties">
          <param name="ImageType" value="Folder"/>
          <!-- Note that without a new line here, hhc.exe fails to parse the file. -->
          <xsl:value-of select="$newLine"/>
        </object>
        <ul>
          <xsl:apply-templates/>
        </ul>
      </xsl:when>
      <xsl:otherwise>
        <ul class="tocRoot">
          <xsl:apply-templates/>
        </ul>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Process "TOCGroup" elements -->
  <xsl:template match="doc:TOCGroup">
    <xsl:choose>
      <xsl:when test="$OutputHHCFile">
        <li>
          <object type="text/sitemap">
            <param name="Name" value="{@Title}"/>
            <!-- Note that without a new line here, hhc.exe fails to parse the file. -->
            <xsl:value-of select="$newLine"/>
          </object>
          <ul>
            <xsl:apply-templates/>
          </ul>
        </li>
      </xsl:when>
      <xsl:otherwise>
        <li class="tocGroupListEntry">
          <span class="nolink"><xsl:value-of select="@Title"/></span>
          <ul class="tocGroupChildList">
            <xsl:apply-templates/>
          </ul>
        </li>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <xsl:template match="doc:TOCGroup[@PageName]">
    <xsl:choose>
      <xsl:when test="$OutputHHCFile">
        <li>
          <object type="text/sitemap">
            <param name="Name" value="{@Title}"/>
            <param name="Local" value="_CHMContent\html\{@PageName}.html"/>
            <!-- Note that without a new line here, hhc.exe fails to parse the file. -->
            <xsl:value-of select="$newLine"/>
          </object>
          <ul>
            <xsl:apply-templates/>
          </ul>
        </li>
      </xsl:when>
      <xsl:otherwise>
        <li class="tocGroupListEntry">
          <xsl:choose>
            <xsl:when test="contains($TypeFilesPresent,concat('[',@PageName,']'))">
              <a href="{@PageName}.html" target="_top"><xsl:value-of select="@Title"/></a>
            </xsl:when>
            <xsl:otherwise>
              <span class="missinglink"><xsl:value-of select="@Title"/></span>
            </xsl:otherwise>
          </xsl:choose>
          <ul class="tocGroupChildList">
            <xsl:apply-templates/>
          </ul>
        </li>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Process "TOCEntry" elements -->
  <xsl:template match="doc:TOCEntry">
    <xsl:choose>
      <xsl:when test="$OutputHHCFile">
        <li>
          <object type="text/sitemap">
            <param name="Name" value="{@Title}"/>
            <param name="Local" value="_CHMContent\html\{@PageName}.html"/>
            <!-- Note that without a new line here, hhc.exe fails to parse the file. -->
            <xsl:value-of select="$newLine"/>
          </object>
        </li>
      </xsl:when>
      <xsl:otherwise>
        <li class="tocEntry">
          <xsl:choose>
            <xsl:when test="contains($TypeFilesPresent,concat('[',@PageName,']'))">
              <a href="{@PageName}.html" target="_top"><xsl:value-of select="@Title"/></a>
            </xsl:when>
            <xsl:otherwise>
              <span class="missinglink"><xsl:value-of select="@Title"/></span>
            </xsl:otherwise>
          </xsl:choose>
        </li>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Process "XHTMLContent" elements -->
  <xsl:template match="doc:XHTMLContent">
      <xsl:apply-templates mode="copy"/>
  </xsl:template>

  <!-- Process "Paragraph" elements -->
  <xsl:template match="doc:Paragraph">
    <p><xsl:apply-templates/></p>
  </xsl:template>

  <!-- Process "Code" elements -->
  <xsl:template match="doc:Code">
    <pre class="sourceCode"><xsl:value-of select="."/></pre>
  </xsl:template>

  <!-- Process "Image" elements -->
  <xsl:template match="doc:Image">
    <img alt="{.}" src="../images/{.}.png"/>
  </xsl:template>
  <xsl:template match="doc:Image[@PageName]">
    <xsl:choose>
      <xsl:when test="contains($TypeFilesPresent,concat('[',@PageName,']'))">
        <a href="{@PageName}.html"><img alt="{.}" src="../images/{.}.png"/></a>
      </xsl:when>
      <xsl:otherwise>
        <img class="missingimagelink" alt="{.}" src="../images/{.}.png"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Process "PageRef" elements -->
  <xsl:template match="doc:PageRef">
    <xsl:choose>
      <xsl:when test="contains($TypeFilesPresent,concat('[',@PageName,']'))">
        <a href="{@PageName}.html"><xsl:value-of select="."/></a>
      </xsl:when>
      <xsl:otherwise>
        <span class="missinglink"><xsl:value-of select="."/></span>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Process "FunctionMemberList" elements -->
  <xsl:template match="doc:FunctionMemberList">
    <table class="functionMembers">
      <tr class="functionMembersRow">
        <th class="functionMembersHeader iconColumn"></th>
        <th class="functionMembersHeader nameColumn">Name</th>
        <th class="functionMembersHeader descriptionColumn">Description</th>
      </tr>
      <xsl:apply-templates select="doc:FunctionMemberListEntry"/>
    </table>
  </xsl:template>

  <!-- Process "FunctionMemberListEntry" elements -->
  <xsl:template match="doc:FunctionMemberListEntry">
    <tr class="functionMembersRow">
      <td class="functionMembersData iconColumn">
        <xsl:if test="@Deprecated">
          <img title="Deprecated member" alt="Deprecated member" src="../images/icons/Caution.gif"/>
        </xsl:if>
        <xsl:if test="@Static">
          <img title="Static member" alt="Static member" src="../images/icons/static.gif"/>
        </xsl:if>
        <xsl:choose>
          <xsl:when test="@Visibility='Public'">
            <img title="Public member" alt="Public member" src="../images/icons/pubmethod.gif"/>
          </xsl:when>
          <xsl:when test="@Visibility='Protected'">
            <img title="Protected member" alt="Protected member" src="../images/icons/protmethod.gif"/>
          </xsl:when>
          <xsl:when test="@Visibility='Private'">
            <img title="Private member" alt="Private member" src="../images/icons/privmethod.gif"/>
          </xsl:when>
        </xsl:choose>
      </td>
      <td class="functionMembersData nameColumn">
        <xsl:choose>
          <xsl:when test="@PageName">
            <xsl:choose>
              <xsl:when test="contains($TypeFilesPresent,concat('[',@PageName,']'))">
                <a href="{@PageName}.html"><xsl:value-of select="@Name"/></a>
              </xsl:when>
              <xsl:otherwise>
                <span class="missinglink"><xsl:value-of select="@Name"/></span>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:when>
          <xsl:otherwise>
            <span class="nolink"><xsl:value-of select="@Name"/></span>
          </xsl:otherwise>
        </xsl:choose>
      </td>
      <td class="functionMembersData descriptionColumn">
        <div class="summary">
          <xsl:apply-templates/>
        </div>
      </td>
    </tr>
  </xsl:template>

  <!-- Process "DataMemberList" elements -->
  <xsl:template match="doc:DataMemberList">
    <table class="dataMembers">
      <tr class="dataMembersRow">
        <th class="dataMembersHeader iconColumn"></th>
        <th class="dataMembersHeader nameColumn">Name</th>
        <th class="dataMembersHeader typeColumn">Type</th>
        <th class="dataMembersHeader descriptionColumn">Description</th>
      </tr>
      <xsl:apply-templates select="doc:DataMemberListEntry"/>
    </table>
  </xsl:template>

  <!-- Process "DataMemberListEntry" elements -->
  <xsl:template match="doc:DataMemberListEntry">
    <tr class="dataMembersRow">
      <td class="dataMembersData iconColumn">
        <xsl:if test="@Deprecated">
          <img title="Deprecated member" alt="Deprecated member" src="../images/icons/Caution.gif"/>
        </xsl:if>
        <xsl:if test="@Static">
          <img title="Static member" alt="Static member" src="../images/icons/static.gif"/>
        </xsl:if>
        <xsl:choose>
          <xsl:when test="@Visibility='Public'">
            <img title="Public member" alt="Public member" src="../images/icons/pubfield.gif"/>
          </xsl:when>
          <xsl:when test="@Visibility='Protected'">
            <img title="Protected member" alt="Protected member" src="../images/icons/protfield.gif"/>
          </xsl:when>
          <xsl:when test="@Visibility='Private'">
            <img title="Private member" alt="Private member" src="../images/icons/privfield.gif"/>
          </xsl:when>
        </xsl:choose>
      </td>
      <td class="dataMembersData nameColumn">
        <xsl:choose>
          <xsl:when test="@PageName">
            <xsl:choose>
              <xsl:when test="contains($TypeFilesPresent,concat('[',@PageName,']'))">
                <a href="{@PageName}.html"><xsl:value-of select="@Name"/></a>
              </xsl:when>
              <xsl:otherwise>
                <span class="missinglink"><xsl:value-of select="@Name"/></span>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:when>
          <xsl:otherwise>
            <span class="nolink"><xsl:value-of select="@Name"/></span>
          </xsl:otherwise>
        </xsl:choose>
      </td>
      <td class="dataMembersData typeColumn">
        <xsl:variable name="nextListEntry"><xsl:value-of select="@Type"/></xsl:variable>
        <xsl:choose>
          <xsl:when test="contains($nextListEntry,'!')">
            <xsl:variable name="nextListEntryTypeName"><xsl:value-of select="substring-before($nextListEntry,'!')"/></xsl:variable>
            <xsl:variable name="nextListEntryPageName"><xsl:value-of select="substring-after($nextListEntry,'!')"/></xsl:variable>
            <xsl:choose>
              <xsl:when test="contains($TypeFilesPresent,concat('[',$nextListEntryPageName,']'))">
                <a href="{$nextListEntryPageName}.html"><xsl:value-of select="$nextListEntryTypeName"/></a>
              </xsl:when>
              <xsl:otherwise>
                <span class="missinglink"><xsl:value-of select="$nextListEntryTypeName"/></span>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:when>
          <xsl:otherwise>
            <span class="nolink"><xsl:value-of select="$nextListEntry"/></span>
          </xsl:otherwise>
        </xsl:choose>
      </td>
      <td class="dataMembersData descriptionColumn">
        <div class="summary">
          <xsl:apply-templates/>
        </div>
      </td>
    </tr>
  </xsl:template>

  <!-- Process "ArgumentList" elements -->
  <xsl:template match="doc:ArgumentList">
    <dl class="argumentList">
      <xsl:apply-templates select="doc:ArgumentListEntry"/>
    </dl>
  </xsl:template>

  <!-- Process "ArgumentListEntry" elements -->
  <xsl:template match="doc:ArgumentListEntry">
    <dt class="argumentListEntryType">
      <span class="argumentName"><xsl:value-of select="@Name"/></span>
      <xsl:call-template name="doc:OutputTypeList">
        <xsl:with-param name="remainingListEntries" select="@Type"/>
      </xsl:call-template>
    </dt>
    <dd class="argumentListEntryDescription">
      <xsl:apply-templates/>
    </dd>
  </xsl:template>
  <xsl:template name="doc:OutputTypeList">
    <xsl:param name="remainingListEntries"/>
    <xsl:param name="onFirstListEntry" select="true()"/>
    <xsl:if test="string-length($remainingListEntries)">
      <!-- Insert a leading bracket if this is the first element -->
      <xsl:if test="$onFirstListEntry">
        <xsl:text> [</xsl:text>
      </xsl:if>

      <!-- Insert a link to the next referenced type in the list -->
      <xsl:variable name="nextListEntry"><xsl:value-of select="substring-before(concat($remainingListEntries,';'),';')"/></xsl:variable>
      <xsl:choose>
        <xsl:when test="contains($nextListEntry,'!')">
          <xsl:variable name="nextListEntryTypeName"><xsl:value-of select="substring-before($nextListEntry,'!')"/></xsl:variable>
          <xsl:variable name="nextListEntryPageName"><xsl:value-of select="substring-after($nextListEntry,'!')"/></xsl:variable>
          <xsl:choose>
            <xsl:when test="contains($TypeFilesPresent,concat('[',$nextListEntryPageName,']'))">
              <a href="{$nextListEntryPageName}.html"><xsl:value-of select="$nextListEntryTypeName"/></a>
            </xsl:when>
            <xsl:otherwise>
              <span class="missinglink"><xsl:value-of select="$nextListEntryTypeName"/></span>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:when>
        <xsl:otherwise>
          <span class="nolink"><xsl:value-of select="$nextListEntry"/></span>
        </xsl:otherwise>
      </xsl:choose>

      <!-- Insert a trailing bracket if this is the last element, otherwise insert a separator before the
      next element, and recursively call this template again. -->
      <xsl:choose>
        <xsl:when test="contains($remainingListEntries,';')">
          <xsl:text> / </xsl:text>
          <!-- Recursively call this template. We perform this step within the choose statement above for
          performance reasons, so that we can avoid unnecessary recursion after the last element. -->
          <xsl:call-template name="doc:OutputTypeList">
            <xsl:with-param name="remainingListEntries" select="substring-after($remainingListEntries, ';')"/>
            <xsl:with-param name="onFirstListEntry" select="false()"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:text>]</xsl:text>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:if>
  </xsl:template>

  <!-- Process "ReturnValue" elements -->
  <xsl:template match="doc:ReturnValue">
    <dl class="returnValueList">
      <dt class="returnValueListEntryType">
        <xsl:text>[</xsl:text>
        <xsl:variable name="nextListEntry"><xsl:value-of select="@Type"/></xsl:variable>
        <xsl:choose>
          <xsl:when test="contains($nextListEntry,'!')">
            <xsl:variable name="nextListEntryTypeName"><xsl:value-of select="substring-before($nextListEntry,'!')"/></xsl:variable>
            <xsl:variable name="nextListEntryPageName"><xsl:value-of select="substring-after($nextListEntry,'!')"/></xsl:variable>
            <xsl:choose>
              <xsl:when test="contains($TypeFilesPresent,concat('[',$nextListEntryPageName,']'))">
                <a href="{$nextListEntryPageName}.html"><xsl:value-of select="$nextListEntryTypeName"/></a>
              </xsl:when>
              <xsl:otherwise>
                <span class="missinglink"><xsl:value-of select="$nextListEntryTypeName"/></span>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:when>
          <xsl:otherwise>
            <span class="nolink"><xsl:value-of select="$nextListEntry"/></span>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:text>]</xsl:text>
      </dt>
      <dd class="returnValueListEntryDescription">
        <xsl:apply-templates/>
      </dd>
    </dl>
  </xsl:template>

  <!-- Process "PageRefList" elements -->
  <xsl:template match="doc:PageRefList">
    <ul class="pageRefList">
      <xsl:apply-templates select="doc:PageRefListEntry"/>
    </ul>
  </xsl:template>

  <!-- Process "PageRefListEntry" elements -->
  <xsl:template match="doc:PageRefListEntry">
    <li class="pageRefListEntry">
      <xsl:choose>
        <xsl:when test="contains($TypeFilesPresent,concat('[',@PageName,']'))">
          <a href="{@PageName}.html"><xsl:value-of select="."/></a>
        </xsl:when>
        <xsl:otherwise>
          <span class="missinglink"><xsl:value-of select="."/></span>
        </xsl:otherwise>
      </xsl:choose>
    </li>
  </xsl:template>

</xsl:stylesheet>
