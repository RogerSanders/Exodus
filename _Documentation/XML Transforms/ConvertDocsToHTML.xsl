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
      <link rel="stylesheet" href="../styles/source.css"/>
      <script src="../scripts/highlight.pack.js"></script>
      <script>hljs.initHighlightingOnLoad();</script>
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
          <span class="tocGroupHeaderNoLink"><xsl:value-of select="@Title"/></span>
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
              <a class="tocGroupHeaderWithLink" href="{@PageName}.html" target="_top"><xsl:value-of select="@Title"/></a>
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

  <!-- Process "URL" elements -->
  <xsl:template match="doc:URL">
    <a href="{.}" target="_blank"><xsl:value-of select="."/></a>
  </xsl:template>
  <xsl:template match="doc:URL[@DisplayName]">
    <a href="{.}" target="_blank"><xsl:value-of select="@DisplayName"/></a>
  </xsl:template>

  <!-- Process "Paragraph" elements -->
  <xsl:template match="doc:Paragraph">
    <p><xsl:apply-templates/></p>
  </xsl:template>

  <!-- Process "Code" elements -->
  <xsl:template match="doc:Code">
    <xsl:choose>
      <xsl:when test="@Language='C#'">
        <pre class="sourceCode"><code class="cs"><xsl:value-of select="."/></code></pre>
      </xsl:when>
      <xsl:when test="@Language='XML'">
        <pre class="sourceCode"><code class="xml"><xsl:value-of select="."/></code></pre>
      </xsl:when>
      <xsl:when test="@Language='XAML'">
        <pre class="sourceCode"><code class="xml"><xsl:value-of select="."/></code></pre>
      </xsl:when>
      <xsl:when test="@Language='C'">
        <pre class="sourceCode"><code class="cpp"><xsl:value-of select="."/></code></pre>
      </xsl:when>
      <xsl:when test="@Language='C++'">
        <pre class="sourceCode"><code class="cpp"><xsl:value-of select="."/></code></pre>
      </xsl:when>
      <xsl:when test="@Language='C++/CLI'">
        <pre class="sourceCode"><code class="cpp"><xsl:value-of select="."/></code></pre>
      </xsl:when>
      <xsl:when test="@Language='Python'">
        <pre class="sourceCode"><code class="py"><xsl:value-of select="."/></code></pre>
      </xsl:when>
      <xsl:when test="@Language='Perl'">
        <pre class="sourceCode"><code class="pl"><xsl:value-of select="."/></code></pre>
      </xsl:when>
      <xsl:when test="@Language='Ruby'">
        <pre class="sourceCode"><code class="rb"><xsl:value-of select="."/></code></pre>
      </xsl:when>
      <xsl:when test="@Language='PowerShell'">
        <pre class="sourceCode"><code class="ps"><xsl:value-of select="."/></code></pre>
      </xsl:when>
      <xsl:when test="@Language='WindowsBatch'">
        <pre class="sourceCode"><code class="cmd"><xsl:value-of select="."/></code></pre>
      </xsl:when>
      <xsl:when test="@Language='Bash'">
        <pre class="sourceCode"><code class="sh"><xsl:value-of select="."/></code></pre>
      </xsl:when>
      <xsl:when test="@Language='VB'">
        <pre class="sourceCode"><code class="vb"><xsl:value-of select="."/></code></pre>
      </xsl:when>
      <xsl:when test="@Language='VBScript'">
        <pre class="sourceCode"><code class="vbscript"><xsl:value-of select="."/></code></pre>
      </xsl:when>
      <xsl:otherwise>
        <pre class="sourceCode"><code><xsl:value-of select="."/></code></pre>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Process "TypeRef" elements -->
  <xsl:template match="doc:TypeRef">
    <span class="typeRef"><xsl:value-of select="."/></span>
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

  <!-- Process "Download" elements -->
  <xsl:template match="doc:Download">
    <a href="../downloads/{.}"><xsl:value-of select="."/></a>
  </xsl:template>

  <!-- Process "List" elements -->
  <xsl:template match="doc:List">
    <ul class="list">
      <xsl:apply-templates select="doc:ListEntry"/>
    </ul>
  </xsl:template>

  <!-- Process "ListEntry" elements -->
  <xsl:template match="doc:ListEntry">
    <li class="listEntry">
      <xsl:apply-templates/>
    </li>
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
        <xsl:if test="@Blocked">
          <img title="Blocked member. This member may be called internally, but is not available to be called directly from external code." alt="Blocked member" src="../images/icons/cross.gif"/>
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
          <xsl:when test="@Visibility='Internal'">
            <img title="Internal member" alt="Internal member" src="../images/icons/privmethod.gif"/>
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
          <xsl:when test="@Visibility='Internal'">
            <img title="Internal member" alt="Internal member" src="../images/icons/privfield.gif"/>
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

  <!-- Process "EnumMemberList" elements -->
  <xsl:template match="doc:EnumMemberList">
    <table class="enumMembers">
      <tr class="enumMembersRow">
        <th class="enumMembersHeader iconColumn"></th>
        <th class="enumMembersHeader nameColumn">Name</th>
        <th class="enumMembersHeader descriptionColumn">Description</th>
      </tr>
      <xsl:apply-templates select="doc:EnumMemberListEntry"/>
    </table>
  </xsl:template>

  <!-- Process "EnumMemberListEntry" elements -->
  <xsl:template match="doc:EnumMemberListEntry">
    <tr class="enumMembersRow">
      <td class="enumMembersData iconColumn">
        <xsl:if test="@Deprecated">
          <img title="Deprecated member" alt="Deprecated member" src="../images/icons/Caution.gif"/>
        </xsl:if>
        <img title="Public Enum" alt="Public Enum" src="../images/icons/pubenumeration.gif"/>
      </td>
      <td class="enumMembersData nameColumn">
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
      <td class="enumMembersData descriptionColumn">
        <div class="summary">
          <xsl:apply-templates/>
        </div>
      </td>
    </tr>
  </xsl:template>

  <!-- Process "TypeMemberList" elements -->
  <xsl:template match="doc:TypeMemberList">
    <table class="typeMembers">
      <tr class="typeMembersRow">
        <th class="typeMembersHeader iconColumn"></th>
        <th class="typeMembersHeader nameColumn">Name</th>
        <th class="typeMembersHeader descriptionColumn">Description</th>
      </tr>
      <xsl:apply-templates select="doc:TypeMemberListEntry"/>
    </table>
  </xsl:template>

  <!-- Process "TypeMemberListEntry" elements -->
  <xsl:template match="doc:TypeMemberListEntry">
    <tr class="typeMembersRow">
      <td class="typeMembersData iconColumn">
        <xsl:if test="@Deprecated">
          <img title="Deprecated member" alt="Deprecated member" src="../images/icons/Caution.gif"/>
        </xsl:if>
        <xsl:if test="@Static">
          <img title="Static member" alt="Static member" src="../images/icons/static.gif"/>
        </xsl:if>
        <xsl:choose>
          <xsl:when test="@Visibility='Public'">
            <xsl:choose>
              <xsl:when test="@TypeCategory='Interface'">
                <img title="Public Interface" alt="Public Interface" src="../images/icons/pubinterface.gif"/>
              </xsl:when>
              <xsl:when test="@TypeCategory='Class'">
                <img title="Public Class" alt="Public Class" src="../images/icons/pubclass.gif"/>
              </xsl:when>
              <xsl:when test="@TypeCategory='Struct'">
                <img title="Public Struct" alt="Public Struct" src="../images/icons/pubstructure.gif"/>
              </xsl:when>
              <xsl:when test="@TypeCategory='Enumeration'">
                <img title="Public Enum" alt="Public Enum" src="../images/icons/pubenumeration.gif"/>
              </xsl:when>
              <xsl:when test="@TypeCategory='Delegate'">
                <img title="Public Delegate" alt="Public Delegate" src="../images/icons/pubdelegate.gif"/>
              </xsl:when>
              <xsl:when test="@TypeCategory='Event'">
                <img title="Public Event" alt="Public Event" src="../images/icons/pubevent.gif"/>
              </xsl:when>
              <xsl:when test="@TypeCategory='Library'">
                <img title="Library" alt="Library" src="../images/icons/pubevent.gif"/>
              </xsl:when>
            </xsl:choose>
          </xsl:when>
          <xsl:when test="@Visibility='Protected'">
            <xsl:choose>
              <xsl:when test="@TypeCategory='Interface'">
                <img title="Protected Interface" alt="Protected Interface" src="../images/icons/protinterface.gif"/>
              </xsl:when>
              <xsl:when test="@TypeCategory='Class'">
                <img title="Protected Class" alt="Protected Class" src="../images/icons/protclass.gif"/>
              </xsl:when>
              <xsl:when test="@TypeCategory='Struct'">
                <img title="Protected Struct" alt="Protected Struct" src="../images/icons/protstructure.gif"/>
              </xsl:when>
              <xsl:when test="@TypeCategory='Enumeration'">
                <img title="Protected Enum" alt="Protected Enum" src="../images/icons/protenumeration.gif"/>
              </xsl:when>
              <xsl:when test="@TypeCategory='Delegate'">
                <img title="Protected Delegate" alt="Protected Delegate" src="../images/icons/protdelegate.gif"/>
              </xsl:when>
              <xsl:when test="@TypeCategory='Event'">
                <img title="Protected Event" alt="Protected Event" src="../images/icons/protevent.gif"/>
              </xsl:when>
              <xsl:when test="@TypeCategory='Library'">
                <img title="Library" alt="Library" src="../images/icons/pubevent.gif"/>
              </xsl:when>
            </xsl:choose>
          </xsl:when>
          <xsl:when test="@Visibility='Private'">
            <xsl:choose>
              <xsl:when test="@TypeCategory='Interface'">
                <img title="Private Interface" alt="Private Interface" src="../images/icons/privinterface.gif"/>
              </xsl:when>
              <xsl:when test="@TypeCategory='Class'">
                <img title="Private Class" alt="Private Class" src="../images/icons/privclass.gif"/>
              </xsl:when>
              <xsl:when test="@TypeCategory='Struct'">
                <img title="Private Struct" alt="Private Struct" src="../images/icons/privstructure.gif"/>
              </xsl:when>
              <xsl:when test="@TypeCategory='Enumeration'">
                <img title="Private Enum" alt="Private Enum" src="../images/icons/privenumeration.gif"/>
              </xsl:when>
              <xsl:when test="@TypeCategory='Delegate'">
                <img title="Private Delegate" alt="Private Delegate" src="../images/icons/privdelegate.gif"/>
              </xsl:when>
              <xsl:when test="@TypeCategory='Event'">
                <img title="Private Event" alt="Private Event" src="../images/icons/privevent.gif"/>
              </xsl:when>
              <xsl:when test="@TypeCategory='Library'">
                <img title="Library" alt="Library" src="../images/icons/pubevent.gif"/>
              </xsl:when>
            </xsl:choose>
          </xsl:when>
          <xsl:when test="@Visibility='Internal'">
            <xsl:choose>
              <xsl:when test="@TypeCategory='Interface'">
                <img title="Internal Interface" alt="Internal Interface" src="../images/icons/privinterface.gif"/>
              </xsl:when>
              <xsl:when test="@TypeCategory='Class'">
                <img title="Internal Class" alt="Internal Class" src="../images/icons/privclass.gif"/>
              </xsl:when>
              <xsl:when test="@TypeCategory='Struct'">
                <img title="Internal Struct" alt="Internal Struct" src="../images/icons/privstructure.gif"/>
              </xsl:when>
              <xsl:when test="@TypeCategory='Enumeration'">
                <img title="Internal Enum" alt="Internal Enum" src="../images/icons/privenumeration.gif"/>
              </xsl:when>
              <xsl:when test="@TypeCategory='Delegate'">
                <img title="Internal Delegate" alt="Internal Delegate" src="../images/icons/privdelegate.gif"/>
              </xsl:when>
              <xsl:when test="@TypeCategory='Event'">
                <img title="Internal Event" alt="Internal Event" src="../images/icons/privevent.gif"/>
              </xsl:when>
              <xsl:when test="@TypeCategory='Library'">
                <img title="Library" alt="Library" src="../images/icons/pubevent.gif"/>
              </xsl:when>
            </xsl:choose>
          </xsl:when>
        </xsl:choose>
      </td>
      <td class="typeMembersData nameColumn">
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
      <td class="typeMembersData descriptionColumn">
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

</xsl:stylesheet>
